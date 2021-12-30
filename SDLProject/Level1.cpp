#include "Level1.h"
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMY_COUNT 9

unsigned int level1_data[] =
{
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};



void Level1::Initialize() {
    
    state.menu = false;
    
    state.sceneNum = 1;
    state.nextScene = -1;
    
    //fontTextureID = Util::LoadTexture("font1.png");
    
    GLuint mapTextureID = Util::LoadTexture("pixel_platformer_tile_set_2.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 3, 3);
    // Move over all of the player and enemy code from initialization.
    
    // Initialize Player
    state.player = new Entity();
    state.player-> entityType = PLAYER;
    state.player->position = glm::vec3(5,-5,0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("24by24ModernRPGGuy.png");
    
    state.player->animRight = new int[4] {8, 9, 10, 11};
    state.player->animLeft = new int[4] {12, 13, 14, 15};
    state.player->animUp = new int[4] {0, 1, 2, 3};
    state.player->animDown = new int[4] {4, 5, 6, 7};
    
    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.player->height = 0.8f;
    state.player->width = 0.8f;
    
    state.player->normalJump = 6.0f;
    state.player->bounce = 2.0f;
    
    state.player->jumpPower = state.player->normalJump;
    
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("character_robot_sheet.png");
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].animRight = new int[8] {0, 0, 0, 0, 9, 9, 9, 9};
        
        state.enemies[i].animIndices = state.enemies[i].animRight;
        state.enemies[i].animFrames = 8;
        state.enemies[i].animIndex = 0;
        state.enemies[i].animTime = 0;
        state.enemies[i].animCols = 9;
        state.enemies[i].animRows = 5;
    }
    
    state.enemies[0].spawnPoint = glm::vec3(10,-3,0);
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = WALKING;
    state.enemies[0].speed = 0.25;
    
    state.enemies[1].spawnPoint = glm::vec3(4,-2,0);
    state.enemies[1].aiType = PATROL;
    state.enemies[1].aiState = WALKING;
    state.enemies[1].moveState = LEFT;
    state.enemies[1].stateSpeed = 0.25;
    
    state.enemies[2].spawnPoint = glm::vec3(3,-5,0);
    state.enemies[2].aiType = PATROLCIRCLE;
    state.enemies[2].aiState = WALKING;
    state.enemies[2].moveState = UP;
    state.enemies[2].stateSpeed = 0.5;
    
    state.enemies[3].spawnPoint = glm::vec3(7,-2,0);
    state.enemies[3].aiType = BLOCK;
    state.enemies[3].aiState = WALKING;
    state.enemies[3].blockState = Y;
    state.enemies[3].speed = 0.75;
    
    state.enemies[4].spawnPoint = glm::vec3(2,-4,0);
    state.enemies[4].aiType = BLOCK;
    state.enemies[4].aiState = WALKING;
    state.enemies[4].blockState = X;
    state.enemies[4].speed = 0.75;
    
    state.enemies[5].spawnPoint = glm::vec3(1,-1,0);
    state.enemies[5].aiType = BOUNCER;
    state.enemies[5].aiState = WALKING;
    state.enemies[5].moveState = LEFT;
    state.enemies[5].moveStateB = UP;
    
    state.enemies[6].spawnPoint = glm::vec3(12,-3,0);
    state.enemies[6].aiType = BOUNCER;
    state.enemies[6].aiState = WALKING;
    state.enemies[6].moveState = RIGHT;
    state.enemies[6].moveStateB = UP;
    
    //Bug: For some reasonn, ponger states are made opposite. Attempts to fix it caused bouncing to fail. This will be kept due to time constraints and since it only affects development in a minor way, rather than gameplay in any way.
    
    state.enemies[7].spawnPoint = glm::vec3(5,-3,0);
    state.enemies[7].aiType = PONGER;
    state.enemies[7].aiState = WALKING;
    state.enemies[7].moveState = RIGHT;
    state.enemies[7].moveStateB = UP;
    state.enemies[7].speed = 2.0;

    state.enemies[8].spawnPoint = glm::vec3(7,-5,0);
    state.enemies[8].aiType = PONGER;
    state.enemies[8].aiState = WALKING;
    state.enemies[8].moveState = LEFT;
    state.enemies[8].moveStateB = DOWN;
    state.enemies[8].speed = 2.0;
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].Update(0, state.player, NULL, 0, state.map, NULL, 0);
        state.enemies[i].position = state.enemies[i].spawnPoint;
        state.enemies[i].acceleration = glm::vec3(0,0,0);
        
        state.enemies[i].isActive = false;
    }
    
}
void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map, state.enemies, LEVEL1_ENEMY_COUNT);
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, NULL, 0, state.map, NULL, 0);
    }
    
    if (state.victory) {
        state.player->isActive = false;
        for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
            state.enemies[i].isActive = false;
        }
    }
}
void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
    
    
}
