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


#define WIDTH 1280
#define HEIGHT 800

#define CENTROX WIDTH/2
#define CENTROY HEIGHT/2

#define TAM_BAR_X 10
#define TAM_BAR_Y 8

#define BAR_THICKNESS 1

#define MAX_NUMS 100000

void cartesian(int*, int*, float, float);
void draw(float, float, float, int, int, int);
int nextPrime();

typedef struct square {
  float x;
  float y;
  unsigned long long int r;
  float theta;
} square;

int main ()
{

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(WIDTH, HEIGHT, "Primos");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load a texture from the resources directory
	// Texture wabbit = LoadTexture("wabbit_alpha.png");
  unsigned long long int r = 0;
  float theta = 0;

  square Numeros[MAX_NUMS];
  int quantPrimos = 0;
  float zoomout = 10;
  char zoomLocked = 0;

  int centrox = CENTROX;
  int centroy = CENTROY;

  // Encontra primeiros MAX_NUMS primos
	
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
    if (IsKeyDown(KEY_D)) centrox-=5;
    if (IsKeyDown(KEY_A)) centrox+=5;
    if (IsKeyDown(KEY_W)) centroy+=5; 
    if (IsKeyDown(KEY_S)) centroy-=5;
    if (IsKeyPressed(KEY_R)) {
      zoomLocked = 0;
      centrox = CENTROX;
      centroy = CENTROY;
    }

    float mouseWheelMovement = GetMouseWheelMove();
    if (mouseWheelMovement != 0) {
      zoomLocked = 1;
      zoomout -= mouseWheelMovement;
      if (zoomout <= 0) zoomout = 1;
    }

		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// draw some text using the default font
		// DrawText("Hello Raylib", 200,200,20,WHITE);
    // DrawText("Y: %d", y,);
    // DrawText("R: %d", y,);
    // DrawText("Theta: %d",)

    // drawSquare(r, theta);
    // Gera outro primo
    
    int primo = nextPrime();
    Numeros[quantPrimos].r = primo;
    Numeros[quantPrimos].theta = fmod(primo, 2*PI);

    if (!zoomLocked) zoomout = Numeros[quantPrimos].r/350;
    quantPrimos++;

    for (int i = 0; i < quantPrimos; i++){
      draw(Numeros[i].r, Numeros[i].theta, zoomout, quantPrimos, centrox, centroy);
    }

    DrawRectangle(CENTROX-(TAM_BAR_X/2), CENTROY, TAM_BAR_X, BAR_THICKNESS, WHITE);
    DrawRectangle(CENTROX, CENTROY-(TAM_BAR_Y/2), BAR_THICKNESS, TAM_BAR_Y, WHITE);


		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	// UnloadTexture(wabbit);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}

void draw(float r, float theta, float zoomout, int quantPrimos, int centrox, int centroy){
  int x = 0;
  int y = 0;

  char temp[50];
  sprintf(temp, "Primos calculados: %d", quantPrimos);
  DrawText(temp, 10, 10, 20, WHITE);
  
  sprintf(temp, "Primo atual: %d", r);
  DrawText(temp, 10, 30, 20, WHITE);

  sprintf(temp, "Use o scroll para ajustar o zoom", r);
  DrawText(temp, 10, 50, 20, WHITE);

  sprintf(temp, "WASD para ajustar a câmera", r);
  DrawText(temp, 10, 70, 20, WHITE);

  sprintf(temp, "Aperte R para resetar a câmera", r);
  DrawText(temp, 10, 90, 20, WHITE);

  if (zoomout <= 0) zoomout = 1;

  cartesian(&x, &y, r/zoomout, theta);

  int pointsize = 1/(zoomout/12);
  if (pointsize < 1) pointsize = 1;
  DrawCircle(x+centrox, y+centroy, pointsize, RED);
}

void cartesian(int* x, int* y, float r, float theta){
  *x = r * cos(theta);
  *y = -r * sin(theta);
  return;
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
