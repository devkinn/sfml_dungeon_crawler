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



    EnemyController enemyController;
    enemyController.loadEnemies(dungeon1EnemiesPath);
    enemyController.spawnEnemies(dungeon1.getRooms(), dungeon1.getBossRoom());
    

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

        sf::Vector2f healthBarPosition = view.getCenter() - view.getSize() / 2.f;
        healthBarPosition.x += 10.f;
        healthBarPosition.y += 10.f;


        player.update(dt, healthBarPosition);

        window.draw(player.getSprite());
        window.draw(player.getHealthbar());
        window.draw(player.getWeaponSprite());



        enemyController.update(dt, &player);

        std::vector<sf::Sprite> enemySprites = enemyController.getEnemySprites();

        for (sf::Sprite& sprite : enemySprites) {
            window.draw(sprite);
        }


        collisionController.update(&player);

        window.display();
    }

    delete sword;

    return 0;
}
