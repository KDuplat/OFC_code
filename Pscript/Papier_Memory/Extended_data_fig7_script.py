#%%
import numpy as np
import matplotlib.pyplot as plt
import sciplotlib.style as spstyle
from scipy.optimize import curve_fit

# %pip install SciencePlots

def magnitude(energy):
    return (2/3) * np.log10(energy) - 6.07

def energy(magnitude):
    return 10 ** (1.5 * (magnitude + 6.07))

# def expfct(log_x, a, b):
#     return a + b * log_x

def expfct(x, a, b):
    return a * np.exp(-b * x)

#%% Paramètres d'énergie
Emax = energy(8.9)  # MagMax=9.1
Emin = energy(0)          # MagMin=6.07
#Etot = 6.5297e+22 / 40 
Etot = 6.5297e+22 * 0.37 / 40         # Total yearly energy excluding tohoku 
print('Etot: ', Etot)


#tau_values = np.array([(n + 7) / 10 for n in range(18)])
tau_values = np.array([0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.67, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2])
# Init array
nbins = 9

ntot = len(tau_values)
Sum001 = np.zeros(ntot)

line01x = np.zeros((ntot, nbins))
line01y = np.zeros((ntot, nbins))

norm_line01y = np.zeros((ntot, nbins))

mean_energy = np.zeros(ntot)
mean_energy_y = np.zeros(ntot)
norm_mean_energy_y = np.zeros(ntot)

magnitude_y = np.zeros((ntot, nbins))
mag_norm_y = np.zeros((ntot, nbins))
mean_magnitude_x = np.zeros(ntot)
mean_magnitude_y = np.zeros(ntot)

cdf_mag = np.zeros((ntot, nbins+1))

P_Energy = np.zeros((ntot, nbins))
Rate_All = np.zeros((ntot, nbins))

P_energy8 = np.zeros(ntot)
P_energy0 = np.zeros(ntot)
P_energy3 = np.zeros(ntot)

Rate8 = np.zeros(ntot)
Rate0 = np.zeros(ntot)
Rate3 = np.zeros(ntot)

Ntot = np.zeros(ntot)
Etot2 = np.zeros(ntot)

energy_edges = energy(np.linspace(0, 8.9, nbins+1))
energy_center = np.array(10 ** (1.5 * (np.linspace(0,7.9,nbins) + 0.5 + 6.07)))
magnitude_x = np.linspace(0,7.9,nbins) + 0.5

tau_EQ = 1.67
k_orig_ref8 = Etot * (2 - tau_EQ) / (Emax ** (2 - tau_EQ) - Emin ** (2 - tau_EQ))
N_ref8 = k_orig_ref8 * ( (energy(8.9)**(1-tau_EQ)) - (energy(8)**(1-tau_EQ)) )/(1 - tau_EQ)
N_ref_tot = k_orig_ref8 * ( (energy(8.9)**(1-tau_EQ)) - (energy(0)**(1-tau_EQ)) )/(1 - tau_EQ)
E_ref8 = k_orig_ref8 * ( (energy(8.9)**(2-tau_EQ)) - (energy(8)**(2-tau_EQ)) )/(2 - tau_EQ)
E_ref_tot = k_orig_ref8 * ( (energy(8.9)**(2-tau_EQ)) - (energy(0)**(2-tau_EQ)) )/(2 - tau_EQ) #Recalc Etot 

N_tot_ref=9183139 #nb EQ

print("E tot recalc", E_ref_tot)
print("N tot recalc", N_ref_tot)
# print(1/N_ref8)
T_ref8 = 13.64 #Time between two T8 = 1/N_ref8
Rate_ref8 = 1/T_ref8


