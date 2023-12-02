

#include <Adafruit_NeoPixel.h>  
#include "EEPROMAnything.h"
#include <SoftwareSerial.h>

      

        
// Tamaño de la cuadrícula de LEDs
#define GRID_W              (8)
#define GRID_H              (16)
#define STRAND_LENGTH       (GRID_W*GRID_H)
#define LED_DATA_PIN        (6)
// ¿Conectaste tu cuadrícula en forma de 'S' en lugar de 'Z'? Cambia este valor a 0.
#define BACKWARDS           (0)
// Tamaño máximo de cada pieza de Tetris
#define PIECE_W             (4)
#define PIECE_H             (4)
// Cantidad de tipos de piezas
#define NUM_PIECE_TYPES     (7)
// Opciones de joystick
#define JOYSTICK_DEAD_ZONE  (30)
#define JOYSTICK_PIN        (2)
// Opciones de gravedad
#define DROP_MINIMUM        (25)  // Velocidad máxima a la que puede llegar la gravedad
#define DROP_ACCELERATION   (20)  // Qué tan rápido aumenta la gravedad
#define INITIAL_MOVE_DELAY  (100)
#define INITIAL_DROP_DELAY  (500)
#define INITIAL_DRAW_DELAY  (30)

//////////////////////////////////////////////////////////////////








// Entradas/Salidas para el módulo MAX7219
#define MAX7219_Data_IN      3
#define MAX7219_Chip_Select  4
#define MAX7219_Clock        5
/////////////////////////
// Botones de dirección
int button_left  = A3;  // Botón izquierdo
int button_right = A3;  // Botón derecho
int button_up    = A3;  // Botón arriba
int button_down  = 9;   // Botón abajo
/////////////////////////
// Otros botones y dispositivos
int button_pause = 7;    // Botón de pausa
int button_start = 12;   // Botón de inicio
int joystick_x   = A0;   // Eje X del joystick
int joystick_y   = A1;   // Eje Y del joystick
int entradaAnalogicaBotones = A3;  // Entrada analógica para botones (¿Es correcto que sea la misma que button_left?)
const int umbralIzquierdaSuperior = 720 < 780;  // Umbral para el botón izquierdo (superior)
const int umbralIzquierdaInferior = 650 < 720;   // Umbral para el botón izquierdo (inferior)



// Variables
byte adr = 0x08;                  // Dirección
byte num = 0x00;                  // Número
int i = 0;                         // Variable de iteración
int top_score = 0;                 // Puntuación máxima general
int score = 0;                     // Puntuación actual
int top_score_1 = 0;               // Puntuación máxima 1
int top_score_2 = 0;               // Puntuación máxima 2
int top_score_3 = 0;               // Puntuación máxima 3
int top_score_4 = 0;               // Puntuación máxima 4
int score_1 = 0;                   // Puntuación actual 1
int score_2 = 0;                   // Puntuación actual 2
int score_3 = 0;                   // Puntuación actual 3
int score_4 = 0;                   // Puntuación actual 4
int valorLectura = 0;              // Valor de lectura (¿se utiliza para algo específico?)
bool Pause = false;                // Estado de pausa
bool pause_onece = false;          // Variable de pausa (¿se utiliza para algo específico?)
bool pause_pressed = false;        // Variable para indicar si el botón de pausa está presionado
unsigned long previousMillis = 0;  // Milisegundos previos (para manejar el tiempo)
unsigned long currentMillis = 0;   // Milisegundos actuales (para manejar el tiempo)
///////////////////////////////////





// Función para desplazar datos en el módulo de 7 segmentos
void shift(byte send_to_address, byte send_this_data)
{
  digitalWrite(MAX7219_Chip_Select, LOW);  // Activa el chip select (CS) a bajo nivel para iniciar la transmisión de datos
  shiftOut(MAX7219_Data_IN, MAX7219_Clock, MSBFIRST, send_to_address);  // Desplaza la dirección del módulo
  shiftOut(MAX7219_Data_IN, MAX7219_Clock, MSBFIRST, send_this_data);  // Desplaza los datos a enviar
  digitalWrite(MAX7219_Chip_Select, HIGH);  // Desactiva el chip select (CS) para completar la transmisión de datos
}

