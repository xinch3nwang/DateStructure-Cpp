# -*- coding: utf-8 -*-
import sys
import serial
import serial.tools.list_ports
import numpy as np
import matplotlib.pylab as mp
import matplotlib.animation as ma
import threading




# 打开串口
def openPort():
    #port_list = list(serial.tools.list_ports.comports())
    #print(port_list)
    #ser.port = port_list[0]
    
    ser.port = "COM3"
    ser.baudrate = 115200
    ser.bytesize = 8
    ser.stopbits = 1.0
    ser.parity = 'N'
    try:
        ser.open()
    except:
        print("此串口不能被打开！\n")
        return None

    # 打开串口接收定时器，周期为2ms
    #timer.start(2)

    if ser.isOpen():
        print("串口状态（已开启）\n")


# 关闭串口
def closePort():
    try:
        ser.close()
    except:
        pass

'''
def receiveData():
    #global edge
    while True:
        try:
            num = ser.inWaiting()
        except:
            closePort()
        if num > 0:
            data = ser.read(num)
            num = len(data)
        # 串口接收到的字符串为b'123',要转化成unicode字符串才能输出到窗口中去
            info = data.decode('utf-8','ignore')
            if info[0] == '0':
                edge = "up";
            else:
                edge = "down";
            thDraw = threading.Thread(target=generator, args=(edge,))
            thDraw.start()
        else:
            pass
'''

# 生成器函数
def generator():
    t = 0
    v = 0
    global h_val
    global l_val
    l_val = 0
    h_val = 0
    while True:
        try:
            num = ser.inWaiting()
        except:
            closePort()
        if num > 0:
            data = ser.read(num)
            num = len(data)
            # 串口接收到的字符串为b'123',要转化成unicode字符串才能输出到窗口中去
            info = data.decode('utf-8','ignore')
            infolist = info.split(':');
            edge = infolist[0]
            value = int(infolist[1])
            count = int(infolist[2])
            if edge == '1':
                v = 1
                l_cnt = count
                l_val = value
            else:
                v = 0
                h_cnt = count
                h_val = value
            if h_val and l_val:
                h_time = h_cnt*65535 + h_val
                l_time = l_cnt*65535 + l_val
                cal_duty = h_time/(h_time+l_time)
                cal_time = (h_time+l_time)/375000/2
                print("占空比测量值：{:.4f}%".format(cal_duty*100))
                print("每周期时间测量值：{:.4f}s".format(cal_time))
                mp.title("Real-time Duty {:.4f}%".format(cal_duty*100), fontsize=15)
        else:
            pass

        yield t, v  # 保存状态的返回
        t += 0.05


def update(data):
    t, v = data  # 时间，生成器的值
    x, y = pl.get_data()  # 获取生成器数据
    x.append(t)
    y.append(v)
    # 移动坐标轴位置，以便持续观察数据
    # 获取当前坐标轴的最小值与最大值，即坐标系的左右边界
    x_min, x_max = ax.get_xlim()
    if t >= x_max:
        # 平移坐标轴：将最小值变为当前位置减去窗口宽度，最大值变为当前值
        ax.set_xlim(t - (x_max - x_min), t)
        # 坐标系起点终点都改变了，需要重新画一个画布
        ax.figure.canvas.draw()
    # 修改数据
    pl.set_data(x, y)


if __name__ == "__main__":
    ser = serial.Serial()
    openPort()

    mp.figure("Signal", facecolor='lightgray')
    
    mp.xlabel('Time', fontsize=14)  # 横纵坐标标签
    mp.ylabel('Signal', fontsize=14)
    
    ax = mp.gca()  # 获取坐标轴
    ax.set_ylim(-0.5, 1.5)  # 垂直坐标范围
    ax.set_xlim(0, 5)
    mp.minorticks_on()
    mp.tick_params(labelsize=10)  # 精度值
    mp.grid(which='both',linestyle=':')  # 网格线


    pl = mp.plot([], [], c="orangered")[0]  # 有很多个元素，此处取一个处理
    pl.set_data([], [])  # 设置数据，此处给的空数据，以便于之后将生成器的数据传入


    # 生成动画
    anim = ma.FuncAnimation(mp.gcf(), update, generator, interval=2)

    mp.show()
    
   