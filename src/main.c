#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//#include "raymath.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
int findStartIndexForCulling(primeList*, float, int);

int main (){
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);

  int screenWidth = 1280;
	int screenHeight = 800;
	int centrox = screenWidth/2;
	int centroy = screenHeight/2;

	// Create the window and OpenGL context
	InitWindow(screenWidth, screenHeight, "Primos");

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
  camera.offset = (Vector2){ centrox, centroy };
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  char zoomLocked = 0;
  typedef enum {
    COLOR_CALCULATED,
    COLOR_BREATHING,
    COLOR_CUSTOM_STATIC,
    COLOR_CUSTOM_GRADIENT,
    COLOR_GRADIENT_PURPLE_TO_RED,
    COLOR_GRADIENT_TEMPERATURE,
    MAX_COLOR_MODES} ColorMode;

  char *colorSchemeName[] = {
    "Distância",
    "Breathing",
    "Custom sólido",
    "Custom Degradê",
    "Roxo para Vermelho",
    "Temperatura"
  };

  ColorMode currentColorMode = COLOR_BREATHING;

  // Simula o efeito de "lentamente" renderizando primos, mesmo que eles já tenham sido calculados previamente
  float visibleCount = 0;
  char PPSlock = 0;
  float primesPerSecond = 0;
  char showStats = 1;
  char showControls = 1;
  char showFPS = 1;

  // 0 = Invisível. 1 = Solid color. 2 = Gradient color
  char colorPickerVisible = 0;

  Color customColor = RED;
  Color colorCustom1 = WHITE;
  Color colorCustom2 = BLACK;

    Color colorHOT = (Color){255, 60, 0, 125};
    Color colorCOLD = (Color){0, 150, 255, 255};


  // Inicializa lista de primos
  primeList primes = {0};
  addPrime(&primes, 2);
  unsigned int lastChecked = 0;
  sieveSegment(&primes, &lastChecked, 1000);
	
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
    if (IsWindowResized()) {
      screenWidth = GetScreenWidth();
      screenHeight = GetScreenHeight();
      camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };

      centrox = screenWidth/2;
      centroy = screenHeight/2; 
    }
    // Calcula PPS baseado no zoom da câmera
    if (!PPSlock) primesPerSecond = 500.f + (50.0f / sqrt(camera.zoom)) * 1.0f;
    float moveSpeed = 10.0f / camera.zoom;


    if (IsKeyDown(KEY_W)) camera.target.y -= moveSpeed; 
    if (IsKeyDown(KEY_S)) camera.target.y += moveSpeed;
    if (IsKeyDown(KEY_A)) camera.target.x -= moveSpeed;
    if (IsKeyDown(KEY_D)) camera.target.x += moveSpeed;
    if (IsKeyDown(KEY_DOWN)){
      PPSlock = 1;
      primesPerSecond-=10;
    }
    if (IsKeyDown(KEY_UP)){
      PPSlock = 1;
      primesPerSecond+=10;
    }
    // Fila de teclas para quando fps fica baixo
    int key = GetKeyPressed();
    while (key > 0) {
      if (key == KEY_R) {
        zoomLocked = 0;
        camera.zoom = 1.0f;
        camera.target = (Vector2){0, 0};
      }
      if (key == KEY_ENTER) {
        if (!PPSlock) {
          PPSlock = 1;
          primesPerSecond = 0;
        } else {
          PPSlock = 0;
        }
      }
      if (key == KEY_C) {
        currentColorMode = (currentColorMode + 1) % MAX_COLOR_MODES;
      }
      if (key == KEY_P) {
        TakeScreenshot(TextFormat("prime_spiral_%d.png", (int) visibleCount));
      }
      if (key == KEY_F1) showControls = showControls ? 0 : 1;
      if (key == KEY_F2) showFPS = showFPS ? 0 : 1;
      if (key == KEY_F3) showStats = showStats ? 0 : 1;
      if (key == KEY_TAB) {
        if (colorPickerVisible == 0) colorPickerVisible = 1;
        else if (colorPickerVisible == 1) colorPickerVisible = 2;
        else colorPickerVisible = 0;
      }
      key = GetKeyPressed();
    }

    float mouseWheelMovement = GetMouseWheelMove();
    if (mouseWheelMovement != 0) {
      zoomLocked = 1;
      camera.zoom += (mouseWheelMovement * 0.1f * camera.zoom);
      //if (camera.zoom < 0.0001f) camera.zoom = 0.0001f;
    }
    
    // Primos revelados
    visibleCount += primesPerSecond * GetFrameTime();

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

    // Começa modo 2d para desenhar círculos
    BeginMode2D(camera);

    // LOD quando movendo camera, reduzindo lag ao mover renderizando apenas um a cada dez primos
    char isMoving = (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D) || GetMouseWheelMove() != 0);
    int step = 1;
    if (isMoving && currentLimit > 100000) step = 5;


    // Culling de elementos fora da tela para melhorar performance enquanto visualiza
    // Calcula posições dos cantos da tela
    Vector2 tl = GetScreenToWorld2D((Vector2){0, 0}, camera);
    Vector2 tr = GetScreenToWorld2D((Vector2){screenWidth, 0},camera);
    Vector2 bl = GetScreenToWorld2D((Vector2){0, screenHeight},camera);
    Vector2 br = GetScreenToWorld2D((Vector2){screenWidth, screenHeight}, camera);

    float minX = fminf(fminf(tl.x, tr.x), fminf(bl.x, br.x));
    float maxX = fmaxf(fmaxf(tl.x, tr.x), fmaxf(bl.x, br.x));
    float minY = fminf(fminf(tl.y, tr.y), fminf(bl.y, br.y));
    float maxY = fmaxf(fmaxf(tl.y, tr.y), fmaxf(bl.y, br.y));

    // Calcula o ponto da câmera mais próximo do centro 
    float cx = (minX > 0) ? minX : ((maxX < 0) ? maxX : 0);
    float cy = (minY > 0) ? minY : ((maxY < 0) ? maxY : 0);
    float rMin = sqrtf(cx*cx + cy*cy);

    // Calcula o ponto da câmera mais longe do centro, comparando os quadrados e tirando a raíz do maior
    float rMax = sqrtf(fmaxf(
      fmaxf(tl.x*tl.x + tl.y*tl.y, tr.x*tr.x + tr.y*tr.y), 
      fmaxf(bl.x*bl.x + bl.y*bl.y, br.x*br.x + br.y*br.y)));

    // Encontra com busca binária o primeiro primo que pode aparecer na câmera
    int startIndex = findStartIndexForCulling(&primes, rMin, currentLimit);

    for (int i = startIndex; i < currentLimit; i+=step){
      // Para o desenho se os próximos círculos estão fora da tela
      if (primes.items[i].p > rMax) break;

      // Proporção para os degradês
      float distanceRatio = (float)primes.items[i].p/(float)primes.items[currentLimit].p;
      //Vector2 pos = primes.items[i].pos;
      Vector2 pos = {
        (float) primes.items[i].pos.x,
        (float) primes.items[i].pos.y
      };

       // Não desenha se o ponto está fora da área visível
       if (pos.x < tl.x || pos.x > br.x ||
           pos.y < tl.y || pos.y > br.y)
              continue;

      // Define como colorir os círculos
      Color drawColor;

      switch (currentColorMode) {
      case COLOR_CUSTOM_STATIC:
        drawColor = customColor;
        break;
      case COLOR_CUSTOM_GRADIENT:
        drawColor = ColorLerp(colorCustom1, colorCustom2, distanceRatio);
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

    if (colorPickerVisible == 1) {
      GuiColorPicker((Rectangle){ screenWidth - 250, 50, 200, 200 }, "Escolha uma cor", &customColor);
      currentColorMode = COLOR_CUSTOM_STATIC;
    } else if (colorPickerVisible == 2) {
      GuiColorPicker((Rectangle){ screenWidth - 250, 50, 200, 200 }, "Escolha uma para o centro", &colorCustom1);
      GuiColorPicker((Rectangle){ screenWidth - 250, 300, 200, 200 }, "Escolha uma para a borda", &colorCustom2);
      currentColorMode = COLOR_CUSTOM_GRADIENT;
    }
    
    // Escreve estatísticas  na tela
    if (showStats) {
      DrawText(TextFormat("Primos: %d", currentLimit), 10, 10, 20, WHITE);
      DrawText(TextFormat("Primo atual: %d", primes.items[currentLimit].p), 10, 30, 20, WHITE);
      DrawText(TextFormat("PPS: %.2f", primesPerSecond), 10, 50, 20, WHITE);
      DrawText(TextFormat("Primos calculados: %d", primes.count), 10, 70, 20, WHITE);
      DrawText(TextFormat("Esquema de cores atual: %s", colorSchemeName[currentColorMode]), 10, 90, 20, WHITE);
    }

    // Escreve os controles na tela
    if (showControls){
      DrawText("Use o scroll para ajustar o zoom", 10, screenHeight-30, 20, WHITE);
      DrawText("WASD para ajustar a câmera", 10, screenHeight-50, 20, WHITE);
      DrawText("R para resetar a câmera", 10, screenHeight-70, 20, WHITE);
      DrawText("Enter para pausar", 10, screenHeight-90, 20, WHITE);
      DrawText("C para mudar esquema de cores", 10, screenHeight-110, 20, WHITE);
      DrawText("TAB para escolher uma cor", 10, screenHeight-130, 20, WHITE);
      DrawText("Use as setas para cima e para baixo para alterar PPS", 10, screenHeight-150, 20, WHITE);
      DrawText("P para tirar print", 10, screenHeight-170, 20, WHITE);
      DrawText("F1, F2, F3 para esconder os menus", 10, screenHeight-190, 20, WHITE);
    }

    if (showFPS) DrawFPS(screenWidth - 100, 10);

    // Desenha cursor no centro da tela
    DrawRectangle(centrox-(TAM_BAR_X/2), centroy, TAM_BAR_X, BAR_THICKNESS, WHITE);
    DrawRectangle(centrox, centroy-(TAM_BAR_Y/2), BAR_THICKNESS, TAM_BAR_Y, WHITE);

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

// Usa busca binária para encontrar o primeiro primo que pode aparecer na câmera para Culling
int findStartIndexForCulling(primeList* list, float rMin, int currentLimit){
  if (currentLimit <= 0) return 0;
  int low = 0;
  int high = currentLimit - 1;
  int result = 0;

  while (low <= high){
    int mid = low + (high - low) / 2;
    // Se está dentro do alcance, pode ter um mais cedo
    if (list->items[mid].p >= rMin){
      result = mid;
      high = mid-1;
    } else {
      low = mid + 1;
    }
  }
  return result;
}
