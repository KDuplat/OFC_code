#%%
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname('__file__'), '/home/kduplat/Documents/Data/Pscript2/Fct_scripts')))
sys.path.append(os.path.abspath(os.path.join(os.path.dirname('__file__'), '/home/kduplat/Documents/Data/Pscript2/LateX')))
from Tools import *
from latexmain import *
#%%
foldername = ["Patch_detection"]
Trapport_path = ['/home/kduplat/Documents/Data/Pscript2/Avatrack/Snapshots']
execindicies = [[1]] 

tcorrel = [-1]

sigma_X = 5
sigma_y = 5
matrix_size = 3

#%%Hidden parameters
#Reminder: L1 is the width of the system, L2 is the lenght of the system
L1 = -1
L2 = -1
nu = -1
dim = [2]
ncifras = [1]
nbins = 11
nbfile = 1
casetab = [0]
nbava = -1
bccond = -1
zpar = -1
zmpar = -1
stddev = -1
nshape = -1
iniava = -1
deltaava = -1
sampleava = -1
avgava = -1
nl = -1
nbline = -1

corrlen = -1

#%%*******************FUNCTIONS***********************
def preambule(*packages):
    p = ""
    for i in packages:
        p = p+"\\usepackage{"+i+"}\n"
    return p

def insertfigure(name, dim1):
    fig =''
    """ fig = "\\begin{figure}[h] \n  \\centering\n" """
    fig += f"\\includegraphics[width={dim1}\\textwidth]{{{name}}}\n  \\centering\n"
    """ fig += "\end{figure}\n" """
    return fig

def insertfigure2(name, w):
    fig = ''
    """ fig = "\\begin{figure}[h] \n  \\centering\n" """
    """ w = 0.9/len(name)  """
    for i in range(len(name)):
        fig +="  \\begin{subfigure}{"+f"{w[i]:.2f}"+"\\textwidth}\n    \\centering\n"
        fig +="    \\includegraphics[width=0.9\\textwidth]{" + f"{name[i]}" + "}\n"
        # fig +="    \\caption*{}\n" 
        fig +="  \\end{subfigure}\n"
    """ fig+= "\end{figure}\n" """
    return fig

def insertfigure3(name, w):
    fig = ''
    """ fig = "\\begin{figure}[h] \n  \\centering\n" """
    """ w = 0.9/len(name)  """
    for i in range(len(name)):
        fig +="  \\begin{subfigure}{"+f"{w[i]:.2f}"+"\\textwidth}\n    \\centering\n"
        for j in range(len(name[i])):
            fig +="    \\includegraphics[width=0.9\\textwidth]{" + f"{name[i][j]}" + "}\n"
        fig +="  \\end{subfigure}\n"
    """ fig+= "\end{figure}\n" """
    return fig

def class_contrast(image, thresholds):
    regions = np.digitize(image, bins=thresholds)
    means = [np.mean(image[regions == i]) for i in range(len(thresholds) + 1)]
    contrast = np.std(means)  # Dispersion des moyennes
    return contrast


def otsu_detection(nbclasse, image):
    max_interclasse = 0
    max_nbclasse = 0
    for n in range(2,nbclasse):
        
        thresholds = threshold_multiotsu(image, classes = n)
        regions = np.digitize(image, bins=thresholds)
        contrast = class_contrast(image, thresholds)
        # print("n = {}, Contraste inter-classes :{}".format(n, contrast))
        
        if contrast > max_interclasse:
            max_interclasse = contrast
            max_nbclasse = n

    # max_nbclasse = 5
    thresholds = threshold_multiotsu(image, classes = max_nbclasse)
    regions = np.digitize(image, bins=thresholds)

    feature_areas = []
    all_contours = []
    all_labeled_image = []

    for i in range (1, max_nbclasse + 1):

        mask = regions == i  # Tolérance pour ignorer uniquement les valeurs très faibles
        masked_image = np.where(mask, 1, -1)

        # Appliquer un filtre Gaussien sur les pixels valides
        smoothed_image = filters.gaussian(masked_image, sigma=1)

        original = smoothed_image
        thres = threshold_otsu(smoothed_image) #Trouve un seuil entre le background et les patches
        binary = original > thres
        binary_closed = binary_closing(binary, structure=np.ones((2,2))) #Créé les domaines (dilatation + erosion)
        labeled_image, num_features = label(binary_closed)  #Etiquettes les regions 
        feature_areas.extend(np.bincount(labeled_image.ravel())[1:]) #Enregistre la taille de chaque regions
        all_labeled_image.append(labeled_image)

        contours = measure.find_contours(binary_closed, level=0.5)
        all_contours.extend(contours)
    
    return feature_areas, all_contours, all_labeled_image
    


