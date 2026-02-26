/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#include <math.h>
#include <stdio.h>
#include <stdlib.h>


#define WIDTH 1280
#define HEIGHT 800

#define CENTROX WIDTH/2
#define CENTROY HEIGHT/2

#define TAM_BAR_X 10
#define TAM_BAR_Y 8

#define BAR_THICKNESS 1

typedef struct {
  double x;
  double y;
} doubleVector2;

typedef struct {
  doubleVector2 pos;      // Coordenadas cartesianas calculadas
  unsigned int p;   // Primo
  Color color;
} primePoint;

// Array dinâmico para remover o hard limit
typedef struct {
  primePoint *items;
  int count;
  int capacity;
} primeList;

void addPrime(primeList*, int);
void sieveSegment(primeList*, unsigned int*, int);

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(WIDTH, HEIGHT, "Primos");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

  // Cria textura para renderizar em montes pela gpu
  Image img = GenImageColor(2, 2, WHITE);
  Texture2D dot = LoadTextureFromImage(img);
  UnloadImage(img);

  unsigned long long int r = 0;
  unsigned long long int theta = 0;

  // Seta câmera da Raylib
  Camera2D camera = {0};
  camera.target = (Vector2){0, 0};
  camera.offset = (Vector2){ CENTROX, CENTROY };
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  int centrox = CENTROX;
  int centroy = CENTROY;
  char zoomLocked = 0;
  typedef enum {
    COLOR_CALCULATED,
    COLOR_BREATHING,
    COLOR_RED,
    COLOR_GRADIENT_PURPLE_TO_RED,
    COLOR_GRADIENT_TEMPERATURE,
    MAX_COLOR_MODES} ColorMode;
  ColorMode currentColorMode = COLOR_BREATHING;

  // Simula o efeito de "lentamente" renderizando primos, mesmo que eles já tenham sido calculados previamente
  float visibleCount = 0;
  char PPSlock = 0;
  float primesPerSecond = 0;
  char showStats = 1;
  char showControls = 1;
  char showFPS = 1;


  // Inicializa lista de primos
  primeList primes = {0};
  addPrime(&primes, 2);
  unsigned int lastChecked = 0;
  sieveSegment(&primes, &lastChecked, 1000);
	
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
    if (!PPSlock) primesPerSecond = 500.f + (2.0f / camera.zoom) * 1.0f;
    if (primesPerSecond > 10000) primesPerSecond = 10000;
    float moveSpeed = 10.0f / camera.zoom;
    if (IsKeyDown(KEY_W)) camera.target.y -= moveSpeed; 
    if (IsKeyDown(KEY_S)) camera.target.y += moveSpeed;
    if (IsKeyDown(KEY_A)) camera.target.x -= moveSpeed;
    if (IsKeyDown(KEY_D)) camera.target.x += moveSpeed;
    if (IsKeyPressed(KEY_R)) {
      zoomLocked = 0;
      camera.zoom = 1.0f;
      camera.target = (Vector2){0, 0};
    }
    if (IsKeyPressed(KEY_ENTER)) {
      if (!PPSlock) {
        PPSlock = 1;
        primesPerSecond = 0;
      } else {
        PPSlock = 0;
      }
    }
    if (IsKeyPressed(KEY_C)) {
      currentColorMode = (currentColorMode + 1) % MAX_COLOR_MODES;
    }
    if (IsKeyPressed(KEY_P)) {
      TakeScreenshot(TextFormat("prime_spiral_%d.png", (int) visibleCount));
    }
    if (IsKeyDown(KEY_DOWN)){
      PPSlock = 1;
      primesPerSecond--;
    }
    if (IsKeyDown(KEY_UP)){
      PPSlock = 1;
      primesPerSecond++;
    }
    if (IsKeyPressed(KEY_F1)) showControls = showControls ? 0 : 1;
    if (IsKeyPressed(KEY_F2)) showFPS = showFPS ? 0 : 1;
    if (IsKeyPressed(KEY_F3)) showStats = showStats ? 0 : 1;

    float mouseWheelMovement = GetMouseWheelMove();
    if (mouseWheelMovement != 0) {
      zoomLocked = 1;
      camera.zoom += (mouseWheelMovement * 0.1f * camera.zoom);
      //if (camera.zoom < 0.0001f) camera.zoom = 0.0001f;
    }

    
    // Primos revelados
    visibleCount += primesPerSecond * GetFrameTime();


    // Gera outro primo
    // int primo = nextPrime();
    // addPrime(&primes, primo);

    // Se o zoom estiver próximo, 1.0f / camera.zoom será menor que 2px, então usa uma escala constante de 2 para fazer o pixel maior de perto
    float dotScale = fmaxf(2.0f, 1.0f / camera.zoom);

    // Limita para desenhar apenas alguns circulos por segundo
    if (visibleCount < 0) visibleCount = 0;
    int currentLimit = (int) visibleCount;
    if (currentLimit > primes.count) currentLimit = primes.count;

    // Se os primos já processados estão acabando, processa mais
    if (primes.count - currentLimit <= 1000) sieveSegment(&primes, &lastChecked, 500000);

    // Zoom automático
    if (!zoomLocked) {
      camera.zoom = 100.0f / ((float) currentLimit*3);
    }

    // Drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

    // Usado para esquilibrar cor gradient
    Color colorHOT = (Color){255, 60, 0, 255};
    Color colorCOLD = (Color){0, 150, 255, 255};

    // Começa modo 2d para desenhar círculos
    BeginMode2D(camera);

    // Desenha círculos
    // Culling de elementos fora da tela para melhorar performance enquanto visualiza
    Vector2 topLeft = GetScreenToWorld2D((Vector2){0, 0}, camera);
    Vector2 bottomRight = GetScreenToWorld2D((Vector2){WIDTH, HEIGHT}, camera);
    for (int i = 0; i < currentLimit; i++){
      float distanceRatio = (float)primes.items[i].p/(float)primes.items[currentLimit].p;
      //Vector2 pos = primes.items[i].pos;
      Vector2 pos = {
        (float) primes.items[i].pos.x,
        (float) primes.items[i].pos.y
      };

       // Não desenha se o ponto está fora da área visível
       if (pos.x < topLeft.x || pos.x > bottomRight.x ||
           pos.y < topLeft.y || pos.y > bottomRight.y)
              continue;

      // Define como colorir os círculos
      Color drawColor;

      switch (currentColorMode) {
      case COLOR_RED:
        drawColor = RED;
        break;
      case COLOR_CALCULATED:
        drawColor = primes.items[i].color;
        break;
      case COLOR_BREATHING:
        drawColor = ColorFromHSV(fmodf(GetTime() * 50.0f, 360.0f), 0.7f, 1.0f);
        break;
      case COLOR_GRADIENT_PURPLE_TO_RED:
        drawColor = ColorFromHSV(270.f + 90*distanceRatio, 0.7f, 1.0f);
        break;
      case COLOR_GRADIENT_TEMPERATURE:
        drawColor = ColorLerp(colorCOLD, colorHOT, distanceRatio);
        break;
      }
      DrawTextureEx(dot, pos, 0.0f, dotScale, drawColor);
    }

    // Encerra modo de câmera para escrever textos
    EndMode2D();
    
    // Escreve estatísticas  na tela
    if (showStats) {
    DrawText(TextFormat("Primos: %d", currentLimit), 10, 10, 20, WHITE);
    DrawText(TextFormat("Primo atual: %d", primes.items[currentLimit].p), 10, 30, 20, WHITE);
    DrawText(TextFormat("PPS: %.2f", primesPerSecond), 10, 50, 20, WHITE);
    DrawText(TextFormat("Primos calculados: %d", primes.count), 10, 70, 20, WHITE);
    }

    // Escreve os controles na tela
    if (showControls){
    DrawText("Use o scroll para ajustar o zoom", 10, HEIGHT-30, 20, WHITE);
    DrawText("WASD para ajustar a câmera", 10, HEIGHT-50, 20, WHITE);
    DrawText("R para resetar a câmera", 10, HEIGHT-70, 20, WHITE);
    DrawText("Enter para pausar", 10, HEIGHT-90, 20, WHITE);
    DrawText("C para mudar esquema de cores", 10, HEIGHT-110, 20, WHITE);
    DrawText("Use as setas para cima e para baixo para alterar PPS", 10, HEIGHT-130, 20, WHITE);
    DrawText("P para tirar print", 10, HEIGHT-150, 20, WHITE);
    DrawText("F1, F2, F3 para esconder os menus", 10, HEIGHT-170, 20, WHITE);
    }

    if (showFPS) DrawFPS(WIDTH - 100, 10);

    // Desenha cursor no centro da tela
    DrawRectangle(CENTROX-(TAM_BAR_X/2), CENTROY, TAM_BAR_X, BAR_THICKNESS, WHITE);
    DrawRectangle(CENTROX, CENTROY-(TAM_BAR_Y/2), BAR_THICKNESS, TAM_BAR_Y, WHITE);

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}


