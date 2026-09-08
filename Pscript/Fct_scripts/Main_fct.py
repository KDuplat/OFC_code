from Import import *

def mainfct(histo, lengthTab, nbins, nmin = 1, nmax = 10):
    
    ext1 = lengthTab-1
    d1 = histo[:]
    d1 = d1 / np.sum(d1) 
        
    ndist = np.zeros((nbins, 2))
    ndistloglog = np.zeros((nbins,2))
    nstat = np.zeros((4))
    ndatafit = np.zeros((2,2))

    ndist = logbins(d1, nbins, ext1)   # logbins of P(s)    
    ndistbis = ndist[~np.isnan(ndist[:, 1])] #Delete bins with nan
    
    rratio = int(nbins/len(ndistbis)) if (int(nbins/len(ndistbis))!=0) else 1 #rescale ratio
    
    for x in range(len(ndistbis)):
        for y in range(2):
            if ndistbis[x,y] == 0:
                ndistloglog[x,y] = ndistloglog[x-1,y]
            else:
                ndistloglog[x,y] = math.log(ndistbis[x,y],10)
        
    nstat = leastsquares(ndistloglog, int(nmin/rratio), int(nmax/rratio))                #best linear fit
    ndatafit = twopoints(ndistloglog, int(nmin/rratio) , int(nmax/rratio), nstat)   #fit line
    
    return ndatafit, ndistloglog, nstat

""" def process_folder(folder, nmin, nmax, nbins, L1, L2,col, limdata):
    histo = np.zeros((L1 * L2))
    Lengthtab = L1 * L2
    nblines = 0
    path =' '
    
    if os.path.exists(f'{folder}/Avalanche_outputB.gz'):
        path = f'{folder}/Avalanche_outputB.gz'
    elif os.path.exists(f'{folder}/Avalanche_outputB.txt'):
        path = f'{folder}/Avalanche_outputB.txt'
        
    for chunk in chunked_file_reader(path, chunksize=1e7, usecols=[col]):
        histo += np.histogram(chunk.iloc[:, 0], bins=np.arange(Lengthtab + 1))[0]
        nblines += len(chunk)
        
        if nblines >= limdata:
            break
    
    nonzero_indices = np.nonzero(histo)[0] # Sort tous les bins != 0
    smax = nonzero_indices[-1]
    
    ndatafit, ndistloglog, nstat = mainfct(histo, Lengthtab, nbins, nmin, nmax)
    Tndist = 10 ** ndistloglog
    Tndatafit = 10 ** ndatafit
    Tnstat = nstat[0]

    return Tndist, Tndatafit, Tnstat, smax """

