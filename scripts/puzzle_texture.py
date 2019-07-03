import os
import sys
import imageio
import numpy as np

filename, extension = os.path.splitext(sys.argv[1])

im = imageio.imread(sys.argv[1])
height, width, _ = im.shape

n_rows = 8
n_cols = 6

row_height = int(height/n_rows)
col_width = int(width/n_cols)

slices = []

for row in range(n_rows):
    for col in range(n_cols):
      part = im[row*row_height:(row+1)*row_height, col*col_width:(col+1)*col_width]
      ones = np.ones(part.shape, dtype=part.dtype) * 255
      slices.append(np.concatenate([ones, part], axis=0))

slices[7] = slices[8]
slices[8] = slices[9]
slices[9] = slices[10]
slices[10] = slices[11]

slices[19] = slices[13]
slices[13] = slices[0]

texture = np.concatenate(slices, axis=1)
imageio.imwrite(filename + ".out" + extension, texture)
