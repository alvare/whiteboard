#include <algorithm>
#include <iostream>
#include <memory>
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>

int thickness = 10;

inline float dot(const sf::Vector2f& v1, const sf::Vector2f& v2) {
    return v1.x*v2.x + v1.y*v2.y;
}

inline sf::Vector2f normalize(sf::Vector2f source) {
    float length = std::hypot(source.x, source.y);
    if (length != 0) source /= length;
    return source;
}

void drawSegment(sf::RenderWindow& window,
                 const sf::Vector2f& p0,
                 const sf::Vector2f& p1,
                 const sf::Vector2f& p2,
                 const sf::Vector2f& p3) {
    // skip if zero length
    if(p1 == p2) return;

    // 1) define the line between the two points
    sf::Vector2f line = normalize(p2 - p1);

    // 2) find the normal vector of this line
    sf::Vector2f normal = normalize(sf::Vector2f(-line.y, line.x));

    // 3) find the tangent vector at both the end points:
    //      - if there are no segments before or after this one, use the line itself
    //      - otherwise, add the two normalized lines and average them by normalizing again
    sf::Vector2f tangent1 = (p0 == p1) ? line : normalize(normalize(p1-p0) + line);
    sf::Vector2f tangent2 = (p2 == p3) ? line : normalize(normalize(p3-p2) + line);

    // 4) find the miter line, which is the normal of the tangent
    sf::Vector2f miter1 = sf::Vector2f(-tangent1.y, tangent1.x);
    sf::Vector2f miter2 = sf::Vector2f(-tangent2.y, tangent2.x);

    // 5) find length of miter by projecting the miter onto the normal,
    //    take the length of the projection, invert it and multiply it by the thickness:
    //    length = thickness * ( 1 / |normal|.|miter| )
    float length1 = thickness / dot(normal, miter1);
    float length2 = thickness / dot(normal, miter2);

    std::vector<sf::Vertex> ps =
        { sf::Vertex(p1 - length1 * miter1, sf::Color::Red),
          sf::Vertex(p1 + length1 * miter1, sf::Color::Red),
          sf::Vertex(p2 - length2 * miter2, sf::Color::Red),
          sf::Vertex(p2 + length2 * miter2, sf::Color::Red) };

    window.draw(ps.data(), 4, sf::TriangleStrip);
}

void drawSegments(sf::RenderWindow& window,
                  const std::vector<sf::Vector2f>& points) {
    size_t size = points.size();
    for (int i = 0; i < size; ++i) {
        int a = ((i-1) < 0) ? 0 : (i-1);
        int b = i;
        int c = ((i+1) >= size) ? size-1 : (i+1);
        int d = ((i+2) >= size) ? size-1 : (i+2);

        drawSegment(window, points[a], points[b], points[c], points[d]);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Maths");
    window.setVerticalSyncEnabled(true);

    // empty vec of points to draw
    std::unique_ptr<std::vector<sf::Vector2f>> points =
        std::make_unique<std::vector<sf::Vector2f>>(1, sf::Vector2f());

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
                points->push_back(sf::Vector2f(pos));
                prev_pos = pos;
            }
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw everything
        drawSegments(window, *points);

        // end the current frame
        window.display();
    }

    return 0;
}
