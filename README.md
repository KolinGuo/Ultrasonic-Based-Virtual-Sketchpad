# Ultrasonic-Based Virtual Sketchpad
Embedded System Final Group Project for EEC 172 @ UC-Davis  
The main processing unit is **_TI CC3200 LaunchPad_**

## Group Members: 
  * Yizhi Tao
  * Kolin Guo
  
## Introduction:
Today many artists use digital sketchpads to draw artworks on the computer. However, the physical sketchpad often takes up much space and requires a stylus. We want to design a “virtual” sketchpad that uses sensors to detect the movement of the pen, hence eliminating the need of a physical sketchpad.  
In this open lab project, we implement a “virtual” sketch pad of 32 by 32 pixels using TI CC3200 LaunchPad and two ultrasonic sensors. The painting is displayed on the 128x128 OLED screen in real time. User can also interact with the IR remote to pick the colors of the pen and send the painting to Amazon Web Services (AWS) S3 bucket which automatically converts the painting to a PNG file for downloads. 

## Top-Level Description
![](/images/Top-Level.png "Top-Level Block Diagram")

## Acknowledgements:
  * [Professor Soheil Ghiasi](http://web.ece.ucdavis.edu/~soheil/)
  * TA: [Dan Fong](http://lepsucd.com/?page_id=601)
  * Special Thanks to: Sam Minh Truong ([@Samtruong](https://github.com/Samtruong)) for OpenCV Python library

## Credits:
  * The code for collecting data from Ultrasonic sensor HC-SR04 is originally writted by [Akash Salow](https://github.com/sal0w). The GitHub page is referenced [here](https://github.com/sal0w/CC3200-Ultrasonic-sensor-HC-Sr04). 