for n in range(ntot):
    tau = tau_values[n]
    if tau == 2:
        tau = 2.0000001  # éviter division par zéro
        tau_values[n] = tau

    #Without normalisation
    line01x[n] = energy_center
    E = energy_center
    dE = energy_edges[1:] - energy_edges[:-1]
    
    k_orig = N_tot_ref / np.sum(energy_center**(-tau) * dE) #constant Activity Ntot
    #k_orig = Etot / np.sum(energy_center**(1-tau) * dE) #Remplace le developpement de l'intégrale pour calculer le coefficient
    line01y[n] = k_orig * energy_center ** (-tau) 
    N = line01y[n]
    
    #With normalisation
    norm = np.sum(N*dE) 
    norm_line01y[n] = N/norm
    
    mean_energy[n] = np.sum(energy_center * norm_line01y[n] * dE)
    mean_energy_y[n] = k_orig * mean_energy[n] ** (-tau)
    
    
    #Magnitude, largeur des bins = 1
    magnitude_y[n] = N * 1.5 * np.log(10) * E
    mag_norm_y[n] = magnitude_y[n]/np.sum(magnitude_y[n]) 
    
    #CDF Magnitude
    N_edge = k_orig * energy_edges ** (-tau) 
    cdf_mag[n] = np.cumsum(N_edge * 1.5 * np.log(10) * energy_edges)
    print(np.max(cdf_mag[n])-cdf_mag[n])
    
    """ mean_magnitude_x[n] = np.sum(magnitude_x * mag_norm_y[n])# Moyenne des magnitude """
    mean_magnitude_x[n]= magnitude(mean_energy[n]) #Moyenne des energies en magnitude

    
    Emean_mag = energy(mean_magnitude_x[n])
    yE_mag = k_orig * Emean_mag ** (-tau)
    mean_magnitude_y[n] = yE_mag * 1.5 * np.log(10) * Emean_mag
    
    """ check_norm_mag = np.sum(mag_norm_y[n])
    print("Tes norm mag: ", check_norm_mag) """
    #Pourcentage d'énergie comptenu dans une gamme de magnitude
   
    for k in range(nbins):       
        P_Energy[n,k] = np.sum(E[k] * dE[k] * N[k])/np.sum(E*N*dE)
        Rate_All[n,k] = 1/(N[k] * dE[k])
   
    P_energy8[n] = np.sum(E[-1] * dE[-1] * N[-1])/np.sum(E*N*dE)
    P_energy0[n] = np.sum(E[0:1] * dE[0:1] * N[0:1])/np.sum(E*N*dE)
    P_energy3[n] = np.sum(E[7:8] * dE[7:8] * N[7:8])/np.sum(E*N*dE)
   
    Ntot[n] = np.sum(N * dE)
    Etot2[n] = np.sum(N * dE * E)
    #print(Ntot)
    # Ntot = k_orig * ( (energy(10)**(1.00000001-tau)) - (energy(0)**(1.00000001-tau)) )/(1.00000001 - tau)
    # print(Ntot)
    N8 = N[-1] * dE[-1]
    N0 = N[0:1] * dE[0:1]
    N3 = N[7:8] * dE[7:8]
    
    Rate8[n]=1/N8
    Rate0[n]=1/N0
    Rate3[n]=1/N3
    

# print(mean_energy)
# for n in range(ntot):
#     if tau_values[n] == 1.67:
#         n_eq=n
#         MeanE_EQ=mean_energy[n]
#         print(mean_magnitude_x[n])
        
# print (mean_energy[n_eq-7]/mean_energy[n_eq])        
# print (1/(mean_energy[n_eq+2]/mean_energy[n_eq+1]))  
# print (Rate_All[n_eq ,8])  
# print (Rate_All[n_eq ,0]*365.25*24*60*60)   
# print (1/Rate_All[n_eq-7 ,8])    
# print (Rate_All[n_eq-7 ,8]*365.25*24*60*60)                     
# print (Rate_All[n_eq+3 ,8]) 
# print (P_Energy[n_eq+4 ,8]) 
# print(mean_magnitude_x[n_eq])

#%%


color_list = ['#000000', '#e69f00', '#56b4e9', '#009e73',
              '#f0e442', '#0072b2', '#d55e00', '#cc79a7']

NCURVES = len(tau_values)
colors = [plt.cm.get_cmap('viridis')(i/NCURVES) for i in range(NCURVES)]

