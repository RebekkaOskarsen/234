#include "Ball.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Ball::Ball(float radius, int sectorCount, int stackCount, glm::vec3 color)
    : radius(radius), sectorCount(sectorCount), stackCount(stackCount), color(color), velocity(0.0f)
{
    VAO = 0;
    VBO = 0;
    EBO = 0;

    generateBall();


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

}

void Ball::generateBall() 
{
    float x, y, z, xy;
    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i) 
    {
        stackAngle = M_PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle); // Radius av sirkelen ved gjeldende stabel
        z = radius * sinf(stackAngle); // Høyden på gjeldende stabel

        for (int j = 0; j <= sectorCount; ++j) 
        {
            sectorAngle = j * sectorStep;
            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Legg til fargedata for hvert vertex
            vertices.push_back(color.r);
            vertices.push_back(color.g);
            vertices.push_back(color.b);
        }
    }
    // Looper over alle stabler (breddegradslinjer) fra topp til bunn.
    // For hver stabel, looper over alle sektorer (lengdegradslinjer) for å beregne (x, y, z) posisjonen til hvert vertex.
    // Legger til posisjonen og fargen til hvert vertex til vertex vektoren.


    // Genererer indeksene for å rendre trekanter
    int k1, k2;
    for (int i = 0; i < stackCount; ++i) 
    {
        k1 = i * (sectorCount + 1); // Start vertex for gjeldende stabel
        k2 = k1 + sectorCount + 1; // Start vertex for neste stabel

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) 
        {
            if (i != 0) 
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1)) 
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }// For hver sektor lages det to trekanter
}

Ball::~Ball()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


void Ball::DrawBall(Shader &shader)
{
    glBindVertexArray(VAO);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position); // Translate ballen til sin posisjon

    shader.setMat4("model", model);
    shader.setVec3("objectColor", color); // Send ballens farge til shader

    // Render ballen
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}