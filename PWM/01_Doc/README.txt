---【程序说明】---
（1）程序摘要：基于BIOS的STM32L431工程
（2）运行芯片：本工程基于BIOS运行,不从0启动
（3）软件环境：AHL-GEC-IDE，兼容STM32cubeIDE
（4）硬件环境：AHL-STM32L431，实质是只要是STM32L431即可，
              接AHL-GEC扩展板，有三色灯指示
（5）硬件连接：见"..\05_UserBoard\user"文件
（6）功能简述：
              ① 串口接收到数据后设置占空比，翻转红灯；
              ② 接收上位机发来的数据，将其解释为占空比后更新PWM