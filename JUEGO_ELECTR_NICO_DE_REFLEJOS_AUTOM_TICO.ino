// ==========================================
// JUEGO ELECTRÓNICO DE REFLEJOS AUTOMÁTICO
// ==========================================

// -------- PINES DE LOS LEDS Y COMPONENTES --------
const int leds[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
const int cantidadLEDs = 10;

const int boton = 2;       // Pulsador conectado a D2 y GND
const int buzzer = 13;     // Buzzer activo conectado a D13 y GND

// Posición del LED objetivo (Índice 4 = Pin D7)
const int posicionRojo = 4;

// -------- NIVELES DE DIFICULTAD --------
const int velocidad[] = {250, 150, 80}; // Tiempo entre LEDs en ms

int nivel = 0;
int posicion = 0;

unsigned long tiempoAnterior = 0;
int estadoAnteriorBoton = HIGH; 

// Estados del juego
bool juegoIniciado = false;
bool juegoGanado = false;
int contadorPitidosVictoria = 0; // Para contar los 3 pitidos sincronizados

// ==========================================
// CONFIGURACIÓN INICIAL
// ==========================================

void setup() {
  // Configuración de LEDs como salida (inicialmente apagados)
  for (int i = 0; i < cantidadLEDs; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }

  // Configuración del Buzzer
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  // Configuración del Botón con resistencia de Pull-up interna
  pinMode(boton, INPUT_PULLUP);

  // Pausa de estabilización para evitar el falso arranque al encender
  delay(100);
  estadoAnteriorBoton = digitalRead(boton);

  // Estado inicial en reposo total
  juegoIniciado = false;
  juegoGanado = false;
}

// ==========================================
// PROGRAMA PRINCIPAL
// ==========================================

void loop() {
  int estadoBoton = digitalRead(boton);

  // Detectar pulsación (Flanco de bajada al presionar el botón a GND)
  if (estadoBoton == LOW && estadoAnteriorBoton == HIGH) {
    
    // 1. SI ESTÁ EN REPOSO O YA GANÓ:
    // El primer clic arranca una nueva partida desde el LED 0 (Nivel 1)
    if (!juegoIniciado || juegoGanado) {
      juegoGanado = false;
      juegoIniciado = true;
      nivel = 0;
      posicion = 0;
      contadorPitidosVictoria = 0;
      
      apagarTodosLEDs();
      digitalWrite(buzzer, LOW);
      digitalWrite(leds[posicion], HIGH);
      tiempoAnterior = millis();
    } 
    // 2. SI EL JUEGO YA ESTÁ EN CURSO:
    // Evalúa si la pulsación fue un acierto o un error
    else {
      evaluarRespuesta();
    }
    
    delay(250); // Antirrebote para prevenir lecturas dobles
  }

  estadoAnteriorBoton = estadoBoton;

  // Movimiento secuencial solo si el juego está activo y no se ha ganado
  if (juegoIniciado && !juegoGanado) {
    unsigned long tiempoActual = millis();

    if (tiempoActual - tiempoAnterior >= velocidad[nivel]) {
      digitalWrite(leds[posicion], LOW);

      posicion++;
      if (posicion >= cantidadLEDs) {
        posicion = 0;
      }

      digitalWrite(leds[posicion], HIGH);
      tiempoAnterior = tiempoActual;
    }
  }

  // Animación de victoria: LEDs parpadeando e intermitencia de 3 pitidos
  if (juegoGanado) {
    animacionVictoriaSincronizada();
  }
}

// ==========================================
// EVALUAR LA RESPUESTA DEL JUGADOR
// ==========================================

void evaluarRespuesta() {

  // ------------------------------------------
  // ACIERTO (SE DETIENE EN EL LED ROJO - PIN D7)
  // ------------------------------------------
  if (posicion == posicionRojo) {
    
    // Sonido de Acierto: Doble pitido del buzzer
    digitalWrite(buzzer, HIGH);
    delay(70);
    digitalWrite(buzzer, LOW);
    delay(70);
    digitalWrite(buzzer, HIGH);
    delay(70);
    digitalWrite(buzzer, LOW);

    // Parpadeo de acierto en el LED objetivo
    for (int i = 0; i < 3; i++) {
      digitalWrite(leds[posicion], LOW);
      delay(100);
      digitalWrite(leds[posicion], HIGH);
      delay(100);
    }

    nivel++;

    // Si supera los 3 niveles -> Activa el modo Victoria
    if (nivel >= 3) {
      juegoGanado = true;
      contadorPitidosVictoria = 0;
      return;
    }

    // SI ACIERTA: Apaga el LED actual y continúa el recorrido hacia el siguiente (Pin D8)
    digitalWrite(leds[posicion], LOW);
    posicion++;
    if (posicion >= cantidadLEDs) {
      posicion = 0;
    }
    digitalWrite(leds[posicion], HIGH);
    tiempoAnterior = millis();
  } 
  
  // ------------------------------------------
  // ERROR (SE DETIENE EN UN LED AMARILLO)
  // ------------------------------------------
  else {
    
    // Sonido de Error: Pitido largo
    digitalWrite(buzzer, HIGH);
    delay(400);
    digitalWrite(buzzer, LOW);

    // Parpadeo de error en el LED en que se equivocó
    for (int i = 0; i < 3; i++) {
      digitalWrite(leds[posicion], LOW);
      delay(80);
      digitalWrite(leds[posicion], HIGH);
      delay(80);
    }

    // SI FALLA: Reinicia a Nivel 1, vuelve al LED 0 y CONTINÚA AUTOMÁTICAMENTE
    nivel = 0;
    apagarTodosLEDs();
    posicion = 0;
    digitalWrite(leds[posicion], HIGH);
    tiempoAnterior = millis();
  }
}

// ==========================================
// FUNCIONES AUXILIARES
// ==========================================

void apagarTodosLEDs() {
  for (int i = 0; i < cantidadLEDs; i++) {
    digitalWrite(leds[i], LOW);
  }
}

// Parpadeo de LEDs y pitidos de victoria SIMULTÁNEOS
void animacionVictoriaSincronizada() {
  
  // Encender todos los LEDs del arreglo
  for (int i = 0; i < cantidadLEDs; i++) {
    digitalWrite(leds[i], HIGH);
  }
  
  // Forzar encendido del LED de color diferente (Pin D7 / índice 4)
  digitalWrite(leds[posicionRojo], HIGH);

  // El buzzer suena solo en las primeras 3 intermitencias
  if (contadorPitidosVictoria < 3) {
    digitalWrite(buzzer, HIGH);
  } else {
    digitalWrite(buzzer, LOW);
  }
  
  delay(200);

  // Apagar todos los LEDs y silenciar el buzzer
  apagarTodosLEDs();
  digitalWrite(buzzer, LOW);
  
  contadorPitidosVictoria++;
  delay(200);
}
