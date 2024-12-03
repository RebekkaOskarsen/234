
#ifndef BSPLINESURFACE_H
#define BSPLINESURFACE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "shaderClass.h"

class BSplineSurface 
{
public:
    BSplineSurface();
    ~BSplineSurface();

    void GenerateSurface(int uRes, int vRes); // Genererer flaten basert på u og v oppløsning
    void DrawBSpline(Shader& shaderProgram); // Rendrer flaten
    void DrawNormals(Shader& shaderProgram); // Rendrer normalvektorer på overflaten for å se at flaten har normaler

    glm::vec3 EvaluateSurface(float u, float v); // Evaluerer en punktverdi på flaten basert på u og v parametere

private:
    std::vector<glm::vec3> controlPoints; // Kontrollpunktene
    std::vector<float> uKnots; // Skjøtevektor u
    std::vector<float> vKnots; // Skjøtevektor v
    std::vector<glm::vec3> surfacePoints; // Punktdata for flaten
    std::vector<unsigned int> indices; // Rendre trekanter på flaten
    std::vector<glm::vec3> normals; // Normalvekotren for flaten

    float BasisFunction(int i, int degree, float t, const std::vector<float>& knots); // Beregner basisfunksjonen for et gitt indeks, grad og parameter t
    float BasisFunctionDerivative(int i, int degree, float t, const std::vector<float>& knots); // Beregner derivatet av basisfunksjonen for et gitt indeks, grad og parameter t

    glm::vec3 PartialDerivativeU(float u, float v); // Beregner partielt derivat i u-retningen på flaten
    glm::vec3 PartialDerivativeV(float u, float v); // Beregner partielt derivat i v-retningen på flaten
    glm::vec3 ComputeNormal(float u, float v);  // Beregner normalvektoren på et punkt på flaten

    void SetupMesh();

    GLuint VAO, VBO, EBO;
    GLuint normalVAO, normalVBO;
};

#endif // !BSPLINESURFACE_H