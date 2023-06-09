#!/usr/bin/env python3
import sys
import subprocess
import matplotlib.pyplot as plt
import numpy as np
import math
#plt.style.use('_mpl-gallery')

varpc = sys.argv[1]
train_vecs = sys.argv[2]
test_vecs = sys.argv[3]
sigma = sys.argv[4]
samp_start = sys.argv[5]
samp_ciel = sys.argv[6]
samp_inc = sys.argv[7]
threads = sys.argv[8]
runs_per_thread = sys.argv[9]

lowest_err_index = -1
lowest_err = sys.float_info.max

fig, ax = plt.subplots()


prog_out = subprocess.run(['./classify_ntimes',
                           varpc,
                           train_vecs,
                           test_vecs,
                           sigma,
                           samp_start,
                           samp_ciel,
                           samp_inc,
                           threads,
                           runs_per_thread],
                          stdout = subprocess.PIPE).stdout.decode('utf-8')

out_temp_arr = prog_out.split()

out_arr = np.asarray(out_temp_arr)

arr_size = int(out_arr[0])

avg_best_sample_size = float(out_arr[1])
avg_best_index = (avg_best_sample_size - int(samp_start)) / int(samp_inc)

err_arr = out_arr[2:2+arr_size].astype(float)

print('average best sample size: %f floor avg: %f ciel avg: %f'% (avg_best_sample_size,
                                                                  err_arr[math.floor((avg_best_index))],
                                                                  err_arr[math.ceil(avg_best_index)]))

for i in range(err_arr.size):
    if (err_arr[i] <= lowest_err):
        lowest_err = err_arr[i]
        lowest_err_index = i

print('lowest error: index = %d sample size = %d error = %f'% (lowest_err_index,
                                                               int(samp_start) + (int(samp_inc) * lowest_err_index),
                                                               lowest_err))

title = f'stats over {int(threads) * int(runs_per_thread)} run avg with sigma of {sigma}'
filename = f'stats_over_{int(threads) * int(runs_per_thread)}_run_avg_with_sigma_size_of_{sigma}'
x_label = "sample size"
err_label = "logistic error"
fig.canvas.set_window_title(title)
fig.suptitle(title)
plt.xlabel(x_label)
ax.set_ylabel(err_label)

x = np.arange(int(samp_start),int(samp_inc) * arr_size,int(samp_inc))
#print("shape of x:",x.shape)
ax.scatter(x,err_arr)

#ax.set(xlim= (0,np.amax(x)),xticks = (0,np.arange(x)),
#       ylim= (0,np.amax(y)),yticks = (0,np.arange(y)))
plt.show()
#result.stdout.decode('utf-8')
