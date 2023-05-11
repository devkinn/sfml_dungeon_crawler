#include "includer.hpp"

int main()
{
    // Create SFML window
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(desktop.width, desktop.height), "SFML Window"/*, sf::Style::Fullscreen*/);

    sf::View view(sf::Vector2f(0.f, 0.f), sf::Vector2f(cameraSizeX, cameraSizeY));

    // Set window parameters
    window.setFramerateLimit(144);
    window.setView(view);

    // Create first floor dungeon
    BSPDungeon dungeon1(dungeon1width, dungeon1height);
    dungeon1.generate();

    // Create window background
    BackgroundRenderer background;
    if (!background.load(backgroundTileset, tileSize, 80, 80)) return -1;

    // Create map from dungeon
    MapRenderer map;
    if(!map.load(mapTileset, tileSize, dungeon1.getRooms(), dungeon1.getCorridors())) return -1;

    // Create player
    Player player;
    player.setPosition(dungeon1.getStartingPosition());

    GameTracker gameTracker;
    gameTracker.load(&player, dungeon1.getRooms(), dungeon1.getCorridors());

    // Game loop
    sf::Clock clock;
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        // Handle SFML window events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        view.setCenter(player.getPosition());
        window.setView(view);

        window.clear();

        window.draw(background);
        window.draw(map);
        player.update(dt, window);
        gameTracker.checkForPlayerCollision(window);

        window.display();
    }

    return 0;
}
