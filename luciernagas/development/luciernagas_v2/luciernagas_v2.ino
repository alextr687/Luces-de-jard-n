/*
IDEAS DE CHATTY
- Dar contexto al 200 de la función vaiven.
- crear una función void inicializar_destello(byte i) que genere todos los números aleatorios
- Añadir personaidad a las luciérnagas
*/

enum Estado{
    APAGADA,
    SUBIENDO,
    ENCENDIDA,
    BAJANDO};

struct Firefly{
    byte pin;
    int brilloActual;
    byte brilloMaximo;
    int incrementoBrillo;
    unsigned long velocidadBrillo;
    Estado estado;
    bool vaiven;
    unsigned long inicio;
    unsigned long duracion;
};

Firefly luciernagas[6];
byte pines[] = {3, 5, 6, 9, 10, 11};

void cambiar_brillo(byte i, int incrementoBrillo, unsigned long velocidadBrillo) {
  if (millis() - luciernagas[i].inicio < velocidadBrillo) return;
  luciernagas[i].inicio = millis();
  luciernagas[i].brilloActual += incrementoBrillo;
  analogWrite(luciernagas[i].pin, constrain(luciernagas[i].brilloActual, 0, 255));
  return;
}

void vaiven(byte i, int incrementoBrillo, unsigned long velocidadBrillo) {
  if (millis() - luciernagas[i].inicio < velocidadBrillo) return;
  luciernagas[i].inicio = millis();
  luciernagas[i].brilloActual -= incrementoBrillo;
  analogWrite(luciernagas[i].pin, constrain(luciernagas[i].brilloActual, 0, 255));
  if (luciernagas[i].brilloActual <= (luciernagas[i].brilloMaximo - 200)) {
    luciernagas[i].vaiven = false;
    luciernagas[i].estado = SUBIENDO;
  }
}

void setup() {
  randomSeed(analogRead(A7));
  Serial.begin(9600);
  Serial.println("Inicio de programa");
  for (int i = 0; i < 6; i++) {
    luciernagas[i].pin = pines[i];
    pinMode(luciernagas[i].pin, OUTPUT);
    luciernagas[i].brilloActual = 0;
    luciernagas[i].estado = APAGADA;
    luciernagas[i].brilloMaximo = random(150, 256);
    luciernagas[i].incrementoBrillo = random(10, 20);
    luciernagas[i].velocidadBrillo = random(10, 20);
    luciernagas[i].vaiven = random(2);
    luciernagas[i].duracion = random(2000, 5000);
    luciernagas[i].inicio = millis();
  }
}

void loop() {
  unsigned long ahora = millis();
  for (int i = 0; i < 6; i++) {
    switch (luciernagas[i].estado) {
      case APAGADA:
      //Transición obligatoria: APAGADA --> SUBIENDO (tras un tiempo apagada)
      if ((ahora - luciernagas[i].inicio) >= luciernagas[i].duracion) luciernagas[i].estado = SUBIENDO;
      break;

      case SUBIENDO:
      if (luciernagas[i].brilloActual < luciernagas[i].brilloMaximo) cambiar_brillo(i, luciernagas[i].incrementoBrillo, luciernagas[i].velocidadBrillo);
      //Transicón obligatoria: SUBIENDO --> ENCENDIDA (con el brillo a tope)
      if (luciernagas[i].brilloActual >= luciernagas[i].brilloMaximo) luciernagas[i].estado = ENCENDIDA;
      break;

      case ENCENDIDA:
      if (luciernagas[i].vaiven) vaiven(i, luciernagas[i].incrementoBrillo, luciernagas[i].velocidadBrillo);
      //Transición obligatoria: ENCENDIDA --> BAJANDO (no hay que esperar nada, excepto el vaiven)
      else luciernagas[i].estado = BAJANDO;
      break;

      case BAJANDO:
      if (luciernagas[i].brilloActual >= 1) cambiar_brillo(i, -luciernagas[i].incrementoBrillo, luciernagas[i].velocidadBrillo);
      //Transicón obligatoria: BAJANDO --> APAGADA (con el brillo a 0)
      if (luciernagas[i].brilloActual <= 0) {
        luciernagas[i].estado = APAGADA;
        luciernagas[i].brilloMaximo = random(150, 255);
        luciernagas[i].incrementoBrillo = random(10, 20);
        luciernagas[i].velocidadBrillo = random(10, 20);
        luciernagas[i].vaiven = random(2);
        luciernagas[i].duracion = random(5000, 10000);
        luciernagas[i].inicio = millis();

        Serial.print("Estado de la luciérnaga ");
        Serial.print(i);
        Serial.print(". Brillo máximo: ");
        Serial.print(luciernagas[i].brilloMaximo);
        Serial.print(". Próximo incremento: ");
        Serial.print(luciernagas[i].incrementoBrillo);
        Serial.print(". Próxima duración: ");
        Serial.print(luciernagas[i].duracion);
        Serial.print(". Vaiven? ");
        Serial.println(luciernagas[i].vaiven);
      }
      break;
    }
  }
}