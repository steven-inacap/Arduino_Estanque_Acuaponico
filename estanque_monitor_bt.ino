/**
 * Sistema de Monitoreo de Agua en Estanque - Bluetooth HC-05
 * Control por Serial1 (pines 18/19 en Mega) o SoftwareSerial (Uno)
 * 
 * Protocolo: comandos terminados en '\n'
 *   BOMBA:ON | BOMBA:OFF
 *   MODO:AUTO | MODO:MANUAL
 *   UMBRAL:NIVEL:min:max
 *   UMBRAL:TEMP:min:max
 *   UMBRAL:PH:min:max
 *   ESTADO | AYUDA | RESET
 */

#include "Sensores.h"
#include "Bomba.h"
#include "Umbral.h"
#include "Controlador.h"
#include "BluetoothManager.h"

// ============ PINES ============
#define PIN_TRIG        7
#define PIN_ECHO        8
#define PIN_TEMP        9
#define PIN_PH          A0
#define PIN_RELE_BOMBA  10
#define PIN_LED_ALERTA  13

// Bluetooth en Mega: Serial1 (pines 18=TX, 19=RX)
// En Uno/Nano: usar SoftwareSerial(10, 11)
#define BT_BAUD        9600

// ============ OBJETOS GLOBALES ============
SensorNivel       sensorNivel(PIN_TRIG, PIN_ECHO, 100.0);
SensorTemperatura sensorTemp(PIN_TEMP);
SensorPH          sensorPH(PIN_PH);
BombaAgua         bomba(PIN_RELE_BOMBA);
Controlador       controlador(&sensorNivel, &sensorTemp, &sensorPH, &bomba);
BluetoothManager  bt(&Serial1);   // Mega usa Serial1

// ============ TEMPORIZACIÓN ============
unsigned long ultimaLectura = 0;
unsigned long ultimoEnvioBT = 0;
const unsigned long INTERVALO_LECTURA = 2000;   // 2 s
const unsigned long INTERVALO_ENVIO_BT = 5000;  // 5 s (estado periódico)

// ============ SETUP ============
void setup() {
    Serial.begin(9600);              // USB debug
    Serial.println(F("=== Estanque BT v1.0 ==="));
    
    pinMode(PIN_LED_ALERTA, OUTPUT);
    digitalWrite(PIN_LED_ALERTA, LOW);
    
    // Iniciar Bluetooth
    bt.iniciar(BT_BAUD);
    bt.enviarLinea(F("BT:READY"));    // notificación al emparejar
    
    // Iniciar sensores
    sensorNivel.iniciar();
    sensorTemp.iniciar();
    sensorPH.iniciar();
    bomba.iniciar();
    
    // Umbrales por defecto (personalizables vía BT)
    controlador.configurarUmbral(TIPO_NIVEL,       30.0, 90.0);
    controlador.configurarUmbral(TIPO_TEMPERATURA, 18.0, 30.0);
    controlador.configurarUmbral(TIPO_PH,           6.5,  8.5);
    controlador.setModo(MODO_AUTOMATICO);
    
    Serial.println(F("Sistema listo. Esperando conexión BT..."));
}

// ============ LOOP PRINCIPAL ============
void loop() {
    // 1. Procesar comandos entrantes por Bluetooth
    bt.procesarComandos(controlador);
    
    // 2. Ciclo de monitoreo
    if (millis() - ultimaLectura >= INTERVALO_LECTURA) {
        ultimaLectura = millis();
        controlador.cicloMonitoreo();
        actualizarLED();
    }
    
    // 3. Envío periódico de estado por BT
    if (millis() - ultimoEnvioBT >= INTERVALO_ENVIO_BT && bt.estaConectado()) {
        ultimoEnvioBT = millis();
        enviarEstadoBT();
    }
}

// ============ ESTADO POR BLUETOOTH ============
void enviarEstadoBT() {
    // Formato: STATE:nivel:temp:ph:bomba:modo:alerta
    char buf[80];
    snprintf(buf, sizeof(buf),
        "STATE:%.1f:%.1f:%.2f:%s:%s:%d",
        sensorNivel.leerNivel(),
        sensorTemp.leerTemperatura(),
        sensorPH.leerPH(),
        bomba.getEstadoTexto(),
        controlador.getModoTexto(),
        controlador.hayAlerta() ? 1 : 0);
    bt.enviarLinea(buf);
}

void actualizarLED() {
    digitalWrite(PIN_LED_ALERTA,
        controlador.hayAlerta() ? HIGH : LOW);
}