""" def PDF(folderpath, L1, L2, nbins, Tvar, Tswitch, Tnmin, Tnmax, switchplot = 1, col = 1, limdata = 1e8): # PDF for Avalanche_outputB.gz size for multiple folder
    
    if switchplot == 1:
        plt.figure(figsize=(20,10))
        ax1 = plt.subplot(1,2,1)
        ax2 = plt.subplot(1,2,2)
        
    NCURVES = len(folderpath)
    cmap = plt.cm.get_cmap('inferno')
    colors = [cmap(i / NCURVES) for i in range(NCURVES)]
    
    Tndist=np.zeros((len(folderpath),nbins, 2))
    Tnstat=np.zeros(len(folderpath))
    Tndatafit=np.zeros((len(folderpath),2,2))
    Tsmax=np.zeros(len(folderpath))
        
    results = Parallel(n_jobs=-1)(delayed(process_folder)(folder, nmin, nmax, nbins, L1, L2, col, limdata)
                                  for (folder, nmin, nmax) in zip(folderpath, Tnmin, Tnmax))


    for j, (Tndist_j, Tndatafit_j, Tnstat_j, Tsmax_j) in enumerate(results):
        Tndist[j] = Tndist_j
        Tndatafit[j] = Tndatafit_j
        Tnstat[j] = Tnstat_j    
        Tsmax[j] = Tsmax_j
        
        if switchplot == 1:
            if Tswitch[j] == 0:
                ax1.plot(Tndist[j, :,0][Tndist[j, :,1] != 1],Tndist[j, :,1][Tndist[j, :,1] != 1], color = colors[j], linewidth=4, markersize=6, mfc="none", label = f"{Tvar[j]}\nb = {-Tnstat[j]:.2f}") 
                ax1.plot(Tndatafit[j,:, 0], Tndatafit[j, :,1], color = 'k', linestyle = '--', linewidth = 3)
                
                ax2.scatter(Tvar[j], -Tnstat[j], color = colors[j], s = 120)
            elif Tswitch[j] == 1:
                ndist2 = Tndist[~np.isnan(Tndist[j, :,1])]
                init_guess = [1700, 7.5,0.29]
                popt, _ = curve_fit(extexp, ndist2[[0,7,-1],0],ndist2[[0,7,-1],1], p0 = init_guess, maxfev = int(1e6))
                a, b, d = popt
                y_fit = extexp(ndist2[:,0], *popt)
                
                ax1.plot(Tndist[j, :,0],Tndist[j, :,1], color = colors[j], linewidth=4, markersize=6,mec="k", mfc="none", label = f"{Tvar[j]}\n" +
                                                                                                                            rf'b = {b:.2f}, $\tau_s$ = {1/d:.2f}')
                ax1.plot(ndist2[:,0], y_fit, color = 'b', linestyle = '--', linewidth = 4)
                ax2.scatter(Tvar[j], -Tnstat[j], color = colors[j], s = 120)
            elif Tswitch[j] == 2:
                ax1.plot(Tndist[j, :,0],Tndist[j, :,1], color = colors[j], linewidth=4, markersize=6,mec="k", mfc="none", label = f"{Tvar[j]}")
                ax2.scatter(Tvar[j], -Tnstat[j], color = colors[j], s = 120)
            else: 
                ax1.plot(Tndist[j, :,0][Tndist[j, :,1] != 1],Tndist[j, :,1][Tndist[j, :,1] != 1], color = colors[j], linewidth=4, markersize=6,mec="k", mfc="none", label = "None")
            
        
            
        
    if switchplot == 1:         
        ax1.set_xscale("log")
        ax1.set_yscale("log")
        ax1.set_xlabel('s', fontsize=30)
        ax1.set_ylabel('P(s) ', fontsize=30, rotation = 0, labelpad = 30)
        ax1.tick_params(labelsize = 28,length = 8, width = 2)
        ax1.tick_params(labelsize = 28,length = 5, width = 1, which = 'minor')
        # plt.legend(title = f'{var}', fontsize = 23, title_fontsize = 23)
        
        ax2.set_xlabel(r'$\nu$', fontsize=30)
        ax2.set_ylabel(r'$\tau$', fontsize=30, rotation = 0, labelpad = 30)
        ax2.tick_params(labelsize = 28,length = 8, width = 2)
        ax2.tick_params(labelsize = 28,length = 5, width = 1, which = 'minor')
        plt.show()
    
    return Tndist, Tndatafit, Tnstat, Tsmax  """
 
