// ============ Umbral.h ============
#ifndef UMBRAL_H
#define UMBRAL_H

enum TipoParametro { TIPO_NIVEL, TIPO_TEMPERATURA, TIPO_PH, TIPO_OXIGENO };

class Umbral {
private:
    TipoParametro tipo;
    float minimo;
    float maximo;
    bool  activo;

public:
    Umbral();
    Umbral(TipoParametro t, float min, float max);
    
    bool  setUmbrales(float min, float max);   // valida min < max
    bool  estaFueraDeRango(float valor) const;
    bool  estaBajoMinimo(float valor) const;
    
    float getMinimo() const { return minimo; }
    float getMaximo() const { return maximo; }
    TipoParametro getTipo() const { return tipo; }
    bool  isActivo() const { return activo; }
    void  setActivo(bool a) { activo = a; }
};

#endif
