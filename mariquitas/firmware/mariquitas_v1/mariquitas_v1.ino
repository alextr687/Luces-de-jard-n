/* ===================================================
MARIQUITAS
El código selecciona entre 3 y 4 mariquitas al azar para mantenerse encendidas.
Al detectar la entrada del sensor PIR, durante tiempoEspera las mariquitas cambian su estado de encendido a apagado, y viceversa.
Tras esos minutos, todas las mariquitas (excepto las que siempre están encendidas) vuelven a apagarse.
=================================================== */

struct Ladybug{
  byte pin;
  bool permanente;
  bool estado;
  unsigned long inicio;
  unsigned long duracionEncendida;
  unsigned long duracionApagada;
};

//Configuración de los pines de Arduino Nano
byte pines[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
#define sensor 2

const byte numeroXoaninhas = 10;
const unsigned long tiempoEspera = 300000; //5 minutos
unsigned long ultimaActivacion = 0;
bool modoActivo = false;

Ladybug xoaninha[numeroXoaninhas];

void setup() {
  randomSeed(analogRead(A7));
  byte numPermanentes = random(3, 5);
  unsigned long ahora = millis();
  //Configuración de todas las mariquitas
  for (int z = 0; z < numeroXoaninhas; z++) {
    xoaninha[z].pin = pines[z];
    pinMode(xoaninha[z].pin, OUTPUT);
    xoaninha[z].estado = false;
    xoaninha[z].permanente = false;
    xoaninha[z].inicio = ahora;
    xoaninha[z].duracionEncendida = random(10000, 15000);
    xoaninha[z].duracionApagada = random(10000, 15000);
  }
  //Elección de las mariquitas permanentes
  byte elegidas = 0;
  while (elegidas < numPermanentes) {
    byte indice = random(numeroXoaninhas);
    if (!xoaninha[indice].permanente) {
      xoaninha[indice].permanente = true;
      elegidas++;
    }
  }
  //Encendido de las permanentes
  for (int z = 0; z < numeroXoaninhas; z++) {
    if (xoaninha[z].permanente) digitalWrite(xoaninha[z].pin, HIGH);
    else digitalWrite(xoaninha[z].pin, LOW);
  }
  //Configuración del sensor PIR
  pinMode(sensor, INPUT);
}

void loop() {
  unsigned long ahora = millis();

  //En caso de que se detecte movimiento inicia un contador de "persona detectada"
  if (digitalRead(sensor)) ultimaActivacion = ahora;
  bool nuevoModoActivo = ((ahora - ultimaActivacion) < tiempoEspera);

  //En caso de "persona detectada" se actualizan los estados de las mariquitas
  if (nuevoModoActivo) {
    for (int z = 0; z < numeroXoaninhas; z++) {
      if (xoaninha[z].permanente) {
        digitalWrite(xoaninha[z].pin, HIGH); //Un poco innecesario pero mejor que sobre que no que falte
        continue;
      }
      if (xoaninha[z].estado) {
        digitalWrite(xoaninha[z].pin, HIGH);
        if ((ahora - xoaninha[z].inicio) >= xoaninha[z].duracionEncendida) {
          xoaninha[z].estado = false;
          xoaninha[z].inicio = ahora;
        } 
      }
      else {
        digitalWrite(xoaninha[z].pin, LOW);
        if ((ahora - xoaninha[z].inicio) >= xoaninha[z].duracionApagada) {
          xoaninha[z].estado = true;
          xoaninha[z].inicio = ahora;
          xoaninha[z].duracionEncendida = random(10000, 15000);
          xoaninha[z].duracionApagada = random(10000, 15000);
        }
      }
    }
  }

  //Cuando termina el estado de actividad, se reinician las mariquitas no permanentes
  //Algunas se encenderán al instante, otras tardarán más en encenderse
  if (modoActivo && !nuevoModoActivo) {
    for (int z = 0; z < numeroXoaninhas; z++){
      if (!xoaninha[z].permanente) {
        digitalWrite(xoaninha[z].pin, LOW);
        xoaninha[z].estado = random(2);
        xoaninha[z].inicio = ahora;
        xoaninha[z].duracionEncendida = random(10000, 15000);
        xoaninha[z].duracionApagada = random(10000, 15000);
      }
    }
  }
  modoActivo = nuevoModoActivo;
}