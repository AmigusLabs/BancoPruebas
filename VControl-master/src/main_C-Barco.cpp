#ifdef PLACA_BARCO
#pragma "# Compilando para la placa de Voltaje #"

#include "voltaje.h"
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <Servo.h>
#include <ThingSpeak.h>

#define RECEPTOR_CH3_PIN A1
#define RECEPTOR_CH5_PIN A2
#define SALIDA_ESC_PIN 10

const unsigned long TIEMPO_PASO_TEST = 1000;

// WIFI ESP-01

const int Rx = 10;            // Pin4 -> RX, conectar con Tx del modulo
const int Tx = 11;            // Pin3 -> TX  conectar con Rx del modulo
SoftwareSerial ESP01(Rx, Tx); // Establece comunicación serie en pines Rx y Tx

long baud = 115200;

// Voltaje

volatile int test = 0;
float iteracion_paso_test = 0;

Servo ESC;
int vel_real = 0;
int vAhora_real = 0;

float voltage_final = 0;
volatile int velocidad = 0;
int vAhora = 0;
int velmin = 0;
int incremento = 20;
int voltage_max = 12.0;

int iteraciones_escritura = 10;
int escribe = 0;

const int lecturas = 5;   // Total lecturas V
int index = 0;            // El indice de la lectura actual
float readings[lecturas]; // Lecturas de la entrada analogica
float Vtotal = 0.0;       // Total
float Vmedia = 0.0;       // Promedio

unsigned long timeout_pwm_micros = 50000;

volatile unsigned long siguientePaso = 0; // millis en los que tiene que saltar al siguiente paso

void test_funcion();

void setup()
{
  analogReference(INTERNAL);
  Serial.begin(baud);
  ESP01.begin(baud);

  pinMode(RECEPTOR_CH3_PIN, INPUT);
  pinMode(RECEPTOR_CH5_PIN, INPUT);

  Serial.print("Voltaje Máximo: ");
  Serial.print(voltajeMaximo());
  Serial.println("V");
  Serial.println("");

  ESC.attach(SALIDA_ESC_PIN); // salida al ESC a D10

  // WiFi crear AP

  ESP01.println("AT");
  ESP01.println("AT+CWMODE=2");
  ESP01.println("AT+CWSAP=\"AmigusLabs\",\"Amigus42\",5,3,1,0");
}

bool switchActivado()
{
  unsigned long value1 = pulseIn(RECEPTOR_CH5_PIN, HIGH, timeout_pwm_micros);
  unsigned long value2 = pulseIn(RECEPTOR_CH5_PIN, HIGH, timeout_pwm_micros);
  if ((value1 > 1600) && (value2 > 1600))
  {
    return false;
  }
  return true;
}

void printSerial()
{
  escribe++;
  if (escribe == iteraciones_escritura)
  {
    escribe = 0;

    Serial.print("Limitador activado:");
    Serial.println(switchActivado());
    Serial.print("V: ");
    Serial.println(voltage_final);
    Serial.print("Vel: ");
    Serial.println(velocidad);
    Serial.print("vAhora: ");

    Serial.println(vAhora);
  }
}

void loop()
{
  voltage_final = leerVoltaje();

  if (voltage_final < voltage_max && vAhora >= velmin)
  {
    vAhora = vAhora - incremento;
  }
  else if (vAhora < velocidad)
  {
    vAhora = vAhora + incremento;
  }
  else
  {
    vAhora = velocidad;
  }

  vel_real = map(velocidad, 995, 1989, 0, 100);
  vAhora_real = map(vAhora, 995, 1989, 0, 100);
  if (vAhora_real < 0)
  {
    vAhora_real = 0;
  }
  if (vel_real < 0)
  {
    vel_real = 0;
  }

  readings[index] = voltage_final;
  index = index + 1;

  if (index >= lecturas)
  {
    index = 0; // ...volvemos al inicio:
  }
  Vtotal = 0;

  for (int i = 0; i < lecturas; i++)
  {
    Vtotal = Vtotal + readings[i];
  }

  Vmedia = Vtotal / lecturas;

  String automatico;

  if (switchActivado())
  {
    ESC.writeMicroseconds(vAhora);
    automatico = "A";
  }
  else
  {
    ESC.writeMicroseconds(velocidad);
    automatico = "M";

    vAhora_real = vel_real;
  }

  printSerial();
}

#endif