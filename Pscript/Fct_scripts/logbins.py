#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri May 13 14:58:39 2022

@author: oramos
"""
import numpy as np
import math

def logbins(dist1,nbins, xmax):
    
    dist2=np.zeros((nbins,2))
    
    nmax=dist1.shape[0]
    n=nmax-1
    while n>xmax or dist1[n]==0: # take out the indicies with values > xmax or == 0
        n=n-1
    nmax=n
    step1=math.log(nmax+1,10)/nbins #length of a bin

    for x in range(nbins):
        x1=10**(x*step1)
        x2=10**((x+1)*step1)
        dist2[x,0]=10**((2*x)*step1/2)
        count1=0
        for k in range(nmax):
           if k>=x1 and k<x2:
               dist2[x,1]+=dist1[k]
               count1=count1+1 # normalise the bin by its width

        dist2[x,1]=dist2[x,1]/count1 #rescale the new bin
        """ dist2[x,1]=dist2[x,1]/(x2 - x1) #rescale the new bin """
    
    return dist2        
        
def logbins2(dist1,nbins):
    
    lin_bins = np.arange(0, len(dist1))
    log_histo = np.zeros(nbins)
    log_bins = np.logspace(np.log10(1), np.log10(len(dist1)), nbins + 1)
    
    for i in range(len(log_bins) - 1):
        #Find overlapping bins
        overlap = (lin_bins >= log_bins[i]) & (lin_bins < log_bins[i + 1])
        
        if len(dist1[overlap])!=0:
            log_histo[i] = np.sum(dist1[overlap])/len(dist1[overlap]) #rescale each bins
    
    bin_widths = np.diff(log_bins)  # Largeur de chaque bin logarithmique
    area = np.sum(log_histo * bin_widths)  # Calcul de l'aire sous la courbe
    log_histo_normalized = log_histo / area  # Normalisation

    dist2 = np.column_stack(((log_bins[:-1] + log_bins[1:])/2, log_histo_normalized))
    return log_bins, dist2
    
    
def new_logbins(histo, nbins):

    log_bins = np.logspace(np.log10(1), np.log10(len(histo)), nbins + 1)
    log_histo = np.zeros(nbins)
    lin_bins = np.arange(0, len(histo))

    norm_data = histo/np.sum(histo)
    for i in range(len(log_bins) - 1): #Take the lin histo and turn it into a log histo
        # Trouver les bins linéaires qui chevauchent le bin logarithmique courant
        overlap = (lin_bins >= log_bins[i]) & (lin_bins < log_bins[i + 1])
        # Ajouter les comptages de ces bins linéaires au bin logarithmique
        if len(norm_data[overlap])!=0:
            log_histo[i] = np.sum(norm_data[overlap])/len(norm_data[overlap])
        
    return log_bins, log_histo