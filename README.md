# Ultrasonic-Based Virtual Sketchpad
Embedded System Final Group Project for EEC 172 @ UC-Davis  
The main processing unit is **_TI CC3200 LaunchPad_**

## Group Members 
  * Yizhi Tao
  * Kolin Guo
  
## Introduction
Today many artists use digital sketchpads to draw artworks on the computer. However, the physical sketchpad often takes up much space and requires a stylus. We want to design a “virtual” sketchpad that uses sensors to detect the movement of the pen, hence eliminating the need of a physical sketchpad.  
In this open lab project, we implement a “virtual” sketch pad of 32 by 32 pixels using TI CC3200 LaunchPad and two ultrasonic sensors. The painting is displayed on the 128x128 OLED screen in real time. User can also interact with the IR remote to pick the colors of the pen and send the painting to Amazon Web Services (AWS) S3 bucket which automatically converts the painting to a PNG file for downloads. 

## Top-Level Description
![Top-Level Block Diagram](/images/Top-Level.png "Top-Level Block Diagram")
![Top-Down View of the Sketchpad](/images/Top-Down_View.png "Top-Down View of the Sketchpad")  

The CC3200 is connected to  
  * Two ultrasonic sensors using GPIO pins  
  * An OLED display via SPI protocol to display drawings  
  * An IR sensor via GPIO to receive signals from the IR remote  
  * AWS using the built-in Wi-Fi module and RESTful API  

## Sensors
  | Name | Quantity Used | Descriptions |
  |:----:|:-------------:|:------------:|
  | _HC-SR04_ Ultrasonic Ranging Module |2|2cm – 400cm non-contact measurement, 3mm measurement accuracy, 15° measurement angle, 40kHz working frequency, simply connect using GPIO|
  | _TSOP1236/31236/31336_ IR Receiver Module |1|36kHz carrier frequency, simply connect using GPIO|
  | **AT&T** _S10-S3_ Remote Control |1|Use NEC code, 12 functional keys|
  | _SSD1351_ 128x128 OLED Display |1|16-bit RGB565 format, 65536 colors, connect to CC3200 using SPI|

## Sensor and Setup Details
+ _HC-SR04_ Ultrasonic Sensor

  We use two _HC-SR04_ ultrasonic sensors to measure the distances from the “pen” in perpendicular directions. The pen is a cuboid object in practice because its flat surfaces reflect ultrasonic soundwave well. The two distances are then converted to x and y coordinates of the pixel on the OLED screen. So, the two sensors can track the movement of the pen and update its position on the OLED screen.  
  The _HC-SR04_ ultrasonic sensor has a _trigger_ that sends an 8-cycle burst of ultrasonic soundwave at 40kHz at the object. Then the _echo_ receives the reflected ultrasonic signal. The distance is calculated using the time interval between _trigger_ and _echo_: `Distance = high-level time * 340m/s /2`. The high-level time on the GPIO pin is measured using the general-purpose timers and GPIO interrupts.  
  _HC-SR04_ ultrasonic sensors can measure distance from 2cm to 4m with accuracy up to 3mm. It has a measuring angle of 15 degrees. Because of the limited measuring angle, the pen needs to be placed relatively far away (about 4x the sketchpad side) from the sensors to achieve a larger sketch area. After calculations and testing, we decided to have a 32x32 pixels sketchpad on the OLED. With the 3mm accuracy, the length of the side of effective sketch area is calculated to be `31*3mm = 93mm`. The distance from one sensor to the nearest side is calculated using trigonometry to be 365mm. Because of limited accuracy and measuring angle, having a higher resolution or a larger sketch area would require larger setup area and we thought it would not be feasible. A “pen” made of LEGO bricks (15mm x 15mm) will be used because its flat surfaces can easily reflect ultrasonic soundwave. 

+ IR Remote

  The buttons (0-9) on the remote are programmed to change the color of the pen according to the color mapping summarized in the table below. The MUTE button is programmed to clear the drawing on the screen and LAST button is programmed to send a string containing the pixel values of the sketch to AWS.  

  | IR Remote Keys | Color Mapping/Functionality | IR Remote Keys | Color Mapping/Functionality |
  |:--------------:|:---------------------------:|:--------------:|:---------------------------:|
  | 0 | Black `0x000000` | 6 | Cyan `0x00FFFF` |
  | 1 | White `0xFFFFFF` | 7 | Azure `0x0080FF` |
  | 2 | Red `0xFF0000` | 8 | Blue `0x0000FF` |
  | 3 | Orange `0xFF8000` | 9 | Purple `0x7F00FF` |
  | 4 | Yellow `0xFFFF00` | MUTE | Clear current drawing |
  | 5 | Green `0x00FF00` | LAST | Send current drawing to AWS |  

