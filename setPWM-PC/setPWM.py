# -*- coding: utf-8 -*-
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure 
import sys
import serial
import serial.tools.list_ports
import time
from PyQt5.QtWidgets import QLabel, QToolTip, QPushButton,QApplication, QHBoxLayout, QVBoxLayout, QMainWindow, QPushButton, QWidget, QTextEdit, QComboBox, QLineEdit
from PyQt5.QtGui import QIcon, QFont, QTextCursor
from PyQt5.QtCore import pyqtSignal, QTimer, Qt


class MyComboBox(QComboBox):
    def __init__(self):
        super(MyComboBox, self).__init__()

    def showPopup(self):                    #重写showPopup函数
      
        # 检测所有存在的串口，将信息存储在字典中
        self.Com_Dict = {}
        port_list = list(serial.tools.list_ports.comports())
        self.clear()
        for port in port_list:
            self.Com_Dict["%s" % port[0]] = "%s" % port[1]
            self.addItem(port[0])
        if len(self.Com_Dict) == 0:
            self.addItem("无串口")

        QComboBox.showPopup(self)


class TooltipForm(QMainWindow):
    def __init__(self):
        super(TooltipForm,self).__init__()
        self.initUI()
        self.button_open.clicked.connect(self.onClick_Button_open)
        self.button_setpwm.clicked.connect(self.onClick_Button_set)
        
        self.cb1.currentIndexChanged[str].connect(self.select_COM) # 条目发生改变，发射信号，传递条目内容
        self.cb2.currentIndexChanged[str].connect(self.select_BPS) 
        self.cb3.currentIndexChanged[str].connect(self.select_bytesize)
        self.cb4.currentIndexChanged[str].connect(self.select_stopbits)
        

    def initUI(self):
        self.setWindowTitle("PWM参数配置")
        self.setGeometry(200,200,800,500)
        # 状态栏
        self.status = self.statusBar()
       
        #串口
        self.ser = serial.Serial() 
        self.COM = None

        # 定时器接收数据
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.data_receive)

        #标签1
        self.com_label = QLabel(self)
        self.com_label.setText("串口号：")
        self.com_label.setAlignment(Qt.AlignRight)
        #串口COM下拉框
        self.cb1 = MyComboBox()

        #标签2
        self.bps_label = QLabel(self)
        self.bps_label.setText("波特率：")
        self.bps_label.setAlignment(Qt.AlignRight)
        #波特率下拉框
        self.cb2 = QComboBox()
        #self.cb2.move(100, 20)
        # 单个添加条目
        BAUD = ['110','300','600','1200',
                '2400','4800','9600','14400',
                '19200','38400','56000','57600',
                '115200','128000','256000']
        self.cb2.addItems(BAUD)
        self.cb2.setCurrentIndex(12)
        self.BPS = self.cb2.currentText()

         #标签3
        self.bytesize_label = QLabel(self)
        self.bytesize_label.setText("数据位：")
        self.bytesize_label.setAlignment(Qt.AlignRight)
        #波特率下拉框
        self.cb3 = QComboBox()
        #self.cb2.move(100, 20)
        # 单个添加条目
        BYTESIZE = ['5','6','7','8']
        self.cb3.addItems(BYTESIZE)
        self.cb3.setCurrentIndex(3)
        self.bytesize = self.cb3.currentText()

        #标签4
        self.stopbits_label = QLabel(self)
        self.stopbits_label.setText("停止位：")
        self.stopbits_label.setAlignment(Qt.AlignRight)
        #波特率下拉框
        self.cb4 = QComboBox()
        # 单个添加条目
        STOPBIT = ['1','1.5','2']
        self.cb4.addItems(STOPBIT)
        self.cb4.setCurrentIndex(0)
        self.stopbits = self.cb4.currentText()

        # 设置打开按钮   
        self.button_open = QPushButton() 
        self.button_open.setText("打开串口")

        self.info = QTextEdit()
        self.info.setMaximumHeight(100)
        self.info.setReadOnly(True)
        self.info.setAlignment(Qt.AlignLeft)

        #创建占空比文本框
        self.textEdit = QTextEdit()
        self.textEdit.setMaximumHeight(28)
        self.textEdit.setText('25')

        #标签占空比
        self.duty_label = QLabel(self)
        self.duty_label.setText("占空比：")
        self.duty_label.setAlignment(Qt.AlignLeft)

        # 设置设置按钮   
        self.button_setpwm = QPushButton() 
        self.button_setpwm.setText("PWM参数设置")
        
        self.canvas = Canvas(self, width=8, height=4)
        #self.figure = plt.figure()
        #self.canvas = FigureCanvas(self.figure)
        
        #实例化水平布局
        layoutSerial = QHBoxLayout()
        #相关控件添加到水平布局中
        layoutSerial.addWidget(self.com_label)
        layoutSerial.addWidget(self.cb1)
        layoutSerial.addWidget(self.bps_label)
        layoutSerial.addWidget(self.cb2)
        layoutSerial.addWidget(self.bytesize_label)
        layoutSerial.addWidget(self.cb3)
        layoutSerial.addWidget(self.stopbits_label)
        layoutSerial.addWidget(self.cb4)
        layoutSerial.addWidget(self.button_open)

        layoutInfo = QVBoxLayout()
        layoutInfo.addWidget(self.info)

        layoutPWM = QVBoxLayout()
        layoutSet = QHBoxLayout()
        layoutShow = QVBoxLayout()
        layoutSet.addWidget(self.duty_label)
        layoutSet.addWidget(self.textEdit)
        layoutSet.addWidget(self.button_setpwm)
        layoutShow.addWidget(self.canvas)
        layoutPWM.addLayout(layoutSet)
        layoutPWM.addLayout(layoutShow)
        # 主框架，所有控件的放置位置
        mainFrame = QWidget()

        #全局布局
        mainLayout = QVBoxLayout(mainFrame)

        #全局布局中添加小布局
        mainLayout.addLayout(layoutSerial)
        mainLayout.addLayout(layoutInfo)
        mainLayout.addLayout(layoutPWM)

        mainFrame.setLayout(mainLayout)
        # 使充满屏幕
        self.setCentralWidget(mainFrame)


    def onClick_Button_open(self):
        if self.button_open.text() == "打开串口":
            if self.COM is None or self.COM == "无串口":
                self.info.moveCursor(QTextCursor.End)
                self.info.insertPlainText("请先设置串口！")
            else:
                self.port_open()
                self.button_open.setText("关闭串口")
        else:
            self.port_close()
            self.button_open.setText("打开串口")


    def onClick_Button_set(self):
        dutyInput = self.textEdit.toPlainText()
        lenIn=len(dutyInput)
        if(lenIn==2):
            self.data_send('0'+dutyInput)
        elif(lenIn==1):
            self.data_send('00'+dutyInput)
        elif(lenIn>=3):
            self.data_send('100')
        else:
            self.data_send('000')
        self.info.insertPlainText("已设置占空比为"+dutyInput+'%\n')
        self.canvas.plot(dutyInput)


    # 打开串口
    def port_open(self):
        self.ser.port = self.COM
        self.ser.baudrate = int(self.BPS)
        self.ser.bytesize = int(self.bytesize)
        self.ser.stopbits = float(self.stopbits)
        print(float(self.stopbits))
        self.ser.parity = 'N'
       
        try:
            self.ser.open()
        except:
            self.info.moveCursor(QTextCursor.End)
            self.info.insertPlainText("\n此串口不能被打开！\n")
            return None

        # 打开串口接收定时器，周期为2ms
        self.timer.start(2)

        if self.ser.isOpen():
            self.info.moveCursor(QTextCursor.End)



    # 关闭串口
    def port_close(self):
        self.timer.stop()
        try:
            self.ser.close()
        except:
            pass
        self.info.moveCursor(QTextCursor.End)


    # 发送数据
    def data_send(self,s):#
        if self.ser.isOpen():
            input_s = s
            if input_s != "":
                # 非空字符串
                # ascii发送
                input_s = input_s.encode('utf-8')

                self.ser.write(input_s)
        else:
            pass


    # 接收数据
    def data_receive(self):
        try:
            num = self.ser.inWaiting()
        except:
            self.port_close()
            return None
        if num > 0:
            data = self.ser.read(num)
            num = len(data)
            
            # 串口接收到的字符串为b'123',要转化成unicode字符串才能输出到窗口中去
            self.info.moveCursor(QTextCursor.End)
            self.info.insertPlainText(data.decode('utf-8','ignore'))
        else:
            pass


    def select_COM(self, i):
        self.COM = i

    def select_BPS(self, i):
        self.BPS = i

    def select_bytesize(self, i):
        self.bytesize = i

    def select_stopbits(self, i):
        self.stopbits = i
        
    def select_duty(self, i):
        self.duty = i


class Canvas(FigureCanvas):  
    def __init__(self, parent = None, width = 1, height = 1, dpi = 100):  
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)  
        FigureCanvas.__init__(self, fig)  
        self.setParent(parent)  
        #self.plot()
    def plot(self,i):
        d = []
        xStart=0
        period=8
        dutyInput=int(i)/100
        x_unit=1
        y0=1
        y1=0
        for start in range(xStart, xStart + x_unit * period, x_unit):
            d.append((start, y0))
            middle = start + x_unit * dutyInput
            d.append((middle, y0))
            d.append((middle, y1))
            end = start + x_unit
            d.append((end, y1))
        d = np.array(d)
        plt.figure(figsize=(15, 5))
        plt.plot(d[:, 0], d[:, 1])
        plt.suptitle('duty='+i+'%')
        plt.show()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    main = TooltipForm()
    # 显示窗口
    main.show() 
    # 建立循环
    sys.exit(app.exec_())