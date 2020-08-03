#Update Font

## 描述

该小项目基于标准外设库实现，采用 DMA 双缓存的方式将字库文件由串口发送,并存储在 FLASH 中，最后进行显示。

MCU STM32F407VET6
OLED SSD1306
FLASH W25Q16

![上电](picture/1.jpg)
按下 Key1 更新字体，按下 Key0 显示内容

## 发送字库文件

![XCOM界面](picture/2.png)

![发送](picture/3.jpg)
两个字库文件为 733k
这种方法发送两个字库文件的时间约为 47s

## 显示

![显示](picture/4.jpg)
可以看到，字库发送成功，OLED 屏幕上成功显示出设定的字符串

> 参考资料
> https://github.com/Zcj119303/STM32_FontUpdate
