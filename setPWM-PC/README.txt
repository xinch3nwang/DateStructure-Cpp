---【程序说明】---
（1）语言使用：Python
（2）第三方库：serial、pyserial、PyQT5、sys、time、numpy、matplotlib
（3）功能简述：
              ① 可打开串口并设置串口传输的相应参数（波特率、数据位、停止位）；
              ② 可输入0-100内的数，点击PWM设置按钮，发送三字节的字符串给下位机程序；
	（例：输入25发送‘025’，输入5发送‘005’，输入100发送‘100’）
              ③ 可根据输入的数产生相应占空比的波形图预览。