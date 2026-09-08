#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu May 19 10:14:08 2022

@author: oramos
"""

import numpy as np
import math
import matplotlib.pyplot as plt
from matplotlib import gridspec

from leastsquares import leastsquares
from logbins import logbins
from twopoints import twopoints 


nbins=12

#alpha=[ 14,22, 24, 247, 25]
#ncifras=[100,100,100, 1000,100]
#dim=[2,2,2,2,2]
alpha=[ 14,22, 24, 247, 25, 14,22, 24, 247, 25]
ncifras=[100,100,100, 1000,100, 100,100,100, 1000,100]
dim=[2,2,2,2,2, 2,2,2,2,2]


nalpha=len(alpha)
lenghtTab=20*317**2

d1=np.zeros((lenghtTab,nalpha))

a='Distribution_0{}'
b='/cur/AvalancheA_output.txt'
c='/cur/AvalancheB_output.txt'

a2='/home/oramos/DUPLAT/Stage_master/OFC_SIM_Simp/Vcst(100000)/resultsG{}'
b2='/dist_ava_sites.dat'
#name2='AvalancheA_output.txt'
#name3='AvalancheB_output.txt'


j=0
for k in range(int(nalpha/2)):
    name=a.format(alpha[k])+b
    avalanche_size=np.loadtxt(globals()['name'])[:,1]
    
    i=0
    for x in avalanche_size:
        d1[int(x),j]+=1
        i+=1
    j+=1

for k in range(int(nalpha/2)):
    name=a.format(alpha[k])+c
    avalanche_size=np.loadtxt(globals()['name'])[:,1]
    
    i=0
    for x in avalanche_size:
        d1[int(x),j]+=1
        i+=1
    j+=1    
    
"""
for k in range(int(nalpha/2)):
    i=0
    name=a2.format(alpha[k])+b2
    f=open(name,'r')
    for x in f:
        d1[i,j]=x
        i+=1
    j+=1
"""
    

ext1=np.zeros(nalpha)
max_ava=np.zeros(nalpha)
disip=np.zeros(nalpha)

for i in range (nalpha):
    ext1[i]=lenghtTab-1

#ext1[7]=45000;
"""ext1[0]=4000; ext1[1]=3500; ext1[4]=3500; ext1[5]=3500; ext1[6]=4000; ext1[7]=3500; ext1[8]=3500; ext1[9]=3500; """

suma1=np.zeros(nalpha)
meanvalue1=np.zeros(nalpha)
meanvalue_calcul=np.zeros(nalpha)
    

for x in range(nalpha):
    disip[x]=100*(1-dim[x]*2*alpha[x]/ncifras[x])
    for y in range(lenghtTab):
        suma1[x]=suma1[x]+d1[y,x] # Compte le nombre total d'avalanche

for x in range(nalpha):
    for y in range(lenghtTab):
        d1[y,x]=d1[y,x]/suma1[x] #Normalise l'histograme
        meanvalue1[x]=meanvalue1[x]+(y+1)*d1[y,x] # Calcule la valeur moyenne des avalaches

##-------------------------------------------------------------------  
#%%
        
ndist=np.zeros((nalpha,nbins, 2))
ndistloglog=np.zeros((nalpha,nbins,2))
nstat=np.zeros((4,nalpha))
ndatafit=np.zeros((nalpha,2,2))
nmin=np.zeros(nalpha)
nmax=np.zeros(nalpha)


#fitting control
for i in range (nalpha):
    nmin[i]=1
    nmax[i]=8
#nmin[0]=3
"""nmin[0]=4
nmin[1]=4
nmin[2]=4
nmin[3]=4


nmax[0]=nbins-1
nmax[1]=nbins-1
nmax[2]=nbins-1
nmax[3]=nbins-1
nmax[10]=nbins-1
nmax[9]=nbins-4
nmax[8]=nbins-4
nmax[7]=nbins-4"""
#nmax[9]=7
#nmax[2]=8
#nmax[3]=8

    
for j in range(nalpha):
    
    ndist [j,:,:]=logbins(d1[:,j],nbins, ext1[j])   # logbins of P(s)    
    
    for x in range(nbins):
        for y in range(2):
            if ndist[j,x,y]==0:
                ndistloglog[j,x,y]=ndistloglog[j,x-1,y]
            else:
                ndistloglog[j,x,y]=math.log(ndist[j,x,y],10)
            
    nstat[:,j]=leastsquares(ndistloglog[j,:,:],int (nmin[j]),int (nmax[j]))                #best linear fit
    ndatafit[j,:,:]=twopoints(ndistloglog[j,:,:],int(nmin[j]),int(nmax[j]),nstat[:,j])   #fit line
    
#ERROR BARS------------------------
"""
ndisterror=np.zeros((nalpha,nbins, 2))
ndistloglogerror=np.zeros((nalpha,nbins,2))
nstaterrormin=np.zeros((4,nalpha))
nstaterrormax=np.zeros((4,nalpha))
errordown,errorup=np.zeros(4),np.zeros(4)

    
errorfit=np.array([-1,1])
for index,value in enumerate(errorfit):
    nmin2=np.zeros(nalpha)
    nmax2=np.zeros(nalpha) 
    
    for i in range(nalpha):
        nmin2[i]=nmin[i]+value
    for j in range(nalpha):
        ndisterror [j,:,:]=logbins(d1[:,j],nbins, ext1[j])   # logbins of P(s)
    
        for x in range(nbins):
            for y in range(2):
                ndistloglogerror[j,x,y]=math.log(ndisterror[j,x,y],10)
            
        nstaterrormin[:,j]=leastsquares(ndistloglogerror[j,:,:],int (nmin2[j]),int (nmax[j]))  
        if (nstaterrormin[0,j]-nstat[0,j])<errordown[j]:
            errordown[j]=abs(nstaterrormin[0,j]-nstat[0,j])
        elif (nstaterrormin[0,j]-nstat[0,j])>errorup[j]:
            errorup[j]=abs(nstaterrormin[0,j]-nstat[0,j])

    for i in range(nalpha):
        nmax2[i]=nmax[i]+value
    for j in range(nalpha):
        ndisterror [j,:,:]=logbins(d1[:,j],nbins, ext1[j])   # logbins of P(s)
    
        for x in range(nbins):
            for y in range(2):
                ndistloglogerror[j,x,y]=math.log(ndisterror[j,x,y],10)
            
        nstaterrormax[:,j]=leastsquares(ndistloglogerror[j,:,:],int (nmin[j]),int (nmax2[j])) 
        if (nstaterrormax[0,j]-nstat[0,j])<errordown[j]:
            errordown[j]=abs(nstaterrormax[0,j]-nstat[0,j])
        if (nstaterrormax[0,j]-nstat[0,j])>errorup[j]:
            errorup[j]=abs(nstaterrormax[0,j]-nstat[0,j])               #best linear fit


