#%%
import sys
import os
# sys.path.append(os.path.abspath(os.path.join(os.path.dirname('__file__'), '/home/kduplat/Documents/Data/Pscript2/Fct_scripts')))
# from Tools import *

color_list = ['#0072b2', '#d55e00', '#009e73','#f0e442',
              '#cc79a7', '#e69f00', '#56b4e9', '#000000']

import cv2
from matplotlib.path import Path
from skimage.filters import threshold_multiotsu, threshold_otsu
from skimage import filters, measure
# from scipy.ndimage.morphology import binary_closing
from scipy.ndimage import label
import scipy.ndimage
import numpy as np
import matplotlib.pyplot as plt


#%% Fct

def class_contrast(image, thresholds):
    regions = np.digitize(image, bins=thresholds)
    means = [np.mean(image[regions == i]) for i in range(len(thresholds) + 1)]
    contrast = np.std(means)  # Dispersion des moyennes
    return contrast

def otsu_criterion(image, thresholds):
    """
    Calcule la variance intra-classe totale après seuillage multi-Otsu.
    Plus cette valeur est faible, meilleure est la séparation des classes.
    """
    regions = np.digitize(image, bins=thresholds)
    total_variance = 0
    
    for i in range(len(thresholds) + 1):
        mask = regions == i
        if np.any(mask):
            variance = np.var(image[mask])
            total_variance += variance
    
    return total_variance

def otsu_detection(nbclasse, image, m_size, sigma_X, sigma_Y, mode_switch, min_nbclass = 2):
    max_interclasse = 0
    min_intraclasse = 1e12
    max_nbclasse = 0
    
    """ blurred_image = cv2.GaussianBlur(image, (m_size, m_size), sigmaX = sigma_X, sigmaY = sigma_Y) """
    blurred_image  = cv2.medianBlur(image, 5)
    
    #Find the best number of classes
    for n in range(min_nbclass, nbclasse + 1):
        thresholds = threshold_multiotsu(blurred_image[image!=0], classes = n)
        if mode_switch == 0:
            contrast = class_contrast(image, thresholds)
            if contrast > max_interclasse:
                max_interclasse = contrast
                max_nbclasse = n
                
        elif mode_switch == 1:
            intra_std = otsu_criterion(image, thresholds)
            if intra_std < min_intraclasse:
                min_intraclasse = intra_std
                max_nbclasse = n
        else:
            raise ValueError("otsu_detection mode_switch must be 0 or 1")
    # max_nbclasse = 5
    return cluster_detection(blurred_image, max_nbclasse)

def contours_to_label_fct(labeled_image, contours, num_features):
    L1 = labeled_image.shape[0]
    L2 = labeled_image.shape[1]
    ##########External contour##########
    contour_to_label = []
    for j, contour in enumerate(contours):  
        external_path = Path(contour)
        for k in range(1,num_features + 1):
            patch = np.argwhere(labeled_image == k)
            inside = external_path.contains_points(patch)
            if np.any(inside): #Au moins un élément dans être contenu dans le contour
                contour_to_label.append([j, k])
                # ax.plot(contour[:, 1], contour[:, 0], linewidth=2, color="red", alpha=1)
                break
        else: 
            contour_to_label.append([j, -1]) #Si aucun élément n'est contenu dans le contour, c'est un contour interieur    
            # ax.plot(contour[:, 1], contour[:, 0], linewidth=2, color="blue", alpha=1)  

    ##########Internal contour##########
    contour_to_label = np.array(contour_to_label)
    mask = contour_to_label[:,1] == -1

    for _, int_id_contour in enumerate(contour_to_label[mask][:,0]):
        for _, (id_contour, idpatch) in enumerate(contour_to_label[~mask]):
            external_path = Path(contours[id_contour])
            inside = external_path.contains_points(contours[int_id_contour])
            if np.any(inside): #Au moins un élément dans être contenu dans le contour
                contour_to_label[int_id_contour,1] = idpatch 
                # ax.plot(contours[int_id_contour][:, 1], contours[int_id_contour][:, 0], linewidth=2, color="green", alpha=1)
                break
        else:
            for point in contours[int_id_contour]:
                y, x = np.floor(point).astype(int)

                neighbors = [(y+1, x), (y-1, x), (y, x+1), (y, x-1)]
                for y_neighbor, x_neighbor in neighbors: 
                    if 0 <= x_neighbor < L1 and 0 <= y_neighbor < L2:
                        label_val = labeled_image[y_neighbor, x_neighbor]
                        if label_val:
                            contour_to_label[int_id_contour, 1] = label_val
                            break

                    
        if contour_to_label[int_id_contour,1] == -1:
            print("A contour is not linked to a cluster, id_contour = {}".format(int_id_contour))
        """     ax.plot(contours[int_id_contour][:, 1], contours[int_id_contour][:, 0], linewidth=3, color="orange", alpha=1)
        else :
            ax.plot(contours[int_id_contour][:, 1], contours[int_id_contour][:, 0], linewidth=3, color="green", alpha=1) """
    
    #Change the contour_id to the patch_id since it will be the same in all_contours_to_label and all_contours 
    return contour_to_label

