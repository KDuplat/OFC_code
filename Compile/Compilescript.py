#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Sep 19 10:17:24 2023

@author: Kilian
"""
#%%
import datetime
import os
import subprocess
import numpy as np
from pathlib import Path
from fct import *
#%%
nu = [0.6]
L1 = [64]*len(nu)
L2 = [256]*len(nu)

zmmode = 4   #Threshold mode     0: [0,1];   1: uniform;   3: Gauss;   4: 1
zmode = 1   #z initialisation mode  0: 0;   1: [0,1];    2:[0,0.9]   3; [0, zm] 
zmpar = [0.0] #standard deviation
initmode = 1    #0: Nav = 0; 1: Start from a previous setup
bcmode = 1     #Boundarie condition mode   0: Open;    1:Periodic;    2: Reflective
navmax = [10000000]*len(nu)  #Maximum number of avalanches
PDF = [1, 1e5]    #SwitchPDF, nbdata for PDF
noise_switch = 0  #Switch for noise
noise = [0,65535, 100, 1000] #Mode, nb_noise, nb loop, nb ava in a loop    Mode 0: epicenter is kept; Mode 1: epicenter can change
fractal_switch = 0
fractal_switch_boxlist = 0

randseed = [987]
snapshot = [[0, 1e17, 1e7, 2e7]]*len(nu)  #switch: 0 take log scale snapshot, 1: Use the initial time and the delta; Initial time to take snapshot; Delta between two snapshot; Last snaphot 
timedelta = [1e6]*len(nu)    #Number of avalanche to calculate the avg avalanche time
savedelta = [1e6]*len(nu)    #Number of avalanche to save the system state
shapetime = 10 #Number of configuration to see their shape
propagation_switch = 1  #Switch to save some avalanche propagation
propagation_nbava = 10   #Nb of propa we want to save 

#CORRELATION
#Switch to save  correlation function. 
#1 save a list of correlations function at different time. 2 save the avg correlation since the beginning
correlation_switch = 1
#Start at ini_ava. We compute the correlation every delta_ava. On a sample of nb_ava BEFORE ini_ava we average the correlation every delta_avg. 
#nb_ava/ delta_ava gives the number of configuration we average on.  
#delta ava must be > nbava
ini_ava = [1e6]
nb_ava = [10000]
delta_avg = [100] 
#Correlation  : ini ava, delta ava, nb ava, delta_avg, end_ava_to_take_corr
correlation = np.array([[ini_ava[i], ini_ava[i], nb_ava[i], delta_avg[i], navmax[i]] for i in range(len(nu))]) 
            
correlnbl = [3, 5]  # Correlation: nb of position to calculate correlation, number of line to make the average left and right !!!!!!MUST BE ODD!!!!!!!!!
#COMPACT DATA AND LIMIT OF THE TRANSIANT STATE
#Switch to take compact histogram, lim transiant state, delta time transient histo, delta time sationnary histo.
compact_data =  [[1, 0, 10000, 5000]]*len(nu)
compact_switch_pos_histo = 0

#To take snapshots of the system for 3 differents class of avalanches 1, prediction_size_th[0]<s<prediction_size_th[1], s>prediction_size_th[1]
prediction_switch = 0
prediction_meantime = [0]*len(nu) #Mean time bewteen two N/10 ava.
prediction_nb_snap = [10]*len(nu)
prediction_size_th = [[int(L1[i]*L2[i]/100), int(L1[i]*L2[i]/10)] for i in range(len(nu))]

#To search for the maximal avalanche in configurations
Random_switch = 0 #Switch to take random avalanche and do random topplings on it
Random_sw_histo = 0 #Switch to take histo of s* for each config
Random_delta = [8]*len(nu)  #Energy between two configuration
Random_nbtry = [6553]*len(nu) #Nb of try on a configuration
Random_nbava = [1e8]*len(nu) #Nombre de configuration essayé.


patch_switch = 0
patch = np.array([[100, 100, Random_delta[i],  savedelta[i]/10] for i in range(len(nu))]) #bins area, bins size, time to analyse structure, delta save histo

codeversion = "v16_5"
projectname = "Test"
objectif = "Mise à jour du code"



#%% No need to touch the following lines to change parameters
renice = 19

nu_full = []
navmax_full = []
correlation_full = []
compact_full = []
snapshot_full = []
zmpar_full = []
for i in range(len(nu)):
    for _ in range(len(zmpar)) :
        nu_full.append(nu[i])
        navmax_full.append(navmax[i])
        correlation_full.append(correlation[i])
        compact_full.append(compact_data[i])
        snapshot_full.append(snapshot[i])
    zmpar_full.extend(zmpar)


if(correlnbl[1] % 2 == 0):
    raise ValueError("correlnbl[1] is not odd")


indice = projectname[:2]
indiceexec = 1
indicen = 1

nbfile = 0

nbfile = nbfiledef(L1, nbfile)
nbfile = nbfiledef(L2, nbfile)
nbfile = nbfiledef(nu_full, nbfile)
nbfile = nbfiledef(zmpar_full, nbfile)
nbfile = nbfiledef(randseed, nbfile)
    

resizetab(L1, nbfile)
resizetab(L2, nbfile)
resizetab(nu_full, nbfile)
resizetab(zmpar_full, nbfile)
resizetab(randseed, nbfile)
resizetab(navmax_full, nbfile)
    
#%%

path = Path(os.path.abspath(__file__))
parentpath = path.parent.absolute()
progpath = path.parent.parent.absolute()
rootpath = path.parent.parent.parent.parent.absolute()

execpath = f"{progpath}/Exec"
killpath = f"{progpath}/Kill"
""" databrutpath = f"{rootpath}/Data/Brut" """
databrutpath = "/data1/DATA_Duplat/Brut"
dataparapath = f"{rootpath}/Data/Parameter"


brutprojectpath = f"{databrutpath}/{projectname}"
paraprojectpath = f"{dataparapath}/{projectname}"
execprojectpath = f"{execpath}/{projectname}"
killprojectpath = f"{killpath}/{projectname}"

if not os.path.exists(f"{brutprojectpath}") and not os.path.isdir(f"{brutprojectpath}"):
    os.mkdir(brutprojectpath)
if not os.path.exists(f"{paraprojectpath}") and not os.path.isdir(f"{paraprojectpath}"):
    os.mkdir(paraprojectpath)
if not os.path.exists(f"{execprojectpath}") and not os.path.isdir(f"{execprojectpath}"):
    os.mkdir(execprojectpath)
if not os.path.exists(f"{killprojectpath}") and not os.path.isdir(f"{killprojectpath}"):
    os.mkdir(killprojectpath)
    
while( os.path.exists(f"{execprojectpath}/{indice}{indiceexec}.sh")):
    indiceexec += 1

firstcomp = 0

for i in range(nbfile):
    while (os.path.exists(f"{brutprojectpath}/{indice}{indiceexec}n{indicen}") and os.path.isdir(f"{brutprojectpath}/{indice}{indiceexec}n{indicen}")):
        indicen += 1
        
    name = f"{indice}{indiceexec}n{indicen}"

    paraname = f"{paraprojectpath}/{name}.par"
    directory = f"{brutprojectpath}/{name}"
    
    parameter = open(paraname, "w")
    
    parameter.write("#     # Physical parameters\n")
    parameter.write(f"{L1[i]}   # L1     :lattice size x1\n")
    parameter.write(f"{L2[i]}   # L2     :lattice size x2\n")
    parameter.write(f"{nu_full[i]}   # nu    : dissipation\n")
    parameter.write(f"{zmmode}   #zmmode     :mode   renewing threshold\n")
    parameter.write(f"{zmode}   #zmode     :mode   initializing  z value\n")
    parameter.write(f"{zmpar_full[i]}   #zmpar  : parameter ...\n")
    parameter.write(f"{initmode}    #initmode: initialization mode\n")
    parameter.write(f"{bcmode}    #bcmode: boundary condition mode\n")
    parameter.write(f"{navmax_full[i]}   #navmax     :maximum number of avalanches\n")
    parameter.write(f"{PDF[0]}  {PDF[1]}   #PDF: switch, Nb data for PDF\n")
    parameter.write(f"{noise_switch}    #Switch to add noise\n")
    parameter.write(f"{noise[0]}  {noise[1]}  {noise[2]} {noise[3]}    #Mode, nb_noise, nb loop, nb ava in a loop\n")
    parameter.write(f"{fractal_switch}  {fractal_switch_boxlist}      #Switch fractal dimension, boxlist\n" )
    parameter.write(f"#\n")
    parameter.write("#     # Numerical parameters\n")
    parameter.write("#     #rac and path: racine and path of directory where results are written\n")
    parameter.write(f"*     /Para_used\n")
    parameter.write(f"*     {directory}\n")
    parameter.write(f"{randseed[i]}   # randseed      : numero of seed for random number generator\n")
    parameter.write(f"{snapshot_full[i][0]}  {snapshot_full[i][1]}  {snapshot_full[i][2]}  {snapshot_full[i][3]}     #Snapshot    : mode, initava, delta, endava\n")
    parameter.write(f"{timedelta[i]:.0e}   #Nb of avalanches to calculate the avg avalanche time\n")
    parameter.write(f"{savedelta[i]:.0e}   #Nb of avalanches to save the system state\n")
    parameter.write(f"{shapetime}    #Nb of avalanches where we want to see their shape\n")
    parameter.write(f"{propagation_switch}  {propagation_nbava}    #Switch, Nb of propagation to save\n")
    
    parameter.write(f"{prediction_switch}    #Switch for prediction.\n")
    parameter.write(f"{prediction_meantime[i]}    #Mean time bewteen two N/100 ava divided by 2.\n")
    parameter.write(f"{prediction_nb_snap[i]}    #Nb of snap per class.\n")
    parameter.write(f"{prediction_size_th[i][0]}  {prediction_size_th[i][1]}   #Size threshold for medium and large avalanche.\n")

    parameter.write(f"{correlation_switch}    #Switch for correlation.\n")
    parameter.write(f"{correlation_full[i][0]}  {correlation_full[i][1]}  {correlation_full[i][2]}  {correlation_full[i][3]}    #Correlation  : ini ava, delta ava, nb ava, delta avg\n")
    parameter.write(f"{correlnbl[0]}  {correlnbl[1]}    # Correlation: nb of position, nb of line to avg\n")
    parameter.write(f"{correlation[i][4]}  # Correlation    : end ava\n")
    
    parameter.write(f"{compact_data[i][0]}  {compact_data[i][1]}  {compact_data[i][2]}  {compact_data[i][3]}  #Switch to take compact histogram, lim transiant state, delta time transient histo, delta time sationnary histo\n")
    parameter.write(f"{compact_switch_pos_histo}  #Switch to take compact histogram as function of the distance to boundary\n")
    
    parameter.write(f"{Random_switch}  {Random_delta[i]} #Random switch, energy between two avalanches\n")
    parameter.write(f"{Random_nbtry[i]} #Nb of try on a configuration\n")
    parameter.write(f"{Random_nbava[i]} #Nb of configuration tested\n")
    parameter.write(f"{Random_sw_histo} #Switch to take histo of s* for each config\n")
    
    parameter.write(f"{patch_switch} #Patches switch to analyse snapshots\n")
    parameter.write(f"{patch[i][0]}  {patch[i][1]}  {patch[i][2]}  {patch[i][3]} #Patch bins area, bins size, delta time analyse, delta time save histo\n")
    parameter.close()

    os.mkdir(directory)
    dirparacode = f"{directory}/INFODATA.txt"
    codevpath = f"{progpath}/Code/{codeversion}"
    codeinfo = open(dirparacode, "w")
    codeinfo.write(f"parameter file used: {paraname}\n")
    codeinfo.write(f"code version used: {codevpath}\n")
    codeinfo.write(f"Objectif: {objectif}\n")
    codeinfo.write(f"Date: {datetime.datetime.now()}")
    codeinfo.close()
    print(f"/******{name} FOLDER CREATED******/\n")
    
    if firstcomp == 0:
        commandmake = f"make -f {codevpath}/makefile -C {codevpath} ava"
        terminal = subprocess.run(commandmake, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        
        print(f"Make Output terminal: {terminal.stdout}\n")
        print(f"Make Error output terminal: {terminal.stderr}\n")
    
    
    if "error" not in terminal.stderr.lower():
        #commandexec=f"nice -n -19 {codevpath}/ava {paraname} &\n"
        commandexec = f"{codevpath}/ava {paraname} &\n"
        execfile = open(f"{execprojectpath}/{indice}{indiceexec}.sh", 'a')
        """ killfile = open(f"{killprojectpath}/{indice}{indiceexec}.sh", 'a') """
        if firstcomp == 0:
            execfile.write("#!/bin/bash\n\n")
            """ killfile.write("#!/bin/bash\n\n") """
            execfile.write(f'echo "#!/bin/bash" > {killprojectpath}/{indice}{indiceexec}.sh\necho >> {killprojectpath}/{indice}{indiceexec}.sh\n' )
            firstcomp += 1
            
        execfile.write(f"{commandexec}")
        execfile.write("PID=$!\n")
        execfile.write(f"renice {renice} -p $PID &\n")
        execfile.write(f'echo "kill $PID\n" >> {killprojectpath}/{indice}{indiceexec}.sh\necho >> {killprojectpath}/{indice}{indiceexec}.sh\n')
        execfile.close()