void sieveSegment(primeList *list, unsigned int *lastChecked, int segmentSize){
  // Limite inferior
  unsigned int low = *lastChecked + 1;
  // Limite superior
  unsigned int high = low + segmentSize - 1;

  // Cria um array de números para checagem e marca os que não são primos
  // 0 = primo, 1 = não primo
  char *isNotPrime = calloc(segmentSize, sizeof(char));

  // Se na primeira iteração, não usa lista conhecida para encontrar primos
  if (low == 1) {
    // Marca 1 como não primo
    isNotPrime[0] = 1;

    for (unsigned int p = 2; p*p <= high; p++) {
      // Se o número é primo, elimina seus múltiplos
      if (!isNotPrime[p - low]) 
        for (unsigned int j = p*p; j <= high; j += p)
         isNotPrime[j - low] = 1;
      
    }
  }

  // Itera pelos primos já calculados para "peneirar" a próxima demanda
  for (int i = 0; i < list->count; i++){
    unsigned int p = list->items[i].p;
    if (p*p > high) break;

    // Encontra o primeiro múltiplo de p no range (low, high) truncando a divisão
    unsigned int start = (low / p) * p;
    if (start < low) start += p;

    // Números menores que p*p já devem ter sido eliminados pelas iterações passadas
    if (start < p*p) start = p*p;
    
    for (unsigned int j = start; j <= high; j+=p) {
      // Ajusta posição pro array atual
      isNotPrime[j - low] = 1;
    }
  }

  // Adiciona os que não foram marcados à lista de primos
  for (unsigned int i = 0; i < segmentSize; i++) {
    if (!isNotPrime[i]) addPrime(list, i + low);
  }

  *lastChecked = high;
  free(isNotPrime);
}

void addPrime(primeList *list, int p){
  if (list->count >= list->capacity){
    // Se a lista estiver cheia, dobra a capacidade
    list->capacity = (list->capacity == 0) ? 1024 : list->capacity * 2;
    list->items = realloc(list->items, list->capacity * sizeof(primePoint));
  }

  double r = (double)p;
  double theta = (double)p;

  list->items[list->count].pos.x = r * cos(theta);
  list->items[list->count].pos.y = -r * sin(theta);
  list->items[list->count].p = p;

  float hue = fmodf(list->items[list->count].p * 0.05f, 360.0f);
  list->items[list->count].color = ColorFromHSV(hue, 0.8f, 1.0f);

  list->count++;
}
