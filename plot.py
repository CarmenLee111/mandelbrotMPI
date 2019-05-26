import matplotlib.pyplot as plt 
from matplotlib import colors
import numpy as np 
import os
import sys


def main(argv):
    xmin, xmax, ymin, ymax = [float(x) for x in sys.argv[1:5]]
    f = sys.argv[5]
    cmap = sys.argv[6]
    data = np.loadtxt(f)
    #data = data[::-1]                 # flip the matrix horizontally

    norm = colors.PowerNorm(0.3)
    plt.imshow(data, extent=[xmin, xmax, ymin, ymax], cmap=cmap, origin='lower')#, norm=norm)
    plt.savefig(f+'.pdf')

if __name__ == '__main__':
    main(sys.argv)



