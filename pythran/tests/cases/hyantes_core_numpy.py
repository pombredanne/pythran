#pythran export run(float, float, float, float, float, float, int, int, float[][])
#runas: import numpy ; run(0,0,90,90, 1, 0.0001, 800, 800, numpy.array([ [i/10., i/10., i/20.] for i in xrange(800)]))
import numpy as np
def run(xmin, ymin, xmax, ymax, step, range_, range_x, range_y, t):
    X,Y = t.shape
    pt = np.zeros((X,Y))
    "omp parallel for private(i,j,k,tmp)"
    for i in range(X):
        for j in range(Y):
            for k in t:
                tmp = 6368.* np.arccos( np.cos(xmin+step*i)*np.cos( k[0] ) * np.cos((ymin+step*j)-k[1])+  np.sin(xmin+step*i)*np.sin(k[0]))
                if tmp < range_:
                    pt[i][j]+=k[2] / (1+tmp)
    return pt