import matplotlib.pyplot as plt 
from matplotlib import colors
import numpy as np 
import os
import sys


def main(argv):
    xcenter, ycenter, radius= [float(x) for x in sys.argv[1:4]]
    f = sys.argv[4]
    cmap = sys.argv[5]
    data = np.loadtxt(f)
    # data = data[::-1]                 # flip the matrix horizontally

    norm = colors.PowerNorm(0.3)
    plt.imshow(data, cmap=cmap, origin='lower')#, norm=norm)
    plt.title("C: (%.3E, %.3E), R: %.1E" % (xcenter, -ycenter, radius))
    plt.axis('off')
    plt.tight_layout()

    plt.savefig(f+'.pdf', bbox_inches='tight', pad_inches = 0, dpi=1200)

if __name__ == '__main__':
    main(sys.argv)



