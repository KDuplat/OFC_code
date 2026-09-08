from Import import *

def expfct(x, a, b):
    return a * np.exp(b * x)

def expfct2(x, loga, b):
    #log(y),  np.exp pour récup la bonne valeur
    return loga + b * x

def powerlaw(x, a, b):
    return a*x**b

def powerlaw2(log_x, loga, b):
    #log(y), log(x)
    return loga + b * log_x

def stretched_powerlaw_fit(x, a, b, c, d):
    return a * x **(-b) * np.exp(-(x / c) ** d)

def powerlaw_cdf(x, alpha, xmin):
    return 1 - (x / xmin) ** (1 - alpha)

def gamma_distribution(x, A, n, B):
    return A * x**n * np.exp(-x / B)

def linlaw(x, a, b):
    return a*x+b

def extexp(x, a, b, c):
    return a * np.exp(-b * (x**c))

def exppowlaw(x, a, b, c):
    return a * x**(-b) * np.exp(x/c)

def fitfct(x, a, b, d):
    return (1/a)**15 * np.exp(-(b**4) * x) * x**-d

def gaussfct(x,A,x0,sigma):
    return A * np.exp(-(x - x0)**2/(2 * sigma**2))
