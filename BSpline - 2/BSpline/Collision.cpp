#include "Collision.h"

bool Collision::checkBallCollision(const glm::vec3& pos1, const glm::vec3& pos2, float radius)
{
    float distance = glm::length(pos1 - pos2); // Dette beregner avstanden mellom to baller 
    return distance <= 1.90f * radius; // Sjekker om avstanden mellom de to kulene er mindre enn eller lik 1,90*radius
}
// pos1 er posisjonen til den første ballen
// pos2 er posisjonen til den andre ballen
// radius er for hver ball
// Denne boolian beregner avstanden mellom sentrene til to kuler.

void Collision::checkWallCollision(glm::vec3& position, glm::vec3& velocity, float minX, float maxX, float minZ, float maxZ, float radius)
// minX- og maxX-verdier for x-aksen, venstre og høyre side/vegg
// minZ- og maxZ-verdier for z-aksen, foran- og bak side/vegg
// radius er kulens radius, for å sikre at kulenes overflate ikke går utenfor flaten.
{
    if (position.x + radius > maxX || position.x - radius < minX) 
    {
        velocity.x = -velocity.x; // Revers x hastighet, noe som får ballen til å sprette av veggen
        position.x = glm::clamp(position.x, minX + radius, maxX - radius); // Clamp posisjonen innenfor grensene
    }
    if (position.z + radius > maxZ || position.z - radius < minZ) 
    {
        velocity.z = -velocity.z; // Revers z hastighet, noe som får ballen til å sprette av veggen
        position.z = glm::clamp(position.z, minZ + radius, maxZ - radius); // Clamp posisjonen innenfor grensene
    }
}
// Clamping er for å sikre at ballen ikke prøver å gå utenfor veggene og begrenser posisjonen mellom minX + radius og maxX-radius.
// At ballen ikke går utenfor banen.

void Collision::responseBallCollision(glm::vec3& pos1, glm::vec3& pos2, glm::vec3& vel1, glm::vec3& vel2, float radius)
{
    if (checkBallCollision(pos1, pos2, radius)) // Sjekker om de to ballene har kollidert
    {
        // Skille ballene ved å flytte dem fra hverandre
        glm::vec3 direction = glm::normalize(pos1 - pos2);
        float overlap = (1.90f * radius) - glm::length(pos1 - pos2);

        pos1 += direction * (overlap / 2.0f);  // Beveger ball 1
        pos2 -= direction * (overlap / 2.0f);  // Beveger ball 2
        // flytter begge ballene med halve overlappingsavstanden, slik at kollisjonen løses likt.

        // Reverser hastigheter når ballene er adskilt.
        vel1 = -vel1;
        vel2 = -vel2;
        // Simulerer effekten av å sprette av hverandre.
    }
}
// vel1 og vel2 er hastigheten til de to ballene.
// Direction: beregner den normaliserte retningsvektoren fra ball 2 to ball 1.
// Overlap: beregner hvor mye de to ballene overlapper.

