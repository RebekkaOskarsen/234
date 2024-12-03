#ifndef PUNKTSKY_H
#define PUNKTSKY_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <float.h>

class PunktSky
{
public:
    PunktSky(const std::string& filename); // Leser inn data fra fil
    ~PunktSky();

    void DrawPunktSky(); // Renderer punktskyen
    const std::vector<glm::vec3>& GetPoints() const; // Gir tilgang til punktene i punktskyen

    void DrawTriangles(); // Rendrer treanguleringen til punktskyen

    void DrawNormals(); // Rendrer normalvektoren for å se at punktskyen har normaler

private:
    std::vector<glm::vec3> points; // Lagrer punktene
    void loadAndCenterPoints(const std::string& filename); //Leser punktskydata og gjør om posisjonen sånn at punktskyen er sentrert

    std::vector<unsigned int> indices;
    void generateRegularTriangulation(); // Treangulering av punktene

    std::vector<glm::vec3> normals; // Normalvektoren

    GLuint VAO, VBO, EBO;
    GLuint normalVAO, normalVBO;
};
#endif // !PUNKTSKY_H
