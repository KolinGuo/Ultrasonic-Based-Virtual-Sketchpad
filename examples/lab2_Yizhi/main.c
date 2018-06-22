//*****************************************************************************

//*****************************************************************************
//
// Application Name     - SPI Demo
// Application Overview - The demo application focuses on showing the required 
//                        initialization sequence to enable the CC3200 SPI 
//                        module in full duplex 4-wire master and slave mode(s).
// Application Details  -
// http://processors.wiki.ti.com/index.php/CC32xx_SPI_Demo
// or
// docs\examples\CC32xx_SPI_Demo.pdf
//
//*****************************************************************************


//*****************************************************************************
//
//! \addtogroup SPI_Demo
//! @{
//
//*****************************************************************************

// Standard includes
#include <string.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_ints.h"
#include "spi.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "prcm.h"
#include "uart.h"
#include "interrupt.h"

// Common interface includes
#include "uart_if.h"
#include "pin_mux_config.h"
#include "i2c_if.h"

#include "Adafruit_GFX.h"
#include "test.h"
#include "gpio.h"
#include "glcdfont.h"

#define APPLICATION_VERSION     "1.1.1"
//*****************************************************************************
//
// Application Master/Slave mode selector macro
//
// MASTER_MODE = 1 : Application in master mode
// MASTER_MODE = 0 : Application in slave mode
//
//*****************************************************************************
#define MASTER_MODE      1

#define SPI_IF_BIT_RATE  100000
#define TR_BUFF_SIZE     100

#define MASTER_MSG       "This is CC3200 SPI Master Application\n\r"
#define SLAVE_MSG        "This is CC3200 SPI Slave Application\n\r"

#define UART_PRINT              Report
#define FOREVER                 1
#define CONSOLE                 UARTA0_BASE
#define FAILURE                 -1
#define SUCCESS                 0
#define RETERR_IF_TRUE(condition) {if(condition) return FAILURE;}
#define RET_IF_ERR(Func)          {int iRetVal = (Func); \
                                   if (SUCCESS != iRetVal) \
                                     return  iRetVal;}

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************

#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

void DrawFullChar(void)
{

    fillScreen(BLACK);

    char * stuff = "1234567890qwertyuiop[]\asdfghjkl;'zxcvbnm,./!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:ZXCVBNM<>?"
            "1234567890-=qwertyuiop[]\asdfghjkl;1234567890-=qwertyuiop[]\asdfghjkl;'zxcvbnm,./"
            "1234567890-qwertyuiop[]asdfghjkl;zxcvbnm,./";
    Outstr(stuff);

    delay(100);
}

void PrintHello(void)
{
    fillScreen(BLACK);
    char * hello = "Hello world!";
    Outstr(hello);

    delay(100);
}

void DrawHLines(void)
{
    unsigned int y;

    fillScreen(BLACK);
    for (y=0; y < height()-1; y+=15) {
         drawFastHLine(0, y, width()-1, 100*y);
       }
    delay(100);
}

void DrawVLines(void)
{
    unsigned int x;

    fillScreen(BLACK);
    for (x=0; x < width()-1; x+=15) {
         drawFastVLine(x, 0, height()-1, 100*x);
       }
    delay(100);
}
//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

//*****************************************************************************
//
//! Main function for spi demo application
//!
//! \param none
//!
//! \return None.
//
//*****************************************************************************
void main()
{
    unsigned char ucDevAddr = 0x18;
    unsigned char ucRegOffset = 0x02;
    unsigned char ucRdLen = 6;
    unsigned char aucRdDataBuf[9];
    int x_acc, y_acc;
    int x_coord=64, y_coord=64;

    //
    // Initialize Board configurations
    //
    BoardInit();

    //
    // Muxing UART and SPI lines.
    //
    PinMuxConfig();

    I2C_IF_Open(I2C_MASTER_MODE_FST);

    //
    // Enable the SPI module clock
    //
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);


    //
    // Initialising the Terminal.
    //
    InitTerm();

    //
    // Clearing the Terminal.
    //
    ClearTerm();

    //
    // Display the Banner
    //
    Message("\n\n\n\r");
    Message("\t\t   ********************************************\n\r");
    Message("\t\t        CC3200 SPI Demo Application  \n\r");
    Message("\t\t   ********************************************\n\r");
    Message("\n\n\n\r");

    //
    // Reset the peripheral
    //
    MAP_PRCMPeripheralReset(PRCM_GSPI);

    MAP_SPIReset(GSPI_BASE);


    MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                         SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_0,
                         (SPI_SW_CTRL_CS |
                         SPI_4PIN_MODE |
                         SPI_TURBO_OFF |
                         SPI_CS_ACTIVEHIGH |
                         SPI_WL_8));


    MAP_SPIEnable(GSPI_BASE);
    GPIOPinWrite(GPIOA0_BASE, 0x80, 0x00);
    Adafruit_Init();

    fillScreen(BLACK);
    fillCircle(64, 64, 4, WHITE);
       while(1)
       {
           I2C_IF_Write(ucDevAddr, &ucRegOffset, 1, 0);
           I2C_IF_Read(ucDevAddr, &aucRdDataBuf[0], ucRdLen);
           x_acc = (int)((signed char) aucRdDataBuf[3])/4;
           y_acc = (int)((signed char) aucRdDataBuf[1])/4;
           fillCircle(x_coord, y_coord, 4, BLACK);
           x_coord = x_coord + x_acc;
           y_coord = y_coord + y_acc;
           if(x_coord < 4)       x_coord = 4;
           if(x_coord > 123)     x_coord = 123;
           if(y_coord < 4)       y_coord = 4;
           if(y_coord > 123)     y_coord = 123;
           fillCircle(x_coord, y_coord, 4, WHITE);

           //delay(5);
       }

       /*
    while(1)
    {
        DrawFullChar();
        PrintHello();
        DrawHLines();
        DrawVLines();
        testlines(YELLOW);
        testfastlines(RED, GREEN);
        testdrawrects(BLUE);
        testfillrects(GREEN, BLUE);
        testfillcircles(10, MAGENTA);
        testroundrects();
        testtriangles();
    }
    */
}

