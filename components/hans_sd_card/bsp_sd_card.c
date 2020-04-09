# include"bsp_sd_card.h"

BSP_SD_CONFIG BspSdConfig = 
{
    .mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    },
    .mount_point = MOUNT_POINT,
#ifndef USE_SPI_MODE
    .host = SDMMC_HOST_DEFAULT(),
    .slot_config = SDMMC_SLOT_CONFIG_DEFAULT(),
#else
    .host = SDSPI_HOST_DEFAULT(),
    .bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    },
    .slot_config = SDSPI_DEVICE_CONFIG_DEFAULT(),
#endif //USE_SPI_MODE
};

// sdmmc_card_t* card;
void SD_INIT(void)
{
    esp_err_t ret;
    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
//=========================变量迁移开始=========================
    // esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    //     .format_if_mount_failed = false,
    //     .max_files = 5,
    //     .allocation_unit_size = 16 * 1024
    // };
    // sdmmc_card_t* card;
    // const char mount_point[] = MOUNT_POINT;
//=========================变量迁移结束=========================
    ESP_LOGI(TAG, "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.
#ifndef USE_SPI_MODE
    ESP_LOGI(TAG, "Using SDMMC peripheral");
//=========================变量迁移开始=========================
    // sdmmc_host_t host = SDMMC_HOST_DEFAULT();
//=========================变量迁移结束=========================

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    // BspSdConfig.slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // To use 1-line SD mode, uncomment the following line:
    BspSdConfig.slot_config.width = 1;

    // GPIOs 15, 2, 4, 12, 13 should have external 10k pull-ups.
    // Internal pull-ups are not sufficient. However, enabling internal pull-ups
    // does make a difference some boards, so we do that here.
    gpio_set_pull_mode(15, GPIO_PULLUP_ONLY);   // CMD, needed in 4- and 1- line modes
    gpio_set_pull_mode(2, GPIO_PULLUP_ONLY);    // D0, needed in 4- and 1-line modes
    gpio_set_pull_mode(4, GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
    gpio_set_pull_mode(12, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only
    gpio_set_pull_mode(13, GPIO_PULLUP_ONLY);   // D3, needed in 4- and 1-line modes

    ret = esp_vfs_fat_sdmmc_mount(BspSdConfig.mount_point, &BspSdConfig.host, &BspSdConfig.slot_config, &BspSdConfig.mount_config, &BspSdConfig.card);
#else
    ESP_LOGI(TAG, "Using SPI peripheral");
//=========================变量迁移开始=========================
    // sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    // spi_bus_config_t bus_cfg = {
    //     .mosi_io_num = PIN_NUM_MOSI,
    //     .miso_io_num = PIN_NUM_MISO,
    //     .sclk_io_num = PIN_NUM_CLK,
    //     .quadwp_io_num = -1,
    //     .quadhd_io_num = -1,
    //     .max_transfer_sz = 4000,
    // };
//=========================变量迁移结束=========================   
    ret = spi_bus_initialize(BspSdConfig.host.slot, &BspSdConfig.bus_cfg, SPI_DMA_CHAN);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
//=========================变量迁移开始========================= 
    // sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
//=========================变量迁移结束=========================   
    BspSdConfig.slot_config.gpio_cs = PIN_NUM_CS;
    BspSdConfig.slot_config.host_id = BspSdConfig.host.slot;

    ret = esp_vfs_fat_sdspi_mount(BspSdConfig.mount_point, &BspSdConfig.host, &BspSdConfig.slot_config, &BspSdConfig.mount_config, &BspSdConfig.card);
#endif //USE_SPI_MODE

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }
printf("1\n");
    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, BspSdConfig.card);
}

void SD_UNMOUNT(void)
{
        // All done, unmount partition and disable SDMMC or SPI peripheral
    esp_vfs_fat_sdcard_unmount(BspSdConfig.mount_point, BspSdConfig.card);
    ESP_LOGI(TAG, "Card unmounted");
#ifdef USE_SPI_MODE
    //deinitialize the bus after all devices are removed
    spi_bus_free(BspSdConfig.host.slot);
#endif
}
//调用之前需要先sd卡初始化 SD_INIT();
void sd_text(void)
{
    
    
    // Use POSIX and C standard library functions to work with files.
    // First create a file.
    ESP_LOGI(TAG, "Opening file");
    FILE* f = fopen(MOUNT_POINT"/hello.txt", "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(f, "Hello %s!\n", BspSdConfig.card->cid.name);
    fclose(f);
    ESP_LOGI(TAG, "File written");

    // Check if destination file exists before renaming
    struct stat st;
    if (stat(MOUNT_POINT"/foo.txt", &st) == 0) {
        // Delete it if it exists
        unlink(MOUNT_POINT"/foo.txt");
    }

    // Rename original file
    ESP_LOGI(TAG, "Renaming file");
    if (rename(MOUNT_POINT"/hello.txt", MOUNT_POINT"/foo.txt") != 0) {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }

    // Open renamed file for reading
    ESP_LOGI(TAG, "Reading file");
    f = fopen(MOUNT_POINT"/foo.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);
    // strip newline
    char* pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read ILGH16XB.txt from file: '%s'", line);


//读取文件列表
    printf("读取文件列表\n");
    int res;
    FILINFO info;
    FF_DIR file_dir;
    res = f_opendir(&file_dir, "/font");
    /* read picture file */
    while (1) {
        res = f_readdir(&file_dir, &info);
        printf("read dir, res=%d, fname=%s\n", res, info.fname);
        if (res || info.fname[0] == 0)
            break;
        if (info.fname[0] == '.')
            continue;       
    }
    f_closedir(&file_dir);
}
