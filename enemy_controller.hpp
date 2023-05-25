#pragma once

class EnemyController {
private:

	std::vector<EnemyCharacter*> activeEnemies;

	std::map<unsigned int, std::string> enemyContainer;

	std::string bossAnimPath;

	EnemyCharacter* boss;

	int convertDirectoryNameToInt(const std::string& directoryName) 
	{
		try {
			return std::stoi(directoryName);
		}
		catch (const std::exception& e) {
			// Handle conversion error
			throw std::runtime_error("Failed to convert directory name to int: " + std::string(e.what()));
		}
	}

	void createEnemies(unsigned int room_capacity, const Room* room)
	{
		unsigned int index = 0;
		
		while (index <= room_capacity)
		{
			unsigned int enemyTier = getRandomInRange(0, 100);

			if (enemyTier < tier1EnemyChance) {
				activeEnemies.push_back(new EnemyCharacter(enemyContainer.at(1) + "/idle", enemyContainer.at(1) + "/run", tier1EnemyMvSpeed, tier1EnemyHP, 5, 20));
				index += 1;
			}
			else if (enemyTier < tier2EnemyChance + tier1EnemyChance) {
				activeEnemies.push_back(new EnemyCharacter(enemyContainer.at(2) + "/idle", enemyContainer.at(2) + "/run", tier2EnemyMvSpeed, tier2EnemyHP, 5, 20));
				index += 2;
			}
			else if (enemyTier < tier3EnemyChance + tier2EnemyChance + tier1EnemyChance) {
				activeEnemies.push_back(new EnemyCharacter(enemyContainer.at(3) + "/idle", enemyContainer.at(3) + "/run", tier3EnemyMvSpeed, tier3EnemyHP, 5, 20));
				index += 3;
			}
			else {
				activeEnemies.push_back(new EnemyCharacter(enemyContainer.at(4) + "/idle", enemyContainer.at(4) + "/run", tier3EnemyMvSpeed, tier3EnemyHP, 5, 20));
				index += 3;
			}

			unsigned int x = getRandomInRange(room->getX() + 1.f, room->getX() + room->getWidth() - 1.f) * tileSize.x;
			unsigned int y = getRandomInRange(room->getY() + 1.f, room->getY() + room->getHeight() - 1.f) * tileSize.y;

			activeEnemies.back()->setPosition(sf::Vector2f(x, y));
		}
	}

	void applyDamage(unsigned int weapon_damage, const sf::FloatRect& weapon_bounds) 
	{
		auto it = activeEnemies.begin();

		while (it != activeEnemies.end()) 
		{
			if ((*it)->getGlobalBounds().intersects(weapon_bounds)) 
			{
				(*it)->takeDamage(weapon_damage);

				if ((*it)->getCurrentHP() <= 0) 
				{
					EnemyCharacter* ptr = (*it);
					delete ptr;
					it = activeEnemies.erase(it);
				}
				else {
					++it;
				}
			}
			else {
				++it;
			}
		}

		if (boss->getGlobalBounds().intersects(weapon_bounds)) {
			boss->takeDamage(weapon_damage);
		}
	}

public:

	~EnemyController()
	{
		reset();
	}

	void reset() 
	{
		for (EnemyCharacter* enemy : activeEnemies) {
			delete enemy;
		}
		delete boss;
		activeEnemies.clear();
		enemyContainer.clear();
	}

	void loadEnemies(const std::string& directoryPath)
	{
		reset();

		for (const auto & entry : fs::directory_iterator(directoryPath)) 
		{
			if (fs::is_directory(entry.path())) 
			{
				std::string directoryName = entry.path().filename().string();

				if (directoryName == "boss") {
					for (const auto& entry : fs::directory_iterator(directoryPath + "/" + directoryName))
					{
						if (fs::is_directory(entry.path()))
						{
							bossAnimPath = directoryPath + directoryName + "/" + entry.path().filename().string();
						}
					}
				}
				else {
					unsigned int enemyTier = convertDirectoryNameToInt(directoryName);
					for (const auto& entry : fs::directory_iterator(directoryPath + "/" + directoryName))
					{
						if (fs::is_directory(entry.path()))
						{
							std::string enemyAnimPath = directoryPath + directoryName + "/" + entry.path().filename().string();
							enemyContainer.insert(std::make_pair(enemyTier, enemyAnimPath));
						}
					}
				}
			}
		}
	}

	void spawnEnemies(const std::vector<Room*> rooms, const Room* bossRoom, const Room* spawnRoom)
	{
		for (const Room* room : rooms) 
		{
			if (room == bossRoom) {
				boss = new EnemyCharacter(bossAnimPath + "/idle", bossAnimPath + "/run", 5, 5, 5, 5);
				unsigned int x = getRandomInRange(room->getX() + 1.f, room->getX() + room->getWidth() - 1.f) * tileSize.x;
				unsigned int y = getRandomInRange(room->getY() + 1.f, room->getY() + room->getHeight() - 1.f) * tileSize.y;

				boss->setPosition(sf::Vector2f(x, y));
			}
			else if (room != spawnRoom) {
				unsigned int capacity = (room->getWidth() * room->getHeight()) / tilesPerOneCapacityPoint;
				createEnemies(capacity, room);
			}
		}
	}

	void update(const float& dt, PlayerCharacter* player, CollisionController* cc) 
	{
		for (EnemyCharacter* enemy : activeEnemies) {
			enemy->update(dt, player);
			cc->update(enemy);
		}
		boss->update(dt, player);
		cc->update(boss);
		if (player->canAttack(dt)) {
			applyDamage(player->getWeaponDamage(), player->getWeaponHitbox());
		}
	}

	std::vector<sf::Sprite> getEnemySprites() {
		std::vector<sf::Sprite> v;
		for (EnemyCharacter* enemy : activeEnemies) {
			v.push_back(enemy->getSprite());
		}
		v.push_back(boss->getSprite());
		return v;
	}

	bool bossDefeated() { return boss->getCurrentHP() <= 0; }
};