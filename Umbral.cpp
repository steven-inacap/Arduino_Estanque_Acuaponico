// ============ Umbral.cpp ============
#include "Umbral.h"

Umbral::Umbral() : tipo(TIPO_NIVEL), minimo(0), maximo(100), activo(false) {}

Umbral::Umbral(TipoParametro t, float min, float max)
    : tipo(t), minimo(min), maximo(max), activo(true) {}

bool Umbral::setUmbrales(float min, float max) {
    if (min >= max) return false;      // validación
    minimo = min;
    maximo = max;
    activo = true;
    return true;
}

bool Umbral::estaFueraDeRango(float valor) const {
    if (!activo) return false;
    return (valor < minimo || valor > maximo);
}

bool Umbral::estaBajoMinimo(float valor) const {
    if (!activo) return false;
    return valor < minimo;
}
