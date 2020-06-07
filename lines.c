// LINES!!!,A game made with Raylib as challenge
// Written by Rabia Alhaffar on 7/June/2020
// Some support got from Anata,On Raylib Discord channel (Very thankful)
// NOTES: Works best under 1366x768

//Libs imported
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Lines and their properties,Changes randomoly every wave (5 seconds)
#define LINES 125

int lines_from_x[LINES];
int lines_from_y[LINES];
int lines_to_x[LINES];
int lines_to_y[LINES];
float lines_size[LINES];
Color lines_colors[LINES];

// Game assets
Image rayimg;
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
int animationtimer = 0;

// Buttons,As we will use Raygui
bool startgamebuttonpressed = false;
bool exitgamebuttonpressed = false;

// Game scenes and functions
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
    InitWindow(GetScreenWidth(),GetScreenHeight(),"LINES!!!");
    
    // If game window ready,Start game!!!
    if (IsWindowReady()) {
        SetWindowPosition(0,0);
        SetTargetFPS(fps);
        ToggleFullscreen();
        TraceLog(LOG_INFO,"GAME LAUNCHED SUCCESSFULLY!!!\n");
        
        // Load game assets
        rayimg = LoadImage("resources/raylib_logo.png");
        raytex = LoadTextureFromImage(rayimg);
        rayfont = LoadFont("resources/acme7wide.ttf");
        
        // Game loop
        while (!WindowShouldClose()) {
            if (scene == 1) Splashscreen();
            if (scene == 2) Menu();
            if (scene == 3) Game();
            if (scene == 4) GameOver();
        }
    }
    
    // Else cause of error,Close game!!!
    if ((!IsWindowReady()) || (IsWindowMinimized())) {
        TraceLog(LOG_ERROR,"GAME FAILED TO LAUNCH,CLOSING GAME...\n");
        CloseWindow();
        return 0;
    }
    
    // Else if ESC pressed or closed by default
    CloseWindow();
    return 0;
}

void Splashscreen() {
    TraceLog(LOG_INFO,"SPLASHSCREEN STARTED SUCCESSFULLY!!!\n");
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10,10);
        DrawTexture(raytex, ( GetScreenWidth() - raytex.width) / 2 ,GetScreenHeight() / 2.8,WHITE);
        timer++;
        if (timer > 120) {
            timer = 0;
            scene = 2;
            TraceLog(LOG_DEBUG,"SWITCHING TO MAIN MENU...\n");
        }
    EndDrawing();
}

void Menu() {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("LINES!!!",GetScreenWidth() / 2.7f,50,96,BLACK);
        DrawText("CREATED BY RABIA ALHAFFAR!!!",10,GetScreenHeight() - 32,32,BLUE);
        TraceLog(LOG_DEBUG,"LOADING MAIN MENU GUI...\n");       
        startgamebuttonpressed = GuiButton((Rectangle){ GetScreenWidth() / 3 + 100,GetScreenHeight() / 3 + 125,250,100 },"");
        exitgamebuttonpressed = GuiButton((Rectangle){ GetScreenWidth() / 3 + 100,GetScreenHeight() / 3 + 275,250,100 },"");        
        DrawTextEx(rayfont,"PLAY",(Vector2){ GetScreenWidth() / 3 + 170,GetScreenHeight() / 3 + 150 },48,1.0f,BLACK);
        DrawTextEx(rayfont,"EXIT",(Vector2){ GetScreenWidth() / 3 + 175,GetScreenHeight() / 3 + 300 },48,1.0f,BLACK);       
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
            playerx = GetMouseX();
            playery = GetMouseY();
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
        DrawText("GAME OVER",GetScreenWidth() / 4.5,GetScreenHeight() / 4,128,RED);
        if ((seconds > highscore) || (highscore == 0)) highscore = seconds;
        DrawText(FormatText("BEST TIME SURVIVED: %i SECONDS",highscore),GetScreenWidth() / 12.4,GetScreenHeight() / 2,64,RED);
        DrawText("PRESS SPACE KEY TO GO MAIN MENU,OR R KEY TO RETRY",GetScreenWidth() / 7,GetScreenHeight() / 1.4,32,GREEN);
        if (IsKeyPressed(KEY_SPACE)) scene = 2;
        if (IsKeyPressed(KEY_R)) ResetGame();
        DrawFPS(10,10);
    EndDrawing();
}

void UnloadResources() {
    UnloadImage(rayimg);
    UnloadTexture(raytex);
}

void RemakeLines() {
    for (int i = 0;i < LINES;i++) {
        lines_from_x[i] = GetRandomValue(-GetScreenWidth() / 6,GetScreenWidth() * 1.5);
        lines_from_y[i] = GetRandomValue(-GetScreenHeight() / 6,GetScreenHeight() * 1.5);
        lines_to_x[i] = GetRandomValue(-GetScreenWidth() / 6,GetScreenWidth() * 1.5);
        lines_to_y[i] = GetRandomValue(-GetScreenHeight() / 6,GetScreenHeight() * 1.5);
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
        float v2x = GetMouseX() - lines_from_x[i];
        float v2y = GetMouseY() - lines_from_y[i];
        float u = (v2x * v1x + v2y * v1y) / (v1y * v1y + v1x * v1x);
        if (u >= 0 && u <= 1) dist = pow((lines_from_x[i] + v1x * u - GetMouseX()),2) + pow((lines_from_y[i] + v1y * u - GetMouseY()),2);
        else {
            if (u < 0) dist = pow((lines_from_x[i] - GetMouseX()),2) + pow((lines_from_y[i] - GetMouseY()),2);
            else dist = pow((lines_to_x[i] - GetMouseX()),2) + pow((lines_to_y[i] - GetMouseY()),2);
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
}