//////////////////////////////////////////////////////////////////




// Pieza I
const char piece_I[] = {
  0,0,0,0,
  1,1,1,1,
  0,0,0,0,
  0,0,0,0,
  // Rotación 90 grados
  0,0,1,0,
  0,0,1,0,
  0,0,1,0,
  0,0,1,0,
  // Rotación 180 grados
  0,0,0,0,
  0,0,0,0,
  1,1,1,1,
  0,0,0,0,
  // Rotación 270 grados
  0,1,0,0,
  0,1,0,0,
  0,1,0,0,
  0,1,0,0,
};

// Pieza L
const char piece_L[] = {
  0,0,1,0,
  1,1,1,0,
  0,0,0,0,
  0,0,0,0,
  // Rotación 90 grados
  0,1,0,0,
  0,1,0,0,
  0,1,1,0,
  0,0,0,0,
  // Rotación 180 grados
  0,0,0,0,
  1,1,1,0,
  1,0,0,0,
  0,0,0,0,
  // Rotación 270 grados
  1,1,0,0,
  0,1,0,0,
  0,1,0,0,
  0,0,0,0,
};

// Pieza J
const char piece_J[] = {
  1,0,0,0,
  1,1,1,0,
  0,0,0,0,
  0,0,0,0,
  // Rotación 90 grados
  0,1,1,0,
  0,1,0,0,
  0,1,0,0,
  0,0,0,0,
  // Rotación 180 grados
  0,0,0,0,
  1,1,1,0,
  0,0,1,0,
  0,0,0,0,
  // Rotación 270 grados
  0,1,0,0,
  0,1,0,0,
  1,1,0,0,
  0,0,0,0,
};

// Pieza T
const char piece_T[] = {
  0,1,0,0,
  1,1,1,0,
  0,0,0,0,
  0,0,0,0,
  // Rotación 90 grados
  0,1,0,0,
  0,1,1,0,
  0,1,0,0,
  0,0,0,0,
  // Rotación 180 grados
  0,0,0,0,
  1,1,1,0,
  0,1,0,0,
  0,0,0,0,
  // Rotación 270 grados
  0,1,0,0,
  1,1,0,0,
  0,1,0,0,
  0,0,0,0,
};

// Pieza S
const char piece_S[] = {
  0,1,1,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  // Rotación 90 grados
  0,1,0,0,
  0,1,1,0,
  0,0,1,0,
  0,0,0,0,
  // Rotación 180 grados
  0,0,0,0,
  0,1,1,0,
  1,1,0,0,
  0,0,0,0,
  // Rotación 270 grados
  1,0,0,0,
  1,1,0,0,
  0,1,0,0,
  0,0,0,0,
};

// Pieza Z
const char piece_Z[] = {
  1,1,0,0,
  0,1,1,0,
  0,0,0,0,
  0,0,0,0,
  // Rotación 90 grados
  0,0,1,0,
  0,1,1,0,
  0,1,0,0,
  0,0,0,0,
  // Rotación 180 grados
  0,0,0,0,
  1,1,0,0,
  0,1,1,0,
  0,0,0,0,
  // Rotación 270 grados
  0,1,0,0,
  1,1,0,0,
  1,0,0,0,
  0,0,0,0,
};

// Pieza O
const char piece_O[] = {
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  // Rotación 90 grados
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  // Rotación 180 grados
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  // Rotación 270 grados
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
};

// Un array de punteros a las matrices de las piezas
const char *pieces[NUM_PIECE_TYPES] = {
  piece_S,
  piece_Z,
  piece_L,
  piece_J,
  piece_O,
  piece_T,
  piece_I,
};

