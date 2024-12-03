#ifndef BALL_H
#define BALL_H

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h> 

#include "shaderClass.h"

class Ball
{
	public:
		Ball(float radius, int sectorCount, int stackCount, glm::vec3 color); 
		// Størrelse på ballen, sectorCount antall segmenter (bestemmer hvor glatt ballen ser ut),
		// stackCount antall vertikale divisjoner fra topp til bunn (bestemmer hvor jevn ballen ser ut)

		void DrawBall(Shader &shader);
		~Ball();

		glm::vec3 position;
		glm::vec3 velocity;

	private:
		void generateBall();
		// Brukes til å generere geometrien til ballen.
		// Beregner ball verteksene og indeksene.

		float radius;

		int sectorCount;
		int stackCount;

		std::vector<float> vertices;
		std::vector<unsigned int> indices;

		glm::vec3 color;
		

		GLuint VAO, VBO, EBO;
};
#endif // !BALL_H


