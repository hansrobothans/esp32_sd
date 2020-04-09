/* SD card and FAT filesystem example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#ifndef BSP_SD_CARD_H
#define BSP_SD_CARD_H "BSP_SD_CARD_H"

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "driver/sdmmc_host.h"
#endif

#define TAG  "example"

#define MOUNT_POINT "/sdcard"

// This example can use SDMMC and SPI peripherals to communicate with SD card.
// By default, SDMMC peripheral is used.
// To enable SPI mode, uncomment the following line:

// #define USE_SPI_MODE

// ESP32-S2 doesn't have an SD Host peripheral, always use SPI:
#ifdef CONFIG_IDF_TARGET_ESP32S2
#ifndef USE_SPI_MODE
#define USE_SPI_MODE
#endif // USE_SPI_MODE
// on ESP32-S2, DMA channel must be the same as host id
#define SPI_DMA_CHAN    host.slot
#endif //CONFIG_IDF_TARGET_ESP32S2

// DMA channel to be used by the SPI peripheral
#ifndef SPI_DMA_CHAN
#define SPI_DMA_CHAN    1
#endif //SPI_DMA_CHAN

// When testing SD and SPI modes, keep in mind that once the card has been
// initialized in SPI mode, it can not be reinitialized in SD mode without
// toggling power to the card.

#ifdef USE_SPI_MODE
// Pin mapping when using SPI mode.
// With this mapping, SD card can be used both in SPI and 1-line SD mode.
// Note that a pull-up on CS line is required in SD mode.
#define PIN_NUM_MISO CONFIG_D0_MISO_GPIO
#define PIN_NUM_MOSI CONFIG_CMD_MOSI_GPIO
#define PIN_NUM_CLK  CONFIG_CLK_GPIO
#define PIN_NUM_CS   CONFIG_D3_CS_GPIO
// #define PIN_NUM_MISO 2
// #define PIN_NUM_MOSI 15
// #define PIN_NUM_CLK  14
// #define PIN_NUM_CS   13
#endif //USE_SPI_MODE


struct SD_CONFIG
{
	esp_vfs_fat_sdmmc_mount_config_t mount_config;
	sdmmc_card_t* card;
	char mount_point[64];
#ifndef USE_SPI_MODE
	sdmmc_host_t host;
	sdmmc_slot_config_t slot_config;
#else
	sdmmc_host_t host;
	spi_bus_config_t bus_cfg;
	sdspi_device_config_t slot_config;
#endif //USE_SPI_MODE
};
typedef struct SD_CONFIG BSP_SD_CONFIG;

extern BSP_SD_CONFIG BspSdConfig;

void SD_INIT(void);
void SD_UNMOUNT(void);
void sd_text(void);

#endif//BSP_SD_CARD_H	