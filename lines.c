// LINES!!!,A game made with Raylib as challenge
// Written by Rabia Alhaffar on 7/June/2020
// Special thanks to Anata at Raylib Discord channel

//Libs imported
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Lines and their properties,Changes randomoly every wave (5 seconds)
#define LINES 200

int lines_from_x[LINES];
int lines_from_y[LINES];
int lines_to_x[LINES];
int lines_to_y[LINES];
float lines_size[LINES];
Color lines_colors[LINES];

// Game assets
Texture2D raytex;
Font rayfont;

// Main variables
int scene = 1;
int seconds = 0;
int timer = 0;
int fps = 60;
int linestimer = 0;
float gamespeed = 1.5f;
int highscore = 0;
float explosionsize = 0.0f;
bool alive = true;
int playerx;
int playery;

typedef enum {
    HIGHSCORE = 0
} StorageData;

// Buttons,As we will use Raygui
bool startgamebuttonpressed = false;
bool exitgamebuttonpressed = false;

const char * madeWithTxt = "MADE WITH";
const char * titleTxt = "LINES!!!";
const char * copyrightTxt = "CREATED BY RABIA ALHAFFAR!!!";
const char * gameOverTxt = "GAME OVER";
const char * restartTxt = "PRESS SPACE KEY TO GO MAIN MENU,OR R KEY TO RETRY";
const char * restartgamepadTxt = "ON GAMEPAD,PRESS B BUTTON TO GO MAIN MENU,OR A BUTTON TO RETRY";

void Splashscreen();
void Menu();
void Game();
void GameOver();
void UnloadResources();
void RemakeLines();
void DrawLines();
void CheckCollisions();
void ResetGame();

int main(void) {
    
    // Initializing game window with antialiasing enabled
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(0,0,"LINES!!!");
    
    SetTargetFPS(fps);
    
    // Load game assets
    raytex = LoadTexture("resources/raylib_logo.png");
    rayfont = LoadFont("resources/acme7wide.ttf");

    GuiSetFont(rayfont);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    
    // Resetting player position
    playerx = GetScreenWidth() / 2;
    playery = GetScreenHeight() / 2;
    
    // Game loop
    while (!WindowShouldClose()) {
        if (scene == 1) Splashscreen();
        if (scene == 2) Menu();
        if (scene == 3) Game();
        if (scene == 4) GameOver();
    }
    
    // Else if ESC pressed or closed by default
    CloseWindow();
    return 0;
}

void Splashscreen() {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10,10);
        DrawText(madeWithTxt, (GetScreenWidth() - MeasureText(madeWithTxt, 48)) / 2, GetScreenHeight() / 3 - 45,48, BLACK );
        DrawTexture(raytex, ( GetScreenWidth() - raytex.width) / 2 ,GetScreenHeight() / 3 + 45,WHITE);        
        if (timer++ > 120) scene = 2;
    EndDrawing();
}

void Menu() {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText(titleTxt, (GetScreenWidth() - MeasureText(titleTxt, 96)) / 2,50,96,BLACK);
        DrawText(copyrightTxt, 10, GetScreenHeight() - 32,32,BLUE);
        startgamebuttonpressed = GuiButton((Rectangle){ (GetScreenWidth() - 250) / 2, GetScreenHeight() / 3 + 125,250,100 },"PLAY");
        exitgamebuttonpressed = GuiButton((Rectangle){ (GetScreenWidth() - 250) / 2,GetScreenHeight() / 3 + 275,250,100 },"EXIT");
        if (startgamebuttonpressed) ResetGame();
        if (exitgamebuttonpressed) {
            UnloadResources();
            UnloadFont(rayfont);
            CloseWindow();
            exit(0);
        }    
        DrawFPS(10,10);
    EndDrawing();
}

