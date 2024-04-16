#include <iostream>
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "DK42688_SPI.h"

using namespace std;

#define MOSI GPIO_NUM_11
#define MISO GPIO_NUM_13
#define SCLK GPIO_NUM_12
#define CS GPIO_NUM_10

DK42688_SPI_Config spi_config = {
    .miso = MISO,
    .mosi = MOSI,
    .sclk = SCLK,
    .cs = CS
};

extern "C" void app_main(void)
{   
    DK42688_SPI spi(&spi_config);
    spi.begin();
    while(1){
        spi.reset();
    }
}

