from Import import *
from Signal_analysis_fct import *
         
""" def corrfct(folderpath, deltaava, iniava, L1, L2, nbava, corrlen, tcorrel = [-1], nl = 1):
    
    L22 = int(L2/2)
    f = open(f"{folderpath}/tf_outputB.txt", "r")
    tcorrelp = tcorrel.copy()
    
    NCURVES = len(tcorrel) + 2 
    cmap = plt.cm.get_cmap('YlOrBr')
    colors = [cmap(1 - i / NCURVES) for i in range(NCURVES)]
    c = 0
    clenmax = 0
    
    if(tcorrelp[0] == -1):
        tcorrelp[0] = nbava
    
    corrlen[corrlen[:, 0] <= 15] = 15
    y = [] 
    
    for i in range(nl):
        y.append(int(L1 * (i + 1) / nl))
        
    corrdata = np.zeros((L22, nl))
    x = np.arange(L22)

    plt.figure(figsize=(7,7)) 
    spec = gridspec.GridSpec(ncols=nl, nrows=1, width_ratios=[1]*nl)
    
    j = 0
    time = iniava
    for line in f :
        if not line.startswith('#') and not len(line.strip()) == 0:
            if time in tcorrelp:
                col = line.strip().split(" ")
                for l in range(nl):
                    corrdata[j][l] = float(col[l])
                    
            j += 1 
            
            if j == L22:  
                if time in tcorrelp:
                    print("test")
                    corrdata[(corrdata[:] < -1) | (corrdata[:] > 1)] = 0 # il y a des erreurs dans les valeur de correlation à ce temps donc je les ai corrigé pour les mettre à 0
                    i = int((time - iniava)/deltaava)
                    clen = int(corrlen[i,0])
                    if clen > clenmax : clenmax = clen 
                    for k in range(nl):
                        if corrdata[2,k] >= 0.1:
                            popt, pcov = curve_fit(expfct, x[2:clen], corrdata[2:clen, k])
                            a, b = popt
                            print(a, b)
                            y_fit = expfct(x[2:clen], a, b)
                            if (1/b < 0 or 1/b> L2):
                                b = 1
                        else : 
                            b = 1
                
                    
                    fig, ax1 = plt.subplots(figsize=(10,10))
                    ax1.plot(x[:], corrdata[:], label=f"{time:.1e} avalanches", color = colors[c], linewidth = 2)
                    if corrdata[2] >= 0.1: ax1.plot(x[2:clen], y_fit, color = 'red', label = f'exp fit: '+ r'$\epsilon$' + f' = {1/b:.2f}')
                    
                    ax1.set_xlabel("y", fontsize = 30) 
                    ax1.set_ylabel("$C_{x}(y)$", fontsize = 30, rotation = 0, labelpad=20)
                    ax1.tick_params(labelsize = 28,length = 8, width = 2)
                    ax1.tick_params(labelsize = 28,length = 5, width = 1, which = 'minor')
                    ax1.set_xlim(0, int(L2/2))
                    ax1.legend(fontsize = 25)
                    
                    left, bottom, width, height = [.58, 0.45, 0.3, 0.3]
                    ax2 = fig.add_axes([left, bottom, width, height])
                    ax2.plot(x[:], corrdata[:], color = colors[c], linewidth = 2)
                    ax2.scatter(clen, corrdata[clen, k], color = 'red', s = 50)
                    if corrdata[2] >= 0.1: ax2.plot(x[2:clen], y_fit, color = 'red', linewidth = 2)
                    ax2.tick_params(labelsize = 28,length = 8, width = 2)
                    ax2.tick_params(labelsize = 28,length = 5, width = 1, which = 'minor')
                    ax2.set_xlim(0,math.ceil(clen*1.5))
                    ax2.set_ylim(5e-3,)
                    ax2.set_yscale('log')
                    
                    left, bottom, width, height = [0.2, 0.45, 0.3, 0.3]
                    ax3 = fig.add_axes([left, bottom, width, height])
                    ax3.plot(x[:], corrdata[:], color = colors[c], linewidth = 2)
                    ax3.scatter(clen, corrdata[clen, k], color = 'red', s = 50)
                    ax3.tick_params(labelsize = 28,length = 8, width = 2)
                    ax3.tick_params(labelsize = 28,length = 5, width = 1, which = 'minor')  
                    ax3.set_xlim(0,5)
                    ax3.set_ylim(-0.2,1)
                    
                    c += 1
                j = 0
                time = time + deltaava
    plt.show()
    f.close() """
    
