import subprocess
import matplotlib.pyplot as plt
plt.rcParams['font.sans-serif'] = ['SimHei']

import numpy as np
from scipy.signal import savgol_filter

ans1=[]
ans2=[]

x = np.arange(10,2500,10)
for n in x:
    result = subprocess.run(['problem1.exe',f'{n}'], stdout=subprocess.PIPE)
    output = result.stdout.decode('utf-8')
    # print(output)
    output=output.strip().split('\n')
    # print(output)
    if n%100==0:
        print(f'n={n},ans1={output[0].split("ms")[0].strip()},ans2={output[1].split("ms")[0].strip()}')
    ans1.append(float(output[0].split('ms')[0].strip()))
    ans2.append(float(output[1].split('ms')[0].strip()))

ans1_smooth = savgol_filter(ans1, window_length=11, polyorder=2)
ans2_smooth = savgol_filter(ans2, window_length=11, polyorder=2)

color1 = 'blue'
color2 = 'orange'
plt.figure(figsize=(20, 12))

plt.plot(x, ans1, color=color1, alpha=0.3)
plt.plot(x, ans2, color=color2, alpha=0.3)

plt.plot(x, ans1_smooth, color=color1, label='平凡算法')
plt.plot(x, ans2_smooth, color=color2, label='cache优化算法')

plt.tick_params(labelsize=25)
plt.ylim(0, 55)
plt.xlabel('scale of n',fontsize=30)
plt.ylabel('Time (ms)',fontsize=30)
plt.legend(fontsize=30)

plt.savefig('problem1.png')
plt.show()