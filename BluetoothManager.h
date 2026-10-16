// ============ BluetoothManager.h ============
#ifndef BLUETOOTH_MANAGER_H
#define BLUETOOTH_MANAGER_H

#include <Arduino.h>
#include "Controlador.h"

class BluetoothManager {
private:
    Stream*  puerto;                 // Serial1 (Mega) o SoftwareSerial (Uno)
    String   bufferRx;
    bool     conectado;
    unsigned long ultimoMensaje;
    const unsigned long TIMEOUT_CONEXION = 30000;  // 30 s sin datos = desconectado

public:
    BluetoothManager(Stream* serial);
    void iniciar(unsigned long baud);
    void procesarComandos(Controlador& ctrl);
    void enviarLinea(const String& msg);
    void enviarLinea(const char* msg);
    bool estaConectado() const { return conectado; }
    void setConectado(bool c) { conectado = c; }

private:
    void procesarLinea(const String& linea, Controlador& ctrl);
    void responderOK(const String& eco);
    void responderError(const String& motivo);
};

#endif