def corrlength(folderpath, iniava, deltaava, nbava, L2, numcol = -4,  switch_plot = 1): #Plot the correlation lenght through time
    #Take the correlation length for each time and plot it. 
    #The correlation length is found during the simulation by looking at the noise in the second half of the correlation, 
    #taking the maximum of thus noise and looking for the first point under this noise.
    #The position is noted as the correlation length
    npoints = int((nbava - iniava) / deltaava ) + 1
    clen = np.zeros((npoints, 2))
    avg = np.zeros((npoints))
    valavg = 0
    imin = 0 #To make the average computation start on a time different than iniava
    i = 0
    time = iniava
    # print(npoints)
    # indices = []
    with open(f'{folderpath}/tflen_outputB.txt', "r") as f:
        for line in f :
            if not line.startswith('#'):
                col = line.strip().split("  ")
                try:
                    clen[i][0] = int(col[numcol])
                    clen[i][-1] = time
                except IndexError:
                    print(i)
                
                """ if time < 10**8:
                    if time % 10**7 == 0:
                        indices.append(i)
                elif time < 10**9:
                    if time % 10**8 == 0:
                        indices.append(i)
                else:
                    if time % 10**9 == 0:
                        indices.append(i) """
                
                i += 1
                
                if (i > imin):
                    valavg += int(col[numcol])
                    avg[i - imin - 1] = valavg/(i - imin)
                time += deltaava
                
                if time == nbava + deltaava: #Allow to stop the function on the time wanted (nbava)
                    break
    if time < nbava: 
        clen = clen[:i, :]
        avg = avg[:i]     
        
    """ mvavglog = moving_avg(clen[indices, 0], 20) """
    """ mvavg = moving_avg(clen[:, 0], 100) """
    xnew, ynew = moving_average(clen[imin:, 1], avg[:])
    if switch_plot:
        
        plt.figure(figsize=(10,10)) 
        plt.plot(clen[:, 1], clen[:, 0], lw = 5, label = 'data')
        plt.plot(clen[imin:, 1], avg[:], lw = 4 , color = "r", label = 'avg')
        plt.plot(xnew, ynew, lw = 4 , color = "green", label = 'avg_smooth')
        """ plt.plot(clen[:, 1], mvavg[:], color = 'orange', linewidth = 2, label = 'moving avg') """
        """  plt.plot(clen[indices, 1], mvavglog[:], color = 'green', linewidth = 2, label ='moving avg log') """
        plt.xlabel("$N_{ava}$", fontsize = 30)
        plt.ylabel(r'$\xi$', fontsize = 30, rotation = 0)
        plt.tick_params(labelsize = 28,length = 8, width = 2)
        plt.tick_params(labelsize = 28,length = 5, width = 1, which = 'minor') 
        plt.xscale("log")   
        plt.ylim(0,L2/4) 
        plt.show()
    
    return clen, avg

