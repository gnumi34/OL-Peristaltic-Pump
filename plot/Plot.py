#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 26 18:47:21 2020

@author: gnumi34
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import math

file_name = input("Enter file name : ")

data = pd.read_csv(file_name)

# Number of samplepoints
N = len(data)
Time = data['Time (s)']
T = Time[N-1]
t_plt = np.linspace(0, T, N, endpoint=False)

plt.title("Flow Rate Measurement")
plt.plot(t_plt, data['Flow Rate (uL/min)'], label='Measured Flow')
plt.xlabel('Time [sec]')
plt.ylabel('Flow Rate [uL/min]')
plt.grid(True)
plt.legend()
plt.show()
