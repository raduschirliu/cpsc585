#pragma once

#include <GL/glew.h>

namespace GLDebug
{

void debugOutputHandler(GLenum source, GLenum type, GLuint id, GLenum severity,
                        GLsizei, const GLchar *message, const void *user_param);

void enable();

}  // namespace GLDebug
