#include "nrf24.h"


void CEH(){
    LATC = LATC|(1<<NRF_CE_PIN);
}

void CEL(){
    LATC = LATC & ~(1<<NRF_CE_PIN);
}

void CSH(){
    LATC = LATC|(1<<NRF_CSN_PIN);
}

void CSL(){
    LATC = LATC & ~(1<<NRF_CSN_PIN);
}


void nrf_gpio_init() {
    CEL();   // ???? CE
    CSH(); // ???? CSN
}

// SPI ????
void nrf_write_register(uint8_t reg, uint8_t value) {
    uint8_t tx_data[2] = { NRF_CMD_W_REGISTER | reg, value };
    
    CSL(); // ?? CSN ????
    SPI1_BufferWrite(tx_data, 2);
    CSH(); // ?? CSN ????
}

// SPI ???????????
void nrf_write_register_multi(uint8_t reg, uint8_t *data, size_t length) {
    
    uint8_t tx_data = NRF_CMD_W_REGISTER | reg;
    SPI1_ByteWrite(tx_data);
    
    for(int i=0;i<length;i++){
        SPI1_ByteWrite(*(data+i));
    }
    CSH(); // ?? CSN ????
}

// SPI ? TX PAYLOAD??????
void nrf_write_payload(uint8_t *data, size_t length) {
    CSL();
    uint8_t cmd = NRF_CMD_W_TX_PAYLOAD;
    SPI1_ByteWrite(cmd);
    
    for(int i=0;i<length;i++){
        SPI1_ByteWrite(*(data+i));
    }
    CSH(); // ?? CSN ????
}

void nrf_read_register_multi(uint8_t reg, uint8_t *data, size_t length) {
    CSL();
    uint8_t tx_data = NRF_CMD_R_REGISTER | reg; // ????????????
    SPI1_ByteWrite(tx_data);
    uint8_t *buffer;
    
    SPI1_BufferRead(buffer, length);
    CSH();
}

uint8_t nrf_read_register(uint8_t reg) {
    CSL();
    uint8_t tx_data = NRF_CMD_R_REGISTER | reg;
    SPI1_ByteWrite(tx_data);
    SPI1_ByteWrite(0xFF);
    uint8_t readbyte = SPI1_ByteRead();
    CSH();
    return readbyte;
}

void nrf_init() {
    // ?? nRF24L01 ????
    nrf_write_register(NRF_REG_CONFIG, 0x0E);   // ??????????? CRC
    nrf_write_register(NRF_REG_RF_SETUP, 0x06); // ?? RF ?????????
    nrf_write_register(NRF_REG_RF_CH, 0x02);    // ?? RF ??

    uint8_t tx_address[5] = { 0xE7, 0xC9, 0x3F, 0x03, 0x00 }; // TX ??
    nrf_write_register_multi(NRF_REG_TX_ADDR, tx_address, 5);
    nrf_write_register_multi(NRF_REG_RX_ADDR_P0, tx_address, 5); // RX ??? TX ????

    nrf_write_register(NRF_REG_EN_AA, 0x01); // ??????
    nrf_write_register(NRF_REG_RX_PW_P0, 0x20); // ????????? 32 ??
}

// ????
void nrf_send_data(uint8_t *data, size_t length) {
    uint8_t full_data[32];
    memset(full_data, 0, sizeof(full_data)); // ???? 0
    memcpy(full_data, data, length);        // ????

    nrf_write_payload(full_data, sizeof(full_data)); // ?? 32 ????
    CEH();   // ?? CE ????
    __delay_ms(1);    // ?? 1ms
    CEL();   // ?? CE ????

    uint8_t status = nrf_read_register(NRF_REG_STATUS);
    printf("STATUS: 0x%02X\n", status);

    // ????????
    if (status & (1 << 5)) { // TX_DS (????)
        printf("Data sent successfully!\n");
    } else if (status & (1 << 4)) { // MAX_RT (??????)
        printf("Max retransmission reached. Flushing TX.\n");
        nrf_write_register(NRF_CMD_FLUSH_TX, 0); // ??????
    }

    // ??????
    nrf_write_register(NRF_REG_STATUS, 0x70); // ?? RX_DR, TX_DS, MAX_RT ??
}

void nrf_check_configuration(void) {
    printf("=== Checking nRF24L01 Configuration ===\n");

    // ?? CONFIG ???
    uint8_t config = nrf_read_register(NRF_REG_CONFIG);
    printf("CONFIG: 0x%02X\n", config);
    if ((config & 0x0F) != 0x0E && (config & 0x0F) != 0x0F) {
        printf("ERROR: CONFIG register is not properly configured (0x%02X)\n", config);
    } else {
        printf("CONFIG register is properly configured.\n");
    }

    // ?? EN_AA ???
    uint8_t en_aa = nrf_read_register(NRF_REG_EN_AA);
    printf("EN_AA: 0x%02X (Auto Acknowledgment)\n", en_aa);

    // ?? RF_SETUP ???
    uint8_t rf_setup = nrf_read_register(NRF_REG_RF_SETUP);
    printf("RF_SETUP: 0x%02X (RF settings)\n", rf_setup);

    // ?? RF_CH ???
    uint8_t rf_ch = nrf_read_register(NRF_REG_RF_CH);
    printf("RF_CH: 0x%02X (RF channel)\n", rf_ch);

    // ?? STATUS ???
    uint8_t status = nrf_read_register(NRF_REG_STATUS);
    printf("STATUS: 0x%02X\n", status);

    // ?? RX_ADDR_P0 (????)
    uint8_t rx_address[5];
    nrf_read_register_multi(NRF_REG_RX_ADDR_P0, rx_address, 5);
    printf("RX_ADDR_P0: ");
    for (int i = 0; i < 5; i++) {
        printf("%02X ", rx_address[i]);
    }
    printf("\n");

    // ?? TX_ADDR (????)
    uint8_t tx_address[5];
    nrf_read_register_multi(NRF_REG_TX_ADDR, tx_address, 5);
    printf("TX_ADDR: ");
    for (int i = 0; i < 5; i++) {
        printf("%02X ", tx_address[i]);
    }
    printf("\n");

    printf("=== Configuration Check Complete ===\n");
}


