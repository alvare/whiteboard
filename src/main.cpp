#include <algorithm>
#include <iostream>
#include <memory>
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>

int thickness = 10;

template<typename T>
inline sf::Vector2f product(const sf::Vector2f& v, T c) {
    return sf::Vector2f(c*v.x, c*v.y);
}

inline float dot(const sf::Vector2f& v1, const sf::Vector2f& v2) {
    return v1.x*v2.x + v1.y*v2.y;
}

inline float length(const sf::Vector2f& v) {
    return std::hypot(v.x, v.y);
}

inline sf::Vector2f normalize(sf::Vector2f v) {
    float l = length(v);
    if (l != 0) v /= l;
    return v;
}

void drawCircles(sf::RenderWindow& window,
                 const std::vector<sf::Vector2f>& points) {

    int radius = thickness / 2;
    size_t size = points.size();
    auto c = sf::CircleShape(radius, 10);

    for (int i = 0; i < size; ++i) {
        int a = ((i-1) < 0) ? 0 : (i-1);
        int b = i;

        const sf::Vector2f& p1 = points[a];
        const sf::Vector2f& p2 = points[b];
        float distance = length(p2 - p1);

        int steps = distance / radius;

        for (int j = 0; j < steps; ++j) {
            c.setPosition(p1 + product(p2 - p1, j/(float)steps));
            window.draw(c);
        }

        c.setPosition(p2);
        window.draw(c);
    }
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
        std::make_unique<std::vector<sf::Vector2f>>();

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
        //drawSegments(window, *points);
        drawCircles(window, *points);

        // end the current frame
        window.display();
    }

    return 0;
}
