#include <chrono>
#include <thread>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

// settings
constexpr std::size_t maxIterationCount = 100;
constexpr std::size_t windowSizeX = 1800;
constexpr std::size_t windowSizeY = 1200;
constexpr double zoomValue = 2;

// checks if this point (complex number) included in mandelbrot set
// if yes returns iterationCount
// if no returns number of squarings that it took for this point to leave mandelbrot set
std::size_t mandelbrotCheck(double real, double imaginary) {
    double real0 = real;
    double imaginary0 = imaginary;
    for (int i = 0; i < maxIterationCount; ++i) {
        if (real * real + imaginary * imaginary > 4) return i;

        double newReal = real * real - imaginary * imaginary + real0;
        double newImaginary = 2 * real * imaginary + imaginary0;
        real = newReal;
        imaginary = newImaginary;
    }
    return maxIterationCount;
}

struct color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

//some random colors
constexpr color colors[]{
    color{255, 102, 102},
    color{255, 255, 102},
    color{102, 255, 102},
    color{102, 255, 255},
    color{102, 179, 255},
    color{179, 102, 255},
    color{255, 102, 217},
    color{255, 102, 102},
    color{0, 0, 200},
    color{255, 0, 0}
};

constexpr std::size_t colorCount = std::size(colors);

struct WindowIterator {
    typedef std::pair<std::size_t, std::size_t> pixel;
    pixel state{ 0, 0 };
    WindowIterator begin() {
        return WindowIterator{ { 0,0 } };
    }
    WindowIterator end() {
        return WindowIterator{ { windowSizeX - 1, windowSizeY - 1 } };
    }
    WindowIterator operator++() {
        ++state.first;
        if (state.first >= windowSizeX) {
            state.first = 0;
            ++state.second;
        }
        return *this;
    }
    bool operator!=(const WindowIterator& other) const {
        return state != other.state;
    }
    const pixel& operator*() const {
        return state;
    }
    pixel& operator*() {
        return state;
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(windowSizeX, windowSizeY), "Mandelbrot Set");
    sf::Image img;
    sf::Texture texture;
    sf::Sprite bufferSprite(texture);

    const auto initWindow = [&]() {
        window.setVerticalSyncEnabled(true);
        img.create(windowSizeX, windowSizeY);
        texture.loadFromImage(img);
        bufferSprite.setTexture(texture);
        bufferSprite.setTextureRect(sf::IntRect(0, 0, windowSizeX, windowSizeY));
        bufferSprite.setPosition(0, 0);
        texture.loadFromImage(img);
        window.draw(bufferSprite);
        window.display();
    };

    initWindow();

    //initial bound points on a coordinate plane
    double xLeftBound = -2.0;
    double xRightBound = 1.0;
    double yBottomBound = -1.0;
    double yTopBound = 1.0;

    //size of one pixel
    double pixelLengthX = (xRightBound - xLeftBound) / (windowSizeX - 1);
    double pixelLengthY = (yTopBound - yBottomBound) / (windowSizeY - 1);

    const auto selectColor = [](int iterationCount) {  //different iteration numbers will be presented in different colors
        if (iterationCount == maxIterationCount)
            return sf::Color(0, 0, 0); //points inside mandelbrot set are black
        auto color = colors[iterationCount % colorCount];
        return sf::Color(color.r, color.g, color.b);
    };

    const auto draw = [&]() {
        for (WindowIterator windowPixels; auto & pixel : windowPixels) { //get iterations count for every pixel and color it
            double x = xLeftBound + pixel.first * pixelLengthX;
            double y = yBottomBound + pixel.second * pixelLengthY;
            int iterations = mandelbrotCheck(x, y);
            img.setPixel(pixel.first, pixel.second, selectColor(iterations));
        }
        texture.loadFromImage(img);
        window.draw(bufferSprite);
        window.display();
    };

    draw();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                auto clickX = sf::Mouse::getPosition(window).x; //coordinates of
                auto clickY = sf::Mouse::getPosition(window).y; //a mouse click 

                const auto shiftBounds = [&]() { //we are zooming in so we need to find new point for bottom left corner of image
                    double shiftX = clickX - windowSizeX / (2 * zoomValue);
                    double shiftY = clickY - windowSizeY / (2 * zoomValue);

                    if (shiftX < 0) shiftX = 0;
                    if (shiftY < 0) shiftY = 0;
                    if (shiftX > windowSizeX - windowSizeX / zoomValue) shiftX = windowSizeX - windowSizeX / zoomValue;
                    if (shiftY > windowSizeY - windowSizeY / zoomValue) shiftY = windowSizeY - windowSizeY / zoomValue;

                    xLeftBound += shiftX * pixelLengthX;
                    yBottomBound += shiftY * pixelLengthY;
                };
                shiftBounds();

                pixelLengthX /= zoomValue; //we are zooming in
                pixelLengthY /= zoomValue; //so pixel size should became smaller

                draw();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    window.clear();
    window.display();
}