""" def process_size_from_histo(folder, Lengthtab, nmin = 1, nmax = 50):
    data = np.loadtxt(f"{folder}/stat_histosize.txt", comments = "#")
    
    last_non_zero = np.nonzero(data)[0]
    
    nbins = freedman_diaconis_log_bins(data)
    
    mean = np.sum(data * np.arange(len(data)))/np.sum(data)
    
    ndatafit, ndistloglog, nstat = mainfct(data, Lengthtab, nbins, nmin, nmax)
    Tndist = 10 ** ndistloglog
    Tndatafit = 10 ** ndatafit
    Tnstat = nstat[0]
    
    return Tndist, Tndatafit, Tnstat, last_non_zero[-1], mean, nbins
 
def PDF_s_histogram(Traw_histo, Tstart_x, Tstop, Tvar):

    filtered_Thistogram = [histo[histo[:, 1] != 1] for histo in Traw_histo]
    
    NCURVES = len(Tvar)
    Ttau_s = np.zeros(NCURVES)
    Tperr_s = np.zeros(NCURVES)
    Tana_mean = np.zeros(NCURVES)
    Txfit = []
    Tyfit = []
    
    for i, (histo, start_x, stop, nu) in enumerate(zip(filtered_Thistogram, Tstart_x, Tstop, Tvar)):
        
        if nu == 0.0:
            result, xs, _ , perr = fit_powerlaw(histo[:,0], histo[:,1], stop_x = stop, start_x = start_x, switch = 1,  plot=True, f_scale=1.0,
                                        function_to_fit='power')
        else:
            result, xs, _ , perr = fit_powerlaw(histo[:,0], histo[:,1], n_cutoff = stop, start_x = start_x, plot=True, f_scale=1.0,
                                        function_to_fit='power')
        
        Txfit.append(xs)
        Tyfit.append(powerlaw(xs, *result.x))
        Ttau_s[i] = np.round(result.x[1], 3)
        Tperr_s[i] =  np.round(perr[1], 3)
    
    return filtered_Thistogram, Txfit, Tyfit, Ttau_s, Tperr_s
  """
 
""" def process_top(folder, Lengthtab, nbins, nmin, nmax):
    data = np.loadtxt(f"{folder}/stat_histotop.txt", comments = "#")
    
    last_non_zero = np.nonzero(data)[0]
    
    mean = np.sum(data * np.arange(len(data)))/np.sum(data)
    
    ndatafit, ndistloglog, nstat = mainfct(data, Lengthtab, nbins, nmin, nmax)
    Tndist = 10 ** ndistloglog
    Tndatafit = 10 ** ndatafit
    Tnstat = nstat[0]
    
    return Tndist, Tndatafit, Tnstat, last_non_zero[-1], mean
 
def PDF_top_histogram(Traw_histo, Tstart_x, Tstop, Tvar):
    
    filtered_histo = [histo[histo[:, 1] != 1] for histo in Traw_histo]
    NCURVES = len(Traw_histo)
    Ttau_t = np.zeros(NCURVES)
    Tperr_t = np.zeros(NCURVES)
    
    Txfit = []
    Tyfit = []
    for j, (histo, start_x, stop, nu) in enumerate(zip(filtered_histo, Tstart_x, Tstop, Tvar)):
        
        if nu == 0.0:
            result, xs, _ , perr = fit_powerlaw(histo[:,0], histo[:,1], stop_x = stop, start_x = start_x, switch = 1, plot=True, f_scale=1.0,
                                        function_to_fit='power')
        else:
            result, xs, _ , perr = fit_powerlaw(histo[:,0], histo[:,1], n_cutoff = stop, start_x = start_x, plot=True, f_scale=1.0,
                                        function_to_fit='power')
            
        Ttau_t[j] = np.round(result.x[1], 3)
        Tperr_t[j] =  np.round(perr[1], 3)
        Txfit.append(xs)
        Tyfit.append(powerlaw(xs, *result.x))
        
    return filtered_histo, Txfit, Tyfit, Ttau_t, Tperr_t """
 
 
def new_PDF(folder, nbins = 0):
    data = np.loadtxt(folder, comments = "#")
    
    last_non_zero = np.nonzero(data)[0]
    nmax = last_non_zero[-1]
    if nbins == 0:
        nbins = freedman_diaconis_log_bins(data[:nmax])
        if nbins > len(data):
            nbins = len(data) -1
    if nbins > 300:
        nbins = 300
    mean = np.sum(data * np.arange(len(data)))/np.sum(data)
    bins, histo = new_logbins(data[:nmax], nbins) #Transforme les bins lin en bins log
    bin_center = (bins[1:] + bins[:-1])/2
    
    mask = histo != 0
    f_histo = histo[mask]
    f_bin_center = bin_center[mask]
    
    return f_histo, f_bin_center, nmax, mean, nbins

