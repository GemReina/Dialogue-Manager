#include <SFML/Graphics.hpp>
#include <optional> // Needed for std::optional

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "SFML 3.x window");
    sf::CircleShape shape(200.f);
    shape.setFillColor(sf::Color(204, 77, 5));
    shape.setPosition({ 200.f, 200.f }); // Use sf::Vector2f

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color(18, 33, 43));
        window.draw(shape);
        window.display();
    }
}
