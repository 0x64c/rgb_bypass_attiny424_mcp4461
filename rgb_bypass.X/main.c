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

uint8_t twi_slave_addr = 0x2c; //mcp4461 default
uint8_t twi_data[12];

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
#define mcp4xxx_cmd(a,b,c) ((a<<4)&0xF0)|((b<<2)&0xC)|((c>>8)&0x3)//addr,cmd,dat
#define mcp4xxx_dat(c) (c&0xFF)//dat
#define WIPER_INIT_VAL 0x40
#define TCON_ALL_ON_VAL 0x1FF
#define TCON_ALL_OFF_VAL 0x100

i2c_host_error_t mcp4xxx_write_data(uint16_t address, uint8_t *data, size_t dataLength){
    while(!TWI0_Write(twi_slave_addr,data,2)){
        //i2c busy, keep trying
    }
    while(!TWI0_IsBusy()){
        i2c_host_error_t error = TWI0_ErrorGet();
        if(error == I2C_ERROR_NONE){
            //萬歲
        }
        else{
            //一萬次悲傷
        }
        return error;
    }
    return I2C_ERROR_NONE;
}


i2c_host_error_t mcp4xxx_write_volatile(uint8_t wiper, uint16_t value){
    uint8_t this_wiper;
    if(wiper==3)
        this_wiper=VW3_ADDR;
    else if(wiper==2)
        this_wiper=VW2_ADDR;
    else if(wiper==1)
        this_wiper=VW1_ADDR;
    else
        this_wiper=VW0_ADDR;
    
    twi_data[0]=mcp4xxx_cmd(this_wiper,CMD_WR,value);
    twi_data[2]=mcp4xxx_dat(value);
    return mcp4xxx_write_data(twi_slave_addr,twi_data,2);
}

// set tcon and wipers to default setting (half scale, all pins enabled)
i2c_host_error_t mcp4xxx_wiper_init(){
    twi_data[0]=mcp4xxx_cmd(TC0_ADDR,CMD_WR,TCON_ALL_ON_VAL);
    twi_data[1]=mcp4xxx_dat(TCON_ALL_ON_VAL);
    twi_data[2]=mcp4xxx_cmd(TC1_ADDR,CMD_WR,TCON_ALL_ON_VAL);
    twi_data[3]=mcp4xxx_dat(TCON_ALL_ON_VAL);
    twi_data[4]=mcp4xxx_cmd(VW0_ADDR,CMD_WR,WIPER_INIT_VAL);
    twi_data[5]=mcp4xxx_dat(WIPER_INIT_VAL);
    twi_data[6]=mcp4xxx_cmd(VW1_ADDR,CMD_WR,WIPER_INIT_VAL);
    twi_data[7]=mcp4xxx_dat(WIPER_INIT_VAL);
    twi_data[8]=mcp4xxx_cmd(VW2_ADDR,CMD_WR,WIPER_INIT_VAL);
    twi_data[9]=mcp4xxx_dat(WIPER_INIT_VAL);
    twi_data[10]=mcp4xxx_cmd(VW3_ADDR,CMD_WR,WIPER_INIT_VAL);
    twi_data[11]=mcp4xxx_dat(WIPER_INIT_VAL);
    return mcp4xxx_write_data(twi_slave_addr,twi_data,12);
}

/* Can't use; need 9-12v on HVC pin...
i2c_host_error_t mcp4xxx_unlock(){
    //PORTB.OUTSET |= PIN2_bm;//set HVC pin high
    twi_data[0]=mcp4xxx_cmd(NVW0_ADDR,CMD_INC,0x0); //wiperlock disable
    twi_data[1]=mcp4xxx_cmd(NVW1_ADDR,CMD_INC,0x0);
    twi_data[2]=mcp4xxx_cmd(NVW2_ADDR,CMD_INC,0x0);
    twi_data[3]=mcp4xxx_cmd(NVW3_ADDR,CMD_INC,0x0);
    twi_data[3]=mcp4xxx_cmd(DAT5_ADDR,CMD_INC,0x0); //wp disable
}*/

i2c_host_error_t mcp4xxx_shutdown(){
    twi_data[0]=mcp4xxx_cmd(TC0_ADDR,CMD_WR,TCON_ALL_OFF_VAL);
    twi_data[1]=mcp4xxx_dat(TCON_ALL_OFF_VAL);
    twi_data[2]=mcp4xxx_cmd(TC1_ADDR,CMD_WR,TCON_ALL_OFF_VAL);
    twi_data[3]=mcp4xxx_dat(TCON_ALL_OFF_VAL);
    return mcp4xxx_write_data(twi_slave_addr,twi_data,4);
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
    
    i2c_host_error_t error1,error2;
    
    error1=mcp4xxx_wiper_init();
    
    error2=mcp4xxx_shutdown();
    
    /*
    mcp4xxx_write_volatile(0,0x100);
    mcp4xxx_write_volatile(1,0x100);
    mcp4xxx_write_volatile(2,0x100);
    mcp4xxx_write_volatile(3,0x100);
    */
    

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
