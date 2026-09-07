#%% IMPORT
import numpy as np
import matplotlib.pyplot as plt
import re
import sciplotlib.style as spstyle


def read_parameters(folderpath):
    params = []
    filepath = f"{folderpath}/Para_used.par"
    with open(filepath, 'r') as file:
        lines = file.readlines()

        for line in lines:
            if line.strip() == '' or line.strip().startswith('#'):
                continue
            
            # Identifie les lignes avec un paramètre suivi de sa description
            match = re.match(r"([\d\.\+\-eE]+)\s+#\s*(.*)", line)
            if match:
                value = float(match.group(1)) if '.' in match.group(1) or 'e' in match.group(1) else int(match.group(1))
                params.append(value)
            
            # Identifie les lignes avec plusieurs valeurs et descriptions dans le cas à plusieurs éléments dans une même ligne
            else: 
                match_multi = re.match(r"(([\d\.\+\-eE]+\s+)+)#\s*(.*)", line)
                if match_multi:
                    values = [float(x) if '.' in x or 'e' in x else int(x) for x in match_multi.group(1).strip().split()]
                    params.append(values)
    
    return params

#%%Parameters
folderpath ="/data1/DATA_Duplat/Brut/Ava_track/Av8n1"
file = f"{folderpath}/shape_outputB10.txt"
outputfolder = "/data1/DATA_Duplat/Brut/Test/Te65n1"
nbava = 200
id_max = 1 #If -1 the largest avalanche is taken


#%% Main code
L1 = read_parameters(folderpath)[0]
L2 = read_parameters(folderpath)[1]
data = []
Nava = []
indices = []

with open(file) as f:
    
    for i, line in enumerate(f):
        if line.startswith("#"):
            continue
        col = np.array(line.strip().split()[3:], dtype=int)
        id = line.strip().split()[0]
        y, x = np.unravel_index(col, (L1, L2))
        # if len(x) >11000:
        #     print(len(x), i)
        coo = np.column_stack((x, y))
        data.append(coo.tolist())
        Nava.append(id)
        indices.append(col)
        if i == nbava:
            break

if (id_max == -1):
    id_max = np.argmax([len(coo) for coo in data])
    

Noava = Nava[id_max]
system = np.loadtxt("{}/Snapshots/nav{}_outputB.txt".format(folderpath, Noava)).reshape(L1, L2)
ava = np.array(data[id_max])
ava_indices = indices[id_max]

y = ava[:,0]
x = ava[:, 1]#On ajoute le nombre d'indice en premier dans la liste.

mask = np.zeros_like(system, dtype = bool)
mask[x,y] = True
systembis = np.where(mask, system, 1)

""" histo, bins = np.histogram(systembis[systembis !=2], bins = 100)
fig, ax = plt.subplots(figsize = (5,5))
ax.plot(bins[:-1], histo)
plt.show() """

sysbis_flat = systembis.flatten()

id_most_E = np.where((0.4<sysbis_flat) & (sysbis_flat<0.6))[0]
""" print(id_most_E)
for id in id_most_E:
    if id not in ava_indices:
        print("Error") """

#On ajoute le nombre d'indice en premier dans la liste.
ava_indices = np.insert(id_most_E, 0, int(len(id_most_E)))

np.savetxt("{}/Random_index.txt".format(outputfolder), ava_indices, delimiter = "\n", fmt = "%d")


sysbis_flat = systembis.flatten()
with open("{}/save_B.txt".format(outputfolder), "w") as f:
    f.write("{}  #L1\n{}  #L2\n0  #Nav\n0  #zF\n0  #StackzF\n1  #Seed\n".format(L1, L2))
    for h in sysbis_flat:
        f.write("{:.20f}  1.00000000000000000000  0.0000000000000000000\n".format(1-h))
    
    
fig, (ax1,ax2) = plt.subplots(1,2, figsize = (10,5))
ax1.imshow(system, cmap = "binary", vmin = 0, vmax = 1)
ax2.imshow(systembis, cmap = "binary", vmin = 0, vmax = 1)

ax1.set_xlim(-1, L1)
ax1.set_ylim(-1, L2)
ax1.set_axis_off()

ax2.set_xlim(-1, L1)
ax2.set_ylim(-1, L2)
ax2.set_axis_off()

fig.savefig("{}/Snap_patch.pdf".format(outputfolder), dpi = 600)
plt.show()

# %%