def process_detection(folderpath, nbclasse, nbsnap, nu, matrix_size = 3, sigmaX = 0, sigma_Y = 0, switch = 0):
    L1 = read_parameters(folderpath)[0]
    L2 = read_parameters(folderpath)[1]
    nu = read_parameters(folderpath)[2]

    lin_bins = np.arange(0, L1 * L2)
    histo = np.zeros(len(lin_bins) - 1)
    avg_width = 0
    # file = f"{folderpath}nav100000000000_outputB.txt"
    # file = f"{folderpath}Snapshots/nav6000000000_outputB.txt"


    elements = os.listdir("{}Snapshots/".format(folderpath))
    print(len(elements))
    for element in elements[:nbsnap]:
        data = np.loadtxt("{}Snapshots/{}".format(folderpath,element), comments='#')
        sys = data.reshape(L1, L2).T
        sys_image_bis = (sys * 255).astype(np.uint8)  # Conversion en échelle de 0 à 255 pour OpenCV
        image = cv2.GaussianBlur(sys_image_bis, (matrix_size, matrix_size), sigmaX = 0, sigma_Y = 0)
        
        
        feature_areas, contours, all_labeled_image = otsu_detection(nbclasse, image)
        if switch :
            fig, ax = plt.subplots(1,2 , figsize = (20,10))
            ax[0].imshow(sys_image_bis, cmap='binary')
            for contour in contours:
                ax[0].plot(contour[:, 1], contour[:, 0], linewidth=2, color="red", alpha=0.5)
        if len(feature_areas) > 600 and nu < 0.5:
            image = cv2.medianBlur(sys_image_bis, 5)
            image = cv2.GaussianBlur(image, (7, 7), sigmaX = 0, sigmaY = 0)
            
            feature_areas, contours_bis, all_labeled_image = otsu_detection(nbclasse, image)
            if switch :
                ax[1].imshow(sys_image_bis, cmap='binary')
                for contour in contours_bis:
                    ax[1].plot(contour[:, 1], contour[:, 0], linewidth=2, color="red", alpha=0.5)
                    
        if switch :        
            ax[0].axis('off')
            ax[1].axis('off')
            plt.savefig("Snapshots/{}/{}.png".format(nu, element[:-4]))
            plt.close(fig)


        histo += np.histogram(feature_areas, bins=lin_bins)[0]
        
        # centers = patch_analysis(all_labeled_image, sys_image_bis, contours)
        result_analysis = patch_analysis(all_labeled_image, L2)
        centers = result_analysis[0]
        avg_width += result_analysis[1]
        
    avg_width = avg_width / nbsnap
    return histo, avg_width

#%%******************INIT LATEX*******************
start = "\\documentclass[8pt]{extarticle}\n\\usepackage[utf8]{inputenc}\n"
start = start+preambule('amsmath','amsfonts', 'amssymb','lmodern','babel','graphicx', 'subcaption', 'lipsum', 'tikz', 'geometry')
start += '\\geometry{left = 2cm, right = 2cm, top = 2cm, bottom = 2cm}\n\n'
start = start+"\n\\begin{document}\n\n \n\\tiny\n\n"
end = "\n\\end{document}"    



#%%*******************INITIALIZE******************

brutpath = "/data1/DATA_Duplat/Brut"

for i, project in enumerate(foldername):
    
    index = project[:2]
    projectpath = f"{brutpath}/{project}"
    rapportpath = Trapport_path[i]

    for exec in execindicies[i]:
        #We are looking for the number of folder with the name we want
        pattern = re.compile(f"^{re.escape(projectpath)}/{re.escape(index)}{exec}+n\d+$")
        pattern = re.compile(pattern)
        
        elements = os.listdir(projectpath)        
        elements = sorted(elements, key = extract_nb2)
        folders = []
        nsimu = []
        for element in elements:
            completpath = os.path.join(projectpath, element)
            if os.path.isdir(completpath) and pattern.match(completpath):
                folders.append(element)
                match = re.findall(r'\d+', element)
                nsimu.append(int(match[1]))

        if len(folders) == 0:
            print(f"No exec with the number {exec} in project {project}")
            continue
        
        body = ''
        
        diss = [d.name for d in os.scandir(Trapport_path[i]) if d.is_dir()]
        diss = np.sort(diss)
