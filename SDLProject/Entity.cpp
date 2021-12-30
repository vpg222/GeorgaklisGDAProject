#include "Entity.h"

int randomize(int bound){
    srand((int) time(NULL));
    int randomizer = rand();
    int retr = randomizer % bound ;
    return retr;
};

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 1;
    
    normalJump = 2;
    bounce = 1;
    
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity *other){
    if (!isActive || other->isActive == false) {
        return false;
    }
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    
    if (xdist < 0 && ydist < 0) {
        return true;
    }
    return false;
}

bool Entity::CheckCollisionsY(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                if (objects[i].entityType == ENEMY && entityType == PLAYER && !invincible) {
                    isActive = false;
                }
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                if (objects[i].entityType == ENEMY && entityType == PLAYER && !invincible) {
                    isActive = false;
//                    jumpPower = bounce;
//                    jump = true;
//                    jumpPower = normalJump;
                }
            }
        }
    }
    return collidedTop || collidedBottom;
}

bool Entity::CheckCollisionsY(Map *map)
{
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);
    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    
    return collidedBottom || collidedTop;
}

bool Entity::CheckCollisionsX(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
            if (objects[i].entityType == ENEMY && entityType == PLAYER && !invincible) {
                isActive = false;
            }
        }
    }
    return collidedLeft || collidedRight;
}

bool Entity::CheckCollisionsX(Map *map)
{
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
    return collidedRight || collidedLeft;
}

void Entity::AIWalker(){
    switch (moveState) {
        case LEFT:
            movement = glm::vec3(-1,0,0);
        case RIGHT:
            movement = glm::vec3(1,0,0);
        case UP:
            movement = glm::vec3(0,1,0);
        case DOWN:
            movement = glm::vec3(0,-1,0);
        default:
        break;
    }
}

void Entity::AIJumper(){
    if (collidedBottom) {
        jump = true;
        //movement = glm::vec3(-1,0,0);
        //printf("jum");
    }
}



void Entity::AIPatrolCircle(float deltaTime){
    switch (aiState) {
        case WALKING:
            switch (moveState) {
                case LEFT:
                    movement = glm::vec3(-1,0,0);
                    timeLeft -= deltaTime * stateSpeed;
                    if (timeLeft <= 0) {
                        timeLeft = 1.0f;
                        moveState = DOWN;
                    }
                    break;
                case RIGHT:
                    movement = glm::vec3(1,0,0);
                    timeLeft -= deltaTime  * stateSpeed;
                    if (timeLeft <= 0) {
                        timeLeft = 1.0f;
                        moveState = UP;
                    }
                    break;
                case UP:
                    movement = glm::vec3(0,1,0);
                    timeLeft -= deltaTime * stateSpeed;
                    if (timeLeft <= 0) {
                        timeLeft = 1.0f;
                        moveState = LEFT;
                    }
                    break;
                case DOWN:
                    movement = glm::vec3(0,-1,0);
                    timeLeft -= deltaTime  * stateSpeed;
                    if (timeLeft <= 0) {
                        timeLeft = 1.0f;
                        moveState = RIGHT;
                    }
                    break;
                default:
                    break;
            }
            /*if (glm::distance(position.x, startingPosition.x) > 1){
             if (position.x > startingPosition.x) {
             movement = glm::vec3(-1,0,0);
             }
             else{
             movement = glm::vec3(1,0,0);
             }
             }*/
            
            break;
        default:
            break;
    }
}

void Entity::AIWaitAndGo(Entity* player){
    switch (aiState) {
        case IDLE:
            if(glm::distance(position, player->position) < 3.0f)
                aiState = WALKING;
            break;
        case WALKING:
            if (player->position.x < position.x) {
                movement.x = -1;
            }
            else{
                movement.x = 1;
            }
            if (player->position.y < position.y) {
                movement.y = -1;
            }
            else{
                movement.y = 1;
            }
            movement = glm::normalize(movement);
            break;
        case ATTACKING:
            
            break;
            
        default:
            break;
    }
}

//Blockers follow the player in one direction, attempting to limit their movement.
void Entity::AIBlocker(Entity* player){
    switch (aiState) {
        case IDLE:
            if(glm::distance(position, player->position) < 3.0f)
                aiState = WALKING;
            break;
        case WALKING:
            switch (blockState) {
                case X:
                    if (player->position.x < position.x) {
                        movement.x = -1;
                    }
                    else{
                        movement.x = 1;
                    }
                    break;
                case Y:
                    if (player->position.y < position.y) {
                        movement.y = -1;
                    }
                    else{
                        movement.y = 1;
                    }
                    break;
                default:
                    break;
            }
            movement = glm::normalize(movement);
            break;
        default:
            break;
    }
}

