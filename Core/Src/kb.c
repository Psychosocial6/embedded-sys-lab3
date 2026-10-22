#include "main.h"
#include "pca9538.h"
#include "kb.h"
#include "i2c.h"
#include "sdk_uart.h"
#include "usart.h"

#define KBRD_ADDR 0xE2

HAL_StatusTypeDef Set_Keyboard( void ) {
	HAL_StatusTypeDef ret = HAL_OK;
	uint8_t buf;

	buf = 0;
	ret = PCA9538_Write_Register(KBRD_ADDR, POLARITY_INVERSION, &buf);
	if( ret != HAL_OK ) {
		UART_Transmit((uint8_t*)"Error write polarity\n");
		goto exit;
	}

	buf = 0;
	ret = PCA9538_Write_Register(KBRD_ADDR, OUTPUT_PORT, &buf);
	if( ret != HAL_OK ) {
		UART_Transmit((uint8_t*)"Error write output\n");
	}

exit:
	return ret;
}

uint8_t Check_Row( uint8_t  Nrow ) {
    uint8_t Nkey = 0x00;
    HAL_StatusTypeDef ret = HAL_OK;
    uint8_t buf;
    uint8_t kbd_in;

    ret = Set_Keyboard();
    if( ret != HAL_OK ) {
        return 0;
    }

    buf = Nrow;
    ret = PCA9538_Write_Register(KBRD_ADDR, CONFIG, &buf);
    if( ret != HAL_OK ) {
        return 0;
    }

    ret = PCA9538_Read_Inputs(KBRD_ADDR, &buf);
    if( ret != HAL_OK ) {
        return 0;
    }

    kbd_in = buf & 0x70;
    Nkey = kbd_in;
    if( kbd_in != 0x70) {
        if( !(kbd_in & 0x10) ) {
            Nkey = 0x04; // Left column
        } else if( !(kbd_in & 0x20) ) {
            Nkey = 0x02; // Center column
        } else if( !(kbd_in & 0x40) ) {
            Nkey = 0x01; // Right column
        }
    }
    else Nkey = 0x00;

    return Nkey;
}

char Scan_Keyboard( void ) {
    uint8_t Row[4] = {ROW1, ROW2, ROW3, ROW4};
    I2C1_Lock();
    const char keymap[4][3] = {
        {'1', '2', '3'},
        {'4', '5', '6'},
        {'7', '8', '9'},
        {'O', '0', 'E'}
    };

    for (int i = 0; i < 4; i++) {
        uint8_t key = Check_Row(Row[i]);
        if (key != 0) {
            if (key == 0x04) { I2C1_Unlock(); return keymap[i][0]; }
            if (key == 0x02) { I2C1_Unlock(); return keymap[i][1]; }
            if (key == 0x01) { I2C1_Unlock(); return keymap[i][2]; }
        }
    }
    I2C1_Unlock();
    return 0;
}
