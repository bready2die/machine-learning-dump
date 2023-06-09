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
samp_size = sys.argv[4]
threads = sys.argv[5]
runs_per_thread = sys.argv[6]
sigma_start = float(sys.argv[7])
sigma_end = float(sys.argv[8])
sigma_inc = float(sys.argv[9])


samp_start = samp_size
samp_end = str(int(samp_size) + 1)
samp_inc = '1'

fig, ax = plt.subplots(2,1,sharex=True)

err_arr = np.array([]).astype(float)
sup_arr = np.array([]).astype(float)

x = np.arange(sigma_start,sigma_end,sigma_inc)
for cur_sig in x:
    prog_out = subprocess.run(['./classify_ntimes',
                               varpc,
                               train_vecs,
                               test_vecs,
                               str(cur_sig),
                               samp_start,
                               samp_end,
                               samp_inc,
                               threads,
                               runs_per_thread],
                              stdout = subprocess.PIPE).stdout.decode('utf-8')

    out_temp_arr = prog_out.split()

    out_arr = np.asarray(out_temp_arr)

    arr_size = int(out_arr[0])

    #avg_best_sample_size = float(out_arr[1])
    #avg_best_index = (avg_best_sample_size - int(samp_start)) / int(samp_inc)

    err_arr = np.concatenate((err_arr, out_arr[2:2 + arr_size].astype(float)))
    sup_arr = np.concatenate((sup_arr, out_arr[2 + arr_size:2 + arr_size + arr_size].astype(float)))

'''
print('lowest error: index = %d sample size = %d error = %f'% (lowest_err_index,
                                                               1 + (int(samp_inc) * lowest_err_index),
                                                               lowest_err))
'''
print(err_arr)
title = f'stats over {int(threads) * int(runs_per_thread)} run avg with sample size of {samp_size}'
filename = f'stats_over_{int(threads) * int(runs_per_thread)}_run_avg_with_sample_size_of_{samp_size}'
x_label = "sigma"
err_label = "# of misclassifications"
sup_label = "# of superfluous features"

#print("shape of x:",x.shape)
fig.canvas.set_window_title(title)
fig.suptitle(title)
#plt.title(title)
plt.xlabel(x_label)
ax[0].set_ylabel(err_label)
ax[1].set_ylabel(sup_label)

#ax[0].ylim(min(sup_arr), max(sup_arr))

ax[0].scatter(x,err_arr)
ax[1].scatter(x,sup_arr)
#ax.set(xlim= (0,np.amax(x)),xticks = (0,np.arange(x)),
#       ylim= (0,np.amax(y)),yticks = (0,np.arange(y)))
plt.savefig(filename)
plt.show()
#result.stdout.decode('utf-8')
"""
plt.xlabel(x_label)
plt.ylabel(y2_label)
ax[1].scatter(x,sup_arr)

plt.show()
print(sup_arr)
"""
