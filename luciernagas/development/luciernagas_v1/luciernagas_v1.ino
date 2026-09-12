enum Estado{
    APAGADA,
    SUBIENDO,
    BAJANDO};

struct Firefly{
    byte pin;
    byte brillo;
    byte brilloMaximo;
    byte incrementoSubida;
    byte incrementoBajada;
    byte intervaloSubida;
    byte intervaloBajada;
    Estado estado;
    unsigned long ultimoCambio;
    unsigned long siguiente;
};

Firefly luciernagas[4];
byte pines[] = {3, 5, 6, 9};
const unsigned long tiempo = 5000;
const unsigned long variabilidad = 5000;

void setup()
{
  randomSeed(analogRead(A7));
  for (int i = 0; i < 4; i++)
  {
    luciernagas[i].pin = pines[i];
    luciernagas[i].brillo = 0;
    luciernagas[i].estado = APAGADA;
    luciernagas[i].ultimoCambio = millis();
    luciernagas[i].siguiente = millis() + random(tiempo, tiempo + variabilidad);
    pinMode(luciernagas[i].pin, OUTPUT);
  }
}

void loop()
{
  for (int i = 0; i < 4; i++)
  {
    switch (luciernagas[i].estado) {
      case APAGADA:
        if (millis() >= luciernagas[i].siguiente) {
          luciernagas[i].estado = SUBIENDO;
          luciernagas[i].ultimoCambio = millis();
          // Parámetros de este destello
          luciernagas[i].brilloMaximo = random(50, 256);
          luciernagas[i].incrementoSubida = random(1, 4);
          luciernagas[i].incrementoBajada = random(1, 4);
          luciernagas[i].intervaloSubida = random(1, 4);
          luciernagas[i].intervaloBajada = random(5, 10);
          luciernagas[i].brillo = 0;
          analogWrite(luciernagas[i].pin, 0);
        }
        break;
      case SUBIENDO:
        if (millis() - luciernagas[i].ultimoCambio >= luciernagas[i].intervaloSubida) {
          luciernagas[i].ultimoCambio = millis();
          if (luciernagas[i].brillo + luciernagas[i].incrementoSubida >= luciernagas[i].brilloMaximo) {
            luciernagas[i].brillo = luciernagas[i].brilloMaximo;
            luciernagas[i].estado = BAJANDO;
          }
          else luciernagas[i].brillo += luciernagas[i].incrementoSubida;
          analogWrite(luciernagas[i].pin, luciernagas[i].brillo);
        }
        break;
      case BAJANDO:
        if (millis() - luciernagas[i].ultimoCambio >= luciernagas[i].intervaloBajada)
        {
          luciernagas[i].ultimoCambio = millis();

          if (luciernagas[i].brillo <= luciernagas[i].incrementoBajada) {
            luciernagas[i].brillo = 0;
            analogWrite(luciernagas[i].pin, 0);
            luciernagas[i].estado = APAGADA;
            luciernagas[i].siguiente = millis() + random(tiempo, tiempo + variabilidad);
          }
          else {
            luciernagas[i].brillo -= luciernagas[i].incrementoBajada;
            analogWrite(luciernagas[i].pin, luciernagas[i].brillo);
          }
        }
        break;
    }
  }
}

