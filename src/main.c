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
  Vector2 pos;      // Coordenadas cartesianas calculadas
  unsigned int p;   // Primo
} primePoint;

// Array dinâmico para remover o hard limit
typedef struct {
  primePoint *items;
  int count;
  int capacity;
} primeList;

void addPrime(primeList*, int);
int nextPrime();

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

  // Inicializa lista de primos
  primeList primes = {0};
	
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
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

    float mouseWheelMovement = GetMouseWheelMove();
    if (mouseWheelMovement != 0) {
      zoomLocked = 1;
      camera.zoom += (mouseWheelMovement * 0.1f * camera.zoom);
      if (camera.zoom < 0.0001f) camera.zoom = 0.0001f;
    }

    // Drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

    // Começa modo 2d para desenhar círculos
    BeginMode2D(camera);

    // Gera outro primo
    int primo = nextPrime();
    addPrime(&primes, primo);

    // Zoom automático
    if (!zoomLocked) {
      camera.zoom = 350.0f / (float) primo;
    }

    // Se o zoom estiver próximo, 1.0f / camera.zoom será maior que 1px, desenha ele maior
    float dotScale = fmaxf(2.0f, 1.0f / camera.zoom);
    // Desenha círculos
    for (int i = 0; i < primes.count; i++){
      //DrawCircle(primes.items[i].x, primes.items[i].y, 1.0f / camera.zoom, RED);
      DrawTextureEx(dot, primes.items[i].pos, 0.0f, dotScale, RED);
    }

    // Encerra modo de câmera para escrever textos
    EndMode2D();
    
    // Escreve instruções na tela
    DrawText(TextFormat("Primos: %d", primes.count), 10, 10, 20, WHITE);
    DrawText(TextFormat("Primo atual: %d", primo), 10, 30, 20, WHITE);
    DrawText("Use o scroll para ajustar o zoom", 10, 50, 20, WHITE);
    DrawText("WASD para ajustar a câmera", 10, 70, 20, WHITE);
    DrawText("Aperte R para resetar a câmera", 10, 90, 20, WHITE);

    DrawText(TextFormat("Zoom: %.4f", camera.zoom), 10, 110, 20, WHITE);
    DrawFPS(WIDTH - 100, 10);

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


int nextPrime(){
  static int prime = 1;
  int test = prime;

  // Testa números até achar outro primo
  while (true) {
    test++;
    int isPrime = 1;
    int raiz = ceil(sqrt(test));

    for (int divisor = 2; divisor <= raiz; divisor++){
      if (test % divisor == 0 && test != divisor) {
        isPrime = 0;
        break;
      }
    }

    if (isPrime) {
      prime = test;
      return prime;
    }
  }
}

void addPrime(primeList *list, int p){
  if (list->count >= list->capacity){
    // Se a lista estiver cheia, dobra a capacidade
    list->capacity = (list->capacity == 0) ? 1024 : list->capacity * 2;
    list->items = realloc(list->items, list->capacity * sizeof(primePoint));
  }

  float r = p;
  float theta = p;

  list->items[list->count].pos.x = r * cos(theta);
  list->items[list->count].pos.y = -r * sin(theta);
  list->items[list->count].p = p;

  list->count++;
}
