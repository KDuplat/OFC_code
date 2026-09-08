from Import import *

def moving_avg(signal, window_size):
    filtered_signal = np.zeros(len(signal))
    
    for i in range(len(signal)):
        start_index = i - window_size // 2
        end_index = i + window_size // 2
        
        # Créer une fenêtre avec des zéros au lieu de valeurs manquantes
        window = np.zeros(window_size)
        
        # Calculer les indices valides
        valid_start = max(0, start_index)
        valid_end = min(len(signal), end_index)
        
        # Remplir la partie valide de la fenêtre avec les valeurs du signal
        window[(valid_start - start_index):(valid_end - start_index)] = signal[valid_start:valid_end]
        
        # Calculer la moyenne sur la fenêtre, en incluant les zéros pour les valeurs manquantes
        filtered_signal[i] = np.mean(window)
    
    return filtered_signal

def moving_average(x, y,):
    xnew = np.logspace(np.log10(x.min()), np.log10(x.max()), 20)  
    spl = make_interp_spline(x, y, k = 3)
    ynew = spl(xnew)
    return xnew, ynew

def running_mean(x, N):
    cumsum = np.cumsum(np.insert(x, 0, 0)) 
    return (cumsum[N:] - cumsum[:-N]) / float(N)


def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y

def butter_lowpass(cutoff, fs, order=5):
    return butter(order, cutoff, fs=fs, btype='low', analog=False)
