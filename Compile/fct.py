#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Sep 19 14:57:39 2023

@author: oramos
"""

import subprocess
import concurrent.futures

def nbfiledef(tab1, nbfile):
    if (len(tab1)>nbfile):
         nbfile=len(tab1)
    return nbfile
        
def resizetab(tab1, nbfile):
    while(len(tab1)!=nbfile):
        tab1.append(tab1[0])
        
            
def executer_command(command):
    terminal=subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    return( command, terminal)