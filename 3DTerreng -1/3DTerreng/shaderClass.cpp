#include"shaderClass.h"

string get_file_contents(const char* filename)
{
	//Open the file
	ifstream in(filename, ios::binary); //Open the file in binary mode
	if (in) //If the file is open
	{
		string contents;
		in.seekg(0, ios::end);
		contents.resize(in.tellg());
		in.seekg(0, ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	string vertexCode = get_file_contents(vertexFile);
	string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); //Create a vertex shader	
	glShaderSource(vertexShader, 1, &vertexSource, NULL); //Attach the vertex shader source code to the shader
	glCompileShader(vertexShader); //Compile the vertex shader

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //Create a fragment shader
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL); //Attach the fragment shader source code to the shader
	glCompileShader(fragmentShader); //Compile the fragment shader

	ID = glCreateProgram(); //Create a shader program to link the shaders

	glAttachShader(ID, vertexShader); //Attach the vertex shader to the shader program
	glAttachShader(ID, fragmentShader); //Attach the fragment shader to the shader program
	glLinkProgram(ID); //Link the shader program

	glDeleteShader(vertexShader); //Delete the vertex shader
	glDeleteShader(fragmentShader); //Delete the fragment shader
}


//Activate the shader
void Shader::Activate()
{
	glUseProgram(ID);
}

//Delete the shader
void Shader::Delete()
{
	glDeleteProgram(ID);
}
