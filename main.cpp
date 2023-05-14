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

    // Setup game
    GameStatus game_status = level1;
    bool dungeon_generated = false;

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
    PlayerCharacter player(wizardIdleAnim, wizardRunAnim, 8.f, 6);
    player.setPosition(dungeon1.getStartingPosition());


    CollisionController collisionController;
    collisionController.load(dungeon1.getRooms(), dungeon1.getCorridors());


    Weapon* sword = new Weapon(10, 10, "./assets/weapons/weapon_katana.png");
    player.equipWeapon(sword);


    // Game loop
    sf::Clock clock;


    EnemyCharacter skeleton(skeletonIdleAnim, skeletonRunAnim, 5.f, 6, 3, 5);
    skeleton.setPosition(player.getPosition());

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

        player.update(dt);
        skeleton.update(dt, player.getPosition(), player.getGlobalBounds());

        window.draw(player.getSprite());
        window.draw(skeleton.getSprite());
        window.draw(player.getWeaponSprite());


        collisionController.update(&player);

        window.display();
    }

    delete sword;

    return 0;
}
