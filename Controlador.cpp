// ============ Controlador.cpp ============
#include "Controlador.h"

Controlador::Controlador(SensorNivel* n, SensorTemperatura* t,
                         SensorPH* ph, BombaAgua* b)
    : sNivel(n), sTemp(t), sPH(ph), bomba(b),
      umbralNivel(TIPO_NIVEL, 30, 90),
      umbralTemp(TIPO_TEMPERATURA, 18, 30),
      umbralPH(TIPO_PH, 6.5, 8.5),
      modo(MODO_AUTOMATICO), alertaActiva(false),
      ultimaActivacion(0) {}

void Controlador::cicloMonitoreo() {
    float nivel = sNivel->leer();
    float temp  = sTemp->leer();
    float ph    = sPH->leer();
    verificarAlertas(nivel, temp, ph);
    if (modo == MODO_AUTOMATICO) {
        evaluarAutomatico(nivel, temp, ph);
    }
}

void Controlador::evaluarAutomatico(float nivel, float temp, float ph) {
    bool necesitaBomba = umbralNivel.estaBajoMinimo(nivel);
    bool nivelAlto     = (nivel > umbralNivel.getMaximo());
    bool tempCritica   = (temp > umbralTemp.getMaximo() + 5.0f);
    
    if (nivelAlto || tempCritica) {
        if (bomba->estaActiva()) bomba->desactivar();
        return;
    }
    
    if (necesitaBomba && !bomba->estaActiva()) {
        if (millis() - ultimaActivacion > MIN_TIEMPO_ENTRE_ACTIVACIONES) {
            bomba->activar();
            ultimaActivacion = millis();
        }
    }
    else if (!necesitaBomba && bomba->estaActiva()) {
        float objetivo = umbralNivel.getMinimo() * 1.10f;
        if (nivel >= objetivo) bomba->desactivar();
    }
}

void Controlador::verificarAlertas(float nivel, float temp, float ph) {
    alertaActiva = umbralNivel.estaFueraDeRango(nivel) ||
                   umbralTemp.estaFueraDeRango(temp)  ||
                   umbralPH.estaFueraDeRango(ph);
}

bool Controlador::configurarUmbral(TipoParametro tipo, float min, float max) {
    switch (tipo) {
        case TIPO_NIVEL:       return umbralNivel.setUmbrales(min, max);
        case TIPO_TEMPERATURA: return umbralTemp.setUmbrales(min, max);
        case TIPO_PH:          return umbralPH.setUmbrales(min, max);
        default:               return false;
    }
}

void Controlador::comandoManual(bool encender) {
    if (encender) bomba->activar();
    else          bomba->desactivar();
}
