
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup() {

  u8g2.begin();

  Serial.begin(9600);



  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_logisoso16_tr);
    u8g2.drawStr(5, 38, "@migus Labs");
    u8g2.setFont(u8g2_font_helvR08_tr);
    u8g2.drawStr(18, 52, "Power Module v0.1");
    u8g2.setFont(u8g2_font_helvR08_tr);
    u8g2.drawStr(32, 64, "Calibrando...");
  } while ( u8g2.nextPage() );

}

void loop() {


}
