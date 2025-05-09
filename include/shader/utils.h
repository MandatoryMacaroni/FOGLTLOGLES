#pragma once

#include "es/utils.h"
#include "utils/types.h"
#include "shaderc/shaderc.hpp"
#include "utils/log.h"

#include <GLES3/gl31.h>
#include <glsl-parser/ast.h>
#include <regex>

inline void combineSources(GLsizei count, const GLchar *const* sources, const GLint* length, std::string& destination) {
    for (GLsizei i = 0; i < count; i++) {
        if (sources[i]) {
           if (length && length[i] > 0) {
                 destination.append(sources[i], length[i]);
           } else {
               destination.append(sources[i]);
           }
       }
   }
}

inline int getAstFriendlyTypeFromKind(shaderc_shader_kind kind) {
    switch (kind) {
        case shaderc_vertex_shader:
            return glsl::astTU::kVertex;
        case shaderc_fragment_shader:
            return glsl::astTU::kFragment;
        case shaderc_compute_shader:
            return glsl::astTU::kCompute;
        default: return glsl::astTU::kVertex;
    }
}

inline shaderc_shader_kind getKindFromShader(GLuint shader) {
    GLint shaderType;
    glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);
    
    switch (shaderType) {
        case GL_FRAGMENT_SHADER:
            return shaderc_fragment_shader;
        case GL_VERTEX_SHADER:
            return shaderc_vertex_shader;
        case GL_COMPUTE_SHADER:
            if (ESUtils::version.first == 3 && ESUtils::version.second >= 1) return shaderc_compute_shader;
            throw std::runtime_error("You need OpenGL ES 3.1 or newer for compute shaders!");
        
        default:
            LOGI("%u", shader);
            throw std::runtime_error("Received an unsupported shader type!");

    }
}


inline str getKindStringFromKind(shaderc_shader_kind kind) {
    switch (kind) {
        case shaderc_vertex_shader:
            return "VertexShader";
        case shaderc_fragment_shader:
            return "FragmentShader";
        case shaderc_compute_shader:
            return "ComputeShader";
        default: return "";
    }
}

inline void replaceShaderVersion(std::string& shaderSource, std::string newVersion, std::string type = "") {
    std::regex versionRegex(R"(#version\s+\d+(\s+\w+)?\b)");  // Ensures full match
    std::string replacement = "#version " + newVersion + (type != "" ? " " + type : "") + "\n";

    LOGI("Replacing shader version to %s", replacement.c_str());
    shaderSource = std::regex_replace(shaderSource, versionRegex, replacement);
}

inline bool getShaderVersion(const std::string& source, int& version, std::string& profile) {
    static const std::regex pattern(R"(#version\s+(\d+)(?:\s+(es|core|compatibility))?)");
    std::smatch match;
    
    // Search for #version in the entire source
    if (std::regex_search(source, match, pattern)) {
        version = std::stoi(match[1].str());
        profile = match[2].matched ? match[2].str() : "";

        return true;
    }
    return false;
}