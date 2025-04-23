/*--------------------------------------------------------------------------------------
 Demo code for Monochrome DMD panels

 DMD_STM32a example code for STM32F4 & RP2040 boards
 ------------------------------------------------------------------------------------- */
// choose between Parallel ans SPI wiring
// comment line below for SPI connection
#define DMD_PARA
/*--------------------------------------------------------------------------------------
  Includes
--------------------------------------------------------------------------------------*/
#if defined(DMD_PARA)
#include "DMD_Monochrome_Parallel.h"
#else
#include "DMD_MonoChrome_SPI.h"
#endif

//Number of panels in x and y axis
#define DISPLAYS_ACROSS 2
#define DISPLAYS_DOWN 1

// Enable of output buffering
// if true, changes only outputs to matrix after
// swapBuffers(true) command
// If dual buffer not enabled, all output draw at matrix directly
// and swapBuffers(true) command do nothing
#define ENABLE_DUAL_BUFFER true

// ----- Select pins for P10 matrix connection ------------
/* =================== *
*     STM32F4 pins     *
*  =================== */
// pins A, B, SCLK may be any digital I/O, pin nOE should be Timer3 PWM pin as PB0,PB1
// if connect as SPI, do not use corresponding MiSO pin - PA6 for SPI1 and PB14 for SPI2

#if (defined(__STM32F1__) || defined(__STM32F4__))
#define DMD_PIN_A PA1
#define DMD_PIN_B PA3
#define DMD_PIN_nOE PB1
#define DMD_PIN_SCLK PA15
//#define DMD_PIN_SCLK PA0


#if defined(DMD_PARA)
//=== Config for STM32 Parallel connect ====
// pins for rows at x axis
// example for two rows
// all those pins must be selected from same port!
#ifdef USE_UPPER_8BIT
uint8_t pins[] = { PB9, PB11, PB13 };  // CLK , row1, row 2
#else
uint8_t pins[] = { PA5, PA7, PA6 };  // CLK , row1, row 2
#endif

#else
//=== Config for STM32 SPI connect ====
// SPI specific pins as CLK and R_DATA has predefined values:
//   for SPI(1) CLK = PA5  R_DATA = PA7
//   for SPI(2) CLK = PB13  R_DATA = PB15
// --------------------------------------------------------
#endif
#endif
/* =================== *
*     RP2040 pins     *
*  =================== */
#if defined(ARDUINO_ARCH_RP2040)
#if defined(DMD_PARA)
#define DMD_PIN_A 1
#define DMD_PIN_B 2
#define DMD_PIN_nOE 14
#define DMD_PIN_SCLK 15

// These pins must be consecutive in ascending order
uint8_t pins[] = { 6, 7, 8 };  // CLK , row1, row 2
#else
#error Monochrome_SPI mode unsupported for Rasberry Pico RP2040
#endif
#endif

#if defined(DMD_PARA)
//Fire up the DMD object as dmd
DMD_Monochrome_Parallel dmd(DMD_PIN_A, DMD_PIN_B, DMD_PIN_nOE, DMD_PIN_SCLK, pins, DISPLAYS_ACROSS, DISPLAYS_DOWN,ENABLE_DUAL_BUFFER);
#else

//=== Config for SPI connect ====
SPIClass dmd_spi(1);
DMD_MonoChrome_SPI dmd(DMD_PIN_A, DMD_PIN_B, DMD_PIN_nOE, DMD_PIN_SCLK, DISPLAYS_ACROSS, DISPLAYS_DOWN, dmd_spi, ENABLE_DUAL_BUFFER);
#endif


// Fonts includes
#include "st_fonts/UkrRusArial14.h"
#pragma GCC diagnostic ignored "-Wnarrowing"
#pragma GCC diagnostic ignored "-Woverflow"
#include "gfx_fonts/GlametrixLight12pt7b.h"
#include "gfx_fonts/GlametrixBold12pt7b.h"
#pragma GCC diagnostic warning "-Wnarrowing"
#pragma GCC diagnostic warning "-Woverflow" 
// --- Define fonts ----
// DMD.h old style font
DMD_Standard_Font UkrRusArial_F(UkrRusArial_14);