error=np.zeros((4,2))

for j in range(nalpha):
    error[j,0]=errordown[j]
    error[j,1]=errorup[j]
"""
#%%

fig=plt.figure(figsize=(10,5))

spec = gridspec.GridSpec(ncols=2, nrows=1, width_ratios=[2, 1])

ax1=fig.add_subplot(spec[0])

s0=1
markers1=['d','s','^','o','*','d','s','^','o','*'];

for j in range(nalpha): 
    
    symbols1=markers1[j]+'k-'
    ax1.plot(ndistloglog[j,:,0],ndistloglog[j,:,1],symbols1,linewidth=0.5,markersize=4,mec="k", mfc="none") 
    if j>4:
        symbols1=markers1[j]+'g-'
        ax1.plot(ndistloglog[j,:,0],ndistloglog[j,:,1],symbols1,linewidth=0.5,markersize=4,mec="g", mfc="none") 
    ax1.plot(ndatafit[j,:,0],ndatafit[j,:,1],'r-',linewidth=0.5)
    

ax1.plot(3.2,-0.7,'*g', mfc="none",linewidth=0.5)
ax1.text(3.4,-0.8, '0.25', fontsize=14)
ax1.plot(3.2,-1.4,'ok', mfc="none",linewidth=0.5)
ax1.text(3.4,-1.5, '0.247', fontsize=14)
ax1.plot(3.2,-2.1,'^k', mfc="none",linewidth=0.5)
ax1.text(3.4,-2.2, '0.24', fontsize=14)
ax1.plot(3.2,-2.8,'sk', mfc="none",linewidth=0.5)
ax1.text(3.4,-2.9, '0.22', fontsize=14)
ax1.plot(3.2,-3.4,'dk', mfc="none",linewidth=0.5)
ax1.text(3.4,-3.5, '0.14', fontsize=14)

    
ax1.axis([0, 5, -12, 0])
ax1.set_xlabel('Log [ s(number of sites) ]', fontsize=18)
ax1.set_ylabel('Log [ P(s) ]', fontsize=18)
ax1.tick_params(labelsize=12) 

nstaterror=np.zeros((4,nalpha))
ax2=fig.add_subplot(1,2,2)
s1=20

for i in range(nalpha):
    if (i<5):
        ax2.scatter(disip[i],(-1)*nstat[0,i],c='k',marker='o')
    else:
        ax2.scatter(disip[i],(-1)*nstat[0,i],c='g',marker='o')
    

#ax2.plot(dim[0],(-1)*nstat[0,0],'*k',s1, mfc="none",linewidth=0.5)
#ax2.plot(dim[1],(-1)*nstat[0,1],'sk',s1, mfc="none",linewidth=0.5)
#ax2.plot(dim[2],(-1)*nstat[0,2],'dk',s1, mfc="none",linewidth=0.5)
#ax2.plot(dim[3],(-1)*nstat[0,3],'og',s1, mfc="none",linewidth=0.5)



#ax2.errorbar(dim[0], (-1)*nstat[0,0], yerr=[[error[0,0]],[error[0,1]]],fmt='*',ecolor = 'red',color='k',mfc="none")
#ax2.errorbar(dim[1], (-1)*nstat[0,1], yerr=[[error[1,0]],[error[1,1]]],fmt='s',ecolor = 'red',color='k',mfc="none")
#ax2.errorbar(dim[2], (-1)*nstat[0,2], yerr=[[error[2,0]],[error[2,1]]],fmt='d',ecolor = 'red',color='k',mfc="none")
#ax2.errorbar(dim[3], (-1)*nstat[0,3], yerr=[[error[3,0]],[error[3,1]]],fmt='o',ecolor = 'red',color='green',mfc="none")

ax2.axis([-1, 51, 1.0, 3])
ax2.set_xlabel('Dissipation(%)', fontsize=15, )
ax2.set_ylabel('b', fontsize=15)
ax2.tick_params(labelsize=14)
ax2.grid(True)


plt.savefig('CriticalSlope.eps')
