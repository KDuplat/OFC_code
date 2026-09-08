from Import import *


def avg(tab, nbfile, nbdisip):
    avg=np.zeros((nbdisip, len(tab[0])))
    x=0
    for i in range(len(tab)):
        if (x==nbdisip):
            x=0
        for j in range (len(tab[i])):
            avg[x,j]+=tab[i, j]/nbfile
        x+=1
    
    return avg

def stdev (tab, nbfile, nbdisip, tabmoy):
    stddev=np.zeros((nbdisip,len(tab[0])))
    
    x=0

    for i in range(len(tab)):
        if (x== nbdisip):
            x=0
        for j in range(len(tab[0])):
            stddev[x,j]+=(tab[i,j]-tabmoy[x,j])**2
        x+=1

    stddev=(stddev/nbfile)**(1/2)
    
    return stddev

def coorno(n, L1):
    x1=n%L1
    x2=n//L1
    return (x1, x2)

def no(x1, x2, L1):
    return x1 + x2 * L1

def extract_nb(string):
    # Utilisez une expression régulière pour extraire le nombre de la chaîne
    match = re.search(r'\d+', string)
    return int(match.group()) if match else 0  # Retourne 0 si aucun nombre n'est trouvé

def readlast(f):
    try:
        f.seek(-2, os.SEEK_END)       # Jump to the second last byte.
        while f.read(1) != b"\n":  #  Until newline is found ...
            f.seek(-2, os.SEEK_CUR)   #  ... jump back, over the read byte plus one.
    except OSError:                # Reached begginning of File
        f.seek(0)                  #  Set cursor to beginning of file as well.
    return f.read() 

def read_parameters(folderpath):
    params = []
    filepath = f"{folderpath}/Para_used.par"
    with open(filepath, 'r') as file:
        lines = file.readlines()

        for line in lines:
            if line.strip() == '' or line.strip().startswith('#'):
                continue
            
            # Identifie les lignes avec un paramètre suivi de sa description
            match = re.match(r"([\d\.\+\-eE]+)\s+#\s*(.*)", line)
            if match:
                value = float(match.group(1)) if '.' in match.group(1) or 'e' in match.group(1) else int(match.group(1))
                params.append(value)
            
            # Identifie les lignes avec plusieurs valeurs et descriptions dans le cas à plusieurs éléments dans une même ligne
            else: 
                match_multi = re.match(r"(([\d\.\+\-eE]+\s+)+)#\s*(.*)", line)
                if match_multi:
                    values = [float(x) if '.' in x or 'e' in x else int(x) for x in match_multi.group(1).strip().split()]
                    params.append(values)
    
    return params

def change_bins_log_histo(bins, histo, nb_new_bins):
    nb_old_bins = len(bins)
    factor = nb_old_bins // nb_new_bins
    histo_reduced = np.add.reduceat(histo, np.arange(0, nb_old_bins, factor)) #regroupe les bins ensembles pout former le nouvel histo
    
    bins_reduced = np.logspace(np.log10(bins[0]), np.log10(bins[-1]), nb_new_bins + 1)
    widths_reduced = np.diff(bins_reduced)
    
    integral_reduced = np.sum(histo_reduced * widths_reduced)
    histo_reduced_norm = histo_reduced / integral_reduced
    
    return bins_reduced, histo_reduced_norm
    
    
def chunked_file_reader(filepath, chunksize=1e7, usecols=None):
    if filepath.endswith('.gz'):
        for chunk in pd.read_csv(filepath, compression='gzip', delim_whitespace=True, header=None, chunksize=chunksize, usecols=usecols, comment = "#"):
            yield chunk
    else: 
        for chunk in pd.read_csv(filepath, delim_whitespace=True, header=None, chunksize=int(chunksize), usecols=usecols, comment='#'):  
            yield chunk

def read_gz_with_pandas(filepath, chunk_size=10**6, dtype=None, cols = None):
    chunks = []
    for chunk in chunked_file_reader(filepath, chunksize=chunk_size, usecols = cols):
        chunks.append(chunk.to_numpy())
    return np.vstack(chunks)

def read_gz_with_pandas_nrows(filepath, nrows=1e5, skiprows=None, cols=None, read_last=False):
    if read_last:        
        
        with gzip.open(filepath, 'rt') as f:
            total_lines = sum(1 for line in f if not line.startswith('#'))
            
        # Calculate how many rows to skip to get the last n rows
        skip_n = max(0, total_lines - int(nrows))
        df = pd.read_csv(filepath, compression='gzip', delim_whitespace=True, header=None, nrows=nrows,skiprows=skip_n,comment='#', usecols=cols)
    else:
        # Original behavior - read from start with optional skiprows
        df = pd.read_csv(filepath, compression='gzip', delim_whitespace=True, header=None, nrows=nrows, skiprows=skiprows,comment='#', usecols=cols)
    
    return df.to_numpy()
