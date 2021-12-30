#pragma once

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Map.h"
#include <SDL_mixer.h>

enum EntityType {PLAYER, PLATFORM, ENEMY, SHOT};

enum AIType {WALKER, WAITANDGO, PATROL, JUMPER, PATROLCIRCLE, BLOCK, BOUNCER, PONGER};

enum AIState {IDLE, WALKING, ATTACKING};

enum MoveState {LEFT, RIGHT, UP, DOWN};

enum BlockState {X, Y};

enum InvincibleState {INV, VUL};

class Entity {
public:
    EntityType entityType;
    AIType aiType;
    AIState aiState;
    BlockState blockState;
    MoveState moveState;
    MoveState moveStateB;
    InvincibleState invincibleState = VUL;
    glm::vec3 position;
    glm::vec3 spawnPoint;
    
    
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    
    bool faceLeft = false;
    
    float width = 1;
    float height = 1;
    
    float timeLeft = 1.0f;
    float stateSpeed = 1.0f;
    
    bool invincible = false;
    float iTimeLeft = 1.0f;
    float invinSpeed = 1.0f;
    
    bool jump = false;
    float jumpPower = 0;
    
    float speed;
    
    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    int *animRight = NULL;
    int *animLeft = NULL;
    int *animUp = NULL;
    int *animDown = NULL;
    
    int *animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;
    
    bool isActive = true;
    
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    
    bool collX = false;
    bool collY = false;
    
    int normalJump;
    int bounce;
    
    int lives;
    int startingLives;
    float spawnTime;
    
    Entity();
    
    bool CheckCollision(Entity *other);
    bool CheckCollisionsY(Entity *objects, int objectCount);
    bool CheckCollisionsX(Entity *objects, int objectCount);
    bool CheckCollisionsX(Map *map);
    bool CheckCollisionsY(Map *map);

    
    void Update(float deltaTime, Entity* player, Entity* platforms, int platformCount, Map* map, Entity* enemies, int enemyCount);
    
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    
    void AI(Entity* player, float deltaTime, Map* map);
    void AIWalker();
    void AIWaitAndGo(Entity* player);
    void AIBlocker(Entity* player);
    void AIPatrol(float deltaTime);
    void AIPatrolCircle(float deltaTime);
    void AIJumper();
    void AIBouncer();
    void AIPonger();
};
