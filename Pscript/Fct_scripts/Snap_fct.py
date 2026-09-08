from Import import *

def snapshots(folderpath, L1, L2):
    elements = os.listdir(folderpath)
    files = []
    pattern2 = re.compile(f"^{folderpath}/nav\d+_outputB.txt$")
    files = [element for element in elements if os.path.isfile(os.path.join(folderpath, element)) and pattern2.match(os.path.join(folderpath, element))]
    files = sorted(files, key = extract_nb)
    if len(files) < 5 :
        dimplot2 = len(files)
    else:
        dimplot2 = 2
    dimplot1 = len(files) // dimplot2 + 1

    
    size1 = 20
    size2 = int(size1 * L1/L2)
    
    fig, axes = plt.subplots(dimplot1, dimplot2, figsize=(size1, size2 * dimplot1))
    axes = axes.ravel() # Flatten axes array for easier indexing
    
    for i, file in enumerate(files):
        ax = axes[i]
        match = re.search(r'\d+', file)
        nava = int(match.group())
        
        try:
            sys = np.loadtxt(f'{folderpath}/{file}', comments='#').reshape(L1, L2).T
        except (IOError, ValueError) as e:
            print(f"Error loading {file}: {e}")
            continue
        ax.imshow(sys, cmap="binary")
        ax.axis('off')
        ax.set_title(f"{nava:.1e} avalanches", fontsize=15)
        
    for j in range(i + 1, len(axes)):
        fig.delaxes(axes[j])
     
    plt.show()
    
def snapshotsbis(Tfolderpath, L1, L2, switch = 1, sizeploth = 10, sizeplotw = 10, save = 0):  # Affiche les snapshots en colonne, switch permet side-view du dernier snapshot
    
    nbl = len(Tfolderpath) + switch  # Simplified calculation
    
    if sizeplotw > sizeploth:
        fig, axes = plt.subplots(nbl, 1, figsize=(sizeploth, sizeplotw))
    else:
        fig, axes = plt.subplots(1, nbl, figsize=(sizeploth, sizeplotw))
        
    plt.subplots_adjust(wspace=0.05, hspace=0.05)
    if nbl == 1:
        axes = [axes]
    
    # Load and display all snapshots at once using list comprehension
    systems = [np.loadtxt(file, comments='#').reshape(L1, L2).T for file in Tfolderpath]
    
    # Plot snapshots
    for i, sys in enumerate(systems):
        axes[i].imshow(sys, cmap="binary", vmin = 0, vmax =1)
        axes[i].set_axis_off()
    
    # Plot side view if requested
    if switch:
        ax = axes[-1]
        ax.plot(systems[-1][-1][:], linewidth=2)
        ax.set_xlim(0, L2)
        ax.set_ylim(0, 1)
        ax.tick_params(labelsize=28, length=8, width=2)
        ax.tick_params(labelsize=28, length=5, width=1, which='minor')

    if save:
        plt.savefig("snapshotbis.pdf", dpi = 600)
    plt.show()

def snapandside(file, L1, L2, sizeplotw, sizeploth, l = -1): #Plot a snapshot and the side view of the center of the system
    
    # Create figure and subplots at once
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(sizeplotw, sizeploth))
    plt.subplots_adjust(wspace=0.05, hspace=0.05)
    
    # Load and transform data efficiently
    sys = np.loadtxt(file, comments='#').reshape(L2, L1).T
    
    # Plot snapshot
    ax1.imshow(sys, cmap="binary")
    ax1.set_axis_off()

    
    # Plot side view
    ax2.plot(sys[l][:], linewidth=2)
    ax2.set_xlim(0, L2)
    ax2.set_ylim(0, 1)
    
    # Set tick parameters for bottom plot
    tick_params = {'labelsize': 15, 'length': 8, 'width': 2}
    ax2.tick_params(**tick_params)
    ax2.tick_params(length=5, width=1, which='minor')
    ax2.set_ylabel(r"$z$", fontsize = 20)
    
    plt.show()
    
def snapfromsave(folderpath, L1, L2, sizeplotw, sizeploth, icol = 0, start_line = 6):

    snapshot = np.zeros(L1*L2)
    
    with open(f"{folderpath}/save_B.txt", 'r') as f:
        lines = f.readlines()  # Lire toutes les lignes
        
    data = lines[start_line:]
    data_array = np.array([line.strip().split('  ') for line in data], dtype=float)

    zF = float(lines[3].strip().split('  ')[0])
    snapshot = data_array[:, 1] - (data_array[:, 0] + zF - data_array[:, 2])
    
    sys = snapshot.reshape(L1, L2)
    
    fig, ax = plt.subplots(figsize=(sizeplotw, sizeploth))
    im = ax.imshow(sys, cmap="binary")
    ax.set_xticks([])
    ax.set_yticks([])
    cbar = plt.colorbar(im, fraction = 0.1)


    cbar.ax.tick_params(width=0, length = 0)  # épaisseur des ticks (bord de la colorbar)
    cbar.outline.set_linewidth(1)  # épaisseur du contour de la colorbar
    cbar.ax.tick_params(labelsize=14) 
    
    plt.show()
        