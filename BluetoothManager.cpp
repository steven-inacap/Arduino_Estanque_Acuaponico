// ============ BluetoothManager.cpp ============
#include "BluetoothManager.h"

BluetoothManager::BluetoothManager(Stream* serial)
    : puerto(serial), conectado(false), ultimoMensaje(0) {
    bufferRx.reserve(64);
}

void BluetoothManager::iniciar(unsigned long baud) {
    // En Mega, Serial1 se inicia directo.
    // En Uno, aquí iría: ((SoftwareSerial*)puerto)->begin(baud);
    Serial1.begin(baud);   // ⚠️ específico Mega
    bufferRx = "";
    conectado = true;      // HC-05 conectado por hardware al emparejar
    ultimoMensaje = millis();
}

void BluetoothManager::procesarComandos(Controlador& ctrl) {
    while (puerto->available()) {
        char c = puerto->read();
        ultimoMensaje = millis();
        conectado = true;
        
        if (c == '\n' || c == '\r') {
            if (bufferRx.length() > 0) {
                bufferRx.trim();
                if (bufferRx.length() > 0) {
                    procesarLinea(bufferRx, ctrl);
                }
                bufferRx = "";
            }
        } else if (bufferRx.length() < 60) {
            bufferRx += c;
        } else {
            bufferRx = "";   // descartar línea demasiado larga
        }
    }
    
    // Detectar desconexión por inactividad
    if (conectado && (millis() - ultimoMensaje > TIMEOUT_CONEXION)) {
        conectado = false;
    }
}

void BluetoothManager::procesarLinea(const String& linea, Controlador& ctrl) {
    // Protocolo: CMD:param1:param2:...
    // Ej: BOMBA:ON  |  UMBRAL:NIVEL:25:95
    
    int p1 = linea.indexOf(':');
    String cmd = (p1 < 0) ? linea : linea.substring(0, p1);
    cmd.toUpperCase();
    
    // --- BOMBA:ON / BOMBA:OFF ---
    if (cmd == "BOMBA") {
        if (p1 < 0) { responderError(F("falta_accion")); return; }
        String accion = linea.substring(p1 + 1);
        accion.toUpperCase();
        
        if (accion == "ON") {
            ctrl.comandoManual(true);
            responderOK(F("BOMBA:ON"));
        } else if (accion == "OFF") {
            ctrl.comandoManual(false);
            responderOK(F("BOMBA:OFF"));
        } else {
            responderError(F("accion_invalida"));
        }
        return;
    }
    
    // --- MODO:AUTO / MODO:MANUAL ---
    if (cmd == "MODO") {
        if (p1 < 0) { responderError(F("falta_modo")); return; }
        String m = linea.substring(p1 + 1);
        m.toUpperCase();
        
        if (m == "AUTO") {
            ctrl.setModo(MODO_AUTOMATICO);
            responderOK(F("MODO:AUTO"));
        } else if (m == "MANUAL") {
            ctrl.setModo(MODO_MANUAL);
            responderOK(F("MODO:MANUAL"));
        } else {
            responderError(F("modo_invalido"));
        }
        return;
    }
    
    // --- UMBRAL:TIPO:min:max ---
    if (cmd == "UMBRAL") {
        int p2 = linea.indexOf(':', p1 + 1);
        int p3 = linea.indexOf(':', p2 + 1);
        if (p1 < 0 || p2 < 0 || p3 < 0) {
            responderError(F("formato_umbral"));
            return;
        }
        
        String param = linea.substring(p1 + 1, p2);
        param.toUpperCase();
        float vmin = linea.substring(p2 + 1, p3).toFloat();
        float vmax = linea.substring(p3 + 1).toFloat();
        
        TipoParametro t;
        if      (param == "NIVEL") t = TIPO_NIVEL;
        else if (param == "TEMP")  t = TIPO_TEMPERATURA;
        else if (param == "PH")    t = TIPO_PH;
        else { responderError(F("param_invalido")); return; }
        
        if (ctrl.configurarUmbral(t, vmin, vmax)) {
            char buf[48];
            snprintf(buf, sizeof(buf),
                "OK:UMBRAL:%s:%.1f:%.1f",
                param.c_str(), vmin, vmax);
            enviarLinea(buf);
        } else {
            responderError(F("rango_invalido"));
        }
        return;
    }
    
    // --- ESTADO ---
    if (cmd == "ESTADO") {
        enviarLinea(F("INFO:consultar_estado_periodico"));
        return;
    }
    
    // --- AYUDA ---
    if (cmd == "AYUDA" || cmd == "HELP") {
        enviarLinea(F("CMDS:BOMBA:ON/OFF,MODO:AUTO/MANUAL,"));
        enviarLinea(F("UMBRAL:NIVEL|TEMP|PH:min:max,ESTADO"));
        return;
    }
    
    // --- PING (para mantener conexión viva) ---
    if (cmd == "PING") {
        enviarLinea(F("PONG"));
        return;
    }
    
    // --- RESET (reinicia umbrales) ---
    if (cmd == "RESET") {
        ctrl.configurarUmbral(TIPO_NIVEL,       30.0, 90.0);
        ctrl.configurarUmbral(TIPO_TEMPERATURA, 18.0, 30.0);
        ctrl.configurarUmbral(TIPO_PH,           6.5,  8.5);
        ctrl.setModo(MODO_AUTOMATICO);
        responderOK(F("RESET"));
        return;
    }
    
    responderError(F("cmd_desconocido"));
}

void BluetoothManager::responderOK(const String& eco) {
    enviarLinea("OK:" + eco);
}

void BluetoothManager::responderError(const String& motivo) {
    enviarLinea("ERR:" + motivo);
}

void BluetoothManager::enviarLinea(const String& msg) {
    puerto->println(msg);
}

void BluetoothManager::enviarLinea(const char* msg) {
    puerto->println(msg);
}
