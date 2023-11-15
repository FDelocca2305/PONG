#include <iostream>
#include <string>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include "PONG.h"

const int WIDHT = 1920;
const int HEIGHT = 1080;
const int SPEED = 13;
const int SIZE = 32;
double BALL_SPEED = 20;
const double PI = 3.14159265358979323846;
const int TIMER = 120;

using namespace std;

SDL_Renderer* renderer;
SDL_Window* window;
SDL_Texture* backgroundTexture = nullptr;

SDL_Rect paddle_player, paddle_bot, ball, scoreBoard;
SDL_Rect introRect;
SDL_Rect resultRect;
SDL_Rect timerText;

TTF_Font* font;
TTF_Font* fontBig;
TTF_Font* fontSmall;
TTF_Font* fontTitle;

SDL_Color color;

SDL_Event event;

bool playing;
bool repeat;
bool game;
int frameCount, timerFPS, lastFrame, fps;

double velX, velY;

string scoreLeft, scoreRight;

int leftScore, rightScore;

bool turn;

int startTime;

void setImage(string image, SDL_Rect object)
{
    SDL_Surface* surface;
    SDL_Texture* texture;

    const char* t = image.c_str();

    surface = IMG_Load(t);
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    object.w = surface->w;
    object.h = surface->h;

    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &object);
    SDL_DestroyTexture(texture);
}

void serve()
{
    paddle_player.x = 32;
    paddle_player.y = (HEIGHT / 2) - (paddle_player.h / 2);
    paddle_bot.y = paddle_player.y;
    paddle_bot.x = WIDHT - paddle_bot.w - 32;

    if (turn)
    {
        ball.x = paddle_player.x + (paddle_player.w * 4);
        velX = BALL_SPEED;
    }
    else
    {
        ball.x = paddle_bot.x - (paddle_bot.w * 4);
        velX = -BALL_SPEED;
    }

    ball.y = HEIGHT / 2;
    velY = 0;
    turn = !turn;

    BALL_SPEED = 20;
}

void handleMatchLimits() {
    if (paddle_player.y < 0)
    {
        paddle_player.y = 0;
    }
    if (paddle_player.y + paddle_player.h > HEIGHT)
    {
        paddle_player.y = HEIGHT - paddle_player.h;
    }
    if (paddle_bot.y < 0)
    {
        paddle_bot.y = 0;
    }
    if (paddle_bot.y + paddle_bot.h > HEIGHT)
    {
        paddle_bot.y = HEIGHT - paddle_bot.h;
    }
}

void handleMatchPoint() {
    if (ball.x < 0)
    {
        rightScore++;
        serve();

    }
    if (ball.x + SIZE > WIDHT)
    {
        leftScore++;
        serve();

    }
}

void handleBotPaddle() {
    if (ball.y > (paddle_bot.y + (paddle_bot.h / 2)))
    {
        paddle_bot.y += SPEED - 1;
    }
    if (ball.y < (paddle_bot.y + (paddle_bot.h / 2)))
    {
        paddle_bot.y -= SPEED - 1;
    }
}

void update()
{

    if (SDL_HasIntersection(&ball, &paddle_player))
    {
        double rel = (paddle_player.y + (paddle_player.h / 2)) - (ball.y + (SIZE / 2));
        double normal = rel / (paddle_player.h / 2);
        double bounce = normal * (5 * PI / 24);
        velX = BALL_SPEED * cos(bounce);
        velY = BALL_SPEED * -sin(bounce);

        if (BALL_SPEED <= 40)
        {
            BALL_SPEED *= 1.05;
        }

        playSound("audio/pegadadebola.mp3", 1, 40);
    }

    if (SDL_HasIntersection(&ball, &paddle_bot))
    {
        double rel = (paddle_bot.y + (paddle_bot.h / 2)) - (ball.y + (SIZE / 2));
        double norm = rel / (paddle_bot.h / 2);
        double bounce = norm * (5 * PI / 24);
        velX = -BALL_SPEED * cos(bounce);
        velY = BALL_SPEED * -sin(bounce);

        if (BALL_SPEED <= 40)
        {
            BALL_SPEED *= 1.05;
        }

        playSound("audio/pegadadebola.mp3", 1, 40);
    }

    handleBotPaddle();

    handleMatchPoint();

    if (ball.y<0 || ball.y + SIZE>HEIGHT)
    {

        velY = -velY;
    }

    ball.x += (int)velX;
    ball.y += (int)velY;

    scoreLeft = to_string(leftScore);
    scoreRight = to_string(rightScore);

    handleMatchLimits();
}

void input()
{
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            playing = false;
        }

    }

    if (keystates[SDL_SCANCODE_ESCAPE])
    {
        playing = false;
    }
    if (keystates[SDL_SCANCODE_UP])
    {
        paddle_player.y -= SPEED;
    }
    if (keystates[SDL_SCANCODE_DOWN])
    {
        paddle_player.y += SPEED;
    }
}

void timer()
{
    int currentTime = SDL_GetTicks();
    int elapsedTime = (currentTime - startTime) / 1000;
    int remainingTime = TIMER - elapsedTime;

    if (remainingTime < 0) {
        remainingTime = 0;
    }

    string timeText = to_string(remainingTime);

    write(timeText, timerText, 0, +HEIGHT / 20 - HEIGHT / 2, fontBig, false);

    if (remainingTime == 0)
    {
        playing = false;
    }

}

SDL_Texture* loadBackgroundTexture(const std::string& imagePath) {
    SDL_Surface* backgroundSurface = IMG_Load(imagePath.c_str());
    if (!backgroundSurface) {
        // Manejo de error si no se puede cargar la imagen de fondo
        return nullptr;
    }

    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);

    return backgroundTexture;
}

