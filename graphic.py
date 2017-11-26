from scipy.interpolate import interp1d
import matplotlib.pyplot as plt
import numpy as np

x, y = [], []
with open("output.txt", "r") as f:
    for line in f.readlines():
        line = line.strip().split()
        x.append(int(line[0]))
        y.append(float(line[1]))

f = np.poly1d(np.polyfit(x, y, 1))

plt.plot(x, y, 'o', x, f(x), '-')
plt.show()