def PDF_fitting(Thisto, Tbins, Tstart_x, Tstop, Tvar):
    
    NCURVES = len(Tvar)
    Ttau_s = np.zeros(NCURVES)
    Tperr_s = np.zeros(NCURVES)
    Txfit = []
    Tyfit = []
    
    for i, (histo, bins, start_x, stop_x) in enumerate(zip(Thisto, Tbins, Tstart_x, Tstop)):
        
        """ if nu == 0.0:
            result, xs, ys , perr = fit_powerlaw(bins, histo, stop_x = stop, start_x = start_x, switch = 1,  plot=True, f_scale=1.0,
                                        function_to_fit='power')
        else:
            result, xs, ys , perr = fit_powerlaw(bins, histo, n_cutoff = stop, start_x = start_x, plot=True, f_scale=1.0,
                                        function_to_fit='power')
        
        Txfit.append(xs)
        Tyfit.append(powerlaw(xs, *result.x))
        Ttau_s[i] = np.round(result.x[1], 3)
        Tperr_s[i] =  np.round(perr[1], 3)
        mask = [np.where(bins == val)[0][0] for val in xs]

        r2 = r2_score(histo[mask], powerlaw(xs, *result.x)) """
        
        start = np.where(bins >= start_x)[0][0]
        stop = np.where(bins <= stop_x)[0][-1]
        popt, pcov = curve_fit(powerlaw2, np.log(bins[start:stop]), np.log(histo[start:stop]), p0 = [0.5, 1.6], maxfev = 1000)
        tau = popt[1]
        Tperr_s[i] = np.sqrt(np.diag(pcov))[1]
        Ttau_s[i] = tau
        yfit = np.exp(powerlaw2(np.log(bins[start:stop]), *popt))
        Tyfit.append(yfit)
        Txfit.append(bins[start:stop])
        
        r2 = r2_score(histo[start:stop], yfit)
        print(r2)
        
        
        
    
    return  Txfit, Tyfit, Ttau_s, Tperr_s

""" def new_process_size_from_histo(folder):
    data = np.loadtxt(f"{folder}/stat_histosize.txt", comments = "#")
    
    last_non_zero = np.nonzero(data)[0]
    nmax = last_non_zero[-1]
    nbins = freedman_diaconis_log_bins(data[:nmax])
    mean = np.sum(data * np.arange(len(data)))/np.sum(data)
    
    bins, histo = new_logbins(data[:nmax], nbins) #Transforme les bins lin en bins log
    bin_center = (bins[1:] + bins[:-1])/2
    
    mask = histo != 0
    f_histo = histo[mask]
    f_bin_center = bin_center[mask]
    
    return f_histo, f_bin_center, nmax, mean, nbins


def new_process_top_from_histo(folder, start_x, stop_x, nu):
    data = np.loadtxt(f"{folder}/stat_histotop.txt", comments = "#")
    
    last_non_zero = np.nonzero(data)[0]
    nmax = last_non_zero[-1]
    nbins = freedman_diaconis_log_bins(data[:nmax])
    mean = np.sum(data * np.arange(len(data)))/np.sum(data)
    
    bins, histo = new_logbins(data[:nmax], nbins, nmax) #Transforme les bins lin en bins log
    bin_center = (bins[1:] + bins[:-1])/2
    
    mask = histo != 0
    f_histo = histo[mask]
    f_bin_center = bin_center[mask]
    
    return f_histo, f_bin_center, last_non_zero[-1], mean, nbins """

def shapeava(folderpath, L1, L2, nu, nshape): # Plot the shape of the last avalanches
    size1 = 15
    size2 = int(size1 * L1/L2)
    plt.figure(figsize=(size1,size2)) 

    if nu == 0.0 :
        colors = iter(cm.rainbow(np.linspace(0, 1, 10 + 1)))
    else: 
        colors = iter(cm.rainbow(np.linspace(0, 1, nshape+1)))
    f = f"{folderpath}/shape_outputB.txt"
    shape = open(f, "r")

    for i, line in enumerate(shape):
        if nu == 0.0 and i < nshape - 10:
                continue
        x1, x2 = [], []
        col = line.strip().split()
        for n in range(len(col)):
            
            x1_value, x2_value = coorno(int(col[n]), L1)
            x1.append((L1-1) - x1_value) #Changed L1 to L1 -1
            x2.append(x2_value)
            
        plt.scatter(x2,x1, s = 10, color = next(colors), alpha = 0.5)
        plt.axis([0, L2, 0, L1])
    
    plt.xticks([])
    plt.yticks([])
    plt.show()
    shape.close()       

