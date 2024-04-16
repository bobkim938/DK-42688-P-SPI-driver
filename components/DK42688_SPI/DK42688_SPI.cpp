#include "DK42688_SPI.h"

DK42688_SPI::DK42688_SPI(DK42688_SPI_Config *spi_config) {
    buscfg.mosi_io_num = spi_config->mosi;
    buscfg.miso_io_num = spi_config->miso;
    buscfg.sclk_io_num = spi_config->sclk;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;

    devcfg.command_bits = 8;
    devcfg.address_bits = 0;
    devcfg.dummy_bits = 0;
    devcfg.clock_speed_hz = 1000000; // 1 MHz
    devcfg.duty_cycle_pos = 128;
    devcfg.mode = 3;
    devcfg.spics_io_num = spi_config->cs;
    devcfg.cs_ena_posttrans = 0; // 3 bit cycles on CS activation after transmission
    devcfg.queue_size = 3;
}   

esp_err_t DK42688_SPI::read_spi(uint8_t reg) {
    t.length = 16;
    t.cmd = reg | 0x80;
    t.tx_buffer = 0;
    t.rx_buffer = recvbuf;
    ret = spi_device_transmit(handle, &t);
    return ret;
}

esp_err_t DK42688_SPI::write_spi(uint8_t reg, uint8_t data, uint8_t len) {
    t.length = len * 8;
    t.cmd = reg;
    sendbuf[0] = data;
    t.tx_buffer = sendbuf;
    t.rx_buffer = 0;
    ret = spi_device_transmit(handle, &t);
    return ret;
}

esp_err_t DK42688_SPI::begin() {
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if(ret != ESP_OK) return ret;
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &handle);
    if(ret != ESP_OK) return ret;
    ret = who_am_i();
    if(ret != ESP_OK) return ret;
    ret = write_spi(ICM42688reg::PWR_MGMT0, 0x0f, 2); // turn on gyro and accel sensors in LN mode
    if(ret != ESP_OK) return ret;
    ret = read_spi(ICM42688reg::PWR_MGMT0);
    printf("Received data: 0x%02X\n", recvbuf[0]);
    return ret;
}

esp_err_t DK42688_SPI::reset() {
    ret = write_spi(ICM42688reg::DEVICE_CONFIG, 0x01, 2);
    return ret;
}

esp_err_t DK42688_SPI::who_am_i() {
    ret = read_spi(ICM42688reg::WHO_AM_I);
    printf("Received data: 0x%02X\n", recvbuf[0]);
    assert(recvbuf[0] == 0x47);
    return ret;
}

double DK42688_SPI::get_accel_x() {
    read_spi(ICM42688reg::ACCEL_DATA_X0);
    int16_t accel_data_x0 = recvbuf[0];
    read_spi(ICM42688reg::ACCEL_DATA_X1);
    int16_t accel_data_x1 = recvbuf[0];
    int16_t accel_x_raw = (accel_data_x1 << 8) | accel_data_x0;
    double acc_x = (accel_x_raw * 16/32768.0) * 9.81;
    return acc_x;
}

double DK42688_SPI::get_accel_y() {
    read_spi(ICM42688reg::ACCEL_DATA_Y0);
    int16_t accel_data_y0 = recvbuf[0];
    read_spi(ICM42688reg::ACCEL_DATA_Y1);
    int16_t accel_data_y1 = recvbuf[0];
    int16_t accel_y_raw = (accel_data_y1 << 8) | accel_data_y0;
    double acc_y = (accel_y_raw * 16/32768.0) * 9.81;
    return acc_y;
}

double DK42688_SPI::get_accel_z() {
    read_spi(ICM42688reg::ACCEL_DATA_Z0);
    int16_t accel_data_z0 = recvbuf[0];
    read_spi(ICM42688reg::ACCEL_DATA_Z1);
    int16_t accel_data_z1 = recvbuf[0];
    int16_t accel_z_raw = (accel_data_z1 << 8) | accel_data_z0;
    double acc_z = (accel_z_raw * 16/32768.0) * 9.81;
    return acc_z;
}

double DK42688_SPI::get_gyro_x() {
    read_spi(ICM42688reg::GYRO_DATA_X0);
    int16_t gyro_data_x0 = recvbuf[0];  
    read_spi(ICM42688reg::GYRO_DATA_X1);
    int16_t gyro_data_x1 = recvbuf[0];  
    int16_t gyro_x_raw = (gyro_data_x1 << 8) | gyro_data_x0;  
    double gyro_x = gyro_x_raw * (2000.0 / 32768.0);  
    return gyro_x;
}

double DK42688_SPI::get_gyro_y() {
    read_spi(ICM42688reg::GYRO_DATA_Y0);
    int16_t gyro_data_y0 = recvbuf[0];  
    read_spi(ICM42688reg::GYRO_DATA_Y1);
    int16_t gyro_data_y1 = recvbuf[0];  
    int16_t gyro_y_raw = (gyro_data_y1 << 8) | gyro_data_y0;  
    double gyro_y = gyro_y_raw * (2000.0 / 32768.0);  
    return gyro_y;
}

double DK42688_SPI::get_gyro_z() {
    read_spi(ICM42688reg::GYRO_DATA_Z0);
    int16_t gyro_data_z0 = recvbuf[0]; 
    read_spi(ICM42688reg::GYRO_DATA_Z1);
    int16_t gyro_data_z1 = recvbuf[0]; 
    int16_t gyro_z_raw = (gyro_data_z1 << 8) | gyro_data_z0;  
    double gyro_z = gyro_z_raw * (2000.0 / 32768.0);  
    return gyro_z;
}



