#include "es/utils.h"
#include "gl/glext.h"
#include "gl/header.h"
#include "gles/main.h"
#include "main.h"
#include "utils/strings.h"

#include <GLES/gl.h>
#include <unordered_map>

#ifndef CAST_TO_CUBYTE
#define CAST_TO_CUBYTE(str) reinterpret_cast<const GLubyte*>(str)
#endif

const GLubyte* OV_glGetString(GLenum name);
void OV_glGetIntegerv(GLenum pname, int* v);
const GLubyte* OV_glGetStringi(GLenum pname, int index);

inline std::string glVersion;
inline std::string rendererString;

inline std::unordered_set<str> extensionMap;
inline std::string extensions;

void GLES::registerBrandingOverride() {
    glVersion = string_format(
        "3.2.0 (on ES %i.%i)",
        ESUtils::version.first,
        ESUtils::version.second
    );

    rendererString = string_format(
        "FOGLTLOGLES (on %s)",
        glGetString(GL_RENDERER)
    );

    extensionMap = ESUtils::extensions;

    LOGI("Spoofing ARB_buffer_storage");
    extensionMap.insert("ARB_buffer_storage");

    extensions = join_set(extensionMap, " ");

    REGISTEROV(glGetString);
}

const GLubyte* OV_glGetString(GLenum name) {
    switch (name) {
        case GL_VERSION:
            return CAST_TO_CUBYTE(glVersion.c_str());
        
        case GL_SHADING_LANGUAGE_VERSION:
            return CAST_TO_CUBYTE("1.50 FOGLTLOGLES"); // 1.50 for GL3.2

        case GL_VENDOR:
            return CAST_TO_CUBYTE("ThatMG393");

        case GL_RENDERER:
            return CAST_TO_CUBYTE(rendererString.c_str());

        case GL_EXTENSIONS:
            return CAST_TO_CUBYTE(extensions.c_str());

        default:
            return glGetString(name);
    }
}

void OV_glGetIntegerv(GLenum pname, int* v) {
    switch (pname) {
        case GL_NUM_EXTENSIONS:
            (*v) = extensions.size();
            break;

        default:
            glGetIntegerv(pname, v);
            break;
    }
}

const GLubyte* OV_glGetStringi(GLenum pname, int index) {
    switch (pname) {
        case GL_EXTENSIONS:
            if (index < 1 || index > extensionMap.size()) return nullptr;
            return (GLubyte*) *std::next(extensionMap.begin(), index);

        default:
            return glGetStringi(pname, index);
    }
}