def posava(folderpath, L1, L2, nshape): # Plot the initial avalanche position
    size1 = 30
    size2 = int(size1 * L1/L2)
    plt.figure(figsize=(size1,size2)) 
            
    if os.path.exists(f'{folderpath}/Avalanche_outputB.txt'):
        fpos = np.loadtxt(f'{folderpath}/Avalanche_outputB.txt')
    else :
        fpos = read_gz_with_pandas(f'{folderpath}/Avalanche_outputB.gz')
        
    colors = iter(cm.rainbow(np.linspace(0, 1, nshape+1)))
    c = 0
    for line in fpos:
        c += 1
        if c > (len(fpos) - nshape):
            col = line.strip().split()
            x1, x2 = coorno(int(col[2]), L1)
            plt.scatter(x2,int(L1 - x1), s = 50, color = next(colors), alpha = 0.5)
    plt.axis([0, L2, 0, L1])
    plt.tick_params(labelsize=50) 
    plt.xticks([])
    plt.yticks([])
    plt.show()
    fpos.close()
    
def evoltime(folderpath): #Evolution of the simulation time (s)
    loaddata= np.loadtxt(f"{folderpath}/time_outputB.txt", comments="#")
            
    plt.figure(figsize=(15,15))
    plt.scatter(loaddata[:,0], loaddata[:,3], color = "b")
    plt.xlabel("Number of avalanges", size = 15)
    plt.ylabel("Average time each 1000 avalanches", size = 15)
    plt.yscale("log") 
    plt.gca().xaxis.get_offset_text().set_fontsize(15)
    plt.xticks(size=15)
    plt.yticks(size=15)
    plt.show()
    plt.close()
  
    
def processtime(folder, nbins, minsize, limdata):
    histo = np.zeros(nbins)
    bins = np.logspace(np.log10(10**-12), np.log10(10**2), nbins + 1)
    csize = 1e7
    nbdata = 0
    
    lasttime = 0
    last_filtered_time = 0
    
    for chunk in chunked_file_reader(f'{folder}/Avalanche_outputB.gz', chunksize=csize, usecols = [1,3]):
        
        time = np.array(np.cumsum(chunk.iloc[:, 1])) + lasttime #rebuild the time scale starting from zero using dz
        lasttime = time[-1]  #Save the time for the next chunk
        # filterd_chunk = chunk.iloc[:, 1][chunk.iloc[:, 0] >= minsize]
        
        filtered_time = time[chunk.iloc[:, 0] >= minsize] #Keep only the avalanches with a size above a threshold
        filtered_time = np.insert(filtered_time, 0, last_filtered_time) #add to the array the time of the last avalanches from the previous chunk
        last_filtered_time = filtered_time[-1]  #Keep the time of the last avalanches for the next chunk
        
        if(len(filtered_time) > 0):
            time_inter = np.diff(filtered_time)
            histo += np.histogram(time_inter, bins = bins)[0]
            
        nbdata+=len(chunk.iloc[:, 1])
        
        if nbdata >= limdata: break
        
        
    # histo = histo/nbdata
    bin_centers = (bins[:-1] + bins[1:]) / 2
    mean = np.sum(histo * bin_centers) / np.sum(histo)

    norm_histo = histo/((np.diff(bins)) * histo.sum())
    norm_histo = norm_histo * mean

    norm_bin_centers = bin_centers/mean
        
    return norm_histo, norm_bin_centers