with plt.style.context(spstyle.get_style('nature-reviews')):
    print(spstyle.get_style('nature-reviews'))
    plt.rcParams['font.family'] = 'Arial' 
    
    """ fig, axs = plt.subplots(2,2, figsize = (7.82, 5.6))
    ax1 = axs[0, 0]
    ax2 = axs[0, 1]
    ax3 = axs[1, 0]
    ax4 = axs[1, 1]
    ax32 = ax3.twinx()

    for n in range(1,ntot, 1):
        if tau_values[n] in [2.0000001, 1.5, 1]:
            ax1.plot(line01x[n], line01y[n], '-.o', label=f'tau={tau_values[n]:.2f}', c = 'b')
            ax2.plot(magnitude_x, magnitude_y[n], '-.o', label=f'tau={tau_values[n]:.2f}', c = 'b')
            
        elif tau_values[n] == 1.67:
            ax1.plot(line01x[n], line01y[n], '-.o', label=f'tau={tau_values[n]:.2f}', c = 'r')
            ax2.plot(magnitude_x, magnitude_y[n], '-.o', label=f'tau={tau_values[n]:.2f}', c = 'r')
        else:
            ax1.plot(line01x[n], line01y[n], '-.o', c = colors[n], linewidth=1)
            ax2.plot(magnitude_x, magnitude_y[n], '-.o', c = colors[n], linewidth=1)
            
            
        ax1.plot(mean_energy[n], mean_energy_y[n], marker = 'd', c = color_list[3])  # point moyen
        ax2.plot(mean_magnitude_x[n], mean_magnitude_y[n], marker = 'd', c = color_list[3])

    mask = np.argwhere(np.isin(tau_values, [2.0000001, 1.5, 1]))
    ax32.plot(tau_values[1::1], mean_magnitude_x[1::1], c = color_list[5], marker = 'o', ls = '--')
    ax32.scatter(tau_values[tau_values==1.67], mean_magnitude_x[tau_values==1.67], c = 'r', zorder = 10)
    ax32.scatter(tau_values[mask], mean_magnitude_x[mask], c = 'b', zorder = 10)

    ax3.plot(tau_values[1::1], mean_energy[1::1], c = color_list[1], marker = 'd', ls = '--')

    ax4.plot(tau_values[1::1], P_energy8[1::1]*100, c = color_list[5], marker = 'o', ls = '--')
    ax4.plot(tau_values[1::1], P_energy0[1::1]*100, c = color_list[1], marker = 'o',ls = '--')
    
    ax4.scatter(tau_values[mask], P_energy8[mask]*100, marker = 'o', c = 'b', zorder = 10)
    ax4.scatter(tau_values[tau_values==1.67], P_energy8[tau_values==1.67]*100, marker = 'o', c = 'r', zorder = 10)
    
    ax4.scatter(tau_values[mask], P_energy0[mask]*100, marker = 'd', c = 'b', zorder = 10)
    ax4.scatter(tau_values[tau_values==1.67], P_energy0[tau_values==1.67]*100, marker = 'd', c = 'r', zorder = 10)
    #ax62.plot(tau_values[1::1], P_Energy[1::1,:]*100, c = color_list[5],ls = '--')


    ax1.set_xlabel('Energy (J)')
    ax1.set_ylabel(r'$P(E)$')
    ax1.set_xscale('log')
    ax1.set_yscale('log')

    ax2.set_xlabel('Magnitude')
    ax2.set_ylabel('Number of Earthquakes / year')
    ax2.set_yscale('log')

    ax3.set_xlabel(r'$\tau$')
    ax3.set_ylabel(r'<$E$>')
    ax3.set_yscale('log')
    ax3.spines['top'].set_visible(True)
    ax3.spines['right'].set_visible(True)
    # ax32.set_ylabel('<M>')
    #ax32.set_yscale('log')
    ax32.set_ylabel('<Magnitude>')

    
    ax4.set_xlabel(r'$\tau$')
    ax4.set_ylabel(r'$\%E$')
    #ax4.set_yscale('log')
    # ax4.spines['top'].set_visible(True)
    # ax4.spines['right'].set_visible(True)
    #ax62.set_yscale('log')
    
    fig.text(0.018, 0.98, "a", fontsize=16, fontweight="bold") 
    fig.text(0.528, 0.98, "b", fontsize=16, fontweight="bold")
    fig.text(0.018, 0.48, "c", fontsize=16, fontweight="bold") 
    fig.text(0.528, 0.48, "d", fontsize=16, fontweight="bold")
    
    fig.patch.set_alpha(0.0)
    plt.tight_layout()
    plt.savefig('Ext_data_fig7.pdf', bbox_inches = 'tight', dpi = 300)"""

    
    fig, (ax, ax2) = plt.subplots(1,2, figsize = (8.2, 2.8))
    for n in range(1,ntot, 1):
        inv_cdf = np.max(cdf_mag[n]) - cdf_mag[n]
        mask = inv_cdf !=0
        if tau_values[n] in [2.0000001, 1.5, 1]:
            ax.plot(magnitude_x, inv_cdf[mask], '-.o', c = 'b') 
        elif tau_values[n] == 1.67:
            ax.plot(magnitude_x, inv_cdf[mask], '-.o', c = 'r')
        else:
            ax.plot(magnitude_x, inv_cdf[mask], '-.o', c = colors[n]) 
            
    
        popt1, _ = curve_fit(expfct, magnitude_x, magnitude_y[n])
        # ax2.scatter(tau_values[n], popt[1], c = 'b')
        # ax.plot(magnitude_x, magnitude_y[n], '-.o', label=f'tau={tau_values[n]:.2f}', c = 'k')
        popt2, _ = curve_fit(expfct, magnitude_x, inv_cdf[mask])
        # ax.plot(magnitude_x, expfct(magnitude_x, *popt), '--', label=f'tau={tau_values[n]:.2f}', c = 'r')
        print(popt1)
        ax2.scatter(popt1[1], popt2[1], c = 'r')
        
        
    ax.set_xlabel("Magnitude")
    ax.set_ylabel("CDF")
    ax.set_yscale('log')
    ax2.set_xlabel(r"$\tau$")
    ax2.set_ylabel(r"$\tau_{cdf}$")
    # ax2.set_yscale('log')

    plt.show()



# %%
