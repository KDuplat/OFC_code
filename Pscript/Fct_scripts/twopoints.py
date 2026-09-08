#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri May 13 15:58:50 2022

@author: oramos
"""

import numpy as np


def twopoints(f1,nmin,nmax,lsq1):
    f1=f1[nmin:nmax,:]
    
    out1=np.zeros((2,2))
    
    nmax2=f1.shape[0]
    #d=f1.shape[1]
    
    out1[0,0]=f1[0,0]
    out1[1,0]=f1[nmax2-1,0]
    out1[0,1]=out1[0,0]*lsq1[0]+lsq1[1]
    out1[1,1]=out1[1,0]*lsq1[0]+lsq1[1]
    
    return out1
    