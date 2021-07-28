# STM32F10x 工程模板

## 目录结构
* EWARM - 存放 IAR 工程配置和目标文件等
* MDK-ARM - 存放 Keil 工程配置和目标文件等
* .stttings 目录、.cproject 和 .project - SW4STM32 或 TrueSTUDIO 生成的项目配置
* user - 存放主函数、中断处理等相关源码
* delay、usart、led 等 - 分别存放一些外设的驱动源码，这样放置可方便模块
* ../../Libraries - 这是标准外设库、开源库、启动文件等存放的地方，这些目录是可以复用的

## 新建项目
将 Project 目录下的 Template 目录拷贝一份，修改拷贝的目录名为新项目名。然后根据要用的 IDE 工具不同，编辑不同的项目配置文件。详见首页的 readme 文件。