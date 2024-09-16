#pragma once

#include <glad/glad.h> // Include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // The program ID
    unsigned int ID;

    // Constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);

    // Use/activate the shader
    void use() const;

    // Utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

private:
    // Utility function for checking shader compilation/linking errors
    void checkCompileErrors(unsigned int shader, const std::string& type) const;
};
