#include <iostream>
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "DK42688_SPI.h"
#include "esp_log.h"

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
    while(1) {
        double ax = spi.get_accel_x();
        double ay = spi.get_accel_y();
        double az = spi.get_accel_z();
        cout << "Accel X: " << ax << endl;
        cout << "Accel Y: " << ay << endl;
        cout << "Accel Z: " << az << endl;
        double gx = spi.get_gyro_x();
        double gy = spi.get_gyro_y();
        double gz = spi.get_gyro_z();
        cout << "Gyro X: " << gx << endl;
        cout << "Gyro Y: " << gy << endl;
        cout << "Gyro Z: " << gz << endl;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}   

