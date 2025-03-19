import subprocess
import matplotlib.pyplot as plt
# 调用外部程序
plt.rcParams['font.sans-serif'] = ['SimHei']
ans1=[]
ans2=[]
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import savgol_filter

# 生成示例数据
# x = np.linspace(0, 10, 100)
# y1 = np.sin(x) + np.random.normal(0, 0.1, 100)
# y2 = np.cos(x) + np.random.normal(0, 0.1, 100)

# 平滑化处理
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
plt.figure(figsize=(10, 6))

plt.plot(x, ans1, color=color1, alpha=0.3)
plt.plot(x, ans2, color=color2, alpha=0.3)

plt.plot(x, ans1_smooth, color=color1, label='平凡算法')
plt.plot(x, ans2_smooth, color=color2, label='cache优化算法')


plt.xlabel('n')
plt.ylabel('Time (ms)')
plt.legend()

plt.savefig('problem1.png')
plt.show()