// GFX font with sepatate parts for Latin and Cyrillic chars
DMD_GFX_Font GlametrixL((uint8_t*)&GlametrixLight12pt7b,(uint8_t*)&GlametrixLight12pt8b_rus,0x80,13);


/*--------------------------------------------------------------------------------------
  UTF8 char recoding
 
--------------------------------------------------------------------------------------*/
int utf8_rus(char* dest, const unsigned char* src) {
  
  uint16_t i, j;
  for ( i =0, j =0; src[i]; i++) {
   if ((src[i] == 0xD0 )&& src[i+1])  { dest[j++] = src[++i] - 0x10;}
    else if ((src[i] == 0xD1 )&& src[i+1]) {dest[j++] = src[++i] + 0x30;  }
    else dest[j++] = src[i];
  }
  dest[j] ='\0';
  return j;
}

/*--------------------------------------------------------------------------------------
  setup
  Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/

void setup(void)
{
   
   // initialize DMD objects
   // scan DMD row each 700 us
    dmd.init(700);
    dmd.setBrightness(80);
   
}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void)
{

    // text
    const char m[] = "Привет Ардуино!";
    // transcode message to UTF for use with GFX fonts
    char k[30];
    utf8_rus(k, (const unsigned char* )m);

    // but initially select standard font
    dmd.selectFont(&UkrRusArial_F);

    // shift steps in pixels for running text (positive - shift right, negative - left)
    int8_t step[] = { 1,-1,-2,2 };
    // running text shift interval
    uint32_t interval = 50;
    
    uint32_t prev_step = millis();
    uint8_t i = 0, b = 0;
    uint8_t test = 255;
    uint8_t test_cnt = 4;

     // Cycle for tests:
    // -- running texts moving at x and y axis with single and double speed
    // -- brightness control
    // -- vertical scrolling message
    while (1) {
        if ((millis() - prev_step) > interval) {
            prev_step = millis();  // moved to the start of the loop to prevent "scroll not smooth" issue
            if (test >= test_cnt) {
                test = 0;
                // draw message
                dmd.drawMarqueeX(m, -1 * (dmd.stringWidth(m)), 0);
                dmd.swapBuffers(true);

            }
            switch (test) {
              // moving text at x axis
            case 0:
                if (dmd.stepMarquee(step[i], 0) & MARQUEE_OUT_OF_SCREEN) { // if text is reached screen bounds

                    dmd.clearScreen(true);
                    i++;
                    // if all moving finished
                    if (i > 3) {
                      // go to next stage
                        i =0;
                        test++;
                        dmd.drawMarqueeX(m, 0, 0);
                    }
                    else {
                        if (step[i] < 0) dmd.drawMarqueeX(m, dmd.width() - 1, 0);
                        else dmd.drawMarqueeX(m, -1 * dmd.stringWidth(m), 0);
                    }
                }
              
                // output mem buffer to matrix
                dmd.swapBuffers(true);
                break;
            
            
            // change brightness
            case 1:
                b++;
                dmd.setBrightness(b);
                if (b > 200) {
                    test++;
                    b = 80;
                    i = 0;
                    dmd.setBrightness(b);
                    dmd.drawMarqueeX(m, 0, 0);
                }
                dmd.swapBuffers(true);
                break;

            // moving text at y axis
            case 2:
                if (dmd.stepMarquee(0, step[i]) & MARQUEE_OUT_OF_SCREEN) {

                    // clear the screen
                    dmd.clearScreen(true);
                    // select new moving speed
                    i++;
                    // if all moving finished
                    if (i > 3) {
                      // go to next stage
                        test++;
                        // select GFX font for vertical scroll
                        dmd.selectFont(&GlametrixL);
                        dmd.drawMarquee(k, strlen(k), dmd.width() - 1, 0, 1);

                    }
                    else {
                        if (step[i] < 0) dmd.drawMarqueeX(m, 0, dmd.height());
                        else dmd.drawMarqueeX(m, 0, 0);
                    }
                }
                // output mem buffer to matrix
                dmd.swapBuffers(true);
                break;

            // vertical scrolling    
            case 3:

                dmd.stepMarquee(-1, 0, 1);
                dmd.swapBuffers(true);
                break;

            }

           
        }
    }
}
