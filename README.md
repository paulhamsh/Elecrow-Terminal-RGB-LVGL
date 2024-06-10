# Elecrow-Terminal-RGB-LVGL
LVGL examples for the Elecrow Terminal RGB

## Elecrow Terminal RGB   

A lovely ESP32 S3 board.   
https://www.elecrow.com/esp-terminal-with-esp32-3-5-inch-parallel-480x320-tft-capacitive-touch-display-rgb-by-chip-ili9488.html   

<p align="center">
  <img src="https://github.com/paulhamsh/Elecrow-Terminal-RGB-LVGL/blob/main/Elecrow%20Terminal.jpg" width="400" title="Elecrow Terminal">
</p>

## Build LVGL demos with v8.4.0    

Board manager: ESP32 S3 Dev Module   

Install Arduino librabry lvgl version 8.4.0   
Get the Soruce Code.zip file from here: https://www.elecrow.com/download/product/DLC35010R/Source_Code.zip   

Make a folder and copy over ```ESP Terminal 3.5inch RGB code\LVGL_RGB_``` into it   
Rename the ```LVGL_RGB_``` folder and the ```LVGL_RGB_.ino``` files to something more meaningful   

Copy the lv_conf.h from ```Source Code\Libraries\lvgl.h``` to ```Arduino\libraries\lvgl\src```   
Edit this line:  
```
  #define LV_FONT_MONTSERRAT_24 1   
```
Copy folders ```Arduino\libraries\lvgl\demos``` and ```Arduino\libraries\lvgl\examples```  to ```lvgl\src```   

Also: hold down the Boot button and press the RESET button to initiate firmware download mode   