#%%
        for nf, folder in enumerate(folders):
            L1,L2,nu,nbava,bccond,zpar,zmpar,stddev,nshape,iniava,deltaava,sampleava,avgava,nl,nbline,corrlen= -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1

            dim = [2]
            ncifras = [1]
            nbfile = 1
            casetab = [0]
            
            print(f'folder {folder}')
            bodypara1, bodypara2, bodypara3, bodypara4= '', '', '', ''
            body += '\\noindent '
            

            folderpath = f"{projectpath}/{folder}"
            with open(f"{folderpath}/Para_used.par") as f:
                for line in f :
                    
                    match = re.match(r'(\d+)   # L1     :lattice size x1', line)
                    if match:
                        L1 = int(match.group(1))
                        bodypara1 += f"L1 : {L1}\\\ \n"
                        continue
                        
                    match = re.match(r'(\d+)   # L2     :lattice size x2', line)
                    if match:
                        L2 = int(match.group(1))
                        bodypara1 += f"L2 : {L2}\\\ \n"
                        continue
                        
                    match = re.match(r'(\d+(\.\d+)?)   # nu    : dissipation', line)
                    if match:
                        nu = float(match.group(1))
                        bodypara1 += f"Nu : {nu}\\\ \n"  
                        continue
                        
                    match = re.match(r'(\d+)    #bcmode: boundary condition mode', line)
                    if match:
                        bccond = int(match.group(1))          
                        if bccond == 0:
                            bodypara2 += "Bc: Periodic.\\\ \n"
                        elif bccond == 1:
                            bodypara2 += "Bc:   x2 : periodic, x1 : open.\\\ \n"
                        elif bccond == 2: 
                            bodypara2 += "Bc:   x2 : periodic, x1 = L1 : open, x1 = 0 : reflective.\\\ \n"
                        else:
                            raise ValueError("Boundary conditions found but no matching mode")
                        continue

                    match = re.match(r'(\d+)   #zmode     :mode   initializing  z value', line)
                    if match:
                        zpar = int(match.group(1))
                        if zpar == 0:
                            bodypara2 += "Sites initialized at 0.\\\ \n"
                        elif zpar == 1:
                            bodypara2 += "Sites initialized at a random value between 0 and 1.\\\ \n"
                        elif zpar == 3:
                            bodypara2 += "Sites initialized at a random value between 0 and its threshold value.\\\ \n"
                        else:
                            raise ValueError("zpar found but no matching mode")
                        continue
                        
                    
                    match = re.match(r'(\d+)   #zmmode     :mode   renewing threshold', line)
                    if match:
                        zmpar = int(match.group(1))
                        if zmpar == 0:
                            bodypara3 += "Threshold renewal: set to a random value between 0 and 1. "
                        elif zmpar == 1:
                            bodypara3 += "Threshold renewal: set to a random value between 0.9 and 1.1. "
                        elif zmpar == 3:
                            bodypara3 += "Threshold renewal: set to a random value following a Gaussian. "
                        elif zmpar == 4:
                            bodypara3 += "Threshold renewal: fixed at 1. "
                        else : 
                            raise ValueError("zmpar found but no matching mode")
                        continue
                    
                    match = re.match(r'(\d+(\.\d+)?(?:[eE][+-]?\d+)?)   #zmpar  : parameter ...', line)
                    if match :
                        stddev = float(match.group(1))
                        bodypara3 += f"Std : {stddev}\\\ \n"
                        continue
                        
                    match = re.match(r'(\d+)    #Nb of avalanches where we want to see their shape', line)
                    if match:
                        nshape = int(match.group(1))
                        continue
                    
                    match = re.match(r'(\d+(\.\d+)?(?:[eE][+-]?\d+)?)  (\d+(\.\d+)?(?:[eE][+-]?\d+)?)  (\d+(\.\d+)?(?:[eE][+-]?\d+)?)  (\d+(\.\d+)?(?:[eE][+-]?\d+)?)    #Correlation  : ini ava, delta ava, nb ava, delta avg', line)
                    if match:
                        iniava = float(match.group(1))
                        deltaava = float(match.group(3))
                        sampleava = float(match.group(5))
                        avgava = float(match.group(7))
                        continue
                    
                    match =  re.match(r'(\d+)  (\d+)    # Correlation: nb of position, nb of line to avg', line)
                    if match:
                        nl = int(match.group(1))
                        nbline = int(match.group(2))
                        continue
            
            bodypara4 += f'Correlation computed every {deltaava} avalanches starting from {iniava} averaged on {avgava} in a sample of {sampleava} avalanches and {nbline} lines. \\\ \n'
                       
            
            f.close()
            
            body += bodypara1 + bodypara2 + bodypara3
            """ body += "Toppling site gives its energy to its four neighbour and is reset to 0.\n\n" """

            
            # for snapshots in diss[nf]:
#%%*******************MAIN FUNCTIONS******************

            Tsnap = os.listdir('{}/{}'.format(Trapport_path[i], diss[nf]))
            # print(Tsnap)
            
            names = ['{}/{}/{}'.format(Trapport_path[i], diss[nf], snap)for snap in Tsnap]

            w = [0.20] * len(names)
            body += "\\begin{figure}[h] \n  \\centering\n"
            body += insertfigure2(names,w)
            body += "\end{figure}\n"
                
            body += "\\newpage \n\n"
            
            
#%% WRITE THE LATEX FILE
        container = start + body + end
        filename = f"{rapportpath}/Rapport{index}{exec}.tex"
        if os.path.exists(filename):
            os.remove(filename)
        with open(filename,"x") as f :# "x" pour la création et l'écriture
            f.write(container)

        
        instructions = f"pdflatex -output-directory={rapportpath} " + filename#"
        os.system(instructions)
"""         readpdf = filename + ".pdf"
        os.system(readpdf) """
    



    
    
