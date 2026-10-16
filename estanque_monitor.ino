/**
 * Sistema de Monitoreo de Agua en Estanque
 * con Activación de Bomba - Acuapónico/Acuícola
 * 
 * Hardware:
 *  - Sensor ultrasónico HC-SR04 (nivel de agua)
 *  - Sensor DS18B20 (temperatura)
 *  - Sensor pH analógico (ej. PH-4502C)
 *  - Sensor de oxígeno disuelto (opcional)
 *  - Relé para bomba de agua
 *  - Módulo ESP8266/ESP32 o HC-05 para control remoto
 * 
 * Comunicación serial: 9600 baudios
 * Protocolo de comandos: "CMD:param:valor\n"
 */

#include "Sensores.h"
#include "Bomba.h"
#include "Umbral.h"
#include "Controlador.h"

// ============ PINES ============
#define PIN_TRIG        7
#define PIN_ECHO        8
#define PIN_TEMP        9      // DS18B20
#define PIN_PH          A0
#define PIN_OXIGENO     A1
#define PIN_RELE_BOMBA  10
#define PIN_LED_ALERTA  13

// ============ OBJETOS GLOBALES ============
SensorNivel      sensorNivel(PIN_TRIG, PIN_ECHO);
SensorTemperatura sensorTemp(PIN_TEMP);
SensorPH         sensorPH(PIN_PH);
BombaAgua        bomba(PIN_RELE_BOMBA);
Controlador      controlador(&sensorNivel, &sensorTemp, &sensorPH, &bomba);

// Intervalo de muestreo
unsigned long ultimaLectura = 0;
const unsigned long INTERVALO_LECTURA = 2000;  // 2 segundos

// ============ SETUP ============
void setup() {
    Serial.begin(9600);
    Serial.println(F("=== Sistema Estanque Acuapónico v1.0 ==="));
    
    pinMode(PIN_LED_ALERTA, OUTPUT);
    digitalWrite(PIN_LED_ALERTA, LOW);
    
    // Inicializar subsistemas
    sensorNivel.iniciar();
    sensorTemp.iniciar();
    sensorPH.iniciar();
    bomba.iniciar();
    
    // Umbrales por defecto (personalizables remotamente)
    controlador.configurarUmbral(TIPO_NIVEL,       30.0, 90.0);   // cm
    controlador.configurarUmbral(TIPO_TEMPERATURA, 18.0, 30.0);   // °C
    controlador.configurarUmbral(TIPO_PH,           6.5,  8.5);
    
    controlador.setModo(MODO_AUTOMATICO);
    
    Serial.println(F("Sistema listo. Modo: AUTOMATICO"));
    imprimirAyuda();
}

// ============ LOOP PRINCIPAL ============
void loop() {
    // 1. Procesar comandos remotos (no bloqueante)
    procesarComandoSerial();
    
    // 2. Ciclo de monitoreo
    if (millis() - ultimaLectura >= INTERVALO_LECTURA) {
        ultimaLectura = millis();
        controlador.cicloMonitoreo();
        imprimirEstado();
    }
}

// ============ COMUNICACIÓN REMOTA ============
void procesarComandoSerial() {
    if (!Serial.available()) return;
    
    String linea = Serial.readStringUntil('\n');
    linea.trim();
    if (linea.length() == 0) return;
    
    // Formato: CMD:parametro:valor1:valor2
    // Ejemplos:
    //   BOMBA:ON
    //   BOMBA:OFF
    //   MODO:AUTO | MODO:MANUAL
    //   UMBRAL:NIVEL:25:95
    //   UMBRAL:TEMP:16:32
    //   UMBRAL:PH:6.0:9.0
    //   ESTADO
    
    int p1 = linea.indexOf(':');
    if (p1 < 0) { Serial.println(F("ERR:formato")); return; }
    
    String cmd = linea.substring(0, p1);
    cmd.toUpperCase();
    
    if (cmd == "BOMBA") {
        String accion = linea.substring(p1 + 1);
        accion.toUpperCase();
        if (accion == "ON") {
            controlador.comandoManual(true);
            Serial.println(F("OK:BOMBA:ON"));
        } else if (accion == "OFF") {
            controlador.comandoManual(false);
            Serial.println(F("OK:BOMBA:OFF"));
        } else {
            Serial.println(F("ERR:bomba"));
        }
    }
    else if (cmd == "MODO") {
        String m = linea.substring(p1 + 1);
        m.toUpperCase();
        if (m == "AUTO")   { controlador.setModo(MODO_AUTOMATICO); Serial.println(F("OK:MODO:AUTO")); }
        else if (m == "MANUAL") { controlador.setModo(MODO_MANUAL); Serial.println(F("OK:MODO:MANUAL")); }
        else Serial.println(F("ERR:modo"));
    }
    else if (cmd == "UMBRAL") {
        int p2 = linea.indexOf(':', p1 + 1);
        int p3 = linea.indexOf(':', p2 + 1);
        if (p2 < 0 || p3 < 0) { Serial.println(F("ERR:umbral")); return; }
        
        String param = linea.substring(p1 + 1, p2);
        param.toUpperCase();
        float vmin = linea.substring(p2 + 1, p3).toFloat();
        float vmax = linea.substring(p3 + 1).toFloat();
        
        TipoParametro t;
        if      (param == "NIVEL") t = TIPO_NIVEL;
        else if (param == "TEMP")  t = TIPO_TEMPERATURA;
        else if (param == "PH")    t = TIPO_PH;
        else { Serial.println(F("ERR:param")); return; }
        
        if (controlador.configurarUmbral(t, vmin, vmax)) {
            Serial.print(F("OK:UMBRAL:")); Serial.print(param);
            Serial.print(':'); Serial.print(vmin);
            Serial.print(':'); Serial.println(vmax);
        } else {
            Serial.println(F("ERR:rango"));
        }
    }
    else if (cmd == "ESTADO") {
        imprimirEstado();
    }
    else if (cmd == "AYUDA" || cmd == "HELP") {
        imprimirAyuda();
    }
    else {
        Serial.println(F("ERR:cmd_desconocido"));
    }
}

// ============ SALIDA DE ESTADO ============
void imprimirEstado() {
    Serial.print(F("NIVEL="));  Serial.print(sensorNivel.leerNivel(), 1);
    Serial.print(F(" TEMP="));  Serial.print(sensorTemp.leerTemperatura(), 1);
    Serial.print(F(" PH="));    Serial.print(sensorPH.leerPH(), 2);
    Serial.print(F(" BOMBA=")); Serial.print(bomba.getEstadoTexto());
    Serial.print(F(" MODO="));  Serial.print(controlador.getModoTexto());
    Serial.print(F(" ALERTA=")); Serial.println(controlador.hayAlerta() ? "SI" : "NO");
    
    digitalWrite(PIN_LED_ALERTA, controlador.hayAlerta() ? HIGH : LOW);
}

void imprimirAyuda() {
    Serial.println(F("--- Comandos ---"));
    Serial.println(F("BOMBA:ON | BOMBA:OFF"));
    Serial.println(F("MODO:AUTO | MODO:MANUAL"));
    Serial.println(F("UMBRAL:NIVEL:min:max"));
    Serial.println(F("UMBRAL:TEMP:min:max"));
    Serial.println(F("UMBRAL:PH:min:max"));
    Serial.println(F("ESTADO | AYUDA"));
}
