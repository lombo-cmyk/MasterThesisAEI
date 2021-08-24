import matplotlib.pyplot as plt
from common.free_functions import create_directories
import numpy as np

# Samples were collected at University with sampling period T=20s
sensiron_samples = [13, 13, 12, 12, 14, 12, 12, 12, 12, 11,
                    12, 12, 11, 13, 13, 12, 11, 13]
trotec_samples = [11, 14, 11,  6, 11, 10, 9, 9, 9, 10,
                  11,  6,  9, 13,  9,  7, 9, 11]
assert len(sensiron_samples) == len(trotec_samples)

cwd = create_directories("temp_plots")

y_max = max(sensiron_samples + trotec_samples)
y_min = min(sensiron_samples + trotec_samples)
plt.yticks(np.arange(y_min, y_max + 1, 1))
plt.xticks(np.arange(0, len(sensiron_samples), step=2))

plt.scatter(range(0, len(sensiron_samples)), sensiron_samples, s=20,
            facecolors='none', edgecolors='g', linewidths=1.4,
            label="SPS30")
plt.scatter(range(0, len(trotec_samples)), trotec_samples, s=75,
            facecolors='none', edgecolors='r', linewidths=1.4,
            label="BQ20")

plt.xlabel("Probe index", fontsize=20)
plt.ylabel(r"PM2.5 [$\mu g/m^3$]", fontsize=20)
plt.grid()
plt.legend()
plt.savefig(f"{cwd}pm25_comparison.png", dpi=300)
