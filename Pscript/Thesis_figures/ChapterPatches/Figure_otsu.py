import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname('__file__'), '/home/kduplat/Documents/Data/Pscript2/Fct_scripts')))
from Tools import *

color_list = ['#0072b2', '#d55e00', '#009e73','#f0e442',
              '#cc79a7', '#e69f00', '#56b4e9', '#000000']

import cv2
from matplotlib.path import Path
from skimage.filters import threshold_multiotsu, threshold_otsu
from skimage import filters, measure
# from scipy.ndimage.morphology import binary_closing
from scipy.ndimage import label
import scipy.ndimage



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


from matplotlib.path import Path
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
    
    return np.array(feature_areas), np.array(all_contours, dtype = object), np.array(all_labeled_image), thresholds, np.array(all_contours_to_label)
           
                

def process_detection(folderpath, nbclasse, nbsnap, nu, m_size = 3, sigma_X = 0, sigma_Y = 0, mode_switch = 0, nbins = 15):
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
    return histo, avg_width

def process_detection_brut(folderpath, nbclasse, nbsnap, nu, m_size = 3, sigma_X = 0, sigma_Y = 0, mode_switch = 0, nbins = 15):
    L1 = read_parameters(folderpath)[0]
    L2 = read_parameters(folderpath)[1]
    nu = read_parameters(folderpath)[2]

    logbins = np.logspace(np.log(1), np.log(L1*L2), nbins+1)
    histo = np.zeros(nbins)
    avg_width = 0

    
    
    elements = os.listdir("{}Snapshots/".format(folderpath))
    for element in elements[:nbsnap]:
        
        data = np.loadtxt("{}Snapshots/{}".format(folderpath,element), comments='#')
        sys = data.reshape(L1, L2).T
        sys_image_bis = (sys * 255).astype(np.uint8)  # Conversion en échelle de 0 à 255
        
        feature_areas, all_contours, _, _, _ = otsu_detection(nbclasse, sys_image_bis, m_size, sigma_X, sigma_Y, mode_switch)
            
        if len(all_contours) > 600 and nu < 0.5:
            image = cv2.medianBlur(sys_image_bis, 5)            
            feature_areas, all_contours, _, _, _ = otsu_detection(nbclasse, image, 7,0,0, mode_switch)
       
        
        histo += np.histogram(feature_areas, bins=logbins)[0]
        
    avg_width = avg_width / nbsnap
    return histo, avg_width


    
#%% Parameters
folderpaths = ["/data1/DATA_Duplat/Brut/Patch_detection/Pa2n{}/".format(nb) for nb in range(1, 7)]
# folderpaths = ["/data1/DATA_Duplat/Brut/Patch_detection/Pa1n2/"]
# nbclasse = 6
nbsnap = 2000
Tnu = [read_parameters(folderpath)[2] for folderpath in folderpaths]
Tcriterion = [1, 1, 1, 0, 1, 0] # 1: Interclass, 0: Intraclass
Tnbclass =   [5, 5, 5, 5, 5, 4]
Tmsize =     [3, 3, 3, 3, 5, 3]
Tsigma_X =   [0, 0, 0, 0, 3, 2]
Tsigma_Y = Tsigma_X

nbins = 50

#%%Main
# results = Parallel(n_jobs=-1)(delayed(process_detection)(folderpath, nbclass, nbsnap, nu, msize, sigma_X, sigma_Y, mode_switch, nbins)
#                                   for (folderpath, nu, msize, sigma_X, sigma_Y, nbclass, mode_switch) in zip(folderpaths, Tnu, Tmsize, Tsigma_X, Tsigma_Y, Tnbclass, Tcriterion))


results = Parallel(n_jobs=-1)(delayed(process_detection_brut)(folderpath, nbclass, nbsnap, nu, msize, sigma_X, sigma_Y, mode_switch, nbins)
                                   for (folderpath, nu, msize, sigma_X, sigma_Y, nbclass, mode_switch) in zip(folderpaths, Tnu, Tmsize, Tsigma_X, Tsigma_Y, Tnbclass, Tcriterion))


#%% Save data

arr_results = np.array(results, dtype = object)
Thisto = arr_results[:,0]

np.savez("/home/kduplat/Documents/Data/Pscript2/Thesis_figures/ChapterPatches/Figure_otsu_brut.npz", Thisto=Thisto)

#%% Plot

Tnu = [read_parameters(folderpath)[2] for folderpath in folderpaths]
L1 = read_parameters(folderpaths[0])[0]
L2 = read_parameters(folderpaths[0])[1]
NCURVES = len(Tnu)
colors = [plt.cm.get_cmap('viridis')(i/NCURVES) for i in range(NCURVES)]
log_bins = np.logspace(np.log(1), np.log(L1 * L2), nbins+1)


xmin = 10
xmax = [50000, 50000, 10000, 10000, 10000, 8000]
with plt.style.context('/home/kduplat/Documents/py38/lib/python3.8/site-packages/sciplotlib/stylesheets/thesis.mplstyle'):
    plt.rcParams['font.family'] = 'Arial' 
    fig, ax1 = plt.subplots(figsize=(3.5, 2.8))

    ax2 = fig.add_axes([0.26, 0.23, 0.28, 0.28])
    for j, (histo, x) in enumerate(zip(Thisto[:], Tnu[:])):


        mask = histo != 0
        fhist = histo[mask]
        fbins = log_bins[:-1][mask]
        idmin = np.argwhere(fbins>=xmin)[0][0]
        idmax = np.argwhere(fbins<=xmax[j])[-1][0]
        
        xfit = fbins[idmin: idmax]
        popt,_ = curve_fit(powerlaw2, np.log(xfit), np.log(fhist[idmin:idmax]))
        a, b = popt
        yfit = np.exp(powerlaw2(np.log(xfit), *popt))
        ax1.plot(fbins[idmin:],fhist[idmin:], color = colors[j])
        ax1.plot(xfit, yfit, 'k--')
        
        ax2.scatter(x, -np.round(b, 3), c = colors[j])
        # ax2.errorbar(x, -np.round(result.x[1], 3), yerr = perr[1], fmt = 'o', color = colors[j])
    ax2.axhline(1.5, ls = '--', c = 'k')

    # ax1.axvline(L1 * L2, ls = '--', c = 'k')
    ax1.set_xlabel(r'$s_p$')
    ax1.set_ylabel(r'$P(s_p)$')
    ax1.set_xscale('log')
    ax1.set_yscale('log')
    ax2.set_xlabel(r"$\nu$", fontsize = 7)
    ax2.set_ylabel(r"$\tau _p$", fontsize = 7)
    ax2.set_ylim(1.2, 2)
    ax2.set_xlim(0, 0.7)
    # ax2.set_xticks([0.2,0.6])
    # ax2.set_yticks([1.5,1.6])
    ax2.tick_params(labelsize = 6, length = 3, pad = 1)
    ax2.spines['top'].set_visible(True)
    ax2.spines['right'].set_visible(True)
    

    # plt.tight_layout()
    # ax.legend(fontsize = 20)
    plt.savefig("/home/kduplat/Documents/Data/Pscript2/Thesis_figures/ChapterPatches/P(s_p)_brut.pdf", bbox_inches = 'tight', dpi = 300)