// Un array de colores asociados a cada tipo de pieza
const long piece_colors[NUM_PIECE_TYPES] = {
  0x009900, // Verde para la pieza S
  0xFF0000, // Rojo para la pieza Z
  0xFF8000, // Naranja para la pieza L
  0x000044, // Azul para la pieza J
  0xFFFF00, // Amarillo para la pieza O
  0xFF00FF, // Púrpura para la pieza T
  0x00FFFF, // Cian para la pieza I
};



// Instancia de la clase Adafruit_NeoPixel para controlar la tira de LEDs
Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRAND_LENGTH, LED_DATA_PIN, NEO_RGB + NEO_KHZ800);

// Variables para recordar el estado de los botones y la posición de las piezas
int old_button = 0;  // Estado anterior del botón
int old_px = 0;      // Posición anterior en el eje X
int old_i_want_to_turn = 0;  // Estado anterior de la intención de girar

// Variables para recordar la posición y estado de la pieza que cae
int piece_id;
int piece_rotation;
int piece_x;
int piece_y;

// Arreglo que representa la secuencia de piezas disponibles
char piece_sequence[NUM_PIECE_TYPES];
char sequence_i = NUM_PIECE_TYPES;

// Variables para controlar la velocidad del movimiento y la caída de las piezas
long last_move;
long move_delay;  // 100ms = 5 veces por segundo

long last_drop;
long drop_delay;  // 500ms = 2 veces por segundo

long last_draw;
long draw_delay;  // 60 fps

// Arreglo para recordar la posición de las piezas en la cuadrícula
long grid[GRID_W * GRID_H];

// Encender el punto P(x, y), que está X desde la izquierda y Y desde arriba.
// También se puede especificar el tiempo en microsegundos que se desea mantener encendido.
void p(int x, int y, long color) {
  int a = (GRID_H - 1 - y) * GRID_W;
  if ((y % 2) == BACKWARDS) {  // % es el operador módulo.
    // y%2 es falso cuando y es un número par - filas 0,2,4,6.
    a += x;
  } else {
    // y%2 es verdadero cuando y es un número impar - filas 1,3,5,7.
    a += GRID_W - 1 - x;
  }
  a %= STRAND_LENGTH;
  strip.setPixelColor(a, color);
}

// La cuadrícula contiene la memoria de la placa Arduino del tablero de juego, incluida la pieza que está cayendo.
void draw_grid() {
  int x, y;
  for (y = 0; y < GRID_H; ++y) {
    for (x = 0; x < GRID_W; ++x) {
      if (grid[y * GRID_W + x] != 0) {
        p(x, y, grid[y * GRID_W + x]);
      } else {
        p(x, y, 0);
      }
    }
  }
  strip.show();
}

// Elige una nueva pieza de la secuencia.
// La secuencia es una lista aleatoria que contiene una de cada pieza.
// De esta manera, se garantiza un número par de piezas con el tiempo,
// aunque el orden es aleatorio.
void choose_new_piece() {
  if (sequence_i >= NUM_PIECE_TYPES) {
    // lista agotada
    int i, j, k;
    for (i = 0; i < NUM_PIECE_TYPES; ++i) {
      do {
        // elige una pieza al azar
        j = rand() % NUM_PIECE_TYPES;
        // asegúrate de que no esté ya en la secuencia.
        for (k = 0; k < i; ++k) {
          if (piece_sequence[k] == j) break;  // ya está en la secuencia
        }
      } while (k < i);
      // no está en la secuencia. Agregarlo.
      piece_sequence[i] = j;
    }
    // rebobina el contador de secuencia
    sequence_i = 0;
  }
  
  // obtener la siguiente pieza en la secuencia.
  piece_id = piece_sequence[sequence_i++];
  // siempre comienza la pieza en la parte superior del centro.
  piece_y = -4;  // -4 cuadrados fuera de la parte superior de la pantalla.
  piece_x = 3;
  // siempre comienza en la misma orientación.
  piece_rotation = 0;
}


