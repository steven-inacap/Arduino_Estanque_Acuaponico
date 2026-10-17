// ============ Umbral.cpp ============
#include "Umbral.h"

Umbral::Umbral() : tipo(TIPO_NIVEL), minimo(0), maximo(100), activo(false) {}

Umbral::Umbral(TipoParametro t, float min, float max)
    : tipo(t), minimo(min), maximo(max), activo(true) {}

bool Umbral::setUmbrales(float min, float max) {
    if (min >= max) return false;
    minimo = min;
    maximo = max;
    activo = true;
    return true;
}

bool Umbral::estaFueraDeRango(float v) const {
    return activo && (v < minimo || v > maximo);
}

bool Umbral::estaBajoMinimo(float v) const {
    return activo && (v < minimo);
}
