1.主控芯片，stm32f103ze   512k flash    64k ram
2.带触摸的液晶屏320*480
3.触摸电容不用校准，电阻需要校准，校准值存在外部eeprom(iic),当前电阻屏
4.电阻屏显示使用fsmc nor/sram  bank4    触摸使用spi
5.lvgl占据ram太大，所以使用的是fsmc外扩ram（1M），使用的是 noR/sram bank3
6.操作系统：freertos    
7.ui系统：lvgl