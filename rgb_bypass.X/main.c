 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
*/

/*
© [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"
#include <util/delay.h>
/*
    Main application
*/

uint8_t twi_data[12];
uint8_t twi_slave_addr = 0x2c;//0b0101100
//                      aaaa ccxd dddddddd
#define VW0_ADDR  0x0 //0000 0000
#define VW1_ADDR  0x1 //0001 0000
#define NVW0_ADDR 0x2
#define NVW1_ADDR 0x3
#define TC0_ADDR  0x4 //0100 0000
#define STAT_ADDR 0x5
#define VW2_ADDR  0x6 //0110 0000
#define VW3_ADDR  0x7 //0111 0000
#define NVW2_ADDR 0x8
#define NVW3_ADDR 0x9
#define TC1_ADDR  0xA //1010 0000
#define DAT0_ADDR 0xB
#define DAT1_ADDR 0xC
#define DAT3_ADDR 0xD
#define DAT4_ADDR 0xE
#define DAT5_ADDR 0xF
#define CMD_WR    0x0 //0000 0000
#define CMD_RD    0xC //0000 1100
#define CMD_INC   0x4 //0000 0100
#define CMD_DEC   0x8 //0000 1000
//general call commands
//addr wiper data (only wipers 0 and 1)
//0x00 0x80 0x22
//0x00 0x90 0x22
#define mcp4461(a,b,c) ((a<<4)&0xF0)|((b<<2)&0xC)|(c&0x3)
#define mcp4461_wr(a,b,c) ((a<<4)&0xF0)|((b<<2)&0xC)|((c>>8)&0x3),(c&0xFF)
uint8_t pot_init_table[16]={
    //first two commands can be omitted because these are default values anyway
    mcp4461(TC0_ADDR,CMD_WR,1),
    0b11111111,
    mcp4461(TC1_ADDR,CMD_WR,1),
    0b11111111,
    //unlock wipers by decrement
    mcp4461(VW0_ADDR,CMD_DEC,0),
    mcp4461(VW1_ADDR,CMD_DEC,0),
    mcp4461(VW2_ADDR,CMD_DEC,0),
    mcp4461(VW3_ADDR,CMD_DEC,0),
    //set to arbitrary value
    mcp4461_wr(VW0_ADDR,CMD_WR,0xff),
    mcp4461_wr(VW1_ADDR,CMD_WR,0xff),
    mcp4461_wr(VW2_ADDR,CMD_WR,0xff),
    mcp4461_wr(VW3_ADDR,CMD_WR,0xff)
};

void digipot_write_all(uint16_t data){
    uint8_t my_databuf[8]={
        mcp4461_wr(VW3_ADDR,CMD_WR,data),
        mcp4461_wr(VW3_ADDR,CMD_WR,data),
        mcp4461_wr(VW3_ADDR,CMD_WR,data),
        mcp4461_wr(VW3_ADDR,CMD_WR,data)
    };
    TWI0_Write(twi_slave_addr,my_databuf,8);
}

int main(void)
{
    SYSTEM_Initialize();
    PORTB.OUTSET |= PIN3_bm;//set high: reset pin on digipot; active low
    PORTB.OUTCLR |= PIN2_bm;//set low: a0/HVC pin on digipot
    PORTA.OUTSET |= PIN6_bm;//set high: writeprotect pin on digipot; active low
    PORTA.OUTSET |= PIN5_bm;//set low: address select pin on mux to pass through RGB
    
    //_delay_ms(200);
    
    //toggle reset on digipot; this works
    PORTB.OUTCLR |= PIN3_bm;//set low: reset pin on digipot; active low
    //_delay_ms(200);
    PORTB.OUTSET |= PIN3_bm;//set high: reset pin on digipot; active low
    //_delay_ms(200);
    
    //general call commands for testing
    /*uint16_t tcon_gc_cmd[]={
        0x00,
        0b11000010,
        0b00000000
    };
    uint16_t tcon_gc_cmd2[]={
        0x00,
        0b11000010,
        0b11111111
    };*/
    // 7-bit command | data bit 8 | const 0 | data bits 7:0 
    //                      8  76543210
    //                aaaaxxd0 dddddddd
    // data bit 8 is const 1 (reserved)
    //TWI0_Write(0x00,tcon_gc_cmd,0x3);//write 3rd byte to tcon
    //_delay_ms(200);
    //TWI0_Write(0x00,tcon_gc_cmd2,0x3);
    //_delay_ms(200);
    
    //set up digipot device
    TWI0_Write(twi_slave_addr,pot_init_table,16);
    //_delay_ms(200);

    //write value to all wipers to see if changes on oscilloscope
    digipot_write_all(0x20);
    //_delay_ms(200);

    //main loop code go here
    while(1)
    {
        ;
    }
}

/*circuit implementation notes:
 * Attiny424 connected to MCP4461 via i2c (and reset/a0 by GPIO)
 * MCP4461 A1 tied to gnd
 * tried both internal pullups in AVR and 4.7k resistors directly; mcp4461 seems
 *      to not even respond to reset pin with the latter???
 * nothing else on i2c bus... so bus capacitance should be low
 *      (est 20-40pF total?)
 * MCP4461 seems to have just initial values...
 * code seems to hang with bus busy; no error codes?
 * MCP4461 datasheet claims doesn't support 10-bit addressing, but the mcc 
 *      generated code seems to resolve to 7-bit?
 */