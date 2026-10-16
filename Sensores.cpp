// ============ Sensores.cpp ============
#include "Sensores.h"

// ---------- SensorNivel ----------
SensorNivel::SensorNivel(uint8_t trig, uint8_t echo, float altura)
    : pinTrig(trig), pinEcho(echo), nivelActual(0), alturaEstanque(altura) {}

void SensorNivel::iniciar() {
    pinMode(pinTrig, OUTPUT);
    pinMode(pinEcho, INPUT);
    digitalWrite(pinTrig, LOW);
}

float SensorNivel::leer() {
    // Pulso de 10us en TRIG
    digitalWrite(pinTrig, LOW);  delayMicroseconds(2);
    digitalWrite(pinTrig, HIGH); delayMicroseconds(10);
    digitalWrite(pinTrig, LOW);
    
    // Medir eco (timeout 30ms)
    unsigned long dur = pulseIn(pinEcho, HIGH, 30000UL);
    if (dur == 0) return nivelActual;   // sin eco, mantener último
    
    // Distancia sensor→superficie (cm)
    float distancia = dur * 0.0343f / 2.0f;
    if (distancia < 0)      distancia = 0;
    if (distancia > alturaEstanque) distancia = alturaEstanque;
    
    // Nivel de agua = altura total - distancia
    nivelActual = alturaEstanque - distancia;
    return nivelActual;
}

// ---------- SensorTemperatura ----------
SensorTemperatura::SensorTemperatura(uint8_t pin)
    : pinDatos(pin), oneWire(nullptr), ds(nullptr), ultimaTemp(25.0) {}

void SensorTemperatura::iniciar() {
    oneWire = new OneWire(pinDatos);
    ds      = new DallasTemperature(oneWire);
    ds->begin();
}

float SensorTemperatura::leer() {
    ds->requestTemperatures();
    float t = ds->getTempCByIndex(0);
    if (t == DEVICE_DISCONNECTED_C) return ultimaTemp;  // sensor desconectado
    ultimaTemp = t;
    return t;
}

// ---------- SensorPH ----------
SensorPH::SensorPH(uint8_t pin, float pH7, float slope)
    : pinAnalogico(pin), pHNeutro(pH7), pendiente(slope) {}

void SensorPH::iniciar() {
    pinMode(pinAnalogico, INPUT);
}

float SensorPH::leer() {
    // Promedio de 10 muestras para estabilizar
    long suma = 0;
    for (uint8_t i = 0; i < 10; i++) {
        suma += analogRead(pinAnalogico);
        delay(5);
    }
    float adc = suma / 10.0f;
    float voltaje = adc * (5.0f / 1023.0f);
    // pH = 7 + (V_neutro - V) / pendiente
    float ph = 7.0f + (pHNeutro - voltaje) / pendiente;
    if (ph < 0)  ph = 0;
    if (ph > 14) ph = 14;
    return ph;
}