void render()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    frameCount++;

    int timerFPS = SDL_GetTicks() - lastFrame;

    if (timerFPS < (1000 / 60))
    {
        SDL_Delay((1000 / 60) - timerFPS);
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

    setImage("images/paleta.png", paddle_bot);
    setImage("images/paleta.png", paddle_player);
    setImage("images/slime.png", ball);

    write(scoreLeft, scoreBoard, -250, +HEIGHT / 10 - HEIGHT / 2, fontTitle, false);
    write(scoreRight, scoreBoard, +250, +HEIGHT / 10 - HEIGHT / 2, fontTitle, false);

    timer();

    SDL_RenderPresent(renderer);
}

void waitKey(int typeCase)
{
    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    switch (typeCase)
    {
    case 1:
        while (SDL_WaitEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                game = false;
            }
            if (event.type == SDL_KEYUP)
            {
                break;
            }
        }
        break;

    case 2:
        while (SDL_WaitEvent(&event))
        {
            if (keystates[SDL_SCANCODE_ESCAPE])
            {
                game = false;
                break;
            }
            if (keystates[SDL_SCANCODE_RETURN])
            {
                break;
            }
        }
        break;
    }
}



void write(string text, SDL_Rect post, int x, int y, TTF_Font* tempFont, bool presentRender)
{
    SDL_Surface* surface;
    SDL_Texture* texture;

    const char* t = text.c_str();

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

    surface = TTF_RenderText_Solid(tempFont, t, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    post.w = surface->w;
    post.h = surface->h;

    post.x = WIDHT / 2 - post.w / 2 + x;
    post.y = HEIGHT / 2 - post.h / 2 + y;

    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &post);
    SDL_DestroyTexture(texture);
    if (presentRender)
    {
        SDL_RenderPresent(renderer);
    }
    else if (!presentRender)
    {
        return;
    }
}

void playSound(string sound, int loopN, int volume)
{
    const char* s = sound.c_str();
    Mix_Music* music = Mix_LoadMUS(s);

    Mix_VolumeMusic(volume);

    Mix_PlayMusic(music, loopN);
}

void endScreenWinner(string result)
{
    write(result, resultRect, 0, 0, fontBig, true);

    string retry = "Presiona ENTER para reiniciar";
    write(retry, resultRect, 0, +HEIGHT / 6, fontSmall, true);
    string quit = "Presiona ESC para abandonar la partida";
    write(quit, resultRect, 0, +HEIGHT / 4, fontSmall, true);
}

void menu()
{
    playSound("audio/menu.mp3", -1, 40);
    string introTitle = "MINECRONG";
    write(introTitle, introRect, 0, -HEIGHT / 3, fontTitle, true);

    string introDevelopment = "By Frankito De lokita";
    write(introDevelopment, introRect, 210, (int)(- HEIGHT / 3.5), fontSmall, true);

    string introPress = "Presiona cualquier tecla para jugar";
    write(introPress, introRect, 0, -HEIGHT / 30, fontBig, true);
    
    waitKey(1);
}

void calculateWinner()
{
    if (leftScore > rightScore)
    {
        endScreenWinner("GANADOR PLAYER");
        playSound("audio/Ganaste.mp3", -1, 40);
        waitKey(2);
    }

    else if (leftScore < rightScore)
    {
        endScreenWinner("GANADOR CPU");
        playSound("audio/Perdiste.mp3", -1, 60);
        waitKey(2);
    }

    else if (leftScore == rightScore)
    {
        endScreenWinner("EMPATE");
        playSound("audio/Empate.mp3", -1, 40);
        waitKey(2);
    }
}

void setUpFonts() {
    font = TTF_OpenFont("fonts/MINECRAFT.ttf", 34);
    fontBig = TTF_OpenFont("fonts/MINECRAFT.ttf", 50);
    fontTitle = TTF_OpenFont("fonts/MINECRAFT.ttf", 130);
    fontSmall = TTF_OpenFont("fonts/MINECRAFT.ttf", 24);
}

bool init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        cout << "Failed at SDL_Init()" << endl;
        return false;
    }

    if (SDL_CreateWindowAndRenderer(WIDHT, HEIGHT, 0, &window, &renderer) < 0)
    {
        cout << "Failed at SDL_CreateWindowAndRenderer()" << endl;
        return false;
    }

    SDL_SetWindowTitle(window, "Minecrong");

    if (TTF_Init() == -1) {
        return false;
    }

    backgroundTexture = loadBackgroundTexture("images/background.jpg");
    if (!backgroundTexture) {
        return 1;
    }

    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(48000, AUDIO_S16SYS, 2, 2048);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    setUpFonts();

    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    return true;
}

void setUpVariablesForGameplay() {
    leftScore = 0;
    rightScore = 0;
    paddle_player.x = 32;
    paddle_player.h = HEIGHT / 4;
    paddle_player.y = (HEIGHT / 2) - (paddle_player.h / 2);
    paddle_player.w = 24;
    paddle_bot = paddle_player; paddle_bot.x = WIDHT - paddle_bot.w - 32;
    color.r = 0;
    color.g = 0;
    color.b = 0;
    ball.w = ball.h = SIZE;
}

int main(int argc, char* args[]) {
    
    if (!init())
    {
        return 0;
    }

    game = true;

    while (game)
    {
        
        setUpVariablesForGameplay();
        
        menu();
        serve();

        playing = true;

        int lastTime = 0;
        startTime = SDL_GetTicks();

        while (playing)
        {
            lastFrame = SDL_GetTicks();
            if (lastFrame >= (lastTime + 1000))
            {
                lastTime = lastFrame;
                fps = frameCount;
                frameCount = 0;

            }
            update();
            input();
            render();
        }
        calculateWinner();
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}