#include "BSplineSurface.h"
#include <iostream>
#include <glm/glm.hpp> // For glm::clamp

BSplineSurface::BSplineSurface() 
{
    VAO, VBO, EBO = 0;
    normalVAO, normalVBO = 0;

    // Kontrollpunkter for en bikvadratisk B-spline flate
    controlPoints = 
    {
        {0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {3, 0, 0},
        {0, 1, 0}, {1, 1, 2}, {2, 1, 2}, {3, 1, 0},
        {0, 2, 0}, {1, 2, 0}, {2, 2, 0}, {3, 2, 0}
    };

    // Skjøtevektorer
    uKnots = { 0, 0, 0, 1, 2, 2, 2 }; // 7 skjøteverdier for 4 kontrollpunkter + grad 2
    vKnots = { 0, 0, 0, 1, 1, 1 };    // 6 skjøteverdier for 3 kontrollpunkter + grad 2
}

BSplineSurface::~BSplineSurface()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

float BSplineSurface::BasisFunction(int i, int degree, float t, const std::vector<float>& knots) {
    if (i + degree >= knots.size() || i + degree + 1 >= knots.size()) 
    {
        return 0.0f;
    }

    if (degree == 0) {
        return (t >= knots[i] && t < knots[i + 1]) ? 1.0f : 0.0f; // Verdien 1.0 hvis t ligger mellom knots[i] og knots [i+1]
    }

    float left = 0.0f; // Venstre basisfunksjon
    float right = 0.0f; // Høyre basisfunksjon

    //Venstre
    // Denne finner hvor langt t har kommet inn i intervallet 
    if (knots[i + degree] - knots[i] != 0.0f) {
        left = (t - knots[i]) / (knots[i + degree] - knots[i]) * BasisFunction(i, degree - 1, t, knots);
    }
    //Høyre
    // Denne finner hvor langt t er fra slutten av intervalet
    if (knots[i + degree + 1] - knots[i + 1] != 0.0f) {
        right = (knots[i + degree + 1] - t) / (knots[i + degree + 1] - knots[i + 1]) * BasisFunction(i + 1, degree - 1, t, knots);
    }

    return left + right; // Slår dem sammen for å få jevnhet tilsammen
}

float BSplineSurface::BasisFunctionDerivative(int i, int degree, float t, const std::vector<float>& knots)
{
    if (degree == 0) 
    {
        return 0.0f;
    }

    float left = 0.0f;
    float right = 0.0f;

    // Bergner venstre delen av derivatet
    if (knots[i + degree] - knots[i] != 0.0f) 
    {
        left = degree / (knots[i + degree] - knots[i]) * BasisFunction(i, degree - 1, t, knots); // Venstre del øker når t går inn i intervallet
    }
    // Beregner høyre deleb av derivatet
    if (knots[i + degree + 1] - knots[i + 1] != 0.0f) 
    {
        right = degree / (knots[i + degree + 1] - knots[i + 1]) * BasisFunction(i + 1, degree - 1, t, knots); // Høyre minker når t går ut av intervallet
    }

    return left - right; // Finne forskjellen mellom venstre og høyre
}

glm::vec3 BSplineSurface::PartialDerivativeU(float u, float v)
{
    glm::vec3 derivative(0.0f);
    int uDegree = 2; // Graden for u retning
    int vDegree = 2; // Graden for v retning
    int uSize = 4; // Antall kontrollpunkter i u retning
    int vSize = 3; // Antall kontrollpunkter i v retning

    for (int i = 0; i < uSize; ++i) // Beregner summen for alle kontrollpunkter
    {
        for (int j = 0; j < vSize; ++j) 
        {
            int index = j * uSize + i;
            float BuPrime = BasisFunctionDerivative(i, uDegree, u, uKnots); // Derivert basisfunksjon for u retning
            float Bv = BasisFunction(j, vDegree, v, vKnots); // Basisfunkson for v retning
            derivative += BuPrime * Bv * controlPoints[index]; //summerer verdiene av funksjonene og kontrollpunktet
        }
    }

    return derivative;
}

glm::vec3 BSplineSurface::PartialDerivativeV(float u, float v)
{
    glm::vec3 derivative(0.0f);
    int uDegree = 2; // Graden for u retning
    int vDegree = 2; // Graden for v retning
    int uSize = 4; // Antall kontrollpunkter i u retning
    int vSize = 3; // Antall kontrollpunkter i v retning

    for (int i = 0; i < uSize; ++i) // Beregner summen for alle kontrollpunkter
    {
        for (int j = 0; j < vSize; ++j) 
        {
            int index = j * uSize + i;
            float Bu = BasisFunction(i, uDegree, u, uKnots);
            float BvPrime = BasisFunctionDerivative(j, vDegree, v, vKnots);
            derivative += Bu * BvPrime * controlPoints[index];
        }
    }

    return derivative;
}

glm::vec3 BSplineSurface::EvaluateSurface(float u, float v) {
    glm::vec3 point(0.0f);

    int uDegree = 2; 
    int vDegree = 2; 
    int uSize = 4; 
    int vSize = 3;

    for (int i = 0; i < uSize; ++i) {
        for (int j = 0; j < vSize; ++j) {
            int index = j * uSize + i;
            float Bu = BasisFunction(i, uDegree, u, uKnots); // Basisfunksjonen i u retning
            float Bv = BasisFunction(j, vDegree, v, vKnots); // Basisfunksjonen i v retning
            point += Bu * Bv * controlPoints[index]; // Beregner punktet på overflaten
        }
    }

    return point; // Evaluerte punktet
}

void BSplineSurface::GenerateSurface(int uRes, int vRes) 
{
    surfacePoints.clear();
    indices.clear();
    normals.clear();

    for (int i = 0; i <= uRes; ++i) 
    {
        float u = glm::clamp( // Holder u verdien innenfor
            static_cast<float>(i) / uRes * (uKnots.back() - uKnots.front()) + uKnots.front(),
            uKnots.front(),
            uKnots.back() - 1e-4f); // Clamper for å unngå overskridelse
        for (int j = 0; j <= vRes; ++j) 
        {
            float v = glm::clamp( // Holder v verdien innenfor
                static_cast<float>(j) / vRes * (vKnots.back() - vKnots.front()) + vKnots.front(),
                vKnots.front(),
                vKnots.back() - 1e-4f); // Clamper for å unngå overskridelse
            glm::vec3 point = EvaluateSurface(u, v);
            surfacePoints.push_back(point);

            // Beregne normalvektoren til flaten ved å bruke U og V parameteret
            glm::vec3 normal = ComputeNormal(u, v); 
            if (glm::length(normal) > 0.0f) 
            {
                normals.push_back(glm::normalize(normal));
            }
            else 
            {
                normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
            }

            // Setter opp indekser for flaten
            if (i < uRes && j < vRes) 
            {
                unsigned int topLeft = j * (uRes + 1) + i;
                unsigned int topRight = topLeft + 1;
                unsigned int bottomLeft = (j + 1) * (uRes + 1) + i;
                unsigned int bottomRight = bottomLeft + 1;

                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);

                indices.push_back(topLeft);
                indices.push_back(bottomRight);
                indices.push_back(topRight);
            }
        }
    }
    SetupMesh();
}

