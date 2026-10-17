// ============ Bomba.h ============
#ifndef BOMBA_H
#define BOMBA_H

#include <Arduino.h>

enum EstadoBomba { BOMBA_INACTIVA, BOMBA_ACTIVA, BOMBA_FALLA };

class BombaAgua {
private:
    uint8_t       pinRele;
    EstadoBomba   estado;
    bool          releActivoBajo;
    unsigned long inicioActivacion, tiempoTotalActiva;
public:
    BombaAgua(uint8_t pin, bool activoBajo = true);
    void iniciar();
    void activar();
    void desactivar();
    EstadoBomba getEstado() const { return estado; }
    const char* getEstadoTexto() const;
    bool estaActiva() const { return estado == BOMBA_ACTIVA; }
};

#endif
