#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri May 13 15:27:50 2022

@author: oramos
"""

import numpy as np
import math

def leastsquares(f1,nmin,nmax):
    f1=f1[nmin:nmax,:]
    
    
    out1=np.zeros(4)
    
    nmax=f1.shape[0]
    #d=f1.shape[1]
    
    sumX=0; sumY=0; sumXY=0; sumX2=0
    for n in range(nmax):
        sumY=sumY+f1[n,1]
        sumX=sumX+f1[n,0]
        sumXY=sumXY+f1[n,1]*f1[n,0]
        sumX2=sumX2+f1[n,0]*f1[n,0]
    
    aveY=sumY/nmax
    aveX=sumX/nmax
    out1[0]=(nmax*sumXY-sumX*sumY)/(nmax*sumX2-sumX*sumX)
    out1[1]=(sumY*sumX2-sumX*sumXY)/(nmax*sumX2-sumX*sumX)
    
    sXX=0; sYY=0; sXY=0
    
    for n in range(nmax):
        sXX=sXX+(f1[n,0]-aveX)*(f1[n,0]-aveX)
        sYY=sYY+(f1[n,1]-aveY)*(f1[n,1]-aveY)
        sXY=sXY+(f1[n,0]-aveX)*(f1[n,1]-aveY)
    
    var2=math.sqrt((sYY-out1[0]*sXY)/(nmax-2))
    
    out1[2]=var2/math.sqrt(sXX)
    out1[3]=var2*math.sqrt((1/nmax)+(aveX*aveX/sXX))

    return out1