// ============ Sensores.h ============
#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class ISensorAgua {
public:
    virtual void  iniciar() = 0;
    virtual float leer()    = 0;
    virtual ~ISensorAgua() {}
};

class SensorNivel : public ISensorAgua {
private:
    uint8_t pinTrig, pinEcho;
    float nivelActual, alturaEstanque;
public:
    SensorNivel(uint8_t trig, uint8_t echo, float altura = 100.0);
    void  iniciar() override;
    float leer() override;
    float leerNivel() { return leer(); }
    float getNivelActual() const { return nivelActual; }
};

class SensorTemperatura : public ISensorAgua {
private:
    uint8_t pinDatos;
    OneWire* oneWire;
    DallasTemperature* ds;
    float ultimaTemp;
public:
    SensorTemperatura(uint8_t pin);
    void  iniciar() override;
    float leer() override;
    float leerTemperatura() { return leer(); }
};

class SensorPH : public ISensorAgua {
private:
    uint8_t pinAnalogico;
    float pHNeutro, pendiente;
public:
    SensorPH(uint8_t pin, float pH7 = 2.5, float slope = 0.18);
    void  iniciar() override;
    float leer() override;
    float leerPH() { return leer(); }
};

#endif
