#include "DK42688_SPI.h"

DK42688_SPI::DK42688_SPI(DK42688_SPI_Config *spi_config) {
    buscfg.mosi_io_num = spi_config->mosi;
    buscfg.miso_io_num = spi_config->miso;
    buscfg.sclk_io_num = spi_config->sclk;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;

    devcfg.command_bits = 0;
    devcfg.address_bits = 0;
    devcfg.dummy_bits = 0;
    devcfg.clock_speed_hz = 24000000;
    devcfg.duty_cycle_pos = 128;
    devcfg.mode = 3;
    devcfg.spics_io_num = spi_config->cs;
    devcfg.cs_ena_posttrans = 3; // 3 bit cycles on CS activation after transmission
    devcfg.queue_size = 3;
}   

esp_err_t DK42688_SPI::begin() {
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if(ret != ESP_OK) return ret;
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &handle);
    return ret;
}

void DK42688_SPI::test() {
    sendbuf[0] = 0x12;
    t.length = sizeof(sendbuf) * 8;
    t.tx_buffer = sendbuf;
    t.rx_buffer = recvbuf;
    ret = spi_device_transmit(handle, &t);
}