def cluster_detection(blurred_image, nbclass):
    feature_areas = []
    all_contours = []
    all_labeled_image = []
    
    thresholds = threshold_multiotsu(blurred_image[blurred_image!=0], classes = nbclass)#Find the thresholds
    regions = np.digitize(blurred_image, bins=thresholds)
    all_contours_to_label = []
    
    for i in range (0, nbclass):
        # print(i)
        mask = regions == i  # Tolérance pour ignorer uniquement les valeurs très faibles
        masked_image = np.where(mask, 1, -1)

        smoothed_image = filters.gaussian(masked_image, sigma=1) #Allow to fill the possible holes in the clusters
        
        original = smoothed_image
        thres = threshold_otsu(smoothed_image) #Find a threshold between background and the class
        binary = original > thres

        binary_closed = scipy.ndimage.binary_closing(binary, structure=np.ones((2,2))) #Créé les domaines (dilatation + erosion)
        labeled_image, num_features = label(binary_closed)  #Etiquettes les regions 
        feature_areas.extend(np.bincount(labeled_image.ravel())[1:]) #Enregistre la taille de chaque regions (ravel aplati le tableau)
        all_labeled_image.append(labeled_image)

        contours = measure.find_contours(binary_closed, level=0.5)
        all_contours.extend(contours)
    
        ##########Link contours to labeled clusters##########

        contours_to_label = contours_to_label_fct(labeled_image, contours, num_features)
        contours_to_label[:,0] = i
        all_contours_to_label.extend(contours_to_label)  
    
    return np.array(feature_areas, dtype = np.int32), np.array(all_contours, dtype = object), np.array(all_labeled_image), thresholds, np.array(all_contours_to_label)
           
""" def process_detection(folderpath, nbclasse, nbsnap, nu, m_size = 3, sigma_X = 0, sigma_Y = 0, mode_switch = 0, nbins = 15):
    L1 = read_parameters(folderpath)[0]
    L2 = read_parameters(folderpath)[1]
    nu = read_parameters(folderpath)[2]

    lin_bins = np.arange(0, L1 * L2)
    logbins = np.logspace(np.log(1), np.log(L1*L2), nbins+1)
    histo = np.zeros(nbins)
    avg_width = 0

    
    
    elements = os.listdir("{}Snapshots/".format(folderpath))
    for element in elements[:nbsnap]:
        
        data = np.loadtxt("{}Snapshots/{}".format(folderpath,element), comments='#')
        sys = data.reshape(L1, L2).T
        sys_image_bis = (sys * 255).astype(np.uint8)  # Conversion en échelle de 0 à 255
        
        feature_areas, all_contours, all_labeled_image, thresholds, contour_to_label = otsu_detection(nbclasse, sys_image_bis, m_size, sigma_X, sigma_Y, mode_switch)
            
        if len(all_contours) > 600 and nu < 0.5:
            image = cv2.medianBlur(sys_image_bis, 5)            
            feature_areas, all_contours, all_labeled_image, thresholds, contour_to_label = otsu_detection(nbclasse, image, 7,0,0, mode_switch)
       
        
        histo += np.histogram(feature_areas, bins=logbins, density = True)[0]
        
    histo /= nbsnap 
    avg_width = avg_width / nbsnap
    return histo, avg_width """

