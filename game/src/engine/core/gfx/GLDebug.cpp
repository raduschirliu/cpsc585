#include "engine/core/gfx/GLDebug.h"

#include <regex>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"

void GLDebug::debugOutputHandler(GLenum source, GLenum type, GLuint id,
                                 GLenum severity, GLsizei,
                                 const GLchar *message, const void *user_param)
{
    UNUSED(user_param);

    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    {
        return;
    }

    std::string source_name, type_name;

    // clang-format off
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             source_name = "API";                break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_name = "Window System";      break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: source_name = "Shader Compiler";    break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     source_name = "Third Party";        break;
        case GL_DEBUG_SOURCE_APPLICATION:     source_name = "Application";        break;
        case GL_DEBUG_SOURCE_OTHER:           source_name = "Other";              break;
    }

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               type_name = "Error";                  break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_name = "Deprecated Behaviour";   break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_name = "Undefined Behaviour";    break;
        case GL_DEBUG_TYPE_PORTABILITY:         type_name = "Portability";            break;
        case GL_DEBUG_TYPE_PERFORMANCE:         type_name = "Performance";            break;
        case GL_DEBUG_TYPE_MARKER:              type_name = "Marker";                 break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          type_name = "Push Group";             break;
        case GL_DEBUG_TYPE_POP_GROUP:           type_name = "Pop Group";              break;
        case GL_DEBUG_TYPE_OTHER:               type_name = "Other";                  break;
    }
    // clang-format on

    std::string format = "[OPENGL] [{}] {} #{} -- {}: {}";
    std::string message_str = message;
    message_str =
        std::regex_replace(message_str, std::regex("^\\s+|\\s+$"), "$1");
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            debug::LogError(format.c_str(), source_name, "high", id, type_name,
                            message_str);
            DEBUG_BREAKPOINT();
            break;

        case GL_DEBUG_SEVERITY_MEDIUM:
            debug::LogWarn(format.c_str(), source_name, "medium", id, type_name,
                           message_str);
            break;

        case GL_DEBUG_SEVERITY_LOW:
            debug::LogInfo(format.c_str(), source_name, "low", id, type_name,
                           message_str);
            break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
            debug::LogDebug(format.c_str(), source_name, "debug", id, type_name,
                            message_str);
            break;

        default:
            debug::LogInfo(format.c_str(), source_name, "unknown", id,
                           type_name, message_str);
            break;
    }
}

void GLDebug::enable()
{
    GLint flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDebug::debugOutputHandler, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                              nullptr, GL_TRUE);
        debug::LogInfo("Enabling debug mode for opengl");
    }
    else
    {
        debug::LogWarn("Unable to enable debug mode for opengl");
    }
}
