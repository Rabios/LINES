// LINES!!!,A game made with Raylib as challenge
// Written by Rabia Alhaffar on 7/June/2020
// Special thanks to Anata,darltrash/ohnodario,And minus at Raylib Discord channel for support,and Linux binaries
// Last update: v0.0.6 on 15/June/2020

//Libs imported
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Lines and their properties,Changes randomoly every wave (5 seconds)
#ifdef __ANDROID__ || TARGET_OS_EMBEDDED || TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
    #define LINES 75
#else
    #define LINES 200
#endif

int lines_from_x[LINES];
int lines_from_y[LINES];
int lines_to_x[LINES];
int lines_to_y[LINES];
float lines_size[LINES];
Color lines_colors[LINES];
bool lines_drawn[LINES];
bool lines_activated[LINES];

// Game assets
Texture2D raytex;
Texture2D gamelogo;
Font rayfont;
Font gamefont;
Sound explosion;
Sound gameover;
Sound click;

// Main variables
int scene = 1;
int seconds = 0;
int timer = 0;
int linestimer = 0;
int activationtimer = 0;
int fps = 60;
float gamespeed = 1.5f;
int highscore = 0;
float explosionsize = 0.0f;
bool alive = true;
int playerx;
int playery;
typedef enum { HIGHSCORE = 0 } StorageData;

// Buttons,As we will use Raygui
bool startgamebuttonpressed = false;
bool exitgamebuttonpressed = false;

const char * madeWithTxt = "MADE WITH";
const char * titleTxt = "LINES!!!";
const char * copyrightTxt = "CREATED BY RABIA ALHAFFAR";
const char * gameOverTxt = "GAME OVER";
const char * restartTxt = "[SPACE KEY]: Menu   [R]: Retry";
const char * restartTouchTxt = "[TOUCH LEFT SIDE]: Menu   [TOUCH RIGHT SIDE]: Retry"; 

void Splashscreen();
void Menu();
void Game();
void GameOver();
void UnloadResources(int r);
void RemakeLines();
void DrawLines();
void CheckCollisions();
void RestartGame();

Color fade = BLACK;
Rectangle Screen;

int main(void) {
    
    // Set flags and enable Antialiasing and VSync
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    
    // Initialize game window and audio device if microphone or speaker found
    InitWindow(0,0,"LINES!!!");
    InitAudioDevice();
    
    SetTargetFPS(fps);
    SetTextureFilter(GetFontDefault().texture, FILTER_POINT); // Fix for HighDPI display problems
    
    // Load game resources and unload it!!!
    raytex = LoadTexture("resources/raylib_logo.png");
    gamelogo = LoadTexture("resources/gamelogo.png");
    rayfont = LoadFont("resources/acme7wide.ttf");
    gamefont = LoadFont("resources/pixelated.ttf");
    click = LoadSound("resources/click.wav");
    explosion = LoadSound("resources/boom.mp3");
    gameover = LoadSound("resources/gameover.wav");
    
    // GUI defaults
    GuiSetFont(gamefont);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    
    // Setting screen rectangle
    Screen.width = GetScreenWidth();
    Screen.height = GetScreenHeight();

    // Game loop
    while (!WindowShouldClose()) {
        if (scene == 1) Splashscreen();
        if (scene == 2) Menu();
        if (scene == 3) Game();
        if (scene == 4) GameOver();
    }
    
    // Else if ESC pressed or closed by default
    UnloadResources(1);
    UnloadResources(2);
    CloseAudioDevice();
    CloseWindow();
    exit(0);
    return 0;
}

float time = 0;

void Splashscreen() {
    BeginDrawing();
        ClearBackground(BLACK);
        DrawTextEx(gamefont,madeWithTxt,(Vector2) { (GetScreenWidth() - MeasureText(madeWithTxt, 48)) / 2, GetScreenHeight() / 3 - 45 },48,2.0f,WHITE);
        DrawTexture(raytex, (GetScreenWidth() - raytex.width) / 2, GetScreenHeight() / 3 + 45, WHITE);

        time += 4;
        fade.a = time;
        if (fade.a > fps * 4) {
            scene = 2;
        }

        DrawRectangleRec(Screen, fade);
        DrawFPS(10, 10);
    EndDrawing();
}

int decrease = 1; //1 = Decrease, 2 = Increase, 3 = Keep there

