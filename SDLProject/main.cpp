#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Util.h"
#include "Effects.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "title.h"

#include <SDL_timer.h>

//Requirements:
//Name of game + enter to start: Check
//approx. 2 Minutes of gameplay: EXACTLY 2 mins
//3 Moving AI: AI Spawn in at certain intervals
//Lose: Run out of health
//Win: Time runs out
//BGM: Check
//Sound: When Damaged

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

glm::mat4 uiViewMatrix, uiProjectionMatrix;
GLuint fontTextureID;

Scene *currentScene;
Scene *sceneList[4];

int currScene;

Effects *effects;

bool gameOver = false;

int lives = 1;
int maxHealth = 10;
int health = maxHealth;

Mix_Music *music;
Mix_Chunk *damage;

float deltaTime;
#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

bool hardMode;

bool lastCollidedBottom = false;

float startTicks = 0;
float currTicks;
float maxTime = -1;

float timeLeft;

bool TimeUp(float maxTime, float currTicks){
    if (maxTime < 0) {
        return false;
    }
    else if(currTicks >= maxTime){
        return true;
    }
    return false;
}

//SDL_TimerID gameClock;
//Uint32 ticks;

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    currentScene->Initialize();
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Project 6: Top-Down Survival", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    
    uiViewMatrix = glm::mat4(1.0);
    uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);
    
    fontTextureID = Util::LoadTexture("font1.png");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    

    
    // Initialize Game Objects
    
    sceneList[0] = new Title();
    sceneList[1] = new Level1();
    SwitchToScene(sceneList[0]);
    
    effects = new Effects(projectionMatrix, viewMatrix);
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("hull_et_belle.ogg");
    
    damage = Mix_LoadWAV("error_008.ogg");
    
    maxTime = 120;
    
}

void ProcessInput() {
    
    currentScene->state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        currentScene->state.player->movement.x = -1.0f;
                        currentScene->state.player->animIndices = currentScene->state.player->animLeft;
                        break;
                    case SDLK_RIGHT:
                        currentScene->state.player->movement.x = 1.0f;
                        currentScene->state.player->animIndices = currentScene->state.player->animRight;
                        break;
                    case SDLK_UP:
                        currentScene->state.player->movement.y = 1.0f;
                        currentScene->state.player->animIndices = currentScene->state.player->animUp;
                        break;
                    case SDLK_DOWN:
                        currentScene->state.player->movement.y = -1.0f;
                        currentScene->state.player->animIndices = currentScene->state.player->animDown;
                        break;
                    case SDLK_RETURN:
                        if(currentScene->state.menu){
                            SwitchToScene(sceneList[1]);
                            Mix_PlayMusic(music, -1);
                            Mix_VolumeMusic(MIX_MAX_VOLUME/10);
                            startTicks = lastTicks;
                        }
                    case SDLK_h:
                        if(currentScene->state.menu){
                            SwitchToScene(sceneList[1]);
                            Mix_PlayMusic(music, -1);
                            Mix_VolumeMusic(MIX_MAX_VOLUME/15);
                            startTicks = lastTicks;
                            hardMode = true;
                        }
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->movement.x = -1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->movement.x = 1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animRight;
    }
    
    if (keys[SDL_SCANCODE_UP]) {
        currentScene->state.player->movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        currentScene->state.player->movement.y = -1.0f;
    }
    
    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }

}