// Elimina la pieza del tablero.
void erase_piece_from_grid() {
  int x, y;
  
  const char *piece = pieces[piece_id] + (piece_rotation * PIECE_H * PIECE_W);
  
  for (y = 0; y < PIECE_H; ++y) {
    for (x = 0; x < PIECE_W; ++x) {
      int nx = piece_x + x;
      int ny = piece_y + y;
      if (ny < 0 || ny >= GRID_H) continue;
      if (nx < 0 || nx >= GRID_W) continue;
      if (piece[y * PIECE_W + x] == 1) {
        grid[ny * GRID_W + nx] = 0;  // cero borra la ubicación en el tablero.
      }
    }
  }
}

// Agrega la pieza al tablero.
void add_piece_to_grid() {
  int x, y;
  
  const char *piece = pieces[piece_id] + (piece_rotation * PIECE_H * PIECE_W);
  
  for (y = 0; y < PIECE_H; ++y) {
    for (x = 0; x < PIECE_W; ++x) {
      int nx = piece_x + x;
      int ny = piece_y + y;
      if (ny < 0 || ny >= GRID_H) continue;
      if (nx < 0 || nx >= GRID_W) continue;
      if (piece[y * PIECE_W + x] == 1) {
        grid[ny * GRID_W + nx] = piece_colors[piece_id];  // cero borra la ubicación en el tablero.
      }
    }
  }
}

// Elimina una fila dada del tablero y realiza otras operaciones relacionadas con la puntuación.
void delete_row(int y) {
  delay(35);
  
  // Actualiza la puntuación
  score = score + 10;
  if (score > top_score) {
    EEPROM_writeAnything(1, score);
  }
  
  EEPROM_readAnything(1, top_score);
  top_score_1 = top_score - ((top_score / 10) * 10);
  top_score_2 = ((top_score - ((top_score / 100) * 100)) - top_score_1) / 10;
  top_score_3 = ((top_score - ((top_score / 1000) * 1000)) - top_score_1 - top_score_2) / 100;
  top_score_4 = (top_score - top_score_1 - top_score_2 - top_score_3) / 1000;
  
  score_1 = score - ((score / 10) * 10);
  score_2 = ((score - ((score / 100) * 100)) - score_1) / 10;
  score_3 = ((score - ((score / 1000) * 1000)) - score_1 - score_2) / 100;
  score_4 = (score - score_1 - score_2 - score_3) / 1000;
  
  // Actualiza los dígitos en el display de 7 segmentos
  shift(0x08, score_4); //dígito 7 (el más a la izquierda) data
  shift(0x07, score_3);
  shift(0x06, score_2);
  shift(0x05, score_1);
  shift(0x04, top_score_4);
  shift(0x03, top_score_3);
  shift(0x02, top_score_2);
  shift(0x01, top_score_1); //dígito 0 (el más a la derecha) data
  
  int x;
  for (; y > 0; --y) {
    for (x = 0; x < GRID_W; ++x) {
      grid[y * GRID_W + x] = grid[(y - 1) * GRID_W + x];
    }
  }
  // Todo se mueve hacia abajo 1, por lo que la fila superior debe estar vacía o el juego se acabaría.
  for (x = 0; x < GRID_W; ++x) {
    grid[x] = 0;
  }
}


// Acelera la caída de la pieza.
void fall_faster() { 
  if (drop_delay > DROP_MINIMUM) drop_delay -= DROP_ACCELERATION;
}

// Elimina las filas completas del tablero y ajusta la velocidad de caída.
void remove_full_rows() {
  int x, y, c;
  char row_removed = 0;
  
  for (y = 0; y < GRID_H; ++y) {
    // contar los espacios llenos en esta fila
    c = 0;
    for (x = 0; x < GRID_W; ++x) {
      if (grid[y * GRID_W + x] > 0) c++;
    }
    if (c == GRID_W) {
      // ¡fila completa!
      delete_row(y);
      fall_faster();
    }
  }  
}

