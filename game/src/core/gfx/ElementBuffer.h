#pragma once

#include <GL/glew.h>

#include <vector>

#include "core/gfx/GLHandles.h"

class ElementBuffer
{
  public:
    ElementBuffer();

    // Because we're using the VertexBufferHandle to do RAII for the buffer for
    // us and our other types are trivial or provide their own RAII we don't
    // have to provide any specialized functions here. Rule of zero
    //
    // https://en.cppreference.com/w/cpp/language/rule_of_three
    // https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Rc-zero

    void Bind() const;
    void Upload(const std::vector<uint32_t>& indices, GLenum usage);

  private:
    VertexBufferHandle buffer_id_;
};