void prepActivate(int enemyID, float timeToStart){
    if (!currentScene->state.enemies[enemyID].isActive && timeLeft < timeToStart) {
        currentScene->state.enemies[enemyID].isActive = true;
    }
}

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;
    
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return; }
    while (deltaTime >= FIXED_TIMESTEP) {
        currentScene->Update(FIXED_TIMESTEP);
        program.SetLightPosition(currentScene->state.player->position);
            
        if (lastCollidedBottom == false && currentScene->state.player->collidedBottom) {
        }
        lastCollidedBottom = currentScene->state.player->collidedBottom;
            
        effects->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    currTicks = lastTicks - startTicks;
    
    timeLeft = maxTime - currTicks;
    
    viewMatrix = glm::mat4(1.0f);
    if (currentScene->state.player->position.x > 5) {
        viewMatrix = glm::translate(viewMatrix,
                                    glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    } else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }
    
    viewMatrix = glm::translate(viewMatrix, effects->viewOffset);
    
    if (!gameOver && !currentScene->state.victory) {
        
        //AIs will spawn when there's a certain amount of time left for each, so don't take all my points when there's only one AI to start.
        
        prepActivate(0, 120);
        prepActivate(5, 110);
        prepActivate(1, 90);
        if(hardMode)prepActivate(7, 70);
        if(hardMode)prepActivate(4, 60);
        prepActivate(6, 50);
        prepActivate(2, 40);
        prepActivate(5, 30);
        if(hardMode)prepActivate(3, 20);
        
        if (currentScene->state.player->position.y < -10) {
            health = 0;
            currentScene->state.player->isActive = false;
        }
        
        if (currentScene->state.player->isActive == false && health > 1) {
            Mix_Volume(-1,MIX_MAX_VOLUME/7.5);
            Mix_PlayChannel(-1, damage, 0);
            health = health - 1;
            currentScene->state.player->invincibleState = INV;
            currentScene->state.player->isActive = true;
        }
        else if(currentScene->state.player->isActive == false){
            lives = lives - 1;
            if (lives > 0) {
                health = maxHealth;
                SwitchToScene(sceneList[currentScene->state.sceneNum]);
            }
        }
        
        if (health <= 1 && lives <= 0) {
            currentScene->state.player->isActive = false;
            gameOver = true;
        }
        
        if (timeLeft <= 0 && !gameOver) {
            currentScene->state.victory = true;
        }
    }
    

    
    
    
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (currentScene->state.player->invincible || (gameOver && !currentScene->state.victory)) {
        program.Load("shaders/vertex_textured.glsl", "shaders/effects_textured.glsl");
    }
    else{
        program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    }
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    glUseProgram(program.programID);
    currentScene->Render(&program);
    
    program.SetProjectionMatrix(uiProjectionMatrix);
    program.SetViewMatrix(uiViewMatrix);
    if(currentScene->state.menu){
        Util::DrawText(&program, fontTextureID, "Project 6: Top-Down Survival", 0.5, -0.3f, glm::vec3(-2, 0, 0));
        Util::DrawText(&program, fontTextureID, "Press enter to start", 0.5, -0.3f, glm::vec3(-6, 3.2, 0));
    }
    else if(currentScene->state.victory){
        Util::DrawText(&program, fontTextureID, "You Win!", 0.5, -0.3f, glm::vec3(-1, 3.2, 0));
        Util::DrawText(&program, fontTextureID, "You Win!", 0.5, -0.3f, glm::vec3(-1, 2.7, 0));
        Util::DrawText(&program, fontTextureID, "You Win!", 0.5, -0.3f, glm::vec3(-1, 2.2, 0));
        
        Util::DrawText(&program, fontTextureID, "CREDITS:", 0.5, -0.3f, glm::vec3(-6, 2.0, 0));
        Util::DrawText(&program, fontTextureID, "Project By: Billy Georgaklis", 0.5, -0.275f, glm::vec3(-6, 1.0, 0));
        Util::DrawText(&program, fontTextureID, "Player Sprites: OVFudj on Open Game Art", 0.5, -0.275f, glm::vec3(-6, 0.5, 0));
        Util::DrawText(&program, fontTextureID, "Music: Gobusto on Open Game Art", 0.5, -0.275f, glm::vec3(-6, 0.0, 0));
        Util::DrawText(&program, fontTextureID, "Damage Sound, Enemy Sprites: Kenney on kenney.nl", 0.5, -0.275f, glm::vec3(-6, -0.5, 0));
        Util::DrawText(&program, fontTextureID, "Environment Tiles: CoderMcCode on Open Game Art", 0.5, -0.275f, glm::vec3(-6, -1.0, 0));
        if(!hardMode)Util::DrawText(&program, fontTextureID, "Press H instead of enter next time to try Hard Mode!", 0.5, -0.275f, glm::vec3(-6, -2.0, 0));
        else{Util::DrawText(&program, fontTextureID, "You actually beat Hard Mode!", 0.5, -0.275f, glm::vec3(-6, -2.0, 0));}
    }
    else if(gameOver){
        Util::DrawText(&program, fontTextureID, "You Lose", 0.5, -0.3f, glm::vec3(-1, 3.2, 0));
    }
    else {
        Util::DrawText(&program, fontTextureID, "Health: "+ std::to_string(health), 0.5, -0.3f, glm::vec3(-6, 3.2, 0));
        Util::DrawText(&program, fontTextureID, "Time: "+ std::to_string(timeLeft), 0.5, -0.3f, glm::vec3(-6, 2.7, 0));
        if (hardMode) {
            Util::DrawText(&program, fontTextureID, "Warning: Hard Mode!", 0.5, -0.3f, glm::vec3(-6, 2.2, 0));
        }
    }
    
    effects->Render();
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        
        if (currentScene->state.nextScene >= 0) {
            SwitchToScene(sceneList[currentScene->state.nextScene]);
        }
        
        Render();
    }
    
    Shutdown();
    return 0;
}