// Intenta mover la pieza lateralmente según la entrada del joystick.
void try_to_move_piece_sideways() {

  Serial.println(valorLectura);
  
  // ¿Qué indica el ángulo del joystick?
  int dx = map(analogRead(joystick_x), 715 < 735, 760 < 775, 780 < 790, 800 < 810);
  //int dx = map(analogRead(joystick_x), 700, 720, 770, 800);
  int valorLectura = analogRead(entradaAnalogicaBotones);
  
  int new_px = 0;
  // ¿El joystick se está moviendo realmente?
  if (dx > JOYSTICK_DEAD_ZONE) {
    new_px = -1;
  }
  if (dx < -JOYSTICK_DEAD_ZONE) {
    new_px = 1;
  }
    
  // Controla el movimiento lateral basado en el valor de los botones
  if (valorLectura > 720 && valorLectura < 760) {
     new_px = -1;
  } else if (valorLectura > 770 && valorLectura < 810) {
    new_px = 1;
  }
  
  // Si la posición nueva es diferente a la antigua y la pieza cabe, actualiza la posición.
  if (new_px != old_px && piece_can_fit(piece_x + new_px, piece_y, piece_rotation) == 1) {
    piece_x += new_px;
  }

  old_px = new_px;
}

  
// Intenta rotar la pieza según la entrada del joystick.
void try_to_rotate_piece() {
  //int dy = map(analogRead(joystick_y), 830, 840, 860, 870);
  int dy = map(analogRead(joystick_y), 825 < 830, 845 < 850, 860 < 870, 880 < 890);
  int i_want_to_turn = 0;
  
  int valorLectura = analogRead(entradaAnalogicaBotones);
  if (valorLectura > 840 && valorLectura < 870) {//BOTON ARRIBA
    i_want_to_turn = 1;
  } else {
    // Puedes agregar más lógica aquí si es necesario.
  }
  
  // ¿Qué indica el botón del joystick?
  int new_button = digitalRead(button_up);
  // Si el estado del botón ha cambiado y se está soltando,
  old_button = new_button;
  
  // Sube en el joystick para rotar
  if (dy > JOYSTICK_DEAD_ZONE) i_want_to_turn = 1;
  
  if (i_want_to_turn == 1 && i_want_to_turn != old_i_want_to_turn) {
    // Determina cómo se verá en ese nuevo ángulo.
    int new_pr = (piece_rotation + 1) % 4;
    // Si puede encajar en ese nuevo ángulo (no choca con nada)
    if (piece_can_fit(piece_x, piece_y, new_pr)) {
      // Realiza la rotación.
      piece_rotation = new_pr;
    } else {
      // Wall kick (ajuste en la pared)
      if (piece_can_fit(piece_x - 1, piece_y, new_pr)) {
        piece_x = piece_x - 1;
        piece_rotation = new_pr;
      } else if (piece_can_fit(piece_x + 1, piece_y, new_pr)) {
        piece_x = piece_x + 1;
        piece_rotation = new_pr;
      } 
    }
  }
  old_i_want_to_turn = i_want_to_turn;
}

// ¿Puede la pieza encajar en esta nueva ubicación?
int piece_can_fit(int px, int py, int pr) {
  if (piece_off_edge(px, py, pr)) return 0;
  if (piece_hits_rubble(px, py, pr)) return 0;
  return 1;
}

// ¿La pieza está fuera del límite del tablero?
int piece_off_edge(int px, int py, int pr) {
  int x, y;
  const char *piece = pieces[piece_id] + (pr * PIECE_H * PIECE_W);
  
  for (y = 0; y < PIECE_H; ++y) {
    for (x = 0; x < PIECE_W; ++x) {
      int nx = px + x;
      int ny = py + y;
      if (ny < 0) continue;  // fuera de la parte superior, no importa
      if (piece[y * PIECE_W + x] > 0) {
        if (nx < 0) return 1;  // sí: fuera del lado izquierdo
        if (nx >= GRID_W) return 1;  // sí: fuera del lado derecho
      }
    }
  }
  
  return 0;  // dentro de los límites
}

