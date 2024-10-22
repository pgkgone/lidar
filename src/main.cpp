#include <visualizer.hpp>

int main() {   
    auto frames = TFrameReader("data").read();
    TFrameVisualizer visualizer(frames);

    if (!visualizer.InitOpenGL()) {
        std::cerr << "Failed to initialize OpenGL\n";
        return -1;
    }

    visualizer.Run();

    return 0;
}
