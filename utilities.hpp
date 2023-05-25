#pragma once

enum GameState { gameLoop, gameEndWin, gameEndLost, exitMenu };

class EndGameScreen {
private:

	sf::RenderWindow& window;
	sf::Font font;
	sf::Text text;

public:

	EndGameScreen(sf::RenderWindow& window) : window(window)
	{
		if (!font.loadFromFile("./assets/fonts/font.ttf")) {
			// Handle font loading error
		}

		text.setFont(font);
		text.setCharacterSize(32);
		text.setScale(.2f, .2f);
		text.setString("You Lost\nPress [ENTER] to restart the game");
	}

	bool handleInput()
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);
	}

	void setText(const std::string& _text)
	{
		text.setString(_text);
	}

	void render(sf::View& viewport)
	{
		sf::Vector2f viewportCenter = viewport.getCenter();
		sf::FloatRect textBounds = text.getLocalBounds();
		text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
		text.setPosition(viewportCenter.x, viewportCenter.y);
		
		window.draw(text);
	}
};

class Game {
private:

	sf::RenderWindow window;
	sf::View view;

	BSPDungeon* currentDungeon;
	MapRenderer* mapRenderer;
	BackgroundRenderer* backgroundRenderer;

	PlayerCharacter* playerCharacter;

	CollisionController* collisionController;

	EnemyController* enemyController;

	WeaponContainer* weaponPool;
	WeaponContainer* weaponsOnGround;
	ChestContainer* chestContainer;

	EndGameScreen* endGameScreen;

	sf::Clock frameClock;

	bool generateLevel;
	unsigned int currentLevel;

	GameState gameState;

public:

	Game(unsigned int window_width, unsigned int window_height) :
		window(sf::VideoMode(window_width, window_height), "SFML Window", sf::Style::Fullscreen), view(sf::Vector2f(0.f, 0.f), sf::Vector2f(cameraSizeX, cameraSizeY)),
		currentDungeon(nullptr), mapRenderer(nullptr), backgroundRenderer(nullptr), playerCharacter(nullptr), collisionController(nullptr), enemyController(nullptr), weaponPool(nullptr), endGameScreen(nullptr)
	{
		window.setFramerateLimit(defaultFPS);
		window.setView(view);

		endGameScreen = new EndGameScreen(window);

		restartGame();
	}

	~Game()
	{
		delete currentDungeon;
		delete mapRenderer;
		delete backgroundRenderer;
		delete playerCharacter;
		delete collisionController;
		delete enemyController;
		delete endGameScreen;
		delete weaponPool;
		delete weaponsOnGround;
		delete chestContainer;
	}

	void generateDungeon(unsigned int width, unsigned int height, std::string enemyTexturePath)
	{
		delete currentDungeon;
		currentDungeon = new BSPDungeon(width, height);
		currentDungeon->generate();

		delete collisionController;
		collisionController = new CollisionController;
		collisionController->load(currentDungeon->getRooms(), currentDungeon->getCorridors());

		delete enemyController;
		enemyController = new EnemyController;
		enemyController->loadEnemies(enemyTexturePath);
		enemyController->spawnEnemies(currentDungeon->getRooms(), currentDungeon->getBossRoom(), currentDungeon->getSpawnRoom());

		chestContainer->spawnChests(currentDungeon->getChestRooms());

		playerCharacter->setPosition(currentDungeon->getStartingPosition());
	}

	void createMap(std::string dungeonTileset, std::string background1Tileset, sf::Vector2u tileSize, unsigned int backgroundWidth, unsigned int backgroundHeight)
	{
		delete backgroundRenderer;
		backgroundRenderer = new BackgroundRenderer;
		backgroundRenderer->load(background1Tileset, tileSize, backgroundWidth, backgroundHeight);

		delete mapRenderer;
		mapRenderer = new MapRenderer;
		mapRenderer->load(dungeonTileset, tileSize, currentDungeon->getRooms(), currentDungeon->getCorridors());
	}

	void createPlayer(std::string idleAnimPath, std::string runAnimPath, unsigned int mv_speed, unsigned int HP)
	{
		delete playerCharacter;
		playerCharacter = new PlayerCharacter(idleAnimPath, runAnimPath, mv_speed, HP);
		playerCharacter->equipWeapon(new Weapon(mediumWeaponDamage, mediumWeaponAttackCooldown, "./assets/weapons/medium/weapon_baton_with_spikes.png"));
	}

