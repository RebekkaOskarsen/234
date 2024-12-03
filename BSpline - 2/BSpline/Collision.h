#ifndef COLLISION_H
#define COLLISION_H

#include <glm/glm.hpp>

class Collision 
{
    public:  
        static bool checkBallCollision(const glm::vec3& pos1, const glm::vec3& pos2, float radius);
        static void checkWallCollision(glm::vec3& position, glm::vec3& velocity, float minX, float maxX, float minZ, float maxZ, float radius);
        static void responseBallCollision(glm::vec3& pos1, glm::vec3& pos2, glm::vec3& vel1, glm::vec3& vel2, float radius);
};

#endif // !COLLISION_H

