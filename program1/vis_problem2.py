import subprocess
import matplotlib.pyplot as plt
plt.rcParams['font.sans-serif'] = ['SimHei']

import numpy as np
from scipy.signal import savgol_filter
#['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf']
ans1=[]
ans2=[]
ans3=[]
ans4=[]
ans5=[]
ans6=[]
ans7=[]
x = np.arange(16,4000,16)
for n in x:
    result = subprocess.run(['problem2.exe',f'{n**2}'], stdout=subprocess.PIPE)
    output = result.stdout.decode('utf-8')
    # print(output)
    output=output.strip().split('\n')
    # print(output)
    if n%100==0:
        print(f'n={n},ans1={output[0].split("ms")[0].strip()},ans2={output[1].split("ms")[0].strip()}')
    ans1.append(float(output[0].split('ms')[0].strip()))
    ans2.append(float(output[1].split('ms')[0].strip()))  
    ans3.append(float(output[2].split('ms')[0].strip()))
    ans4.append(float(output[3].split('ms')[0].strip()))
    ans5.append(float(output[4].split('ms')[0].strip()))
    ans6.append(float(output[5].split('ms')[0].strip()))
    ans7.append(float(output[6].split('ms')[0].strip()))

ans1_smooth = savgol_filter(ans1, window_length=11, polyorder=2)
ans2_smooth = savgol_filter(ans2, window_length=11, polyorder=2)
ans3_smooth = savgol_filter(ans3, window_length=11, polyorder=2)
ans4_smooth = savgol_filter(ans4, window_length=11, polyorder=2)
ans5_smooth = savgol_filter(ans5, window_length=11, polyorder=2)
ans6_smooth = savgol_filter(ans6, window_length=11, polyorder=2)
ans7_smooth = savgol_filter(ans7, window_length=11, polyorder=2)

color1 = '#1f77b4'
color2 = '#ff7f0e'
color3 = '#2ca02c'
color4 = '#d62728'
color5 = '#9467bd'
color6 = '#8c564b'
color7 = '#e377c2'

plt.figure(figsize=(20, 12))

plt.plot(x, ans1, color=color1, alpha=0.2)
plt.plot(x, ans2, color=color2, alpha=0.2)
plt.plot(x, ans3, color=color3, alpha=0.2)
plt.plot(x, ans4, color=color4, alpha=0.2)
plt.plot(x, ans5, color=color5, alpha=0.2)
plt.plot(x, ans6, color=color6, alpha=0.2)
plt.plot(x, ans7, color=color7, alpha=0.2)

plt.plot(x, ans1_smooth, color=color1, label='平凡算法')
plt.plot(x, ans2_smooth, color=color2, label='两路优化算法')
plt.plot(x, ans3_smooth, color=color3, label='四路优化算法')
plt.plot(x, ans4_smooth, color=color4, label='八路优化算法')
plt.plot(x, ans5_smooth, color=color5, label='十六路优化算法')
plt.plot(x, ans6_smooth, color=color6, label='三十二路优化算法')
plt.plot(x, ans7_smooth, color=color7, label='递归优化算法')


plt.tick_params(labelsize=25)
# plt.ylim(0, 55)
plt.xlabel('scale of sqrt(n)',fontsize=30)
plt.ylabel('Time (ms)',fontsize=30)
plt.legend(fontsize=30)

plt.savefig('problem2.png')
plt.show()