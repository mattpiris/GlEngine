#include "ComputeShader.h"
#include <fstream>
#include <string>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <../../utils/stuff/Log.h>

ComputeShader::ComputeShader(const char* computePath, glm::uvec3 workSize, glm::vec3 localWorkSize)
    :m_workSize(workSize), m_localWorkSize(localWorkSize), m_texture3D(0)
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
        LOG("failed to read compute shader file");
        LOG(e.what());
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
}

unsigned int ComputeShader::createSSBO(size_t size, unsigned int bindingIndex) {
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    return ssbo;
}

unsigned int ComputeShader::setSSBOData(std::vector<float> data, unsigned int bindingIndex) {
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, data.size(), data.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    return ssbo;
}

void ComputeShader::setTexture3dData(std::vector<float> data, glm::vec3 size, unsigned int bindingIndex) {
    glGenTextures(1, &m_texture3D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, m_texture3D);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // create empty texture
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F,  0, GL_RED, GL_FLOAT, static_cast<GLsizei>(size.x),
        static_cast<GLsizei>(size.y), static_cast<GLsizei>(size.z), data.data());
    glBindImageTexture(bindingIndex, m_texture3D, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
}

unsigned int ComputeShader::createAtomicBuffer(unsigned int bindingIndex) {
    glGenBuffers(1, &atomic);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomic);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, bindingIndex, atomic);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    return atomic;
}

void ComputeShader::use() {
    glUseProgram(m_programId);
}

void ComputeShader::dispatch() {
    // Dispatch compute shader with your workgroup sizes
    glDispatchCompute(m_workSize.x, m_workSize.y, m_workSize.z);
   // glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void ComputeShader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(m_programId, name.c_str()), (int)value);
}

void ComputeShader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_programId, name.c_str()), value);
}
void ComputeShader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(m_programId, name.c_str()), value);
}

void ComputeShader::setMat2(const std::string& name, const glm::mat2& mat) const {
    glUniformMatrix2fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void ComputeShader::setMat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void ComputeShader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ComputeShader::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(m_programId, name.c_str()), 1, &value[0]);
}

void ComputeShader::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(m_programId, name.c_str()), x, y);
}

void ComputeShader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(m_programId, name.c_str()), 1, &value[0]);
}

void ComputeShader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(m_programId, name.c_str()), x, y, z);
}

void ComputeShader::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(m_programId, name.c_str()), 1, &value[0]);
}

void ComputeShader::setVec4(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(m_programId, name.c_str()), x, y, z, w);
}
