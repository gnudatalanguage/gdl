# simple example for GDL's python interface
# note that the matplotlib package has *much* more capabilities.
# feel free to experiment.

from matplotlib.matlab import *

def py_plot( x, y):
    plot(x, y, linewidth=1.0)
    show()
