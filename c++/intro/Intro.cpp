#include <SDL2/SDL.h>
#include <SDL2/SDL_TTF.h>

#include <stack>
#include <iostream>

using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const string WINDOW_CAPTION = "SDL Introduction";

const int FRAMES_PER_SECOND = 30;
const double FRAME_RATE = 1000/FRAMES_PER_SECOND;

struct StateStruct {
  void (*StatePointer)();
};

stack<StateStruct> g_StateStack;

SDL_Surface* g_Bitmap = nullptr;
SDL_Window* g_Window = nullptr;
SDL_Renderer* g_Renderer = nullptr;
SDL_Event g_Event;
int g_Timer;

void Menu();
void Game();
void Exit();

void Init();
void Shutdown();
void DrawBackground();
void ClearScreen();
void DisplayText(string text, int x, int y, int size, int fR, int fG, int fB,
                 int bR, int bG, int bB);
void HandleMenuInput();
void HandleGameInput();
void HandleExitInput();
bool FrameHasElapsed();
void FinishFrame();
void ClearStateStack();

int main(int arg, char**argv) {
  Init();

  while (!g_StateStack.empty()) {
    g_StateStack.top().StatePointer();
  }
  Shutdown();

  return 0;
}

void Init() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

  g_Window = SDL_CreateWindow(WINDOW_CAPTION.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
  g_Renderer = SDL_CreateRenderer(g_Window, -1, 0);
  g_Timer = SDL_GetTicks();
  g_Bitmap = SDL_LoadBMP("../../shared/background.bmp");

  StateStruct state;
  state.StatePointer = Exit;
  g_StateStack.push(state);

  state.StatePointer = Menu;
  g_StateStack.push(state);

  TTF_Init();
}

void Shutdown() {
  SDL_FreeSurface(g_Bitmap);
  SDL_Quit();
  TTF_Quit();
}

void Menu() {
  if (!FrameHasElapsed()) return;

  HandleMenuInput();
  ClearScreen();
  DisplayText("(S)tart Game", 350, 250, 20, 255, 255, 255, 0, 0, 0);
  DisplayText("(Q)uit Game", 350, 275, 20, 255, 255, 255, 0, 0, 0);
  FinishFrame();
}

void Game() {
  if (!FrameHasElapsed()) return;

  HandleGameInput();
  ClearScreen();
  DrawBackground();
  FinishFrame();
}

void Exit() {
  if (!FrameHasElapsed()) return;

  HandleExitInput();
  ClearScreen();
  DisplayText("Quit Game (Y or N)?", 350, 250, 12, 255, 255, 255, 0, 0, 0);
  FinishFrame();
}

void DrawBackground() {
  SDL_Rect source = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
  SDL_Rect destination = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

  SDL_Texture* texture = SDL_CreateTextureFromSurface(g_Renderer, g_Bitmap);
  SDL_RenderCopy(g_Renderer, texture, nullptr, nullptr);
}

void ClearScreen() {
  SDL_RenderClear(g_Renderer);
  SDL_UpdateWindowSurface(g_Window);
}

void DisplayText(string text, int x, int y, int size, int fR, int fG, int fB,
                 int bR, int bG, int bB) {
  TTF_Font* font = TTF_OpenFont("../../shared/arial.ttf", 24);

  SDL_Color foreground = {static_cast<Uint8>(fR), static_cast<Uint8>(fG), static_cast<Uint8>(fB)};
  SDL_Color background = {static_cast<Uint8>(bR), static_cast<Uint8>(bG), static_cast<Uint8>(bB)};

  SDL_Surface* temp = TTF_RenderText_Shaded(font, text.c_str(), foreground, background);

  int w, h;
  TTF_SizeText(font, text.c_str(), &w, &h);
  SDL_Rect destination = {x, y, w, h};

  SDL_Texture* texture = SDL_CreateTextureFromSurface(g_Renderer, temp);
  SDL_RenderCopy(g_Renderer, texture, nullptr, &destination);

  SDL_FreeSurface(temp);
  TTF_CloseFont(font);
}

void HandleMenuInput() {
  if (!SDL_PollEvent(&g_Event)) return;

  if (g_Event.type == SDL_QUIT) {
    ClearStateStack();
  }
  else if (g_Event.type == SDL_KEYDOWN) {
    switch (g_Event.key.keysym.sym) {
      case SDLK_ESCAPE:
      case SDLK_q:
        g_StateStack.pop();
        break;

      case SDLK_s:
        StateStruct temp;
        temp.StatePointer = Game;
        g_StateStack.push(temp);
        break;

      default:
        break;
    }
  }
}

void HandleGameInput() {
  if (!SDL_PollEvent(&g_Event)) return;

  if (g_Event.type == SDL_QUIT) {
    ClearStateStack();
  }
  else if (g_Event.type == SDL_KEYDOWN) {
    switch(g_Event.key.keysym.sym) {
      case SDLK_ESCAPE:
        g_StateStack.pop();
        break;

      default:
        break;
    }
  }
}

void HandleExitInput() {
  if (!SDL_PollEvent(&g_Event)) return;

  if (g_Event.type == SDL_QUIT) {
    ClearStateStack();
  }
  else if (g_Event.type == SDL_KEYDOWN) {
    switch(g_Event.key.keysym.sym) {
      case SDLK_y:
        g_StateStack.pop();
        break;

      case SDLK_n:
        StateStruct temp;
        temp.StatePointer = Menu;
        g_StateStack.push(temp);
        break;

      default:
        break;
    }
  }
}

bool FrameHasElapsed() {
  return ((SDL_GetTicks() - g_Timer) >= FRAME_RATE);
}

void FinishFrame() {
    SDL_UpdateWindowSurface(g_Window);
    SDL_RenderPresent(g_Renderer);
    g_Timer = SDL_GetTicks();
}

void ClearStateStack() {
  while (!g_StateStack.empty()) {
    g_StateStack.pop();
  }
}
