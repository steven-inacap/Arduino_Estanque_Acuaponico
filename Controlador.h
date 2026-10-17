// ============ Controlador.h ============
#ifndef CONTROLADOR_H
#define CONTROLADOR_H

#include "Sensores.h"
#include "Bomba.h"
#include "Umbral.h"

enum ModoOperacion { MODO_AUTOMATICO, MODO_MANUAL };

class Controlador {
private:
    SensorNivel*       sNivel;
    SensorTemperatura* sTemp;
    SensorPH*          sPH;
    BombaAgua*         bomba;
    Umbral umbralNivel, umbralTemp, umbralPH;
    ModoOperacion modo;
    bool alertaActiva;
    unsigned long ultimaActivacion;
    const unsigned long MIN_TIEMPO_ENTRE_ACTIVACIONES = 3000;
public:
    Controlador(SensorNivel*, SensorTemperatura*, SensorPH*, BombaAgua*);
    void cicloMonitoreo();
    bool configurarUmbral(TipoParametro tipo, float min, float max);
    void comandoManual(bool encender);
    void setModo(ModoOperacion m) { modo = m; }
    const char* getModoTexto() const {
        return (modo == MODO_AUTOMATICO) ? "AUTO" : "MANUAL";
    }
    bool hayAlerta() const { return alertaActiva; }
private:
    void evaluarAutomatico(float nivel, float temp, float ph);
    void verificarAlertas(float nivel, float temp, float ph);
};

#endif
