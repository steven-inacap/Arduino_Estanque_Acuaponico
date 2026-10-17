// ============ Bomba.cpp ============
#include "Bomba.h"

BombaAgua::BombaAgua(uint8_t pin, bool activoBajo)
    : pinRele(pin), estado(BOMBA_INACTIVA), releActivoBajo(activoBajo),
      inicioActivacion(0), tiempoTotalActiva(0) {}

void BombaAgua::iniciar() {
    pinMode(pinRele, OUTPUT);
    desactivar();   // fail-safe
}

void BombaAgua::activar() {
    if (estado == BOMBA_ACTIVA) return;
    digitalWrite(pinRele, releActivoBajo ? LOW : HIGH);
    estado = BOMBA_ACTIVA;
    inicioActivacion = millis();
}

void BombaAgua::desactivar() {
    if (estado == BOMBA_ACTIVA) {
        tiempoTotalActiva += millis() - inicioActivacion;
    }
    digitalWrite(pinRele, releActivoBajo ? HIGH : LOW);
    estado = BOMBA_INACTIVA;
}

const char* BombaAgua::getEstadoTexto() const {
    switch (estado) {
        case BOMBA_ACTIVA:   return "ON";
        case BOMBA_INACTIVA: return "OFF";
        case BOMBA_FALLA:    return "FALLA";
    }
    return "?";
}
