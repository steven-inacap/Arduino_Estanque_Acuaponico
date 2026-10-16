// ============ Bomba.cpp ============
#include "Bomba.h"

BombaAgua::BombaAgua(uint8_t pin, bool activoBajo)
    : pinRele(pin), estado(BOMBA_INACTIVA), releActivoBajo(activoBajo),
      ultimoCambio(0), tiempoTotalActiva(0), inicioActivacion(0) {}

void BombaAgua::iniciar() {
    pinMode(pinRele, OUTPUT);
    desactivar();          // arrancar apagada por seguridad
}

void BombaAgua::activar() {
    if (estado == BOMBA_ACTIVA) return;
    digitalWrite(pinRele, releActivoBajo ? LOW : HIGH);
    estado = BOMBA_ACTIVA;
    ultimoCambio = millis();
    inicioActivacion = millis();
}

void BombaAgua::desactivar() {
    if (estado == BOMBA_INACTIVA) return;
    if (estado == BOMBA_ACTIVA) {
        tiempoTotalActiva += millis() - inicioActivacion;
    }
    digitalWrite(pinRele, releActivoBajo ? HIGH : LOW);
    estado = BOMBA_INACTIVA;
    ultimoCambio = millis();
}

const char* BombaAgua::getEstadoTexto() const {
    switch (estado) {
        case BOMBA_ACTIVA:   return "ON";
        case BOMBA_INACTIVA: return "OFF";
        case BOMBA_FALLA:    return "FALLA";
    }
    return "?";
}

unsigned long BombaAgua::getTiempoActiva() const {
    unsigned long t = tiempoTotalActiva;
    if (estado == BOMBA_ACTIVA) t += millis() - inicioActivacion;
    return t;
}
