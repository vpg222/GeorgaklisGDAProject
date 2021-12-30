#include "Level3.h"
#define LEVEL3_WIDTH 14
#define LEVEL3_HEIGHT 8

#define LEVEL3_ENEMY_COUNT 1

unsigned int level3_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0,
    3, 1, 1, 1, 1, 0, 0, 0, 0, 3, 0, 0, 3, 3,
    3, 2, 2, 2, 2, 0, 1, 0, 0, 0, 0, 0, 3, 3
};



void Level3::Initialize() {
    
    state.menu = false;
    
    state.sceneNum = 3;
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);
    // Move over all of the player and enemy code from initialization.
    
    // Initialize Player
    state.player = new Entity();
    state.player-> entityType = PLAYER;
    state.player->position = glm::vec3(2,0,0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};
    
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
    
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(4,-2.25,0);
    
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(6,-3.25,0);
    state.enemies[0].acceleration = glm::vec3(0,-9.81,0);
    
    
    state.enemies[0].speed = 1;
    state.enemies[0].stateSpeed = 0.5f;
    state.enemies[0].aiType = JUMPER;
    
    state.enemies[0].normalJump = 4.0f;
    
    state.enemies[0].jumpPower = state.player->normalJump;
    
    state.enemies[0].isActive = true;
    
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        
    }
}
void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map, state.enemies, LEVEL3_ENEMY_COUNT);
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, NULL, 0, state.map, NULL, 0);
    }
}


void Level3::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
    
    if (state.player->position.x >= 12) {
        state.victory = true;
    }
}