// Beregner normalvektorer ved å bruke kryssproduktet av partiellderivater:
// Normal(u, v) = du x dv
glm::vec3 BSplineSurface::ComputeNormal(float u, float v)
{
    glm::vec3 du = PartialDerivativeU(u, v);
    glm::vec3 dv = PartialDerivativeV(u, v);
    return glm::normalize(glm::cross(du, dv));
}

void BSplineSurface::SetupMesh() 
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    std::vector<glm::vec3> vertexData;
    for (size_t i = 0; i < surfacePoints.size(); ++i) 
    {
        vertexData.push_back(surfacePoints[i]); 
        vertexData.push_back(normals[i]);      
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(glm::vec3), &vertexData[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    
    glGenVertexArrays(1, &normalVAO);
    glGenBuffers(1, &normalVBO);

    std::vector<glm::vec3> normalLines;
    for (size_t i = 0; i < surfacePoints.size(); ++i) 
    {
        glm::vec3 start = surfacePoints[i];
        glm::vec3 end = start + normals[i] * 0.2f; 
        normalLines.push_back(start);
        normalLines.push_back(end);
    }

    glBindVertexArray(normalVAO);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normalLines.size() * sizeof(glm::vec3), &normalLines[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void BSplineSurface::DrawBSpline(Shader& shaderProgram) {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void BSplineSurface::DrawNormals(Shader& shaderProgram)
{
    glBindVertexArray(normalVAO);
    glDrawArrays(GL_LINES, 0, surfacePoints.size() * 2);
    glBindVertexArray(0);
}
