#include "PunktSky.h"

PunktSky::PunktSky(const std::string& filename) 
{
    VAO = 0;
    VBO = 0;
    EBO = 0;
    normalVAO = 0;
    normalVBO = 0;

    loadAndCenterPoints(filename); // Punktskydata blir lasta inn fra filen og punktene blir sentrert rundt origo
    generateRegularTriangulation(); // Generer triangulering av punktene

    // Create VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    //Bufferhåndtering, fordi det reeduserer hvor mange ganger data må gå igjennom CPU og GPU
    std::vector<float> vertexData;
    for (size_t i = 0; i < points.size(); ++i)
    {
        vertexData.push_back(points[i].x);
        vertexData.push_back(points[i].y);
        vertexData.push_back(points[i].z);
        vertexData.push_back(normals[i].x);
        vertexData.push_back(normals[i].y);
        vertexData.push_back(normals[i].z);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glEnableVertexAttribArray(1);
}

PunktSky::~PunktSky() 
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

}

void PunktSky::DrawPunktSky() // Rendrer punktskyen med individuelle punkter
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, points.size());
}

const std::vector<glm::vec3>& PunktSky::GetPoints() const 
{
    return points;
}

void PunktSky::DrawTriangles() // Rendrer trianguleringen basert på indekser
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void PunktSky::DrawNormals() // Normalvektoren blir tegnet som linjer
{
    glGenVertexArrays(1, &normalVAO);
    glGenBuffers(1, &normalVBO);

    std::vector<glm::vec3> normalLines;
    for (size_t i = 0; i < points.size(); ++i)
    {
        normalLines.push_back(points[i]);
        normalLines.push_back(points[i] + normals[i] * 2.0f);
    }

    glBindVertexArray(normalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normalLines.size() * sizeof(glm::vec3), &normalLines[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(normalVAO);
    glDrawArrays(GL_LINES, 0, normalLines.size());

    glDeleteVertexArrays(1, &normalVAO);
    glDeleteBuffers(1, &normalVBO);
}

// Leser punktskydata fra filen
void PunktSky::loadAndCenterPoints(const std::string& filename) 
{
    std::ifstream file(filename);
    if (!file.is_open()) 
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    glm::vec3 min(FLT_MAX), max(-FLT_MAX);
    std::string line;

   
    if (std::getline(file, line)) 
    {
        std::cout << "Point count header: " << line << std::endl;
    }

    // Leser koordinatene fra filen
    int lineNumber = 1;
    while (std::getline(file, line)) 
    {
        lineNumber++;
        glm::vec3 position;
        std::stringstream ss(line);
        ss >> position.x >> position.z >> position.y;

        if (ss.fail()) 
        {
            std::cerr << "Error: Failed to read coordinates on line " << lineNumber << ": " << line << std::endl;
            continue;
        }

        // Oppdaterer min- og maksverdier for å finne bounding box
        min = glm::min(min, position);
        max = glm::max(max, position);
        points.push_back(position);
    }

    file.close(); // Lukker filen

    // Beregner midtpunktet og justerer alle punktene
    glm::vec3 center = (min + max) / 2.0f;
    for (auto& position : points) 
    {
        position -= center;
    }

    std::cout << "Loaded " << points.size() << " points centered around "
        << center.x << ", " << center.y << ", " << center.z << std::endl;
}

void PunktSky::generateRegularTriangulation()
{
    glm::vec3 min(FLT_MAX), max(-FLT_MAX);
    for (const auto& point : points)
    {
        min = glm::min(min, point);
        max = glm::max(max, point);
        // max og min finner maksimum og minimum koordinater for å finne punktskyen
    }

    float gridSpacing = 10.0f;

    // Beregner grid-dimensjoner
    int gridWidth = static_cast<int>((max.x - min.x) / gridSpacing) + 1;
    int gridHeight = static_cast<int>((max.z - min.z) / gridSpacing) + 1;

    // Oppretter griden for å kartlegge punktene
    std::vector<std::vector<int>> grid(gridWidth, std::vector<int>(gridHeight, -1));

    //Plasserer punktene i gridet
    for (size_t i = 0; i < points.size(); ++i)
    {
        int xIndex = static_cast<int>((points[i].x - min.x) / gridSpacing);
        int zIndex = static_cast<int>((points[i].z - min.z) / gridSpacing);

        if (xIndex >= 0 && xIndex < gridWidth && zIndex >= 0 && zIndex < gridHeight)
        {
            grid[xIndex][zIndex] = static_cast<int>(i);
        }
    }

    // Generer trianguleringene
    for (int row = 0; row < gridHeight - 1; ++row)
    {
        for (int col = 0; col < gridWidth - 1; ++col)
        {
            // 4 naboer
            int topLeft = grid[col][row];
            int topRight = grid[col + 1][row];
            int bottomLeft = grid[col][row + 1];
            int bottomRight = grid[col + 1][row + 1];

            if (topLeft != -1 && topRight != -1 && bottomLeft != -1 && bottomRight != -1)
            {
                // Første trekant
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                // Andre trekant
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }
    }



    // Beregning av normalvektorer
    normals.resize(points.size(), glm::vec3(0.0f)); // (0, 0, 0)

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        glm::vec3 v0 = points[i0];
        glm::vec3 v1 = points[i1];
        glm::vec3 v2 = points[i2];

        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0)); // Kryssproduktet for å beregne trekantens normale

        // Normale vektorer til 3 punkter
        normals[i0] += normal;
        normals[i1] += normal;
        normals[i2] += normal;
    }

    // Normalisere vektorene, for at de skal ha lengde på 1
    for (auto& normal : normals)
    {
        normal = glm::normalize(normal);
    }
}
