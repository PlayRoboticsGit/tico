TICO is a 3D printed, Arduino powered Tic-Tac-Toe robot.
It was designed in order to inspire kids learn coding, robotics & electronics.

Documentation can be found here:
https://playrobotics.com/blog/tico-tic-tac-toe-arduino-robot-documentation

**Important!**

You might be getting "Scatch too big" error during upload , this is because the IRremote is supporting many remote control protocols (Sony, Samsung etc).
We can disable all hte protocols we don't need. If you are using the same remote we used you will only need NEC protocol.

**How to do it?** 

Go to the folder where the IRremote library is stored, (should be something like C:\Users\alex\Documents\Arduino\libraries\IRremote\src). 

Open IRremote.h file and go to line 70. 
Now add comments to all the protocols you don't need , your code should look like this (in our case ony NEC protocol is active):



//#define DECODE_DENON      

//#define DECODE_JVC

//#define DECODE_KASEIKYO

//#define DECODE_PANASONIC    

//#define DECODE_LG

#define DECODE_NEC      

//#define DECODE_SAMSUNG

//#define DECODE_SONY

//#define DECODE_RC5

//#define DECODE_RC6



