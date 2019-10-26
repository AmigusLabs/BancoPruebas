#include "HX711.h"

#define DOUT  3
#define CLK  4

HX711 balanza(DOUT, CLK);

void setup() {
  Serial.begin(9600);
  Serial.print("Lectura del valor del ADC:  ");
  Serial.println(balanza.read());
  Serial.println("No ponga ningun  objeto sobre la balanza");
  Serial.println("Destarando...");
  balanza.set_scale(); //La escala por defecto es 1
  balanza.tare(100);  //El peso actual es considerado Tara.
  Serial.println("Coloque un peso conocido:");
  
}


void loop() {

  Serial.print("Valor de lectura:  ");
  Serial.println(balanza.get_value(10),0);
  delay(100);
}
