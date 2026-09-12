/* ===================================================
LUCIÉRNAGAS
Se presentan dos tipos de luciérnagas: una versión analógica, controlada por PWM, y una versión digital.
La versión analógica está controlada por una máquina de estados que define cuatro estados posibles (APAGADA, SUBIENDO, ENCENDIDA y BAJANDO).
La versión digital está controlada por dos if/else, que evalúan el bool estado (estado == true, luciérnaga encendida; estado == false, luciérnaga apagada)
=================================================== */

enum Estado{
  APAGADA,
  SUBIENDO,
  ENCENDIDA,
  BAJANDO
};

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

struct SimpleFirefly{
  byte pin;
  bool estado;
  unsigned long inicio;
  unsigned long duracion;
};

//Configuración de los pines de Arduino Nano
byte pines_pwm[] = {3, 5, 6, 9, 10, 11};
byte pines_gpio[] = {2, 4, 7, 8, 12, 13};

const byte bajadaBrillo = 200;
const unsigned long tiempoApagada = 150;
const byte numeroLuciernagasRealistas = 6;
const byte numeroLuciernagasSimples = 6;

Firefly vagalume_pwm[numeroLuciernagasRealistas];
SimpleFirefly vagalume_gpio[numeroLuciernagasSimples];

void cambiar_brillo(byte i, int incrementoBrillo, unsigned long velocidadBrillo, unsigned long ahora) {
  if (ahora - vagalume_pwm[i].inicio < velocidadBrillo) return;
  vagalume_pwm[i].inicio = ahora;
  vagalume_pwm[i].brilloActual += incrementoBrillo;
  analogWrite(vagalume_pwm[i].pin, constrain(vagalume_pwm[i].brilloActual, 0, 255));
}

//La función vaiven baja el brillo parcialmente, y luego configura el estado de la luciérnaga para que vuelva a subir y baje con normalidad. Para ello:
//  - Setea a false el bool vaiven, de forma que no se vuelva a llamar a la función.
//  - Cambia el estado a SUBIENDO, para que el propio estado aumente de nuevo el brillo y luego baje con normalidad.
void vaiven(byte i, int incrementoBrillo, unsigned long velocidadBrillo, unsigned long ahora) {
  if (ahora - vagalume_pwm[i].inicio < velocidadBrillo) return;
  vagalume_pwm[i].inicio = ahora;
  vagalume_pwm[i].brilloActual -= incrementoBrillo;
  analogWrite(vagalume_pwm[i].pin, constrain(vagalume_pwm[i].brilloActual, 0, 255));
  if (vagalume_pwm[i].brilloActual <= (vagalume_pwm[i].brilloMaximo - bajadaBrillo)) {
    vagalume_pwm[i].vaiven = false;
    vagalume_pwm[i].estado = SUBIENDO;
  }
}

//Actualiza todos los valores aleatorios de la luciérnaga analógica
void random_firefly(byte i, unsigned long ahora) {
  vagalume_pwm[i].brilloActual = 0;
  vagalume_pwm[i].brilloMaximo = random(150, 256);
  vagalume_pwm[i].incrementoBrillo = random(10, 20);
  vagalume_pwm[i].velocidadBrillo = random(10, 20);
  vagalume_pwm[i].estado = APAGADA;
  vagalume_pwm[i].vaiven = random(2);
  vagalume_pwm[i].inicio = ahora;
  vagalume_pwm[i].duracion = random(5000, 10000);
}

void setup() {
  randomSeed(analogRead(A7));
  unsigned long ahora = millis();
  //Configuración de todas las luciérnagas analógicas
  for (int i = 0; i < numeroLuciernagasRealistas; i++) {
    vagalume_pwm[i].pin = pines_pwm[i];
    pinMode(vagalume_pwm[i].pin, OUTPUT);
    random_firefly(i, ahora);
  }
  //Configuración de todas las luciérnagas digitales
  for (int z = 0; z < numeroLuciernagasSimples; z++) {
    vagalume_gpio[z].pin = pines_gpio[z];
    pinMode(vagalume_gpio[z].pin, OUTPUT);
    digitalWrite(vagalume_gpio[z].pin, HIGH);
    vagalume_gpio[z].estado = true;
    vagalume_gpio[z].inicio = ahora;
    vagalume_gpio[z].duracion = random(10000, 15000);
  }
}

void loop() {
  unsigned long ahora = millis();

  //Bucle para las luciérnagas analógicas
  for (int i = 0; i < numeroLuciernagasRealistas; i++) {
    switch (vagalume_pwm[i].estado) {
      case APAGADA:
      //Transición obligatoria: APAGADA --> SUBIENDO (tras un tiempo apagada)
      if ((ahora - vagalume_pwm[i].inicio) >= vagalume_pwm[i].duracion) vagalume_pwm[i].estado = SUBIENDO;
      break;

      case SUBIENDO:
      if (vagalume_pwm[i].brilloActual < vagalume_pwm[i].brilloMaximo) cambiar_brillo(i, vagalume_pwm[i].incrementoBrillo, vagalume_pwm[i].velocidadBrillo, ahora);
      //Transicón obligatoria: SUBIENDO --> ENCENDIDA (con el brillo a tope)
      if (vagalume_pwm[i].brilloActual >= vagalume_pwm[i].brilloMaximo) vagalume_pwm[i].estado = ENCENDIDA;
      break;

      case ENCENDIDA:
      if (vagalume_pwm[i].vaiven) vaiven(i, vagalume_pwm[i].incrementoBrillo, vagalume_pwm[i].velocidadBrillo, ahora);
      //El vaiven devuelve la máquina de estados a SUBIENDO, esta vez con el bool vaiven a false. La próxima vez que entre en ENCENDIDA, pasará directamente a BAJANDO. 
      //Transición obligatoria: ENCENDIDA --> BAJANDO (no hay que esperar nada, excepto el vaiven)
      else vagalume_pwm[i].estado = BAJANDO;
      break;

      case BAJANDO:
      if (vagalume_pwm[i].brilloActual >= 1) cambiar_brillo(i, -vagalume_pwm[i].incrementoBrillo, vagalume_pwm[i].velocidadBrillo, ahora);
      //Transicón obligatoria: BAJANDO --> APAGADA (con el brillo a 0)
      if (vagalume_pwm[i].brilloActual <= 0) random_firefly(i, ahora);
      break;
    }
  }

  //Bucle para las luciérnagas digitales
  for (int z = 0; z < numeroLuciernagasSimples; z++) {
    if (vagalume_gpio[z].estado) {
      digitalWrite(vagalume_gpio[z].pin, HIGH);
      if ((ahora - vagalume_gpio[z].inicio) >= vagalume_gpio[z].duracion) {
        vagalume_gpio[z].estado = false;
        vagalume_gpio[z].inicio = ahora;
      } 
    }
    else {
      digitalWrite(vagalume_gpio[z].pin, LOW);
      if ((ahora - vagalume_gpio[z].inicio) >= tiempoApagada) {
        vagalume_gpio[z].estado = true;
        vagalume_gpio[z].inicio = ahora;
        vagalume_gpio[z].duracion = random(10000,15000);
      }
    }
  }
}