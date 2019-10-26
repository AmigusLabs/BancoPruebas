// Scale Libraries and setup

#include <HX711.h> 

#define DOUT  3
#define CLK  5
#define calibration_factor -970 // Calibration factor of scale

HX711 scale(DOUT, CLK);

// End scale libraries and setup


// OLED Libraries and setup

#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);

// End OLED libraries and setup


// Variables

int loadCellValue = 1;
  
  
void setup() {   
  
  Serial.begin(9600);
  scale.set_scale(calibration_factor); 
  scale.tare();
  
  display.begin(SH1106_SWITCHCAPVCC, 0x3C); // this was the missing line of code preventing this from working

}

void loop() {
  
  Serial.println(F("Entering Loop")); // Used to show loop has been entered
  
  loadCellValue = scale.get_units(); // Take reading from scale
  
  //Some Example Text 
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(3,5);
  display.println(" Some Example Text ");
  display.println();
  display.setTextColor(WHITE); 
  display.setTextSize(3);
  display.setCursor(15,21);
  display.print(loadCellValue);
  display.print("/");
  display.print("20");
  display.display();
  delay(2000);

}
