# Solution

Solution to the assignment.

# eeprom.c

```C
void eeprom_read(uint8_t address, uint8_t *buffer, const uint32_t n)
{
    // READ Read from memory array 0000X011
    f_push(&tx, 0x03); // Instruction
    f_push(&tx, address); // Address

    // Copy dummy data to transmit FIFO
    for(uint32_t i=0; i<n; i++)
    {
        f_push(&tx, 0x00);
    }

    // Start transfer, TXFIFO is already filled
    lpspi1_tx(NULL, 0, 0);

    // Read the data from the RXFIFO
    lpspi1_rx_read(buffer, n);
}

void eeprom_write(uint8_t address, uint8_t *buffer, const uint32_t n)
{
    // WRITE Write to memory array 0000X010
    f_push(&tx, 0x02); // Instruction
    f_push(&tx, address); // Address

    // Write the data, ignore data from EEPROM
    lpspi1_tx(buffer, n, 1);
}

uint8_t eeprom_rdsr(void)
{
    // RDSR Read Status register 0000 X101
    uint8_t tx_data[2] = {0x05, 0x00};
    uint8_t rx_data[2] = {0x00, 0x00};
    
    lpspi1_tx(tx_data, sizeof(tx_data), 0);
    lpspi1_rx_read(rx_data, sizeof(rx_data));

    return rx_data[1];
}

bool eeprom_wip(void)
{
    uint8_t sr = eeprom_rdsr();
    
    return (sr & 0x01) != 0;
}

void eeprom_we(bool wel)
{
    // WREN Write enable 0000X110
    // WRDI Write disable 0000X100

    uint8_t tx_data[1] = {wel ? 0x06 : 0x04};
    uint8_t rx_data[1] = {0x00};
    
    lpspi1_tx(tx_data, sizeof(tx_data), 0);
    lpspi1_rx_read(rx_data, sizeof(rx_data));
}

```
