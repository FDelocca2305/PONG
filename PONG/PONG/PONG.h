#pragma once
#include <iostream>
#include <string>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>

using namespace std;

void write(string text, SDL_Rect post, int x, int y, TTF_Font* tempFont, bool presentRender);
void serve();
void update();
void input();
void timer();
void render();
void waitKey(int typeCase);
void imagePost(string image, SDL_Rect object);
void playSound(string sound, int loop, int volume);
void endScreenWinner(string result);
void introScreen();
void winnerDetermine();








