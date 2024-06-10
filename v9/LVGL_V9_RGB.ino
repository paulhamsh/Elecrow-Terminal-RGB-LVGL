// Board manager: ESP32 S3 Dev Module, QSPI PSRAM

// Install Arduino librabry lvgl version 8.4.0
// Get the Soruce Code.zip file from here: https://www.elecrow.com/download/product/DLC35010R/Source_Code.zip
//
// Make a folder and copy over ESP Terminal 3.5inch RGB code \ LVGL_RGB_ into it
// Rename the LVGL_RGB_ folder and the LVGL_RGB_.ino files to something more meaningful
//
// Copy the lv_conf.h from Source Code\Libraries\lvgl.h to Arduino \ libraries \ lvgl \ src
// Edit this line:
//   #define LV_FONT_MONTSERRAT_24 1
//
// Copy folders Arduino \ libraries \ lvgl \ demos and examples from to lvgl \ src

// Also: hold down the Boot button and press the RESET button to initiate firmware download mode


#include <Arduino.h>
#include <lvgl.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "FT6236.h"


#include <examples/lv_examples.h>
#include <demos/lv_demos.h>
const int i2c_touch_addr = TOUCH_I2C_ADD;


static uint32_t my_tick_function(void) {
  return millis();
}


#define LCD_BL 46

#define SDA_FT6236 38
#define SCL_FT6236 39
//FT6236 ts = FT6236();

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9488 _panel_instance;
    lgfx::Bus_Parallel16 _bus_instance;
  public:
    LGFX(void)
    {
      {
        auto cfg = _bus_instance.config();
        cfg.port = 0;
        cfg.freq_write = 80000000;
        cfg.pin_wr = 18;
        cfg.pin_rd = 48;
        cfg.pin_rs = 45;

        cfg.pin_d0 = 47;
        cfg.pin_d1 = 21;
        cfg.pin_d2 = 14;
        cfg.pin_d3 = 13;
        cfg.pin_d4 = 12;
        cfg.pin_d5 = 11;
        cfg.pin_d6 = 10;
        cfg.pin_d7 = 9;
        cfg.pin_d8 = 3;
        cfg.pin_d9 = 8;
        cfg.pin_d10 = 16;
        cfg.pin_d11 = 15;
        cfg.pin_d12 = 7;
        cfg.pin_d13 = 6;
        cfg.pin_d14 = 5;
        cfg.pin_d15 = 4;
        _bus_instance.config(cfg);
        _panel_instance.setBus(&_bus_instance);
      }

      {
        auto cfg = _panel_instance.config();

        cfg.pin_cs = -1;
        cfg.pin_rst = -1;
        cfg.pin_busy = -1;
        cfg.memory_width = 320;
        cfg.memory_height = 480;
        cfg.panel_width = 320;
        cfg.panel_height = 480;
        cfg.offset_x = 0;
        cfg.offset_y = 0;
        cfg.offset_rotation = 2;
        cfg.dummy_read_pixel = 8;
        cfg.dummy_read_bits = 1;
        cfg.readable = true;
        cfg.invert = false;
        cfg.rgb_order = false;
        cfg.dlen_16bit = true;
        cfg.bus_shared = true;
        _panel_instance.config(cfg);
      }
      setPanel(&_panel_instance);
    }
};

LGFX tft;

static const uint16_t screenWidth  = 480;
static const uint16_t screenHeight = 320;
static const int buf_size = screenWidth * screenHeight * sizeof(lv_color_t) / 10;
static uint16_t buf[buf_size];

lv_display_t *disp;
lv_indev_t   *indev;

/* Display flushing */
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
  uint32_t w = ( area->x2 - area->x1 + 1 );
  uint32_t h = ( area->y2 - area->y1 + 1 );

  //v8 tft.startWrite();
  //v8 tft.setAddrWindow( area->x1, area->y1, w, h );
  //v8 tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
  //v8 tft.endWrite();

  lv_draw_sw_rgb565_swap(px_map, w * h);
  tft.pushImage(area->x1, area->y1, w, h, (uint16_t *)px_map);

  lv_disp_flush_ready(disp);
}

void my_touchpad_read( lv_indev_t * indev_driver, lv_indev_data_t * data )
{
  int pos[2] = {0, 0};

  ft6236_pos(pos);
  if (pos[0] > 0 && pos[1] > 0)
  {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = tft.width()-pos[1];
    data->point.y = pos[0];
    Serial.printf("x-%d,y-%d\n", data->point.x, data->point.y);
  }
  else {
    data->state = LV_INDEV_STATE_REL;
  }
}


void touch_init()
{
  // I2C init
  Wire.begin(SDA_FT6236, SCL_FT6236);
  byte error, address;

  Wire.beginTransmission(i2c_touch_addr);
  error = Wire.endTransmission();

  if (error == 0)
  {
    Serial.print("I2C device found at address 0x");
    Serial.print(i2c_touch_addr, HEX);
    Serial.println("  !");
  }
  else if (error == 4)
  {
    Serial.print("Unknown error at address 0x");
    Serial.println(i2c_touch_addr, HEX);
  }
}

void setup()
{
  Serial.begin(115200);           // prepare for possible serial debug 
  
  tft.begin();                    // TFT init
  tft.setRotation(1);             // Landscape orientation, flipped 
  tft.fillScreen(TFT_BLACK);
  delay(500);

  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);
  touch_init();

  lv_init();
  
  lv_tick_set_cb(my_tick_function);

  disp = lv_display_create(screenWidth, screenHeight);
  lv_display_set_flush_cb(disp, my_disp_flush);
  //lv_display_set_buffers(disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_FULL);
  lv_display_set_buffers(disp, buf, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

  indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

//#if 0
//  lv_example_get_started_4();
//#else
  lv_demo_widgets();   
//#endif

  Serial.println( "Setup done" );
}

void loop()
{
  lv_timer_handler(); // let the GUI do its work
  delay(5);
}
