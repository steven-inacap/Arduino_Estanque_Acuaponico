// ============ Umbral.h ============
#ifndef UMBRAL_H
#define UMBRAL_H

#include <Arduino.h>

enum TipoParametro { TIPO_NIVEL, TIPO_TEMPERATURA, TIPO_PH };

class Umbral {
private:
    TipoParametro tipo;
    float minimo, maximo;
    bool  activo;
public:
    Umbral();
    Umbral(TipoParametro t, float min, float max);
    bool  setUmbrales(float min, float max);
    bool  estaFueraDeRango(float v) const;
    bool  estaBajoMinimo(float v) const;
    float getMinimo() const { return minimo; }
    float getMaximo() const { return maximo; }
    TipoParametro getTipo() const { return tipo; }
};

#endif
