1、这是选择采用官方例程（https://github.com/espressif/esp-idf/tree/master/examples/storage/sd_card），期间经历了很多曲折。官方例程，按照提示使用spi通信协议，但是一直提示错误。我前前后后换了esp32板子（同一个型号），新的sd卡模块（同一款https://item.taobao.com/item.htm?spm=a230r.1.14.43.7bbf1a39pFEXX0&id=526928470015&ns=1&abbucket=6#detail），不同的sd卡（考虑大小，fat支持大小有区别。考虑有TF卡和micro sd区别）还是不行，所以我猜想是因为sd卡模块的问题，因为这个sd卡多了一个芯片，可能通信协议不同。换成这款（https://detail.tmall.com/item.htm?id=583120549732&spm=a1z09.2.0.0.42a82e8d7q9oar&_u=i2hdv2b85d83），强烈建议这款。
	曲折1：sd卡模块选择有问题，导致在这上面耽误了一个星期。
	解决办法1：使用这款sd卡模块，不需要再加上拉电阻。https://detail.tmall.com/item.htm?id=583120549732&spm=a1z09.2.0.0.42a82e8d7q9oar&_u=i2hdv2b85d83）
   换成这款就没有问题，什么上拉电阻都不需要。直接按照官方的接线就行，官网例程提示更改就行（就需要去掉一个注释）。
2、官方移植的fatfs，但是没有给api介绍，直接给了fatfs的官网网址，但是esp32移植fatfs后重写了stdio.h文件，统一为标准c接口。所以就形成了打开文件fatfs使用f_open,而esp32使用fopen的区别，还有很多都是这样，所以我只能转观c语言标准文件读取的函数说明。这里也建议去看看c语言标准的文件操作函数。
	曲折2：fatfs官方提供文档和esp32移植的文件操作函数协议不同。
	解决办法2：使用POSIX and C标准的文件操作函数（Use POSIX and C standard library functions to work with files.）
3、fatfs移植后默认只支持短文件名，就是8.3的格式，也就意味着，文件读取只能读取文件名八个字符以内的，后缀名小于三个字符的文件。但那是再读取html文件的时候，后缀名是html，所以读取失败。在这个问题上我耽误了一天，晚上熬到两点多……。只需要修改menuconfig选项就行
	曲折3：fatfs默认支持文件名为8.3格式，无法读取后缀名为html的文件
	解决办法：在工程根目录运行
				make menuconfig
			选择Component config选项
			选择FAT Filesystem support选项
			出现

	│ │		    OEM Code Page (US (CP437))  --->                    │ │  编码，可以选择中文，但是为了避免不必要麻烦，还是使用英文为好
  	│ │        Long filename support (Long filename buffer in heap)  --->   │ │  这个是重点，选择长文件名
  	│ │    (255) Max long filename length                                   │ │  
  	│ │        API character encoding (API uses ANSI/OEM encoding)  --->    │ │  
  	│ │    (0) Number of simultaneously open files protected by lock functio│ │  
  	│ │    (10000) Timeout for acquiring a file lock, ms                    │ │  
  	│ │    [*] Use separate cache for each file     			│ │
	曲折4：果云esp32-wroom核心板使用sdmmc模式无法运行成功
	解决办法：焊掉gio2旁边的阻值为512的电阻（经过各方面的核查，我查看原理图发现果云esp32-wroom在gpio2引脚加了一个5k下拉电阻，在没有pcb源档的情况下，我核实发现，这个电阻就是模组旁边的512电阻，我焊下来之后，发现程序就可以跑通了）
	曲折5：插上sd卡之后无法下载程序，也无法运行程序
	解决办法：因为下载程序需要gpio2拉低，但是使用sd卡有需要拉高，所以插上sd卡相当于将gpio2拉高，所以无法下载。只需要在下载程序事将sd卡的vcc断电即可。至于无法运行程序是因为和曲折4一样的原因。只需要解决曲折4即正常

4、函数说明
	函数名				函数原型								参数解释							示例
	1.fopen()			FILE* f = fopen(path, mobel);		path="路径"，mobel="模式" 		FILE* f = fopen("/sdcard/a.html", "r");
	2.stat()			stat(path, &st)						path="路径"st是返回值，有没有		struct stat st;stat(path, &st) == 0;
	3.fgets()			fgets(line, sizeof(line), f)										