def corrlengthfit(folderpath, L2, iniava, deltaava, corrlen, numcol = -4, switch_plot = 1):
    # Take the correlation function at each time and fit it with an exponential if possible. Else is set to 1 (need modification)
    # The coorelation length is found by looking at the correlation function and fitting the cf by an exponential from x = 1 to x = clen[i]
    # with clen[i] bying the first position under the noise found by corrlength()
    time = iniava
    i = 0
    x2 = 0
    L22 = L2/2
    
    corrdata = np.zeros(int(L2/2))
    clenfit = np.zeros((len(corrlen),2))
    avgfit = np.zeros(len(corrlen))
    avg_smooth = np.zeros(0)
    valavg = 0
    x = np.arange(L22)
    indices = []

    with open(f'{folderpath}/tf_outputB.txt', 'r') as f:
        for line in f :
            if not line.startswith('#') and not len(line.strip()) == 0:
                col = line.strip().split(" ")
                corrdata[x2] = float(col[numcol])    
                
                x2 += 1 
                
                
                if x2 == L2/2: 

                    if corrdata[2] >= 0.1:
                        if corrlen[i,0] < 4 :
                            #popt, _ = curve_fit(expfct2, x[2:5], np.log10(corrdata[2:5]))
                            b = -1
                        else:
                            try:
                                popt, _ = curve_fit(expfct2, x[3:int(corrlen[i,0])+1], np.log10(abs(corrdata[3:int(corrlen[i,0])+1]))) #it overflows sometimes but still give normal values NEED FIX
                                a, b = popt
                                # print(a, -1/b)
                            except ValueError:
                                print(corrdata[3:int(corrlen[i,0])+1])
                                raise
                        if ( -1/b < 0 or -1/b > L2):
                            """ print(-1/b)
                            fig, ax1 = plt.subplots(figsize=(10,10))
                            ax1.plot(x[:], corrdata[:], label=f"{time:.1e} avalanches", c = 'C0')
                            yfit = 10**expfct2(x[2:int(corrlen[i,0])+1], *popt)
                            ax1.plot(x[2:int(corrlen[i,0])+1],yfit,  c = 'r')
                            ax1.set_xlabel("y", fontsize = 30) 
                            ax1.set_ylabel("$C_{x}(y)$", fontsize = 30, rotation = 0, labelpad=20)
                            ax1.tick_params(labelsize = 28,length = 8, width = 2)
                            ax1.tick_params(labelsize = 28,length = 5, width = 1, which = 'minor')
                            ax1.set_xlim(0, int(L2/2))
                            ax1.set_yscale('log')
                            ax1.legend(fontsize = 25)
                            fig.show() """
                            b = -1
                            
                        clenfit[i,1] = -1/b
                        valavg += -1/b
                        # avgfit[i] = valavg / (i + 1)
                        avgfit[i] = valavg 
                        
                    else:
                        clenfit[i,1] = 0
                        # valavg += 1
                        avgfit[i] = avgfit[i-1]
                    clenfit[i,0] = time
                    
                    if time < (iniava*10):
                        if time % iniava == 0:
                            indices.append(i)
                    elif time < (iniava*100):
                        if time % (iniava*10) == 0:
                            indices.append(i)
                    else:
                        if time % (iniava*100) == 0:
                            indices.append(i)
                    
                    # avgfit[i] = valavg / (i + 1)
                    x2 = 0
                    i += 1
                    time = time + deltaava
                    
                    if i == len(corrlen):
                        break    
                    """ if time == 8290000000:
                        print(time)
                        print(corrdata)
                        fig, ax1 = plt.subplots(figsize=(10,10))
                        ax1.plot(x[:], corrdata[:], label=f"{time:.1e} avalanches", c = 'C0')
                        yfit = 10**expfct2(x[2:int(corrlen[i,0])+1], *popt)
                        ax1.plot(x[2:int(corrlen[i,0])+1],yfit,  c = 'r')
                        # if corrdata[2] >= 0.1: 
                        #     ax1.plot(x[2:clen], y_fit, color = 'red', label = f'exp fit: '+ r'$\epsilon$' + f' = {1/b:.2f}')
                        ax1.set_xlabel("y", fontsize = 30) 
                        ax1.set_ylabel("$C_{x}(y)$", fontsize = 30, rotation = 0, labelpad=20)
                        ax1.tick_params(labelsize = 28,length = 8, width = 2)
                        ax1.tick_params(labelsize = 28,length = 5, width = 1, which = 'minor')
                        ax1.set_xlim(0, int(L2/2))
                        ax1.legend(fontsize = 25)
                        fig.show()
                        print(time)
                        return 0,0,0 """
    mask = clenfit[:, 1] != 1
    filt_clenfit = clenfit[mask]
    filt_avgfit = avgfit[mask]
    filt_avgfit /= np.arange(1,len(filt_avgfit)+1)

    xnew, ynew = moving_average(filt_clenfit[:, 0], filt_avgfit[:])
    avg_smooth = np.column_stack((xnew, ynew))
    
    if switch_plot:
        # mvavglog = moving_avg(clenfit[indices, 1], 15)
        """ mvavg = moving_avg(clenfit[:, 1], 200) """                
        plt.figure(figsize=(10,10)) 
        plt.plot(filt_clenfit[:, 0], filt_clenfit[:, 1], linewidth = 3, label = 'data')
        plt.plot(filt_clenfit[:, 0], filt_avgfit[:], color = "r", linewidth = 3, label = 'avg')
        """plt.plot(xnew, ynew, lw = 4 , color = "green", label = 'avg_smooth')"""
        """ plt.plot(clenfit[:, 0], mvavg[:], color = "orange", linewidth = 3, label = 'moving avg') """
        """ plt.plot(clenfit[indices, 0], mvavglog[:], color = "green", linewidth = 2, label = 'moving avg log') """
        
        plt.xlabel("$N_{ava}$", fontsize = 30)
        plt.ylabel(r'$\xi$', fontsize = 30, rotation = 0, labelpad = 30)
        plt.xscale("log")
        plt.tick_params(labelsize = 28,length = 8, width = 2)
        plt.tick_params(labelsize = 28,length = 5, width = 1, which = 'minor')    
        plt.ylim(0,L2)
        plt.legend(fontsize = 35)
        fig = plt.gcf()
        fig.patch.set_alpha(0.0)
        plt.show()
    
    return filt_clenfit, filt_avgfit, avg_smooth
    