def PDFtpsbava(folderpath, vartab, nbins, size = 2, nmin = [], nmax = [],
                     switch_plot = 1, limdata = 1e8, xlim_min = 1e-10, xlim_max = 2e2, ylim_min = 1e-10, ylim_max = 1e6): #PDF time between avalanches for multiple folders
    
    NCURVES = len(folderpath) 
    cmap = plt.cm.get_cmap('inferno')
    colors = [cmap(i / NCURVES) for i in range(NCURVES)]
    
    Tb, Tb2 = np.zeros(len(folderpath)), np.zeros(len(folderpath))
    
    histo = np.zeros((len(folderpath), nbins))
    bins = np.zeros((len(folderpath), nbins))
    
    results = Parallel(n_jobs=-1)(delayed(processtime)(folder, nbins, size, limdata) for folder in folderpath)
    for i, result in enumerate(results):
        histo[i], bins[i] = result
    
    if switch_plot == 1:
        
        plt.figure(figsize = (20,10))
        ax1 = plt.subplot(1,2,1)
        for i, (v, c) in enumerate(zip(vartab, colors)):
            ax1.plot(bins[i, :][histo[i, :] != 0], histo[i][histo[i, :] != 0] , color = c, label = f'{v}', marker = 'o', linewidth = 3)
            
            try: 
                # popt, _ = curve_fit(powerlaw2, np.log(bins[i, 11:22]), np.log(histo[i, 11:22]))
                # a,b = popt
                # yfit = np.exp(powerlaw2(np.log(bins[i, 11:22]), *popt))
                # Tb[i] = b
                """ ax1.plot(bins[i, 11:22], yfit2, linestyle = "--", color = 'k', linewidth = 3) """
                if len(nmin) == 0:
                    nmin = np.full(len(folderpath), 2)
                
                if len(nmax) == 0:
                    nmax = np.full(len(folderpath), 40)
                    
                mask = histo[i, nmin[i]:nmax[i]] != 0
                popt2, _ = curve_fit(powerlaw2, np.log(bins[i, nmin[i]:nmax[i]][mask]), np.log(histo[i, nmin[i]:nmax[i]][mask]))
                a2,b2 = popt2
                yfit2 = np.exp(powerlaw2(np.log(bins[i, nmin[i]:nmax[i]][mask]), *popt2))
                Tb2[i] = b2
                
                ax1.plot(bins[i, nmin[i]:nmax[i]][mask], yfit2, linestyle = "--", color = 'b', linewidth = 3)
            
            except ValueError:
                print(f"Error in {folderpath[i]}")
                continue
         
        
        poisson = gamma_distribution(bins[0, :], 1, -0.0, 1.)
        ax1.plot(bins[0, :], poisson, linestyle = '--', color = 'k')
        
            
        ax1.set_yscale('log')
        ax1.set_xscale('log')
        ax1.set_xlabel(r'$\frac{T}{<T>}$', fontsize = 30)
        ax1.set_ylabel(r'$P(\frac{T}{<T>})$', fontsize = 30, rotation = 0,labelpad= 40)
        ax1.tick_params(labelsize = 28,length = 8, width = 2)
        ax1.tick_params(labelsize = 28,length = 5, width = 1, which = 'minor')
        ax1.set_xlim(xlim_min, xlim_max)
        ax1.set_ylim(ylim_min, ylim_max)
        # ax1.legend(title = rf'$\nu$', fontsize = 23, title_fontsize = 23)
            
        ax2 = plt.subplot(1,2,2)
        
        """ ax2.scatter(vartab, -Tb, color  = 'k', s = 120) """
        ax2.scatter(vartab, -Tb2, color  = colors, s = 120)
        """ plt.xscale('log')
        plt.yscale('log') """
        ax2.set_xlabel(r'$\nu$', fontsize = 30)
        ax2.set_ylabel(r'$\gamma$', fontsize = 30, rotation = 0)
        ax2.tick_params(labelsize = 28,length = 8, width = 2)
        ax2.tick_params(labelsize = 28,length = 5, width = 1, which = 'minor')
    
    plt.show()
    
    return histo, bins, Tb, Tb2