	void drawSprites()
	{
		window.draw(*backgroundRenderer);
		window.draw(*mapRenderer);

		std::vector<sf::Sprite> enemySprites = enemyController->getEnemySprites();
		for (sf::Sprite& sprite : enemySprites) {
			window.draw(sprite);
		}

		std::vector<sf::Sprite> chestSprites = chestContainer->getChestSprites();
		for (sf::Sprite& sprite : chestSprites) {
			window.draw(sprite);
		}

		std::vector<sf::Sprite> weaponSprites = weaponsOnGround->getWeaponSprites();
		for (sf::Sprite& sprite : weaponSprites) {
			window.draw(sprite);
		}

		window.draw(playerCharacter->getSprite());
		window.draw(playerCharacter->getWeaponSprite());
		window.draw(playerCharacter->getHealthbar());
	}

	void updateModules(const float& dt)
	{
		sf::Vector2f healthBarPosition = view.getCenter() - view.getSize() / 2.f;
		healthBarPosition.x += 10.f;
		healthBarPosition.y += 10.f;
		playerCharacter->update(dt, healthBarPosition);
		collisionController->update(playerCharacter);

		chestContainer->update(dt, playerCharacter);
		weaponsOnGround->update(playerCharacter);

		enemyController->update(dt, playerCharacter, collisionController);
	}

	void gameStateUpdater() 
	{
		if (gameState == exitMenu) return;

		if (playerCharacter->getCurrentHP() <= 0) {
			gameState = gameEndLost;
			return;
		}

		if (enemyController != nullptr && enemyController->bossDefeated()) generateLevel = true;

		if (generateLevel || currentDungeon == nullptr) {
			if (currentLevel == 1) {
				generateDungeon(dungeon1width, dungeon1height, dungeon1EnemiesDir);
				createMap(dungeon1Tileset, background1Tileset, tileSize, 60, 80);
			}
			else if (currentLevel == 2) {
				generateDungeon(dungeon2width, dungeon2height, dungeon2EnemiesDir);
				createMap(dungeon2Tileset, background2Tileset, tileSize, 80, 100);
			}
			else if (currentLevel == 3) {
				generateDungeon(dungeon3width, dungeon3height, dungeon3EnemiesDir);
				createMap(dungeon3Tileset, background3Tileset, tileSize, 100, 120);
			}
			else {
				gameState = gameEndWin;
				return;
			}
			currentLevel++;
			generateLevel = false;
		}
	}

	void restartGame()
	{
		generateLevel = true;
		currentLevel = 1;
		gameState = gameLoop;

		delete weaponPool;
		weaponPool = new WeaponContainer;
		weaponPool->load(weaponsDir);

		delete weaponsOnGround;
		weaponsOnGround = new WeaponContainer;

		delete chestContainer;
		chestContainer = new ChestContainer(weaponPool, weaponsOnGround);

		createPlayer(knightIdleAnim, knightRunAnim, 8, 6);
	}

	void renderGame(const float& dt)
	{
		if (gameState == gameLoop)
		{
			view.setCenter(playerCharacter->getPosition());
			window.setView(view);

			updateModules(dt);
			drawSprites();
		}
		else if (gameState == gameEndLost)
		{
			endGameScreen->setText("You Lost!\nPress [ENTER] to restart the game\nor [ESC] to exit the game");
			endGameScreen->render(view);
			if (endGameScreen->handleInput())
			{
				restartGame();
			}
		}
		else if (gameState == gameEndWin) 
		{
			endGameScreen->setText("Congrats, You won!\nPress [ENTER] to restart the game\nor [ESC] to exit the game");
			endGameScreen->render(view);
			if (endGameScreen->handleInput())
			{
				restartGame();
			}
		}
		else if (gameState == exitMenu)
		{
			endGameScreen->setText("Exit the game?\nPress [ENTER] to confirm\nor [LMB] to keep playing");
			endGameScreen->render(view);

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) gameState = gameLoop;
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) window.close();
		}
	}

	void startGame()
	{
		while (window.isOpen())
		{
			float dt = frameClock.restart().asSeconds();

			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();
				}

				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
					if (gameState == gameLoop) gameState = exitMenu;
				}
			}

			gameStateUpdater();

			view.setCenter(playerCharacter->getPosition());
			window.setView(view);

			window.clear();

			renderGame(dt);

			window.display();
		}
	}
};