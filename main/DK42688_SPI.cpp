#include "DK42688_SPI.h"

DK42688_SPI::DK42688_SPI(const spi_host_device_t spi_peripheral, DK42688_SPI_Config *spi_config) {
    esp_err_t ret = ESP_OK;
    host = spi_peripheral; // Configuring SPI host inside ESP32

    transaction.tx_buffer = nullptr; // ptr to the memory buffer to be sent to slave device from ESP32
    transaction.rx_buffer = nullptr; // ptr to the memory buffer to be received from slave device to ESP32
    _spi_bus_cfg.mosi_io_num = spi_config->mosi; // Master Out Slave In
    _spi_bus_cfg.miso_io_num = spi_config->miso; // Master In Slave Out
    _spi_bus_cfg.sclk_io_num = spi_config->sclk; // Serial Clock
    _spi_bus_cfg.quadwp_io_num = -1; // Quad Write Protect disabled
    _spi_bus_cfg.quadhd_io_num = -1; // Quad Hold disabled
    _spi_bus_cfg.max_transfer_sz = 100; // Maximum transfer size in bytes

    ret = spi_bus_initialize(host, &_spi_bus_cfg, SPI_DMA_CH_AUTO); // Initialize the SPI bus
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "spi_bus_initialize failed: %s", esp_err_to_name(ret));
    }

    _spi_interface_cfg.command_bits = 0;
    _spi_interface_cfg.address_bits = 0;
    _spi_interface_cfg.dummy_bits = 0;
    _spi_interface_cfg.mode = 3;
    _spi_interface_cfg.duty_cycle_pos = 128; // 50% duty cycle
    _spi_interface_cfg.cs_ena_pretrans = 0; // number of cycles the CS should be active before the transmission
    _spi_interface_cfg.cs_ena_posttrans = 0; // number of cycles the CS should be active after the transmission
    _spi_interface_cfg.spics_io_num = spi_config->cs;
    _spi_interface_cfg.queue_size = 5; // max number of transactions that can be queued
    _spi_interface_cfg.pre_cb = NULL;
    _spi_interface_cfg.post_cb = NULL;
    _spi_interface_cfg.clock_speed_hz = 24000000; // 24 MHz
    ret = spi_bus_add_device(host, &_spi_interface_cfg, &_handle); // Add a device to the SPI bus
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "spi_bus_add_device failed: %s", esp_err_to_name(ret));
    }
}

esp_err_t DK42688_SPI::transferByte(const uint8_t reg_addr, const uint8_t data, bool readWrite) {
    transaction.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
    // transaction.cmd = reg_addr & 0x80;
    // print cmd
    ESP_LOGI(TAG, "cmd: %d", transaction.cmd);
    transaction.length = 8;
    transaction.tx_data[0] = reg_addr & 0x80;
    return spi_device_transmit(_handle, &transaction);
}


uint8_t DK42688_SPI::ReadRegister(const uint8_t reg_addr) {
    transferByte(reg_addr, 0, true);
    return transaction.rx_data[0];
}

esp_err_t DK42688_SPI::WriteRegister(const uint8_t reg_addr, const uint8_t reg_data) {
    esp_err_t status{ESP_OK};
    status |= transferByte(reg_addr, reg_data, false);
    return status;
}

esp_err_t DK42688_SPI::who_am_i() {
    uint8_t who_am_i = ReadRegister(ICM42688reg::WHO_AM_I);
    if (who_am_i != 0x47) {
        ESP_LOGE(TAG, "WHO_AM_I register read failed: %x", who_am_i);
        return ESP_FAIL;
    }
    return ESP_OK;
}

void DK42688_SPI::begin() {
    esp_err_t ret = ESP_OK;
    // ret = WriteRegister(ICM42688reg::DEVICE_CONFIG, 0x01); // Reset the device
    vTaskDelay(100 / portTICK_PERIOD_MS);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Device reset failed");
    }
    ret = who_am_i();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "WHO_AM_I register read failed");
    }
}



