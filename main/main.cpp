// #include <iostream>
// #include <driver/spi_master.h>
// #include <driver/gpio.h>
// #include "DK42688_SPI.h"

#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_timer.h"

using namespace std;

#define MOSI GPIO_NUM_11
#define MISO GPIO_NUM_13
#define SCLK GPIO_NUM_12
#define CS GPIO_NUM_10

#ifdef CONFIG_IDF_TARGET_ESP32
#define SENDER_HOST HSPI_HOST
#else
#define SENDER_HOST SPI2_HOST
#endif


// DK42688_SPI_Config spi_config = {
//     .miso = MISO,
//     .mosi = MOSI,
//     .sclk = SCLK,
//     .cs = CS
// };

extern "C" void app_main(void)
{
    esp_err_t ret;
    spi_device_handle_t handle;
    spi_bus_config_t buscfg = {};
    spi_device_interface_config_t devcfg = {};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    buscfg.mosi_io_num = MOSI;
    buscfg.miso_io_num = MISO;
    buscfg.sclk_io_num = SCLK;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;

    devcfg.command_bits = 8;
    devcfg.address_bits = 0;
    devcfg.dummy_bits = 0;
    devcfg.clock_speed_hz = 1000000;
    devcfg.duty_cycle_pos = 128;
    devcfg.mode = 3;
    devcfg.spics_io_num = CS;
    devcfg.cs_ena_posttrans = 0; // 3 bit cycles on CS activation after transmission
    devcfg.queue_size = 3;
    
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    assert(ret == ESP_OK);

    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &handle);
    assert(ret == ESP_OK);

    uint8_t sendbuf[1] = {0};
    uint8_t recvbuf[1] = {0};
    
    t.cmd = 0x11;
    sendbuf[0] = 0x01;
    t.length = 16;
    t.tx_buffer = sendbuf;
    t.rx_buffer = recvbuf;
    ret = spi_device_transmit(handle, &t);
    vTaskDelay(100);

    t.cmd = 0x75 | 0x80;
    sendbuf[0] = 0;
    while(1) {
        // int res = snprintf(sendbuf, sizeof(sendbuf), "Sender, transmission no. %04i. Last time, I received: \"%s\"", n, recvbuf);
        // if (res >= sizeof(sendbuf)) {
        //     printf("Data truncated\n");
        // }
        t.length = 16;               // Transmit only 8 bits (1 byte)
        // std::cout << t.length << endl;
        t.tx_buffer = sendbuf;      // Set tx_buffer to sendbuf
        t.rx_buffer = recvbuf;      // Set rx_buffer to recvbuf
        ret = spi_device_transmit(handle, &t);
        assert(ret == ESP_OK);

        // printf("Command sent: 0x%02X\n", t.cmd);
        // printf("Data sent: 0x%02X\n", sendbuf[0]);
        printf("Received data: 0x%02X\n", recvbuf[0]);

        // vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

