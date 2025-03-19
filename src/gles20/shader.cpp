#include "shader/converter.h"
#include "shader/utils.h"
#include "gles20/main.h"
#include "main.h"
#include "utils/env.h"

#include <GLES2/gl2.h>
#include <stdexcept>
#include <string>

void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
void OV_glLinkProgram(GLuint program);

void GLES20::registerShaderOverrides() {
    REGISTEROV(glShaderSource);
    REGISTEROV(glLinkProgram);
}

void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length) {
    std::string combinedSource;
    combineSources(count, string, length, combinedSource);
    
    int version = 0; // useless
    std::string profile = "";
    if (!getShaderVersion(combinedSource, version, profile)) {
        throw std::runtime_error("Shader with no version preprocessor!");
    }

    if (profile != "es") {
        if (profile == "core") LOGI("Let's see how this goes...");

        ShaderConverter::convertAndFix(getKindFromShader(shader), combinedSource);
        const GLchar* newSource = combinedSource.c_str();

        glShaderSource(shader, 1, &newSource, nullptr);
    } else {
        LOGI("Shader already ESSL, no need to convert");
        glShaderSource(shader, 1, string, nullptr);
    }
}

void OV_glLinkProgram(GLuint program) {
    glLinkProgram(program);

    if (getEnvironmentVar("LIBGL_VGPU_DUMP") == "1") {
        GLint success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        
        if (success != GL_TRUE) {
            GLchar bufLog[4096] = { 0 };
            GLint size = 0;
            glGetProgramInfoLog(program, 4096, &size, bufLog);
            
            LOGI("Link error for program %u: %s", program, bufLog);
            throw std::runtime_error("Failed to link program!");
        }
    }
}