def process_time_from_histo(folder, nbins, switch = 1):
    if switch == 2:
        histo = np.loadtxt(f"{folder}/stat_histoitime2.txt", comments = "#")
    else: 
        histo = np.loadtxt(f"{folder}/stat_histoitime.txt", comments = "#")
        
    bins = np.logspace(np.log10(10**-12), np.log10(2), nbins)

    bin_centers = (bins[:-1] + bins[1:]) / 2
    mean = np.sum(histo * bin_centers) / np.sum(histo) 
    norm_histo = histo/(np.diff(bins) * histo.sum())
    norm_histo = norm_histo * mean
    norm_bin_centers = bin_centers/mean

    return norm_bin_centers, norm_histo

def PDF_it_histogram(Traw_histo, Tstart_x, Tstop_x, nb_reduced_bins):
    NCURVES = len(Traw_histo) 
    Tgamma = np.zeros(NCURVES)
    Tperr = np.zeros(NCURVES)

    Tbins_reduced = []
    Thisto_reduced = []
    Txfit = []
    Tyfit = []
    
    Tbins, Thisto = Traw_histo[:, 0, :], Traw_histo[:, 1, :]
    
    for i, (bins, histo, start_x, stop_x) in enumerate(zip(Tbins, Thisto, Tstart_x, Tstop_x)):
        
        bins_reduced, histo_reduced = change_bins_log_histo(bins, histo, nb_reduced_bins)
        mask = histo_reduced!=0
        
        filtered_bins = bins_reduced[:-1][mask]
        filtered_histo = histo_reduced[mask]
        
        Tbins_reduced.append(filtered_bins)
        Thisto_reduced.append(filtered_histo)
        
        # result, xs, _ , perr = fit_powerlaw(filtered_bins, filtered_histo, stop_x = stop, start_x = start_x, f_scale=1.0,
        #                             function_to_fit='power', switch = 1)
        
        start = np.where(bins >= start_x)[0][0]
        stop = np.where(bins <= stop_x)[0][-1]
        popt, pcov = curve_fit(powerlaw2, np.log(bins[start:stop]), np.log(histo[start:stop]), p0 = [0.5, 1.6], maxfev = 1000)
        
        Tgamma[i] = popt[1]
        Tperr[i] =  np.sqrt(np.diag(pcov))[1]

        yfit = np.exp(powerlaw2(np.log(bins[start:stop]), *popt))
        Tyfit.append(yfit)
        Txfit.append(bins[start:stop])
        
    Thisto_reduced = [np.stack((bins, histo), axis=1) for histo, bins in zip(Thisto_reduced, Tbins_reduced)]
        
    return Thisto_reduced, Txfit, Tyfit, Tgamma, Tperr


def PDFNava(folderpath, L1, L2, vartab, var, nbins = 12, switch_plot = 1): #Nombre d'avalanche pour 1 d'énergie incrémenté
    NCURVES = len(folderpath) + 1
    cmap = plt.cm.get_cmap('YlOrBr')
    colors = [cmap(1 - i / NCURVES) for i in range(NCURVES)]
    S = L1 * L2
    
    
    histo = np.zeros((len(folderpath), nbins))
    bins = np.zeros((len(folderpath), nbins + 1))
    
    for i, (folder, c , v) in enumerate(zip(folderpath, colors[1:], vartab)):
        if os.path.exists(f'{folder}/Avalanche_outputB.txt'):
            avatab = np.loadtxt(f'{folder}/Avalanche_outputB.txt')
        else :
            avatab = read_gz_with_pandas(f'{folder}/Avalanche_outputB.gz')
            
        data = []
        nbava = 0
        Emin = np.min(avatab[0][0])
        
        for line in avatab[:, 0]:
            if (line - Emin ) < 1:
                nbava +=1
            else:
                data.append(nbava)
                nbava = 1
                Emin = line

        histo[i] , bins[i] = np.histogram(data[1:], bins = nbins)
        histo[i] = histo[i] / np.sum(histo[i])
        
        params, covariance = curve_fit(gaussfct, bins[i, :-1], histo[i], p0 = [np.max(histo), np.mean(data), np.std(data)], maxfev = 10000)
        """ plt.plot(bins[i, :-1], histo[i], color = c, linewidth = 2, label = f'{v}') 
        plt.plot(bins[i, :-1], gaussfct(bins[i, :-1], *params), color = 'k', linestyle = '--', label = r'$\mu$' + f' = {np.mean(data):.2f}\n' + r'$\sigma$' +f' = {np.std(data):.2f}')
     """
     
    if switch_plot == 1:
        plt.figure(figsize = (10,10))
            
        for i in range(len(histo)):
            plt.plot(bins[i, :-1], histo[i], color = c, linewidth = 2, label = f'{v}') 
            plt.plot(bins[i, :-1], gaussfct(bins[i, :-1], *params), color = 'k', linestyle = '--', label = r'$\mu$' + f' = {np.mean(data):.2f}\n' + r'$\sigma$' +f' = {np.std(data):.2f}')


        plt.ylabel('$P(N_{ava})$', rotation=0, fontsize = 30)
        plt.xlabel('$N_{ava}$', fontsize = 30)
        plt.subplots_adjust(left = 0.15)
        plt.legend(title = rf'$\{var}$', fontsize = 23, title_fontsize = 23)
        plt.tick_params(labelsize = 28,length = 8, width = 2)
        plt.tick_params(labelsize = 28,length = 5, width = 1, which = 'minor')
        plt.show()
    
    return histo, bins
    

