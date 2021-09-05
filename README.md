TICO is a 3D printed, Arduino powered Tic-Tac-Toe robot.
It was designed in order to inspire kids learn coding, robotics & electronics.

Documentation can be found here:
https://playrobotics.com/blog/tico-tic-tac-toe-arduino-robot-documentation

**Important!**
You might be getting "Scatch too big" error during upload , this is because the IRremote is supporting many remote control protocols (Sony, Samsung etc).
We can disable all hte protocols we don't need. If you are using the same remote we used you will only need NEC protocol.

**How to do it?** 
Go to the folder where the IRremote library is stored, (should be something like C:\Users\alex\Documents\Arduino\libraries\IRremote\src). 
Open IRremote.h file and go to line 60. 
Now add comments to all the protocols you don't need , your code should look like this:

/****************************************************
 *                     PROTOCOLS
 ****************************************************/

/*
 * Supported IR protocols
 * Each protocol you include costs memory and, during decode, costs time
 * Disable (deactivate the line by adding a trailing comment "//") all the protocols you do not need/want!
 */
#if (!(defined(DECODE_DENON) || defined(DECODE_JVC) || defined(DECODE_KASEIKYO) \
|| defined(DECODE_PANASONIC) || defined(DECODE_LG) || defined(DECODE_NEC) || defined(DECODE_SAMSUNG) \
|| defined(DECODE_SONY) || defined(DECODE_RC5) || defined(DECODE_RC6) || defined(DECODE_HASH) \
|| defined(DECODE_BOSEWAVE) || defined(DECODE_LEGO_PF) || defined(DECODE_MAGIQUEST) || defined(DECODE_WHYNTER)))
//#define DECODE_DENON        // Includes Sharp
//#define DECODE_JVC
//#define DECODE_KASEIKYO
//#define DECODE_PANASONIC    // the same as DECODE_KASEIKYO
//#define DECODE_LG
#define DECODE_NEC          // Includes Apple and Onkyo
//#define DECODE_SAMSUNG
//#define DECODE_SONY
//#define DECODE_RC5
//#define DECODE_RC6