void Entity::AIPatrol(float deltaTime){
    switch (aiState) {
        case WALKING:
            switch (moveState) {
                case LEFT:
                    movement = glm::vec3(-1,0,0);
                    timeLeft -= deltaTime * stateSpeed;
                    if (timeLeft <= 0) {
                        timeLeft = 1.0f;
                        moveState = RIGHT;
                    }
                    break;
                case RIGHT:
                    movement = glm::vec3(1,0,0);
                    timeLeft -= deltaTime  * stateSpeed;
                    if (timeLeft <= 0) {
                        timeLeft = 1.0f;
                        moveState = LEFT;
                    }
                    break;
                case UP:
                    movement = glm::vec3(0,1,0);
                    timeLeft -= deltaTime * stateSpeed;
                    if (timeLeft <= 0) {
                        timeLeft = 1.0f;
                        moveState = DOWN;
                    }
                    break;
                case DOWN:
                    movement = glm::vec3(0,-1,0);
                    timeLeft -= deltaTime  * stateSpeed;
                    if (timeLeft <= 0) {
                        timeLeft = 1.0f;
                        moveState = UP;
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}


//Bouncers change directions when they hit walls.
void Entity::AIBouncer(){
    switch (aiState) {
        case WALKING:
            switch (moveState) {
                case LEFT:
                    movement.x = -1;
                    if (collidedLeft) {
                        //Mix_PlayChannel(-1, bounceSound, 0);
                        moveState = RIGHT;
                    }
                    break;
                case RIGHT:
                    movement.x = 1;
                    if (collidedRight) {
                        //Mix_PlayChannel(-1, bounceSound, 0);
                        moveState = LEFT;
                    }
                    break;
                default:
                    break;
            }
            switch (moveStateB) {
                case UP:
                    movement.y = 1;
                    if (collidedTop) {
                        //Mix_PlayChannel(-1, bounceSound, 0);
                        moveStateB = DOWN;
                    }
                    break;
                case DOWN:
                    movement.y = -1;
                    if (collidedBottom) {
                        //Mix_PlayChannel(-1, bounceSound, 0);
                        moveStateB = UP;
                    }
                    break;
                default:
                    break;
            }
            //movement = glm::normalize(movement);
            break;
        default:
            break;
    }
}


//Pongers act similarly to bouncers. However, their trajectories are random on collision, rather than fixed on collision.
//Bug: For some reasonn, ponger states are made opposite. Attempts to fix it caused bouncing to fail. This will be kept due to time constraints and since it only affects development in a minor way, rather than gameplay in any way.
void Entity::AIPonger(){
    //    Mix_Chunk *bounceSound;
    //    bounceSound = Mix_LoadWAV("bounce.wav");
    switch (aiState) {
        case WALKING:
            switch (moveState) {
                case LEFT:
                    if(movement.x == 0){
                        movement.x = -1;}
                    if (collidedLeft) {
                        movement.x = randomize(10) / 10.0f;
                        moveState = RIGHT;
                    }
                    break;
                case RIGHT:
                    if(movement.x == 0){
                        movement.x = 1;}
                    if (collidedRight) {
                        movement.x = randomize(10) / 10.0f;
                        movement.x = -movement.x;
                        moveState = LEFT;
                    }
                    break;
                default:
                    break;
            }
            switch (moveStateB) {
                case UP:
                    if(movement.y == 0){
                        movement.y = 1;}
                    if (collidedTop) {
                        movement.y = randomize(10) / 10.0f;
                        movement.y = -movement.y;
                        moveStateB = DOWN;
                    }
                    break;
                case DOWN:
                    if(movement.y == 0){
                        movement.y = -1;}
                    if (collidedBottom) {
                        movement.y = randomize(10) / 10.0f;
                        moveStateB = UP;
                    }
                    break;
                default:
                    break;
            }
            //movement = glm::normalize(movement);
            break;
        default:
            break;
    }
}

void Entity::AI(Entity* player, float deltaTime, Map *map){
    switch (aiType) {
        case WALKER:
            AIWalker();
            break;
        case WAITANDGO:
            AIWaitAndGo(player);
            break;
        case PATROL:
            AIPatrol(deltaTime);
            break;
        case PATROLCIRCLE:
            AIPatrolCircle(deltaTime);
            break;
        case JUMPER:
            AIJumper();
            break;
        case BLOCK:
            AIBlocker(player);
            break;
        case BOUNCER:
            AIBouncer();
            break;
        case PONGER:
            AIPonger();
            break;
        default:
            break;
    }
}

void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Map* map, Entity* enemies, int enemyCount)
{
    if (entityType == ENEMY) {
        AI(player, deltaTime, map);
    }
    
    
    if (!isActive) {
        return;
    }
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
//    if (entityType == ENEMY) {
//        AI(player, deltaTime, map);
//    }
    
    switch (invincibleState) {
        case INV:
            invincible = true;
            iTimeLeft -= deltaTime  * invinSpeed;
            if (iTimeLeft <= 0) {
                iTimeLeft = 1.0f;
                invincibleState = VUL;
            }
            break;
        case VUL:
            invincible = false;
            break;
        default:
            break;
    }
    
    collY = CheckCollisionsY(enemies, enemyCount);
    collX = CheckCollisionsX(enemies, enemyCount);
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;
            
            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    
    if (jump) {
        jump = false;
        
        velocity.y += jumpPower;
    }
    
    velocity.x = movement.x * speed;
    velocity.y = movement.y * speed;
    velocity += acceleration * deltaTime;
    

    
    position.y += velocity.y * deltaTime;
    CheckCollisionsY(map);
    CheckCollisionsY(objects, objectCount);
    position.x += velocity.x * deltaTime;
    CheckCollisionsX(map);
    CheckCollisionsX(objects, objectCount);
    // Move on Y
    // Fix if needed
    // Move on X
    // Fix if needed

    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    if (!isActive) {
        return;
    }
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