def corrlenlast(folderpath): #Give the correlation lenght at the end of the simulations
    with open(f'{folderpath}/tflen_outputB.txt', 'rb') as f:
        return readlast(f)    
    
def meancorrfct(folderpath,  deltaava, iniava, L1, L2, nbava, corrlen, startmean, tcorrel = [-1], numcol = -1, xmin_fit = 3, switch_plot = 1): #Fait une moyenne temporelle des correlations depuis startmean jusqu'à la fin
    #Fonctionne que si tcorrelp = -1
    L22 = int(L2/2)
    tcorrelp = tcorrel.copy()
    
    NCURVES = len(tcorrel) + 2 
    cmap = plt.cm.get_cmap('YlOrBr')
    colors = [cmap(1 - i / NCURVES) for i in range(NCURVES)]
    c = 0
    
    b = -1

    
    if(tcorrelp[0] == -1):
        tcorrelp[0] = nbava
    
    # corrlen[corrlen[:] <= 15] = 15

    corrdata = np.zeros(L22)
    x = np.arange(L22)
    
    plt.figure(figsize=(7,7)) 
    
    j = 0
    time = iniava
    with open(f"{folderpath}/tf_outputB.txt", "r") as f:
        for line in f :
            if not line.startswith('#') and not len(line.strip()) == 0:
                if time >= startmean:
                    col = line.strip().split(" ")
                    corrdata[j] += float(col[numcol])
                j += 1 
                
                if j == L22:
                    if np.any(np.isclose(np.array(tcorrelp) - deltaava, time)): #Check if the time is in tcorrelp (it is the beggning time of the correlation)
                        corrdata = corrdata/((nbava - startmean) / deltaava) #Average over the time
                        
                        """ corrdata[(corrdata[:] < -1) | (corrdata[:] > 1)] = 0 """ # il y a des erreurs dans les valeur de correlation à certain temps donc je les corrigé en les mettant à 0
                        # i = int((time - iniava)/deltaava)
                        
                        maxvar = 0
                        clen = int(L22/2)
                        for val in corrdata[int(L22/2):]: # Find the max noise
                            if maxvar < abs(val):
                                maxvar = abs(val)
                        
                        for i, val in enumerate(corrdata[2:int(L22/2)+1]): # When it finds the first value under the max noise it stops
                            if val < maxvar:
                                clen = i + 2
                                break
                        
                        # clen = int(L22) - 1
                        if corrdata[2] >= 0.1: # make an exponential fit
                            popt, pcov = curve_fit(expfct, x[xmin_fit:clen], corrdata[xmin_fit:clen])
                            a, b = popt
                            """ print(a, b) """
                            y_fit = expfct(x[xmin_fit:clen], a, b)
                        else : 
                            b = 1
                        
                        # if corrdata[2] >= 0.1:
                        #     popt, pcov = curve_fit(expfct, x[2:L22], corrdata[2:L22])
                        #     a, b = popt
                        #     """ print(a, b) """
                        #     y_fit = expfct(x[3:L22], a, b)
                        # else : 
                        #     b = 1
                        
                        """ fit2 = [expfct(xval, a,b) for xval in x] """
                        
                        if switch_plot == 1:
                            fig, ax1 = plt.subplots(figsize=(10,10))
                            ax1.plot(x[:], corrdata[:], label=f"{time:.1e} avalanches", color = colors[c], linewidth = 4)
                            """ ax1.plot(x[:], fit2[:],  color = 'green', linestyle = '--', linewidth = 2) """
                            if corrdata[2] >= 0.1: ax1.plot(x[xmin_fit:clen], y_fit, color = 'red', label = f'exp fit: '+ r'$\xi$' + f' = {1/b:.2f}', lw = 2)
                            
                            ax1.set_xlabel("y", fontsize = 30) 
                            ax1.set_ylabel("$C_{x}(y)$", fontsize = 30, rotation = 0, labelpad=20)
                            ax1.tick_params(labelsize = 28,length = 8, width = 2)
                            ax1.tick_params(labelsize = 28,length = 5, width = 1, which = 'minor')
                            ax1.set_xlim(0, int(L2/2))
                            ax1.legend(fontsize = 25)
                            
                            left, bottom, width, height = [.58, 0.45, 0.3, 0.3]
                            ax2 = fig.add_axes([left, bottom, width, height])
                            ax2.plot(x[:], corrdata[:], color = colors[c], linewidth = 2)
                            ax2.scatter(clen, corrdata[clen], color = 'red', s = 50)
                            if corrdata[2] >= 0.1: ax2.plot(x[xmin_fit:clen], y_fit, color = 'red', linewidth = 2)
                            ax2.tick_params(labelsize = 28,length = 8, width = 2)
                            ax2.tick_params(labelsize = 28,length = 5, width = 1, which = 'minor')
                            ax2.set_xlim(0,math.ceil(clen*1.5))
                            ax2.set_ylim(1e-3,)
                            ax2.set_yscale('log')
                            
                            left, bottom, width, height = [0.2, 0.45, 0.3, 0.3]
                            ax3 = fig.add_axes([left, bottom, width, height])
                            ax3.plot(x[:], corrdata[:], color = colors[c], linewidth = 3)
                            ax3.scatter(clen, corrdata[clen], color = 'red', s = 50)
                            """ f corrdata[2] >= 0.1: ax3.plot(x[2:clen], y_fit, color = 'red', linewidth = 2) """
                            ax3.tick_params(labelsize = 28,length = 8, width = 2)
                            ax3.tick_params(labelsize = 28,length = 5, width = 1, which = 'minor')  
                            ax3.set_xlim(0,5)
                            ax3.set_ylim(-0.2,1)
                            
                            c += 1
                    j = 0
                    time = time + deltaava
                    if time == nbava:
                        break
    if switch_plot == 1:
        plt.show()
    """ print(f'{1/b:.2f}') """
    plt.close()
    return 1/b      
    