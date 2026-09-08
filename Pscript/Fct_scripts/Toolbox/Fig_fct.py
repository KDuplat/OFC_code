from Import import *


def para_fig(ax, xlabel, ylabel, type = 'lin', labelsize = 30, tickssize = 28, length_M = 8, length_m = 5, width_M = 2, width_m = 1, fontweight = 'normal'):
    if type == 'semilog':
        ax.set_yscale("log")
    if type == 'loglog' :
        ax.set_xscale("log")
        ax.set_yscale("log")
    ax.set_xlabel(xlabel, fontsize=labelsize, fontweight = fontweight)
    ax.set_ylabel(ylabel, fontsize=labelsize, labelpad = 30,  fontweight = fontweight)
    ax.tick_params(labelsize = tickssize,length = length_M, width = width_M)
    ax.tick_params(labelsize = tickssize,length = length_m, width = width_m, which = 'minor')
    if type == 'imshow':
        ax.set_xticks([])
        ax.set_yticks([])

def para_fig_Nature(ax, xlabel, ylabel, type = 'lin'):
    if type == 'semilog':
        ax.set_yscale("log")
    if type == 'loglog' :
        ax.set_xscale("log")
        ax.set_yscale("log")
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    if type == 'imshow':
        ax.set_xticks([])
        ax.set_yticks([])

def freedman_diaconis_log_bins(histo_lin):
    """
    Calcule le nombre optimal de bins logarithmiques pour un histogramme 
    selon la règle de Freedman-Diaconis, en transposant les données en échelle logarithmique.
    
    Paramètres :
    - data : array-like, histogramme des données (entiers uniquement)
    
    Retourne :
    - int : nombre de bins logarithmiques optimal
    """
    
    # Filtrer les valeurs positives car log(0) n'est pas défini
    histo_lin = np.array(histo_lin)
    histo_lin = histo_lin[histo_lin > 0]
    
    # Appliquer la transformation logarithmique sur les données
    log_data = np.log(histo_lin)
    
    # Calculer l'IQR (interquartile range) des données en échelle logarithmique
    Q1 = np.percentile(log_data, 25)
    Q3 = np.percentile(log_data, 75)
    IQR = Q3 - Q1
    
    # Appliquer la règle de Freedman-Diaconis
    n = len(log_data)
    bin_width = 2 * IQR / np.cbrt(n)  # largeur optimale en échelle log
    
    # Calcul du nombre de bins : basé sur la plage des valeurs log-transformées
    log_min = np.min(log_data)
    log_max = np.max(log_data)
    num_bins = int(np.ceil((log_max - log_min) / bin_width))
    
    return num_bins

def fit_powerlaw(x, n, n_cutoff=1e-30, start_x=1e2, stop_x = 1e5, switch = 0, plot=True, f_scale=1.0, function_to_fit='power_cut_off'):
    """
    fit n(x) = a*x**b*exp(-(x/c)**d) for n > n_cutoff (which defines a maximum x to fit) and x > start_x.
    Loss function is soft_l1. Adjust f_scale to weight more or less smaller values.
    Adjust x0 directly in code.
    start_x : cut off little value of x => noise
    n_cutoff : cut large value of x if n to small
    """
    
    if switch == 0: #stop probability 
        mask = n > n_cutoff
        stop_x = x[np.where(n > n_cutoff)[0][-1]]
    else: #Stop size
        mask = x < stop_x
    
    xs = x[mask]
    n_f = n[mask]

    # print("test")
    start = np.where(xs > start_x)[0][0]
    stop = np.where(xs < stop_x)[0][-1]

    # print(f"start x = {start_x:.2e}")
    # print(f"stop x = {stop_x:.2e}")

    # if stop - start < 2:
    #     raise ValueError

    def residuals(params): #Calculate the difference between the data and the fit
        if function_to_fit == 'power_cut_off':
            return np.log(powercutoff_fit(xs[start:stop], *params)) - np.log(n_f[start:stop])
        else:
            return np.log(powerlaw(xs[start:stop], *params)) - np.log(n_f[start:stop])


    if function_to_fit == 'power_cut_off':
        result = least_squares(residuals, x0=(n_f[0], -1, stop_x / 2, 1), loss="soft_l1", max_nfev=10000,
                               bounds=[[0, -np.inf, 0, 0], [np.inf, 0, stop_x * 1e4, np.inf]], f_scale=f_scale)
        names = ["a", "b", "c", "d"]
        # print(result.message)
        # print("n(x) = a*x**b*exp(-(x/c)**d)")
        # for i in range(len(result.x)):
        #     print(f"{names[i]} = {result.x[i]:.2e}")

        U, s, Vh = linalg.svd(result.jac, full_matrices=False)
        tol = np.finfo(float).eps * s[0] * max(result.jac.shape)
        w = s > tol
        cov = (Vh[w].T / s[w] ** 2) @ Vh[w]  # robust covariance matrix
        perr = np.sqrt(np.diag(cov))
        # print("perr = ", perr)

    else:
        result = least_squares(residuals, x0=(n_f[0], -1), loss="soft_l1", max_nfev=10000,
                               bounds=[[0, -np.inf], [np.inf, 0]], f_scale=f_scale)

        names = ["a", "b"]
        # print(result.message)
        # print("n(x) = a*x**b")
        # for i in range(len(result.x)):
        #     print(f"{names[i]} = {result.x[i]:.2e}")
        U, s, Vh = linalg.svd(result.jac, full_matrices=False)
        tol = np.finfo(float).eps * s[0] * max(result.jac.shape)
        w = s > tol
        cov = (Vh[w].T / s[w] ** 2) @ Vh[w]  # robust covariance matrix
        perr = np.sqrt(np.diag(cov))
        # print("perr = ", perr)

    return result, xs[start:], n_f[start:], perr