void Menu() {
    BeginDrawing();
        if (decrease == 1) {
            fade.a -= 4;
            if (fade.a < 1)
                decrease = 3;
        } else if (decrease == 2) {
            fade.a += 4;
            if (fade.a > fps * 4)
                RestartGame();
                RemakeLines();
        } else {
            fade.a = 0;
        }
        ClearBackground(BLACK);       
        #ifdef __ANDROID__ || TARGET_OS_EMBEDDED || TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
            DrawTextureEx(gamelogo,(Vector2) { (GetScreenWidth() - (gamelogo.width / 2.5)) / 2, GetScreenHeight() / 7 },0.0f,0.4f,WHITE);
        #else
            DrawTexture(gamelogo,(GetScreenWidth() - (gamelogo.width)) / 2, GetScreenHeight() / 7,WHITE);
        #endif
        DrawTextEx(gamefont,copyrightTxt,(Vector2) { 10, GetScreenHeight() - 32 },22,2.0f,BLUE);
        startgamebuttonpressed = GuiButton((Rectangle){ (GetScreenWidth() - 250) / 2, GetScreenHeight() / 3 + 125,250,100 }, "PLAY");
        exitgamebuttonpressed = GuiButton((Rectangle){ (GetScreenWidth() - 250) / 2,GetScreenHeight() / 3 + 275,250,100 }, "EXIT");
        if (startgamebuttonpressed) {
            PlaySound(click);
            decrease = 2;
        }
        if (exitgamebuttonpressed) {
            UnloadResources(2);
            CloseAudioDevice();
            CloseWindow();
            exit(0);
        }

        DrawRectangleRec(Screen, fade);
        DrawFPS(10,10);
    EndDrawing();
}

Color explosionColor = WHITE;

void Game() {
    BeginDrawing();
        ClearBackground(BLACK);
        DrawText(FormatText("%is",seconds),GetScreenWidth() / 2.1,10,64,RED);
        if (alive) {
            // Check OS,If mobile only use touch
            // Else,Use Gamepad,Keyboard,Mouse (Cause OS is desktop)
            #ifdef __ANDROID__ || TARGET_OS_EMBEDDED || TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
                playerx = GetTouchX();
                playery = GetTouchY();
            #else
                //Kept the drag for some of reasons
                if (IsMouseButtonDown(0)) {
                    playerx = GetMouseX();
                    playery = GetMouseY();                    
                }
            #endif
            if (playery < 10) playery = 10;
            if (playerx < 5) playerx = 5; 
            DrawCircle(playerx,playery,5.0f,PURPLE);
            timer++;
            linestimer++;
        }
        explosionColor.a = 255.0f - ((explosionsize / 80.0f) * 255.0f);
        DrawCircle(playerx,playery,explosionsize,explosionColor);      
        if (linestimer >= fps * 2) {
            DrawLines();
            if (linestimer >= fps * 4) {
                for (int i = 0;i < LINES;i++) {
                    if(activationtimer > i / 4) lines_activated[i] = true;
                    if(lines_activated[i]) {
                        lines_size[i] = 3.0f;
                        lines_colors[i] = RED;
                        CheckCollisions();
                    }
                }
                activationtimer++;
            }
            if (linestimer >= fps * 5) {
                CheckCollisions();
                RemakeLines();
                activationtimer = 0;
                linestimer = 0;
            }
            if (!alive) {      
                explosionsize += 1.0f;
                if (linestimer >= fps * 4)
                {
                    for (int i = 0; i < LINES; i++)
                    {
                        lines_size[i] = (explosionColor.a / 255.0f) * 3.0f;
                        lines_colors[i].a = explosionColor.a;                       
                    }                    
                }
            }
        }
        DrawFPS(10,10);
        if (timer >= fps) {
            timer = 0;
            seconds++;
        }        
        if (explosionsize > 80.0f) {
            PlaySound(explosion);
            scene = 4;
        }
    EndDrawing();
}

