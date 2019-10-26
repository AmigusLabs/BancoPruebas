#include <Servo.h>

#include <EEPROM.h>
#include <U8g2lib.h>
#include <Wire.h>



U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


const byte testPin = 2;
int test = 0;
float inicio_test = 0;

Servo ESC;
int Receptor_CH3 = A1;
int Receptor_CH5 = A2;
int vel_real = 0;
int vAhora_real = 0;
float Varduino = 4.8; // valor real de la alimentacion de Arduino, Vcc
float R1 = 1000000; // 1M
float R2 = 100000; // 100K


float Vfinal = 0;
int velocidad = 1000;
int vAhora = 1000;
int velmin = 1000;
int incremento = 20;
int voltage_max = 12.0;

int iteraciones_escritura = 10;
int escribe = 0;

const int lecturas = 5; // Total lecturas V
int index             = 0; // El indice de la lectura actual
float readings[lecturas]; // Lecturas de la entrada analogica
float Vtotal           = 0.0; // Total
float Vmedia         = 0.0; // Promedio


void setup() {

  u8g2.begin();

  Serial.begin(9600);


  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_logisoso16_tr);
    u8g2.drawStr(5, 38, "@migus Labs");
    u8g2.setFont(u8g2_font_helvR08_tr);
    u8g2.drawStr(18, 54, "Power Module v4A");

  } while ( u8g2.nextPage() );

  delay(300);


  pinMode(testPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(testPin), test_funcion, CHANGE);


  pinMode(Receptor_CH3, INPUT);
  pinMode(Receptor_CH5, INPUT);

  Serial.print("Voltaje Máximo: ");
  Serial.print((int)(Varduino / (R2 / (R1 + R2))));
  Serial.println("V");
  Serial.println("");

  ESC.attach(10);  // salida al ESC a D10

  //delay(2000);
}



float leerVoltaje() {
  float v = (analogRead(0) * Varduino) / 1024.0;
  return (v / (R2 / (R1 + R2)));
}

bool switchActivado() {
  int value = pulseIn(Receptor_CH5, HIGH);
  if (value > 1600) {
    return false;
  }
  return true;
}

void test_funcion() {

  test = test + 1;
  if (test > 1 && velocidad > 0) {
    test = 0;
  }
  velocidad = 0;
  inicio_test = 0;
}

void printSerial() {
  escribe++;
  if (escribe == iteraciones_escritura) {
    escribe = 0;
    
   Serial.print("Limitador activado:");    Serial.println(switchActivado());
    Serial.print("V: ");    Serial.println(Vfinal);
    Serial.print("Vel: ");    Serial.println(velocidad);
    Serial.print("vAhora: ");   
   
    Serial.println(vAhora);

  }
}
void loop() {

  Vfinal = leerVoltaje();

  if (test == 1) {

    inicio_test = inicio_test + 1;

    if (inicio_test > 100) {
      if (velocidad < 101) {
        velocidad = velocidad + 1;
        inicio_test = 0;
      } else {
        test = 0;
        velocidad = 0;
      }
    }

  } else {
    velocidad = pulseIn(Receptor_CH3, HIGH);

  }



  if (Vfinal < voltage_max && vAhora >= velmin) {
    vAhora = vAhora - incremento;
  } else if (vAhora < velocidad) {
    vAhora = vAhora + incremento;
  } else {
    vAhora = velocidad;
  }

  vel_real = map(velocidad, 995, 1989, 0, 100 );
  vAhora_real = map(vAhora, 995, 1989, 0, 100 );
  if (vAhora_real < 0) {
    vAhora_real = 0;
  }
  if (vel_real < 0) {
    vel_real = 0;
  }


  readings[index] = Vfinal;

  index = index + 1;

  if (index >= lecturas) {
    index = 0;   // ...volvemos al inicio:
  }
  Vtotal = 0;

  for (int i = 0; i <= lecturas; i++) {
    Vtotal = Vtotal + readings[i];
  }

  Vmedia = Vtotal / lecturas;


  String automatico;

  if (test == 1) {

    automatico = "T";
    vAhora_real = velocidad;
  } else {


    if (switchActivado()) {
      ESC.writeMicroseconds(vAhora);
      automatico = "A";
    } else {
      ESC.writeMicroseconds(velocidad);
      automatico = "M";

      vAhora_real = vel_real;



    }
  }



  char vA[10];
  dtostrf(vAhora_real, 5, 0, vA);
  char vR[10];
  dtostrf(vel_real, 5, 0, vR);
  char vo[10];
  dtostrf(Vfinal, 5, 2, vo);
  char voM[10];
  dtostrf(Vmedia, 5, 2, voM);
  char Vis[10];


  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_helvR14_tr);
    u8g2.drawStr(14, 14, vA);
    u8g2.drawStr(60, 14, vR);

    u8g2.drawStr(58, 12, "/");
    u8g2.drawStr(80, 64, voM);
    u8g2.setFont(u8g2_font_logisoso24_tr);
    u8g2.drawStr(50, 44, vo);


    u8g2.setFont(u8g2_font_helvR08_tr);
    u8g2.drawStr(0, 12, "Vel:");
    u8g2.drawStr(0, 34, "Voltaje:");
    u8g2.drawStr(0, 62, "Voltaje Medio:");

    u8g2.setFont(u8g2_font_tenstamps_mf);

    if (automatico == "A") {
      u8g2.drawStr(110, 12, "A");
    }
    if (automatico == "M") {
      u8g2.drawStr(110, 12, "M");
    }
    if (automatico == "T") {
      u8g2.drawStr(110, 12, "T");
    }


  } while ( u8g2.nextPage() );

  printSerial();

}
