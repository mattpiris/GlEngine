#include "ComputeShaderEx.h"
#include <string>
#include <sstream>
#include <istream>
#include <fstream>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <../../utils/stuff/Log.h>

ComputeShaderEx::ComputeShaderEx(const char* computePath, glm::uvec3 workSize)
    :m_workSize(workSize)
{
    // read in shader code
    std::string compute_code;
    std::ifstream file;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        file.open(computePath);
        std::stringstream file_stream;
        file_stream << file.rdbuf();
        file.close();

        compute_code = file_stream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "failed to read compute shader file" << std::endl;
        std::cerr << e.what() << std::endl;
    }

    const char* c_shader_code = compute_code.c_str();

    // compile shader
    unsigned int shader;
    shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &c_shader_code, NULL);
    glCompileShader(shader);

    m_programId = glCreateProgram();
    glAttachShader(m_programId, shader);
    glLinkProgram(m_programId);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        std::cerr << "Compute Shader Compilation Failed: " << log << std::endl;
    }

    glDeleteShader(shader);

    // create texture for reading/writing data
    glGenTextures(1, &m_textureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, m_textureId);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // create empty texture
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, m_workSize.x, m_workSize.y, m_workSize.z, 0, GL_RED, GL_FLOAT, NULL);
    glBindImageTexture(0, m_textureId, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
}

// 2d only dispatch supported for now
void ComputeShaderEx::dispatch() {
    glDispatchCompute(m_workSize.x, m_workSize.y, m_workSize.z);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void ComputeShaderEx::use() {
    glUseProgram(m_programId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, m_textureId);
    glBindImageTexture(0, m_textureId, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
}

void ComputeShaderEx::process() {
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void ComputeShaderEx::setData(std::vector<float> newData) {
    if (newData.size() != m_workSize.x * m_workSize.y * m_workSize.z) {
        LOG("ERROR::COMPUTE_SHADER: the specified data must match worksize");
        return;
    }

    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, m_workSize.x, m_workSize.y, m_workSize.z, 0, GL_RED, GL_FLOAT, newData.data());
}

std::vector<float> ComputeShaderEx::getData() {
    std::vector<float> buffer(m_workSize.x * m_workSize.y * m_workSize.z);
    glBindTexture(GL_TEXTURE_3D, m_textureId);
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RED, GL_FLOAT, buffer.data());
    return buffer;
}
