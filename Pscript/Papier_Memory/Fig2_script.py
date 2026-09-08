#%%
import numpy as np
import matplotlib.pyplot as plt
import sciplotlib.style as spstyle
from matplotlib import gridspec

# %pip install SciencePlots

# Tvar = [0.0, 0.005, 0.01, 0.015, 0.02, 0.025, 0.03, 0.06, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6]

L1 = 256
L2 = 256

data = np.load("Figure2.npz", allow_pickle=True)

Tvar = data["Tvar"]
Ttau = data["Ttau_s"]
Tbins = data["Tbins_s"]
Thisto = data["Thisto_s"]
Txfit = data["Txfit_s"]
Tyfit = data["Tyfit_s"]
Tnonzero= data["Tnonzero_s"] 
Tmean_s= data["Tmean_s"]
Tmean_t= data["Tmean_t"]
Tmean_diff= data["Tmean_diff"]

#%%
from matplotlib.ticker import FixedLocator, NullLocator
NCURVES = len(Tvar)

with plt.style.context(spstyle.get_style('nature-reviews')):
    print(spstyle.get_style('nature-reviews'))
    plt.rcParams['font.family'] = 'Arial' 
    fig = plt.figure(figsize=(12.8, 4.8))
    gs = gridspec.GridSpec(2, 4, width_ratios=[1.5, 5, 5, 5], wspace = 0.4, hspace = 0)  # 2 lignes, 2 colonnes
    colors = [plt.cm.get_cmap('viridis')(i/NCURVES) for i in range(NCURVES)]

    ################## P(s) ##################
    ax1 = fig.add_subplot(gs[:, :2])  # La figure 1 prend toute la largeur (2 colonnes)

    for i, (histo, bins, xfit, yfit) in enumerate(zip(Thisto, Tbins, Txfit, Tyfit)):
        ax1.plot(bins,histo, color = colors[i])
        ax1.plot(xfit, yfit, 'k--')

    ax1.set_xticks([1e0, 1e1, 1e2, 1e3, 1e4, 1e5])
    ax1.set_xlim(0.8,1.1e5)
    ax1.set_xlabel(r'$s$')
    ax1.set_ylabel(r'$P(s)$')
    ax1.set_xscale('log')
    ax1.set_yscale('log')
    

    
    ################## Smax ##################
    ax2 = fig.add_subplot(gs[1, 2])
    ax2.scatter(Tvar, Tnonzero, c=colors, marker='o', zorder=2)  # s contrôle la taille des marqueurs
    ax2.plot(Tvar, Tnonzero, '-', c = 'k', zorder=1)  # s contrôle la taille des marqueurs
    ax2.axvline(Tvar[4], ls = '--', c = 'k', label = "{} = {}".format(r"$\nu$", Tvar[2]))
    ax2.axhline(L1 * L2, c = "grey", ls = '--')
    ax2.xaxis.set_visible(True)
    ax2.set_xlabel(r'$\nu$')
    ax2.set_ylabel(r'$s_{max}$')
    ax2.set_yscale('log')

    # para_fig(ax2, r'$\nu$', r'$s_{max}$',type = 'semilog', tickssize = small_debug, labelsize = medium_debug)
    # ax2.yaxis.labelpad = 4
    N = L1*L2
    yticks = [N, N/2, N/4]
    ax2.yaxis.set_major_locator(FixedLocator(yticks))  # Supprime tous les autres ticks majeurs
    ax2.yaxis.set_minor_locator(NullLocator())  # Supprime les ticks mineurs
    ax2.set_yticklabels([r'$N$', r'$N / 2$', r'$N / 4}$'])
    ax2.xaxis.set_label_coords(0.5, -0.2) 
    ax2.set_xlim(-0.05,0.7) 
    ax2.spines['top'].set_visible(True)
    ax2.spines['right'].set_visible(True)

    ax22 = fig.add_axes([0.545, 0.19, 0.065, 0.17])
    ax22.scatter(Tvar, Tnonzero, c=colors, marker='o', zorder=2)  # s contrôle la taille des marqueurs
    ax22.plot(Tvar, Tnonzero, '-', c = 'k', zorder=1)
    ax22.axvline(Tvar[4], ls = '--', c = 'k', label = "{} = {}".format(r"$\nu$", Tvar[2]))
    ax22.set_xlim(-0.01,0.07)
    # para_fig(ax22, r'$\nu$', r'$s_{max}$',type = 'semilog', labelsize = medium_debug, tickssize= small_debug/1.5, width_M = 1, length_M = 5)
    ax22.set_xlabel(r'$\nu$')
    ax22.set_ylabel(r'$s_{max}$')
    ax22.set_yscale('log')
    ax22.yaxis.set_major_locator(FixedLocator(yticks))  # Supprime tous les autres ticks majeurs
    ax22.yaxis.set_minor_locator(NullLocator())  # Supprime les ticks mineurs
    ax22.set_yticklabels([r'$N$', r'$N / 2$', r'$N / 4}$'])
    ax22.spines['top'].set_visible(True)
    ax22.spines['right'].set_visible(True)
    ax22.tick_params(labelsize = 8)
    ax22.xaxis.set_label_coords(0.5, -0.1) 

    ################## Avg_dissipated energy ##################
    # mean_diff = mean_ava - mean_rest
    ax3 = fig.add_subplot(gs[1, 3])  # La figure 1 prend toute la largeur (2 colonnes)
    ax3.scatter(Tvar, Tmean_t * Tvar, c=colors, marker='o', zorder=2)
    ax3.plot(Tvar, Tmean_t * Tvar, c = 'k', zorder = 1)

    

    ax32 = ax3.twinx()
    ax32.plot(Tvar, Tmean_s**(1/2)*Tmean_diff, marker = 'o', c ='#0072b2')
    ax32.axvline(Tvar[4], linestyle = '--', c = 'k', zorder = 2)
    
    ax33 = fig.add_axes([0.815, 0.29, 0.08, 0.19])
    ax33.plot(Tvar, Tmean_s**(1/2)*Tmean_diff, marker = 'o', c = '#0072b2')
    ax33.axvline(Tvar[4], linestyle = '--', c = 'k', zorder = 2)
    
    ax3.set_xlabel(r'$\nu$')
    ax3.set_ylabel(r'$\langle ntop \rangle  \nu$')
    ax3.yaxis.set_label_coords(-0.15, 0.5) 
    ax3.xaxis.set_label_coords(0.5, -0.2) 
    ax32.set_xlabel(r'$\nu$')
    ax32.set_ylabel(r'$\langle s \rangle^{1/2} \langle \delta z \rangle$')
    ax33.set_xlabel(r'$\nu$', fontsize = 12)
    ax33.set_ylabel(r'$\langle s \rangle^{1/2} \langle \delta z \rangle$', fontsize = 12)
    ax33.set_xlim(-0.01,0.07)
    ax33.set_ylim(0.48, 0.75)
    
    ax33.xaxis.labelpad = 1
    ax3.spines['top'].set_visible(True)
    ax3.spines['right'].set_visible(True)
    ax32.spines['top'].set_visible(True)
    ax32.spines['right'].set_visible(True)
    ax33.spines['top'].set_visible(True)
    ax33.spines['right'].set_visible(True)

    ################## Exposant ##################
    ax4 = fig.add_subplot(gs[0, 2])  # La figure 1 prend toute la largeur (2 colonnes)
    ax4.scatter(Tvar, -Ttau, c = colors, zorder=2, marker = 'o')
    ax4.plot(Tvar, -Ttau, linestyle = '-', c = 'k', zorder=1)       
    ax4.axvline(Tvar[4], linestyle = '--', c = 'k', label = '{}'.format(Tvar[3]))
    ax4.set_xlim(-0.05,0.7) 
    ax4.xaxis.set_visible(False)
    ax4.set_xlabel(r'$\nu$')
    ax4.set_ylabel(r'$\tau$')
    ax4.yaxis.set_label_coords(-0.18, 0.5) 
    ax4.spines['top'].set_visible(True)
    ax4.spines['right'].set_visible(True)

    ax42 = fig.add_axes([0.58, 0.51, 0.072, 0.16])
    ax42.plot(Tvar, -Ttau, linestyle = '-', c = 'k', zorder=1)
    ax42.scatter(Tvar, -Ttau, c = colors, zorder=2, marker = 'o')
    ax42.axvline(Tvar[4], linestyle = '--', c = 'k', label = '{}'.format(Tvar[3]))
    ax42.set_xlim(-0.01,0.07)
    ax42.set_ylim(1.1,1.7)

    ax42.xaxis.set_label_coords(-0.5, 1.5) 
    ax42.xaxis.set_ticks_position('top')
    ax42.set_xlabel(r'$\nu$', fontsize = 12)
    ax42.set_ylabel(r'$\tau$', fontsize = 12)
    ax42.spines['top'].set_visible(True)
    ax42.spines['right'].set_visible(True)
    ax42.tick_params(labelsize = 8)
    

    ################## <s> <dz> ##################
    ax5 = fig.add_subplot(gs[0, 3])

    ax5.scatter(Tvar, Tmean_s**(1/2), c = colors, zorder=2, marker = 'o')
    ax5.plot(Tvar, Tmean_s**(1/2), c = 'k', zorder=1)

    ax52 = ax5.twinx()
    ax52.plot(Tvar, Tmean_diff, marker = 'o', c = '#0072b2')
    ax52.axvline(Tvar[4], linestyle = '--', c = 'k', label = '{}'.format(Tvar[3]), zorder = 2)

    ax53 = fig.add_axes([0.845, 0.585, 0.05, 0.11])
    ax53.plot(Tvar, Tmean_diff, marker = 'o', c = '#0072b2')
    ax53.axvline(Tvar[4], linestyle = '--', c = 'k', label = '{}'.format(Tvar[3]), zorder = 2)
    
    ax5.yaxis.labelpad = -1
    ax5.set_xlabel(r'$\nu$')
    ax5.set_ylabel(r'$\langle s \rangle ^{1/2}}$')
    ax5.set_yscale('log')
    ax5.xaxis.set_visible(False)
    ax52.set_xlabel('')
    ax52.set_ylabel(r'$\langle \delta z \rangle$')
    ax52.spines['top'].set_visible(True)
    ax52.spines['right'].set_visible(True)

    
    ax53.set_xlabel(r'$\nu$', fontsize = 12)
    ax53.set_ylabel(r'$\langle \delta z \rangle $', fontsize = 12)
    ax53.set_yscale('log')
    ax53.set_xlim(-0.01,0.07)
    ax53.set_ylim(2e-2,1e-1)
    ax53.yaxis.set_label_coords(-0.2, 0.5) 
    ax53.xaxis.set_label_coords(0.5, 1.65) 
    ax53.tick_params(labelsize = 8)
    ax53.yaxis.set_minor_formatter(plt.NullFormatter())
    ax53.xaxis.set_ticks_position('top')
    ax53.spines['top'].set_visible(True)
    ax53.spines['right'].set_visible(True)
    
    fig.text(0.09, 0.85, "a", fontsize=16, fontweight="bold") 
    fig.text(0.43, 0.85, "b", fontsize=16, fontweight="bold")  
    fig.text(0.67, 0.85, "d", fontsize=16, fontweight="bold") 
    fig.text(0.43, 0.45, "c", fontsize=16, fontweight="bold")
    fig.text(0.67, 0.45, "e", fontsize=16, fontweight="bold")
    
    fig.patch.set_alpha(0.0)
    plt.savefig("Fig2_nature.pdf", bbox_inches='tight', dpi = 300)
# plt.show()