## CC3200 Pin Connections
  | Package Pin # | UART/SPI/GPIO Signals | Usage |
  |:-------------:|:---------------------:|:-----:|
  | 64 | GPIO_09  | Output, ultrasound sensor _trigger_ signal |
  | 8 | GPIO_17   | Input, ultrasound sensor #1 _echo_ signal |
  | 50 | GPIO_00  | Input, ultrasound sensor #2 _echo_ signal |
  | 63 | GPIO_08  | Input, IR sensor signal |
  | 61 | GPIO_06  | Output, OLED D/C signal |
  | 62 | GPIO_07  | Output, OLED Chip Select signal |
  | 18 | GPIO_28  | Output, OLED Reset signal |
  | 5 | GSPI_CLK  | General SPI Clock for OLED display |
  | 7 | GSPI_MOSI | General SPI MOSI for OLED display |
  | 55 | UART0_TX | UART0 TX Data |
  | 57 | UART0_RX | UART0 RX Data |
  | 2 | GPIO_11   | Green LED. Not used actually, can be removed |

## Code Description
+  `IRIntHandler()` collects input from IR sensor.  
+  `US1IntHandler()` starts timer of the 1st ultrasonic sensor when positive edge is detected on echo pin. Then it records the time and stops the timer when negative edge is detected.  
+  `US2IntHandler()` starts timer of the 2nd ultrasonic sensor when positive edge is detected on echo pin. Then it records the time and stops the timer when negative edge is detected.  
+  `TimerIntHandler()` handles timer interrupt for the IR sensor to decode the message.  
+  `ChangeRGBColor(unsigned int)` changes the color in a rainbow fashion according to IR sensor input.  
+  `ResetImage()` clears the OLED screen and resets image with MUTE key.  
+  `GetIRInput()` gets input from IR remote.  
+  `CollectUSData()` collects data from the two ultrasonic sensors, converts them to distances and calls `ConvertToPixelLoc()`.  
+  `ConvertToPixelLoc()` converts distance data to pixel location on OLED.  
+  `DrawSizedPixel(unsigned char)` draws a pixel on OLED of specified size (4-by-4 dots by default).  
+  `SaveToRGBArr()` saves the RGB pixel data to `g_imageRGB[]`.  
+  `StartSketching()` starts sketching on the OLED display by calling `CollectUSData()`, `DrawSizedPixel(4)`, `SaveToRGBArr()` and `GetIRInput()` in an infinite loop.  
+  `http_post()` converts `g_imageRGB[]` array to a JSON readable character string with commas separating the RGB values in hexadecimal format. Then it posts the string to an AWS IoT thing.  

## Amazon Web Services (AWS)
![AWS Flowchart](/images/AWS_Flowchart.png "AWS Flowchart")  
The drawing is sent to an AWS IoT thing shadow as a string of the RGB values using HTTP POST. When the thing shadow is updated, the **_Rule_IoTToS3_** will invoke a Lambda function called `SaveIoTMessageAsS3Object`, passing the message data. This function strips the RGB data from the IoT message and saves it as a binary file named **_“ImageRGBData.bin”_** into an S3 bucket (**_testforjpeg_**). When the binary file is generated in the **_testforjpeg_** S3 bucket, an S3 Put event occurs which invokes another Lambda function called `ConvertRGBToJpeg`, passing the binary file location. This function converts the binary file to a PNG image file and saves it as **_“image.png”_** into another S3 bucket named **_testforjpegjpeg_**. Then, user can download the PNG file. Both Lambda functions are written in Python. `ConvertRGBToJpeg` function uses OpenCV and NumPy libraries to convert RGB data to a PNG file.  
![SaveIoTMessageAsS3Object Lambda Function](/images/SaveIoTMessageAsS3Object.png "SaveIoTMessageAsS3Object Lambda Function")  
<center> Figure 1</center>  


![ConvertRGBToJpeg Lambda Function](/images/ConvertRGBToJpeg.png "ConvertRGBToJpeg Lambda Function")  
<center> Figure 2</center>  


Initially, the `ConvertRGBToJpeg` function converted the RGB data to a JPEG file. However, we found that our 32x32 image is blurry. This is because JPEG is a lossy compression image format and it does not work very well for an image with such few pixels. Therefore, we modified the `ConvertRGBToJpeg` function to save the image as a PNG file which is a lossless image format and thus produces an accurate image.  

## Acknowledgements
  * [Professor Soheil Ghiasi](http://web.ece.ucdavis.edu/~soheil/)
  * TA: [Dan Fong](http://lepsucd.com/?page_id=601)
  * Special Thanks to: Sam Minh Truong ([@Samtruong](https://github.com/Samtruong)) for OpenCV Python library

## Credits
  * The code for collecting data from Ultrasonic sensor HC-SR04 is originally writted by [Akash Salow](https://github.com/sal0w). The GitHub page is referenced [here](https://github.com/sal0w/CC3200-Ultrasonic-sensor-HC-Sr04). 
