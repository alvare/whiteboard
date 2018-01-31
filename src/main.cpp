#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>

std::string vertex_shader = R"glsl(
#version 120;
#extension GL_EXT_geometry_shader4 : enable

int i;
vec4 vertex;

void main() {
    // transform the vertex position
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    for(i = 0; i < gl_VerticesIn; i++) {
      gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[i];
      EmitVertex();
    }

    // transform the texture coordinates
    //gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

    // forward the vertex color
    gl_FrontColor = gl_Color;
}
)glsl";

int main() {
    using PointVec = std::vector<sf::Vertex>;

    // create the window
    sf::ContextSettings settings;
    settings.attributeFlags = sf::ContextSettings::Attribute::Core;
    settings.minorVersion = 2;
    settings.majorVersion = 3;

    sf::RenderWindow window(sf::VideoMode(800, 600), "My window", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);
    auto l = window.getSettings();
    std::cout << l.majorVersion << ',' << l.minorVersion << std::endl;
    //sf::View view = window.getView();
    //view.zoom(2);
    //window.setView(view);

    if (!sf::Shader::isGeometryAvailable()) {
        return 2;
    }

    sf::Shader shader;
    if (!shader.loadFromMemory(vertex_shader, sf::Shader::Geometry)) {
        return 1;
    }

    // empty vec of points to draw
    std::unique_ptr<PointVec> points = std::make_unique<PointVec>(1, sf::Vertex());

    // mouse position
    sf::Vector2i prev_pos;

    // run the program as long as the window is open
    while (window.isOpen()) {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i pos = sf::Mouse::getPosition(window);
            if (pos != prev_pos) {
                points->push_back(sf::Vertex(sf::Vector2f(pos)));
                prev_pos = pos;
                std::cout << points->size() << '\n';
            }
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw everything
        window.draw(points->data(), points->size(), sf::LineStrip, &shader);

        // end the current frame
        window.display();
    }

    return 0;
}
