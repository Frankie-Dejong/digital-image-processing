# -*- coding: utf-8 -*-
import cv2
import numpy as np
from matplotlib import pyplot as plt
img = cv2.imread('./images/lenna.jpeg', 0)
f = np.fft.fft2(img)
fshift = np.fft.fftshift(f)
# fshift = f
magnitude_spectrum = 20 * np.log(np.abs(fshift))
print(magnitude_spectrum[0][0])
plt.subplot(121), plt.imshow(img, cmap='gray')
plt.title('Input Image'), plt.xticks([]), plt.yticks([])
plt.subplot(122), plt.imshow(magnitude_spectrum, cmap='gray')
plt.title('Magnitude Spectrum'), plt.xticks([]), plt.yticks([])
plt.show()

# print(np.log(2))