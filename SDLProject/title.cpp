#define TITLE_WIDTH 14
#define TITLE_HEIGHT 8

#define LEVEL3_ENEMY_COUNT 1

#include "title.h"
unsigned int title_data[] =
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

//Says it CAN be a solid color with text, not that it NEEDS to be.

void Title::Initialize() {
    
    state.menu = true;
    
    state.sceneNum = 0;
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("pixel_platformer_tile_set_2.png");
    state.map = new Map(TITLE_WIDTH, TITLE_HEIGHT, title_data, mapTextureID, 1.0f, 3, 3);
    // Move over all of the player and enemy code from initialization.
    
    // Initialize Player
    state.player = new Entity();
    state.player-> entityType = PLAYER;
    state.player->position = glm::vec3(5,0,0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("24by24ModernRPGGuy.png");
    
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
    GLuint enemyTextureID = Util::LoadTexture("character_robot_sheet.png");
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
void Title::Update(float deltaTime) {
    
}


void Title::Render(ShaderProgram *program) {
    state.map->Render(program);
}