// Función que verifica si una pieza colisiona con escombros en el tablero
int piece_hits_rubble(int px, int py, int pr) {
  int x, y;
  const char *piece = pieces[piece_id] + (pr * PIECE_H * PIECE_W);

  // Iterar a través de la matriz de la pieza actual
  for (y = 0; y < PIECE_H; ++y) {
    int ny = py + y;
    if (ny < 0) continue;  // Ignorar si está fuera de la parte superior del tablero
    for (x = 0; x < PIECE_W; ++x) {
      int nx = px + x;
      if (piece[y * PIECE_W + x] > 0) {
        if (ny >= GRID_H) return 1;  // Sí: se sale por la parte inferior del tablero
        if (grid[ny * GRID_W + nx] != 0) return 1;  // Sí: el tablero ya está lleno en este espacio
      }
    }
  }

  return 0;  // No hay colisión con escombros
}




// Apaga todos los píxeles en la tira de LED y configura el color a negro.
void draw_restart() {
  for (int i = 0; i < STRAND_LENGTH; i++) {
    // pixels.Color toma valores RGB, desde 0,0,0 hasta 255,255,255
    strip.setPixelColor(i, strip.Color(0, 0, 0)); // Color verde moderadamente brillante.
  }
  // Configura el color de un píxel en la posición 55
  strip.setPixelColor(55, strip.Color(150, 150, 150));
  delay(10); // Espera 10 milisegundos

  // Configura el color de varios píxeles en posiciones específicas
  strip.setPixelColor(74, strip.Color(150, 150, 150));
  strip.setPixelColor(77, strip.Color(150, 150, 150));
  strip.setPixelColor(83, strip.Color(150, 150, 150));
  strip.setPixelColor(85, strip.Color(150, 150, 150));
  strip.setPixelColor(90, strip.Color(150, 150, 150));
  strip.setPixelColor(91, strip.Color(150, 150, 150));
  strip.setPixelColor(92, strip.Color(150, 150, 150));
  strip.setPixelColor(93, strip.Color(150, 150, 150));
  strip.setPixelColor(98, strip.Color(150, 150, 150));
  strip.setPixelColor(101, strip.Color(150, 150, 150));
  strip.setPixelColor(106, strip.Color(150, 150, 150));
  strip.setPixelColor(107, strip.Color(150, 150, 150));
  strip.setPixelColor(108, strip.Color(150, 150, 150));
  strip.setPixelColor(109, strip.Color(150, 150, 150));

  strip.show(); // Esto envía el color actualizado de los píxeles al hardware.
}


  void all_white()
{
  for(int i=0; i < STRAND_LENGTH; i++) {
    // La función pixels.Color toma valores RGB, desde 0,0,0 hasta 255,255,255
    strip.setPixelColor(i, strip.Color(120,120,120)); // Color verde moderadamente brillante.
    strip.show(); // Esto envía el color actualizado del píxel al hardware.
    delay(3); // Retardo por un período de tiempo (en milisegundos).
  }
}

void game_over() {
  score = 0;
  game_over_loop_leds();
  delay(1000);
  int x, y;

  long over_time = millis();
  draw_restart();
  currentMillis = millis();
  previousMillis = currentMillis;
  int led_number = 1;
  while(millis() - over_time < 8000) {
    currentMillis = millis();
    if(currentMillis - previousMillis >= 1000){
      previousMillis += 1000;
     
      delay(10);
    
      strip.setPixelColor(55 - led_number, strip.Color(150,150,150));  
      strip.show();
      led_number += 1; 
    }    
   
    // ¿Se presionó el botón?
    if(!digitalRead(button_start)) {
      // ¡Reiniciar!
      all_white();
      delay(400);
      break;
    }
  }
 
  all_white();
  setup();
  return;
}

void game_over_loop_leds()
{
  for(int i = 0; i < STRAND_LENGTH; i++) {
    // La función pixels.Color toma valores RGB, desde 0,0,0 hasta 255,255,255
    strip.setPixelColor(i, strip.Color(0, 150, 0)); // Color verde moderadamente brillante.
    strip.show(); // Esto envía el color actualizado del píxel al hardware.
    delay(10); // Retardo por un período de tiempo (en milisegundos).
  }
}