def process_detection(filepath, nbclasse, nu,L1, L2, m_size = 3, sigma_X = 0, sigma_Y = 0, mode_switch = 0):
        
    data = np.loadtxt(filepath, comments='#')
    sys = data.reshape(L1, L2).T
    sys_image_bis = (sys * 255).astype(np.uint8)  # Conversion en échelle de 0 à 255
    
    feature_areas, all_contours, all_labeled_image, thresholds, contour_to_label = otsu_detection(nbclasse, sys_image_bis, m_size, sigma_X, sigma_Y, mode_switch)
        
    if len(all_contours) > 600 and nu < 0.5:
        image = cv2.medianBlur(sys_image_bis, 5)            
        feature_areas, all_contours, all_labeled_image, thresholds, contour_to_label = otsu_detection(nbclasse, image, 7,0,0, mode_switch)
    
    return feature_areas, all_contours, all_labeled_image, thresholds, contour_to_label
    

    
#%% Parameters

filepath = sys.argv[1]
nu = float(sys.argv[2])
L1 = int(sys.argv[3])
L2 = int(sys.argv[4])


# filepath = "/data1/DATA_Duplat/Brut/Test/Te62n1/Snapshot.txt"
# nu = 0.6
# L1 = 256
# L2 = 256

Tnu = np.array([0.1, 0.2, 0.3, 0.4, 0.5, 0.6])
Tcriterion = [1, 1, 1, 0, 1, 0] # 1: Interclass, 0: Intraclass
Tnbclass =   [5, 5, 5, 5, 5, 4]
Tmsize =     [3, 3, 3, 3, 5, 3]
Tsigma =   [0, 0, 0, 0, 3, 2]

id_nu = np.where(Tnu == nu)[0][0]
nbclass = Tnbclass[id_nu]
msize = Tmsize[id_nu]
sigma = Tsigma[id_nu]
mode = Tcriterion[id_nu]
nbins = 50


#%%Main
feature_areas, all_contours, all_labeled_image, thresholds, contour_to_label = process_detection(filepath, nbclass, nu,L1, L2, msize, sigma, sigma, mode)
#%%

combined_labeled_image = np.zeros((L1,L2), dtype = np.int32)
# print(np.shape(feature_areas))

prev_max = 0
#On fait un seul systeme avec tout les numéros de patches
for i, labeled_image in enumerate(all_labeled_image):
    mask = labeled_image>0
    combined_labeled_image[mask] = labeled_image[mask] + prev_max
    prev_max += np.max(labeled_image)
    
flat_labeled_image = combined_labeled_image.flatten()

# print(flat_labeled_image)

#On écrit pour le fichier C
sys.stdout.buffer.write(flat_labeled_image.tobytes())

# écrire taille du second tableau
sys.stdout.buffer.write(np.array([len(feature_areas)], dtype= np.int32).tobytes())
sys.stdout.buffer.write(feature_areas.tobytes())

sys.stdout.buffer.flush()


#%% Plot

""" data = np.loadtxt(filepath, comments='#')
sys = data.reshape(L1, L2).T
sys_image_bis = (sys * 255).astype(np.uint8)

with plt.style.context('/home/kduplat/Documents/py38/lib/python3.8/site-packages/sciplotlib/stylesheets/thesis.mplstyle'):
    
    fig, ax = plt.subplots(figsize = (3.5,2.8))
    
    ax.imshow(sys_image_bis, cmap = "binary")
    ax.set_axis_off()
    plt.savefig("snap_test.pdf", dpi = 300)
    
    fig, ax = plt.subplots(figsize = (3.5,2.8))
    
    ax.imshow(combined_labeled_image, cmap = "rainbow")
    ax.set_axis_off()
    plt.savefig("result_test.pdf", dpi = 300) """
   