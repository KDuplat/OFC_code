from pathlib import Path
import sys
import os
import re
# from latexmain import mainfct, mainfct2
from matplotlib import gridspec
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from joblib import Parallel, delayed

import math
import matplotlib.cm as cm
import gzip

from scipy import linalg
from scipy.optimize import curve_fit, least_squares
from scipy.stats import ks_2samp
from scipy.signal import butter, lfilter, freqz
from scipy.interpolate import make_interp_spline, BSpline
from sklearn.metrics import r2_score

import scienceplots
plt.style.use('science')
from IPython.display import Markdown, display

import sciplotlib.style as spstyle


from Fitting_fct import *
from Tools_fct import *
from Fig_fct import *
from Signal_analysis_fct import *


from leastsquares import *
from logbins import *
from twopoints import *

from Macro import *