## 目录
### 摘要
### ABSTRACT

## 绪论
### 智能停车场的应用背景
### 智能停车场发展状况

## 智能停车场的总体设计与功能
### 智能停车场的总体设计
智能浇花器系统总体采用 Arduino 单片机为应用核心，利用 Arduino 单片机处理智能停车场在应用时产生的各类数据，并及时控制各类元器件做出反应。智能浇花器系统应用的主要元器件还有两类物体探测元件、LED 灯、RF522 RFID 射频识别模块、ST7735 LCD 显示屏等等。

在整体设计中的边缘运用了 LM393 比较器检测导体电阻，并设定导体电阻阈值，采集车位状态并发送给 Arduino 单片机。
Arduino 遍历每个传感器数据，Arduino 单片机接收后将采集的数据遍历完成，得到空余车位数量，并显示在 LCD 显示屏上。

本设计运用了 RF522 RFID 射频传感器来采集 IC 卡的 ID，并发送给Arduino单片机。单片机记录 ID，并记录时间。当后台数据找不到该 ID 的记录时，视为进入停车场，新建记录并设定入场时间；当后台数据存在该 ID 的记录，视为离开停车场，计算停车时间和费用，显示在 LCD 显示屏上。

利用 LCD 显示屏和 LED 灯可以直观看到停车场空余车位和计时计费信息。

### 智能停车场的功能
空余车位、自动计费
### 测量空余车位
比较器将电阻模拟信号与阈值比较，转化为数字信号发送给 Arduino。
Arduino 遍历每个检测单元，得到空余车位数量。
### 自动计费
射频传感器来采集 IC 卡的 ID，并发送给Arduino单片机。
单片机查找 ID。当后台数据找不到该 ID 的记录时，视为进入停车场，新建记录并设定入场时间；当后台数据存在该 ID 的记录，视为离开停车场，计算停车时间和费用。

## 智能停车场的硬件设计与实现
### 整体设计
### 液晶显示屏部分设计
ST7735 LCD 显示屏。

### 余位检测模块设计
电阻，LM393 比较器。

### 自动计费部分设计
RF522 射频模块。

## 智能停车场的软件设计
参见软件注释。
### 智能停车场系统软件整体设计
### 余位检测模块软件模块设计
### 系统显示模块的软件设计
https://www.arduino.cn/thread-16359-1-1.html
求助UTFT程序编译报错-Arduino中文社区 - Powered by Discuz!

### 自动计费软件设计
https://github.com/miguelbalboa/rfid
miguelbalboa/rfid: Arduino RFID Library for MFRC522

https://blog.csdn.net/ling3ye/article/details/71036479?depth_1-utm_source=distribute.pc_relevant.none-task&utm_source=distribute.pc_relevant.none-task
(2条消息)Arduino Uno + RFID 读取 把你的门禁卡读取看看有什么秘密_数据库_凌顺实验室-CSDN博客

## 智能停车场的整体实现

### 结束语
### 致谢
### 参考文献
### 附录