#%%  UPDATED FUNCTIONS

""" def mainfctv2(histo, lengthTab, nbins):
    
    d1 = histo[:]
    d1 = d1 / np.sum(d1) 
        
    ndist = np.zeros((nbins, 2))
    ndistloglog = np.zeros((nbins,2))

    logbins, ndist = logbins2(d1, nbins)   # logbins of P(s)    
    ndistbis = ndist[~np.isnan(ndist[:, 1])] #Delete bins with nan
    
    for x in range(len(ndistbis)):
        for y in range(2):
            if ndistbis[x,y] != 0:
                ndistloglog[x,y] = math.log(ndistbis[x,y],10)
    
    return ndistloglog, logbins

def process_size_from_histov2(folder, Lengthtab):
    data = np.loadtxt(f"{folder}/stat_histosize.txt", comments = "#")
    
    last_non_zero = np.nonzero(data)[0]
    
    nbins = freedman_diaconis_log_bins(data)
    
    mean = np.sum(data * np.arange(len(data)))/np.sum(data)
    
    ndistloglog, logbins = mainfctv2(data, Lengthtab, nbins)
    Tndist = 10 ** ndistloglog

    
    return Tndist, last_non_zero[-1], mean, nbins, logbins


def PDF_s_histogramv2(Traw_histo, Tstart_x, Tstop, Tvar, Tbins):

    filtered_Thistogram = [histo[histo[:, 1] != 1] for histo in Traw_histo]
    filtered_Tbins = [(bins[:-1][histo[:, 1] != 1] + [bins[-1]]) for histo, bins in zip(Traw_histo,Tbins)]
    NCURVES = len(Tvar)
    Ttau_s = np.zeros(NCURVES)
    Tperr_s = np.zeros(NCURVES)
    Txfit = []
    Tyfit = []
    
    for i, (histo, start_x, stop, nu) in enumerate(zip(filtered_Thistogram, Tstart_x, Tstop, Tvar)):
        
        if nu == 0.0:
            result, xs, _ , perr = fit_powerlaw(histo[:,0], histo[:,1], stop_x = stop, start_x = start_x, switch = 1,  plot=True, f_scale=1.0,
                                        function_to_fit='power')
        else:
            result, xs, _ , perr = fit_powerlaw(histo[:,0], histo[:,1], n_cutoff = stop, start_x = start_x, plot=True, f_scale=1.0,
                                        function_to_fit='power')
        
        Txfit.append(xs)
        Tyfit.append(powerlaw(xs, *result.x))
        Ttau_s[i] = np.round(result.x[1], 3)
        Tperr_s[i] =  np.round(perr[1], 3)
    
    return filtered_Thistogram, Txfit, Tyfit, Ttau_s, Tperr_s, filtered_Tbins """

