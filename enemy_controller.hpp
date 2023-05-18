#pragma once

class EnemyController {
private:

	std::vector<EnemyCharacter*> activeEnemies;

	std::map<unsigned int, std::string> enemyContainer;

	std::string bossAnimPath;

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
				activeEnemies.push_back(new EnemyCharacter(enemyContainer.at(1) + "/idle", enemyContainer.at(1) + "/run", 5, 5, 5, 5, 5));
				index += 1;
			}
			else if (enemyTier < tier2EnemyChance + tier1EnemyChance) {
				activeEnemies.push_back(new EnemyCharacter(enemyContainer.at(2) + "/idle", enemyContainer.at(2) + "/run", 5, 5, 5, 5, 5));
				index += 2;
			}
			else if (enemyTier < tier3EnemyChance + tier2EnemyChance + tier1EnemyChance) {
				activeEnemies.push_back(new EnemyCharacter(enemyContainer.at(3) + "/idle", enemyContainer.at(3) + "/run", 5, 5, 5, 5, 5));
				index += 3;
			}
			else {
				activeEnemies.push_back(new EnemyCharacter(enemyContainer.at(4) + "/idle", enemyContainer.at(4) + "/run", 5, 5, 5, 5, 5));
				index += 3;
			}

			unsigned int x = getRandomInRange(room->getX() + 1.f, room->getX() + room->getWidth() - 1.f) * tileSize.x;
			unsigned int y = getRandomInRange(room->getY() + 1.f, room->getY() + room->getHeight() - 1.f) * tileSize.y;

			activeEnemies.back()->setPosition(sf::Vector2f(x, y));
		}
	}

public:

	void loadEnemies(const std::string& directoryPath)
	{
		enemyContainer.clear();

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

	void spawnEnemies(const std::vector<Room*> rooms, const Room* bossRoom)
	{
		for (const Room* room : rooms) 
		{
			if (room == bossRoom) {
				//activeEnemies.push_back(EnemyCharacter(bossAnimPath + "/idle", bossAnimPath + "/run", 5, 5, 5, 5, 5));
				//unsigned int x = getRandomInRange(room->getX() + 1.f, room->getX() + room->getWidth() - 1.f) * tileSize.x;
				//unsigned int y = getRandomInRange(room->getY() + 1.f, room->getY() + room->getHeight() - 1.f) * tileSize.y;

				//activeEnemies.back().setPosition(sf::Vector2f(x, y));
			}
			else {
				unsigned int capacity = (room->getWidth() * room->getHeight()) / tilesPerOneCapacityPoint;
				createEnemies(capacity, room);
			}
		}
	}

	void update(const float& dt, PlayerCharacter* player) 
	{
		for (EnemyCharacter* enemy : activeEnemies) {
			enemy->update(dt, player);
		}
	}

	std::vector<sf::Sprite> getEnemySprites() {
		std::vector<sf::Sprite> v;
		for (EnemyCharacter* enemy : activeEnemies) {
			v.push_back(enemy->getSprite());
		}
		return v;
	}
};