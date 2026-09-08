import numpy as np
import random
import sys
import matplotlib.pyplot as plt

def gaussian_fixed_mean(mu=1.0, sigma=0.02, N=1000):
    x = np.random.normal(mu, sigma, N)
    x = np.minimum(x, 1)
    x = np.maximum(x, 0)
    x += (mu - x.mean())   # recentrage exact
    x = np.maximum(x, 0)
    x = np.minimum(x, 1)
    return x

switch = int(sys.argv[1])
L1 = int(sys.argv[2])
L2 =  int(sys.argv[3])
div = int(sys.argv[4])
delta = float(sys.argv[5])
snap_nb = int(sys.argv[6])
folder = sys.argv[7]

if switch:
    snap = np.loadtxt("{}/Snapshots/nav{}_outputB.txt".format(folder, snap_nb), comments="#").reshape(L1, L2).T

    snap_mask = np.zeros_like(snap)

    mask1 = snap >= 0.85
    mask2 = (snap >= 0.52) & (snap <0.85)
    mask3 = (snap >= 0.25) & (snap <0.52)
    mask4 = snap < 0.25

    snap_mask[mask1] = 1
    snap_mask[mask2] = 2
    snap_mask[mask3] = 3
    snap_mask[mask4] = 4
    np.savetxt("base_config.txt",snap_mask)
else:
    snap_mask = np.loadtxt("{}/base_config.txt".format(folder))

mask1 = snap_mask == 1
mask2 = snap_mask == 2
mask3 = snap_mask == 3
mask4 = snap_mask == 4

snap_mask[mask1] = gaussian_fixed_mean(0, 0.01, np.sum(mask1))
snap_mask[mask2] = gaussian_fixed_mean(delta, 0.01, np.sum(mask2))
snap_mask[mask3] = gaussian_fixed_mean(2*delta, 0.01, np.sum(mask3))
snap_mask[mask4] = gaussian_fixed_mean(3*delta, 0.01, np.sum(mask4))

x_div = div   #Must be a power of 2
y_div = div
nbsection = x_div*y_div
w_square = int(L1/x_div)
h_square = int(L2/x_div)

Tsection = np.empty((nbsection, w_square, h_square))
for i in range(nbsection):
    n = int(i%(L1/x_div))
    m = int(i//(L1/x_div))
    xmin = x_div * n
    xmax = x_div * (n+1)
    ymin = y_div * m
    ymax = y_div * (m+1) 
    
    Tsection[i] = snap_mask[xmin:xmax, ymin:ymax]
    

new_snap = np.zeros_like(snap)
for i in range(nbsection):
    n = int(i%(L1/x_div))
    m = int(i//(L1/x_div))
    xmin = x_div * n
    xmax = x_div * (n+1)
    ymin = y_div * m
    ymax = y_div * (m+1) 
    
    id = random.randint(0, nbsection-1)
    tilt = random.uniform(np.max(Tsection[id]),1) - np.max(Tsection[id])
    new_snap[xmin:xmax, ymin:ymax] = Tsection[id] + tilt
    

fig, (ax1,ax2) = plt.subplots(1,2, figsize = (8,8))
ax1.imshow(snap, cmap = 'binary', vmin = 0, vmax = 1)
ax2.imshow(new_snap, cmap = 'gray', vmin = 0, vmax = 1)
ax1.set_axis_off()
ax2.set_axis_off()
plt.savefig("{}/Snap_comp.pdf".format(folder), dpi = 600)
