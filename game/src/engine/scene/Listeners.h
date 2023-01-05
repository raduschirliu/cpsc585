#include <glm/glm.hpp>

class IWindowSizeListener
{
  public:
    IWindowSizeListener() : window_size_(glm::ivec2(1, 1))
    {
    }

    // Callback for when the window size changes
    virtual void OnWindowSizeChanged(glm::ivec2 size)
    {
    }

    // Called when the window parameters change
    void SetWindowSize(glm::ivec2 size)
    {
        window_size_ = size;
        OnWindowSizeChanged(size);
    }

    // Gets the size of the display window
    glm::ivec2 GetWindowSize() const
    {
        return window_size_;
    }

  private:
    glm::ivec2 window_size_;
};

class IMouseListener
{
  public:
    // Called when GLFW signals that the cursor has moved
    virtual void OnCursorMove(float xpos, float ypos)
    {
    }

    virtual void OnMouseButtonEvent(int button, int action, int mods)
    {
    }
};

class IKeyListener
{
  public:
    // Called when GLFW signals a key event has occured
    // Return false to indicate that the event has not been sunk
    virtual bool OnKeyEvent(int key, int scancode, int action, int mods)
    {
        return false;
    }
};
