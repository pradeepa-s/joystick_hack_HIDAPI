joystick_hack_HIDAPI
====================

Decoding the standard usb joystick replies through HIDAPI

The project was done using MSVC2005. Demonstrates the usage of HIDAPI to decode standard usb joystick codes. 

Original post can be found at www.unboxNbeyond.com

09/24/2013:

If you click on Start and then press joystick buttons, the pressed button will be displayed in the application. 
Click on stop in order to stop the button detection. If you want to read the status of the joystick only once, 
press the joystick button first and then click on Read to get the status to the Edit Box.

02/14/2015:

Added the VS2013 project separately.

Known issues:

[1]. Input validations are not performed within the functions. Therefore the application may crash if it did not initialized properly.
[2]. File properties are not set, so it contains default values.

Developer: Pradeepa Senanayake