void GameOver() {
    BeginDrawing();
        ClearBackground(BLACK);
        highscore = LoadStorageValue(HIGHSCORE);
        DrawText(gameOverTxt, (GetScreenWidth() - MeasureText(gameOverTxt, 128)) / 2, GetScreenHeight() / 4,128,RED);
        if ((seconds > highscore) || (highscore == 0)) {
            highscore = seconds;
            SaveStorageValue(HIGHSCORE,highscore);
        }
        DrawText(FormatText("BEST TIME SURVIVED: %i SECONDS",LoadStorageValue(HIGHSCORE),seconds),(GetScreenWidth() - MeasureText(FormatText("BEST TIME SURVIVED: %i SECONDS",highscore), 64)) / 2,GetScreenHeight() / 2,64,RED);
        DrawText(FormatText("SURVIVED: %i SECONDS",seconds),(GetScreenWidth() - MeasureText(FormatText("SURVIVED: %i SECONDS",seconds), 32)) / 2,GetScreenHeight() / 1.6,32,RED);     
        #ifdef __ANDROID__ || TARGET_OS_EMBEDDED || TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
            DrawText(restartTouchTxt,(GetScreenWidth() - MeasureText(restartTouchTxt, 22)) / 2,GetScreenHeight() / 1.2,22,GREEN);
            if (GetTouchX() < GetScreenWidth() / 2) {
                decrease = 0;
                scene = 2;
            }
            if (GetTouchX() > GetScreenWidth() / 2) RestartGame();
        #else
            DrawText(restartTxt,(GetScreenWidth() - MeasureText(restartTxt, 32)) / 2,GetScreenHeight() / 1.2,32,GREEN);
            if (IsKeyPressed(KEY_SPACE)) {
                scene = 2;
                decrease = 0;
            }
            if (IsKeyPressed(KEY_R)) RestartGame();
        #endif     
        DrawFPS(10, 10);
    EndDrawing();
}

void UnloadResources(int r) {
    if (r == 1) {
        UnloadTexture(gamelogo);
        UnloadTexture(raytex);
    }
    if (r == 2) {
        UnloadFont(rayfont);
        UnloadFont(gamefont);
        UnloadSound(click);
        UnloadSound(explosion);
        UnloadSound(gameover);
    }
}

void RemakeLines() {
    for (int i = 0;i < LINES;i++) {
        lines_from_x[i] = GetRandomValue(-GetScreenWidth() / 4,GetScreenWidth() * 1.5);
        lines_from_y[i] = GetRandomValue(-GetScreenHeight() / 4,GetScreenHeight() * 1.5);
        lines_to_x[i] = GetRandomValue(-GetScreenWidth() / 4,GetScreenWidth() * 1.5);
        lines_to_y[i] = GetRandomValue(-GetScreenHeight() / 4,GetScreenHeight() * 1.5);
        lines_size[i] = 1.0f;
        lines_colors[i] = WHITE;
        lines_drawn[i] = false;
        lines_activated[i] = false;
    }
}

void DrawLines() {
    for (int i = 0;i < LINES;i++) DrawLineEx((Vector2) { lines_from_x[i] += gamespeed,lines_from_y[i] -= gamespeed },(Vector2) { lines_to_x[i] -= gamespeed,lines_to_y[i] += gamespeed },lines_size[i],lines_colors[i]);
}

void CheckCollisions() {
    for (int i = 0;i < LINES;i++) {
        float dist;
        float v1x = lines_to_x[i] - lines_from_x[i];
        float v1y = lines_to_y[i] - lines_from_y[i];
        float v2x = playerx - lines_from_x[i];
        float v2y = playery - lines_from_y[i];
        float u = (v2x * v1x + v2y * v1y) / (v1y * v1y + v1x * v1x);
        if (u >= 0 && u <= 1) dist = pow((lines_from_x[i] + v1x * u - playerx),2) + pow((lines_from_y[i] + v1y * u - playery),2);
        else {
            if (u < 0) dist = pow((lines_from_x[i] - playerx),2) + pow((lines_from_y[i] - playery),2);
            else dist = pow((lines_to_x[i] - playerx),2) + pow((lines_to_y[i] - playery),2);
        }
        if (dist < pow(5.0f,2)) alive = false;
    }
}

void RestartGame() {
    RemakeLines();
    seconds = 0;
    timer = 0;
    linestimer = 0;
    activationtimer = 0;
    scene = 3;
    explosionsize = 0.0f;
    alive = true;            
    gamespeed = 1.5f;
    playerx = GetScreenWidth() / 2;
    playery = GetScreenHeight() / 2;
}