void try_to_drop_piece() {
  erase_piece_from_grid();  
  if(piece_can_fit(piece_x, piece_y + 1, piece_rotation)) {
    piece_y++;  // Mover la pieza hacia abajo
    add_piece_to_grid();
  } else {
    // ¡Chocó con algo!
    // Devolverla
    add_piece_to_grid();
    remove_full_rows();
    if(game_is_over() == 1) {
      game_over();
    }
    // El juego no ha terminado, elige una nueva pieza
    choose_new_piece();
  }
}

void try_to_drop_faster() {
  int dy = map(analogRead(joystick_y), 900 < 910, 920 < 930, 940 < 950, 960 < 970);

  int valorLectura = analogRead(entradaAnalogicaBotones);
  if(valorLectura > 900 && valorLectura < 940) {// BOTON ABAJO
    try_to_drop_piece();
  } else {
    
  }
}
//--------------------------------------------------------------------------------
// MÉTODO PARA REACCIONAR A LAS ACCIONES DEL JUGADOR
//--------------------------------------------------------------------------------

void react_to_player() {
  erase_piece_from_grid();  // Borrar la pieza actual del tablero
  try_to_move_piece_sideways();  // Intentar mover la pieza lateralmente
  try_to_rotate_piece();  // Intentar rotar la pieza
  add_piece_to_grid();  // Agregar la pieza actual al tablero
  
  try_to_drop_faster();  // Intentar hacer que la pieza caiga más rápido
}

//--------------------------------------------------------------------------------
// MÉTODO PARA VERIFICAR SI EL JUEGO HA TERMINADO
//--------------------------------------------------------------------------------

// ¿Ha terminado el juego?
int game_is_over() {
  int x, y;
  const char *piece = pieces[piece_id] + (piece_rotation * PIECE_H * PIECE_W);
  
  for(y = 0; y < PIECE_H; ++y) {
    for(x = 0; x < PIECE_W; ++x) {      
      int ny = piece_y + y;
      int nx = piece_x + x;
      if(piece[y * PIECE_W + x] > 0) {
        if(ny < 0) return 1;  // Sí: ¡se ha salido por arriba!
      }
    }
  }
  
  return 0;  // No ha terminado aún...
}

//--------------------------------------------------------------------------------
// CONFIGURACIÓN INICIAL DE ARDUINO
//--------------------------------------------------------------------------------

void setup() {  
  Serial.begin(9600);
  
  pinMode(MAX7219_Data_IN, OUTPUT);
  pinMode(MAX7219_Chip_Select, OUTPUT);
  pinMode(MAX7219_Clock, OUTPUT);
  digitalWrite(MAX7219_Clock, HIGH);

  pinMode(button_pause, INPUT_PULLUP);
  pinMode(button_start, INPUT_PULLUP);
  pinMode(button_left, INPUT_PULLUP);
  pinMode(button_right, INPUT_PULLUP);
  pinMode(button_up, INPUT_PULLUP);
  pinMode(button_down, INPUT_PULLUP);
  
  delay(1);


  
   // Configuración inicial
  shift(0x0f, 0x00); // Registro de prueba de visualización - modo de prueba desactivado
  shift(0x0c, 0x01); // Registro de apagado - operación normal
  shift(0x0b, 0x07); // Registro de límite de exploración - mostrar dígitos del 0 al 7
  shift(0x0a, 0x0f); // Registro de intensidad - brillo máximo
  shift(0x09, 0xff); // Registro de modo de decodificación - decodificar todo a código B
  
  EEPROM_readAnything(1, top_score);
  
  top_score_1 = top_score - ((top_score / 10) * 10);
  top_score_2 = ((top_score - ((top_score / 100) * 100)) - top_score_1) / 10;
  top_score_3 = ((top_score - ((top_score / 1000) * 1000)) - top_score_1 - top_score_2) / 100;
  top_score_4 = (top_score - top_score_1 - top_score_2 - top_score_3) / 1000;
  
  shift(0x08, 0x0f); // Dígito 7 (dígito más a la izquierda) datos
  shift(0x07, 0x0f);
  shift(0x06, 0x0f);
  shift(0x05, 0x0f);
  shift(0x04, top_score_4);
  shift(0x03, top_score_3);
  shift(0x02, top_score_2);
  shift(0x01, top_score_1); // Dígito 0 (dígito más a la derecha) datos



  
  int i;
  // Configura los LEDs
  strip.begin();
  strip.show(); // Inicializa todos los píxeles como 'apagados'
  
  // Configura el botón del joystick
  pinMode(JOYSTICK_PIN, INPUT);
  digitalWrite(JOYSTICK_PIN, HIGH);
  
  // Asegúrate de que Arduino conozca que la cuadrícula está vacía.
  for (i = 0; i < GRID_W * GRID_H; ++i) {
      grid[i] = 0;
  }
  
  // Hace que el juego sea un poco más aleatorio: toma un número del espacio y úsalo para 'sembrar' una serie de números aleatorios.
  randomSeed(analogRead(joystick_y) + analogRead(2) + analogRead(3));
  
  // Prepárate para comenzar el juego.
  choose_new_piece();
  
  move_delay = INITIAL_MOVE_DELAY;
  drop_delay = INITIAL_DROP_DELAY;
  draw_delay = INITIAL_DRAW_DELAY;
  
  // Inicia el reloj del juego después de que todo esté listo.
  last_draw = last_drop = last_move = millis();

}


