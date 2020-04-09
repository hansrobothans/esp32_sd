#include "bsp_esp32.h"



void app_main(void)
{
	//sd卡初始化
	SD_INIT();
	//sd卡测试函数
	sd_text();
	//卸载sd卡
	// SD_UNMOUNT();
	
	while(1)
	{
		sd_text();
		vTaskDelay(400);
	}
	
}
