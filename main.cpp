#include <Renderer.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main() {
    Renderer engine(1920, 1080, "opengl");

    int err = engine.setup();
    if (err != 0) {
        std::cout << "ERROR::RENDERER::SETUP" << std::endl;
        return -1;
    }
    engine.render_loop();

    return 0;
}