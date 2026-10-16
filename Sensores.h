// ============ Sensores.h ============
#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// -------- Clase abstracta (interfaz ISensorAgua) --------
class ISensorAgua {
public:
    virtual void  iniciar() = 0;
    virtual float leer()    = 0;
    virtual ~ISensorAgua() {}
};

// -------- Sensor de nivel (HC-SR04) --------
class SensorNivel : public ISensorAgua {
private:
    uint8_t pinTrig, pinEcho;
    float   nivelActual;
    float   alturaEstanque;    // cm
public:
    SensorNivel(uint8_t trig, uint8_t echo, float altura = 100.0);
    void  iniciar() override;
    float leer() override;
    float leerNivel() { return leer(); }
    float getNivelActual() const { return nivelActual; }
};

// -------- Sensor de temperatura (DS18B20) --------
class SensorTemperatura : public ISensorAgua {
private:
    uint8_t         pinDatos;
    OneWire*        oneWire;
    DallasTemperature* ds;
    float           ultimaTemp;
public:
    SensorTemperatura(uint8_t pin);
    void  iniciar() override;
    float leer() override;
    float leerTemperatura() { return leer(); }
};

// -------- Sensor de pH (analógico) --------
class SensorPH : public ISensorAgua {
private:
    uint8_t pinAnalogico;
    float   pHNeutro;      // voltaje a pH 7
    float   pendiente;     // V por unidad de pH
public:
    SensorPH(uint8_t pin, float pH7 = 2.5, float slope = 0.18);
    void  iniciar() override;
    float leer() override;
    float leerPH() { return leer(); }
};

#endif