void Game() {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText(FormatText("%is",seconds),GetScreenWidth() / 2.1,10,64,PURPLE);
        if (alive) {
            // Check OS,If mobile only use touch
            // Else,Use Gamepad,Keyboard,Mouse (Cause OS is desktop)
            #ifdef __ANDROID__
                playerx = GetTouchX();
                playery = GetTouchY();
            #elif TARGET_OS_EMBEDDED
                playerx = GetTouchX();
                playery = GetTouchY();
            #elif TARGET_IPHONE_SIMULATOR
                playerx = GetTouchX();
                playery = GetTouchY();
            #elif TARGET_OS_IPHONE
                playerx = GetTouchX();
                playery = GetTouchY();
            #else
                if (IsMouseButtonDown(0)) {
                    playerx = GetMouseX();
                    playery = GetMouseY();                    
                }
                if(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) playery -= 5;
                if(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) playerx -= 5;
                if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) playery += 5;
                if(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) playerx += 5;
                if(IsGamepadAvailable(GAMEPAD_PLAYER1)) {
                    if(IsGamepadButtonDown(GAMEPAD_PLAYER1,GAMEPAD_BUTTON_LEFT_FACE_UP)) playery -= 5;
                    if(IsGamepadButtonDown(GAMEPAD_PLAYER1,GAMEPAD_BUTTON_LEFT_FACE_LEFT)) playerx -= 5;
                    if(IsGamepadButtonDown(GAMEPAD_PLAYER1,GAMEPAD_BUTTON_LEFT_FACE_DOWN)) playery += 5;
                    if(IsGamepadButtonDown(GAMEPAD_PLAYER1,GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) playerx += 5;
                }
            #endif
            if (playery < 10) playery = 10;
            if (playerx < 5) playerx = 5; 
            DrawCircle(playerx,playery,5.0f,RED);
        }
        DrawCircle(playerx,playery,explosionsize,ORANGE);        
        if (linestimer >= 120) {
            DrawLines();          
            if (linestimer >= 240) {
                for (int i = 0;i < LINES;i++) {
                    lines_size[i] = 3.0f;
                    lines_colors[i] = PINK;
                    CheckCollisions();
                }
            }
            if (linestimer >= 300) {
                CheckCollisions();
                RemakeLines();
                linestimer = 0;
            }
        }
        DrawFPS(10,10);
        if (timer >= 60) {
            timer = 0;
            seconds++;
        }        
        if (!alive) explosionsize += 1.0f;
        if (explosionsize > 80.0f) scene = 4;
        if (alive) timer++;
        if (alive) linestimer++;
    EndDrawing();
}

void GameOver() {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        highscore = LoadStorageValue(HIGHSCORE);
        DrawText(gameOverTxt, (GetScreenWidth() - MeasureText(gameOverTxt, 128)) / 2, GetScreenHeight() / 4,128,RED);
        if ((seconds > highscore) || (highscore == 0)) {
            highscore = seconds;
            SaveStorageValue(HIGHSCORE,highscore);
        }
        DrawText(FormatText("BEST TIME SURVIVED: %i SECONDS",LoadStorageValue(HIGHSCORE)),(GetScreenWidth() - MeasureText(FormatText("BEST TIME SURVIVED: %i SECONDS",highscore), 64)) / 2,GetScreenHeight() / 2,64,RED);
        DrawText(restartTxt,(GetScreenWidth() - MeasureText(restartTxt, 32)) / 2,GetScreenHeight() / 1.4,32,GREEN);
        DrawText(restartgamepadTxt,(GetScreenWidth() - MeasureText(restartTxt, 24)) / 3,GetScreenHeight() / 1.2,24,GREEN);
        if (IsKeyPressed(KEY_SPACE)) scene = 2;
        if (IsKeyPressed(KEY_R)) ResetGame();
        if (IsGamepadAvailable(GAMEPAD_PLAYER1)) {
            if (IsGamepadButtonDown(GAMEPAD_PLAYER1,7)) ResetGame();
            if (IsGamepadButtonDown(GAMEPAD_PLAYER1,6)) scene = 2;
        }
        DrawFPS(10,10);
    EndDrawing();
}

void UnloadResources() {
    UnloadTexture(raytex);
}

void RemakeLines() {
    for (int i = 0;i < LINES;i++) {
        lines_from_x[i] = GetRandomValue(-GetScreenWidth() / 4,GetScreenWidth() * 1.5);
        lines_from_y[i] = GetRandomValue(-GetScreenHeight() / 4,GetScreenHeight() * 1.5);
        lines_to_x[i] = GetRandomValue(-GetScreenWidth() / 4,GetScreenWidth() * 1.5);
        lines_to_y[i] = GetRandomValue(-GetScreenHeight() / 4,GetScreenHeight() * 1.5);
        lines_size[i] = 1.0f;
        lines_colors[i] = BLACK;
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

void ResetGame() {
    RemakeLines();
    seconds = 0;
    timer = 0;
    linestimer = 0;
    scene = 3;
    explosionsize = 0.0f;
    alive = true;            
    gamespeed = 1.5f;
    playerx = GetScreenWidth() / 2;
    playery = GetScreenHeight() / 2;
}