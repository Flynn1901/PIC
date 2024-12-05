/* 
 * File:   nrf24.h
 * Author: Admin
 *
 * Created on December 5, 2024, 9:17 PM
 */

#ifndef NRF24_H
#define	NRF24_H

#include "mcc_generated_files/system/system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

    //CE-PC6 CNS-PC7 SCK-PC3 MOSI-PC5 MISO-PC4
// ?? nRF24L01 ? GPIO ??
#define NRF_CE_PIN 6
#define NRF_CSN_PIN 7

// nRF24L01 ?????
#define NRF_REG_CONFIG      0x00
#define NRF_REG_EN_AA       0x01
#define NRF_REG_RF_SETUP    0x06
#define NRF_REG_RF_CH       0x05
#define NRF_REG_STATUS      0x07
#define NRF_REG_TX_ADDR     0x10
#define NRF_REG_RX_ADDR_P0  0x0A
#define NRF_REG_RX_PW_P0    0x11

// nRF24L01 ??
#define NRF_CMD_W_REGISTER  0x20
#define NRF_CMD_R_REGISTER  0x00
#define NRF_CMD_FLUSH_TX    0xE1
#define NRF_CMD_W_TX_PAYLOAD 0xA0


void CEH(void);

void CEL(void);

void CSH(void);

void CSL(void);

void nrf_gpio_init(void);

// SPI ????
void nrf_write_register(uint8_t,uint8_t);

// SPI ???????????
void nrf_write_register_multi(uint8_t, uint8_t *, size_t);

// SPI ? TX PAYLOAD??????
void nrf_write_payload(uint8_t *, size_t);

void nrf_read_register_multi(uint8_t, uint8_t *, size_t);

uint8_t nrf_read_register(uint8_t);

void nrf_init(void);

// ????
void nrf_send_data(uint8_t *, size_t);

void nrf_check_configuration(void);

#ifdef	__cplusplus
}
#endif

#endif	/* NRF24_H */