// Función para dibujar la pantalla de pausa
void draw_pause()
{  
   for(int i=0; i<STRAND_LENGTH; i++)
   {
    // pixels.Color toma valores RGB, de 0,0,0 hasta 255,255,255
    strip.setPixelColor(i, strip.Color(0, 0, 0)); // Color verde moderadamente brillante.  
   }
   
   strip.setPixelColor(53, strip.Color(150, 150, 150)); 
   strip.setPixelColor(58, strip.Color(150, 150, 150)); 
   strip.setPixelColor(66, strip.Color(150, 150, 150)); 
   strip.setPixelColor(67, strip.Color(150, 150, 150)); 
   strip.setPixelColor(68, strip.Color(150, 150, 150)); 
   strip.setPixelColor(69, strip.Color(150, 150, 150)); 
   strip.setPixelColor(74, strip.Color(150, 150, 150)); 
   strip.setPixelColor(77, strip.Color(150, 150, 150)); 
   strip.setPixelColor(82, strip.Color(150, 150, 150)); 
   strip.setPixelColor(83, strip.Color(150, 150, 150)); 
   strip.setPixelColor(84, strip.Color(150, 150, 150)); 
   strip.setPixelColor(85, strip.Color(150, 150, 150)); 
   strip.show(); // Esto envía el color actualizado de los píxeles al hardware.


    delay(1000);
   }


// Función principal que se ejecuta repetidamente después del setup
void loop() {

  int valorLectura = analogRead(entradaAnalogicaBotones);
  
  long t = millis();

  if(!Pause)
  {
    if(!digitalRead(button_pause) && !pause_pressed)
    {
      Pause = !Pause;
      pause_pressed = true;
    
    }
    if(digitalRead(button_pause) && pause_pressed)
    {      
      pause_pressed = false;
    }
    
    // El juego se ejecuta a una velocidad,
    if(t - last_move > move_delay ) {
      last_move = t;
      react_to_player();
    }
    
    // ... y deja caer el bloque que cae a una velocidad diferente.
    if(t - last_drop > drop_delay ) {
      last_drop = t;
      try_to_drop_piece();
    }
    
    // Cuando no está haciendo esas dos cosas, vuelve a dibujar la cuadrícula.
    if(t - last_draw > draw_delay ) {
      last_draw = t;
      draw_grid();
    }
  }// Fin de !pause

  else
  {
    if(!digitalRead(button_pause) && !pause_pressed)
    {
      Pause = !Pause;
      pause_pressed = true;
    
    }
    if(digitalRead(button_pause) && pause_pressed)
    {      
      pause_pressed = false;
    }
    draw_pause();
    delay(1);
  }
  Serial.println(valorLectura);
}
