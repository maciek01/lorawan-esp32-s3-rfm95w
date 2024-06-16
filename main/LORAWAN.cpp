/*******************************************************************************
 * 
 * ttn-esp32 - The Things Network device library for ESP-IDF / SX127x
 * 
 * Copyright (c) 2018 Manuel Bleichenbacher
 * 
 * Licensed under MIT License
 * https://opensource.org/licenses/MIT
 *
 * Sample program showing how to send and receive messages.
 *******************************************************************************/

#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include <string.h>

#include "TheThingsNetwork.h"
#include "wifi_sta.h"

// NOTE:
// The LoRaWAN frequency and the radio chip must be configured by running 'idf.py menuconfig'.
// Go to Components / The Things Network, select the appropriate values and save.

// Copy the below hex strings from the TTN console (Applications > Your application > End devices
// > Your device > Activation information)

// AppEUI (sometimes called JoinEUI)
const char *appEui = "0000000000000000";
// DevEUI
//const char *devEui = "ab493c97ea90bed3"; // CHIRP
const char *devEui = "70B3D57ED0064033"; // TTN

// AppKey
//const char *appKey = "d2a438a643017caa73d7fb74532152b6";// CHIRP
const char *appKey = "2A79C77D5B0CFE48DA45AFB64E06DAA6";// TTN

// Pins and other resources
//SPI
#define TTN_SPI_HOST      SPI2_HOST
#define TTN_SPI_DMA_CHAN  SPI_DMA_CH_AUTO//SPI_DMA_DISABLED
//#define TTN_SPI_DMA_CHAN  1
#define TTN_PIN_SPI_SCLK  GPIO_NUM_12//5//lora module - 36
#define TTN_PIN_SPI_MOSI  GPIO_NUM_11//27//lora module - 35
#define TTN_PIN_SPI_MISO  GPIO_NUM_13//19//lora module - 37
#define TTN_PIN_NSS       GPIO_NUM_10//18//lora module - 34
//RADIO
#define TTN_PIN_RXTX      TTN_NOT_CONNECTED
#define TTN_PIN_RST       GPIO_NUM_15//14//lora module - 38
#define TTN_PIN_DIO0      GPIO_NUM_1//26
#define TTN_PIN_DIO1      GPIO_NUM_4//35


static TheThingsNetwork ttn;

const unsigned TX_INTERVAL = 1;//30;
static uint8_t msgData[] = "Hello, world";


void sendMessages(void* pvParameter)
{
    while (1) {
        printf("Sending message...\n");
        TTNResponseCode res = ttn.transmitMessage(msgData, sizeof(msgData) - 1);
        printf(res == kTTNSuccessfulTransmission ? "Message sent.\n" : "Transmission failed.\n");

        vTaskDelay(TX_INTERVAL * pdMS_TO_TICKS(1000));
    }
}

void messageReceived(const uint8_t* message, size_t length, ttn_port_t port)
{
    printf("Message of %d bytes received on port %d:", length, port);
    for (int i = 0; i < length; i++)
        printf(" %02x", message[i]);
    printf("\n");
}

extern "C" void app_main(void)
{

    //connect to WIFI
    connectWIFI("lorawan");


    esp_err_t err;
    // Initialize the GPIO ISR handler service
    err = gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    ESP_ERROR_CHECK(err);
    
    // Initialize the NVS (non-volatile storage) for saving and restoring the keys
    err = nvs_flash_init();
    ESP_ERROR_CHECK(err);

    // Initialize SPI bus
    spi_bus_config_t spi_bus_config;
    memset(&spi_bus_config, 0, sizeof(spi_bus_config));
    spi_bus_config.miso_io_num = TTN_PIN_SPI_MISO;
    spi_bus_config.mosi_io_num = TTN_PIN_SPI_MOSI;
    spi_bus_config.sclk_io_num = TTN_PIN_SPI_SCLK;
    err = spi_bus_initialize(TTN_SPI_HOST, &spi_bus_config, TTN_SPI_DMA_CHAN);
    ESP_ERROR_CHECK(err);

    // Configure the SX127x pins
    ttn.configurePins(TTN_SPI_HOST, TTN_PIN_NSS, TTN_PIN_RXTX, TTN_PIN_RST, TTN_PIN_DIO0, TTN_PIN_DIO1);

    ttn.setSubband(0);
    // The below line can be commented after the first run as the data is saved in NVS
    ttn.provision(devEui, appEui, appKey);

    // Register callback for received messages
    ttn.onMessage(messageReceived);

    //ttn.setAdrEnabled(false);
    //ttn.setDataRate(kTTNDataRate_US915_SF7);
    ttn.setMaxTxPower(14);

    printf("Joining...\n");
    if (ttn.join())
    {
        printf("Joined.\n");
        xTaskCreate(sendMessages, "send_messages", 1024 * 4, (void* )0, 3, nullptr);
    }
    else
    {
        printf("Join failed. Goodbye\n");
    }
}