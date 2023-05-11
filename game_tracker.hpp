#pragma once

class GameTracker {
private:
	std::vector<sf::FloatRect*> roomBounds;
	std::vector<sf::FloatRect*> corridorBounds;
	sf::FloatRect* currentRoom = nullptr;
	sf::FloatRect* verticalNeighbourRoom = nullptr;
	sf::FloatRect* horizontalNeighbourRoom = nullptr;

	Player* player;

	sf::FloatRect playerBounds;
	sf::Vector2f playerPosition;

	sf::Vector2f p1;
	sf::Vector2f p2;
	sf::Vector2f p3;
	sf::Vector2f p4;

	void restrictMovementInBounds(sf::FloatRect* verticalBounds, sf::FloatRect* horizontalBounds) {
		playerPosition.x = std::max(horizontalBounds->left + playerBounds.width / 2, std::min(playerPosition.x, horizontalBounds->left + horizontalBounds->width - playerBounds.width / 2));
		playerPosition.y = std::max(verticalBounds->top + 16, std::min(playerPosition.y, verticalBounds->top + verticalBounds->height));
		player->setPosition(sf::Vector2f(playerPosition.x, playerPosition.y));
	}

	void restrictVerticalMovement(sf::FloatRect* verticalBounds) {
		playerPosition.y = std::max(verticalBounds->top + 16, std::min(playerPosition.y, verticalBounds->top + verticalBounds->height));
		player->setPosition(sf::Vector2f(playerPosition.x, playerPosition.y));
	}

	void restrictHorizontalMovement(sf::FloatRect* horizontalBounds) {
		playerPosition.x = std::max(horizontalBounds->left + playerBounds.width / 2, std::min(playerPosition.x, horizontalBounds->left + horizontalBounds->width - playerBounds.width / 2));
		player->setPosition(sf::Vector2f(playerPosition.x, playerPosition.y));
	}

	sf::FloatRect* findCurrentRoom(sf::Vector2f& tr, sf::Vector2f& tl, sf::Vector2f& br, sf::Vector2f& bl) {
		if (currentRoom != nullptr) {
			if (currentRoom->contains(tr) || currentRoom->contains(tl) || currentRoom->contains(br) || currentRoom->contains(bl)) return currentRoom;
		}
		for (sf::FloatRect* room : roomBounds) {
			if (room->contains(tr) || room->contains(tl) || room->contains(br) || room->contains(bl)) return room;
		}
		for (sf::FloatRect* corridor : corridorBounds) {
			if (corridor->contains(tr) || corridor->contains(tl) || corridor->contains(br) || corridor->contains(bl)) return corridor;
		}
		currentRoom = nullptr;
	}

	sf::FloatRect* findNeighbourRoom(sf::Vector2f& p1, sf::Vector2f& p2, sf::FloatRect* neighbourRoom) {
		if (neighbourRoom != nullptr && neighbourRoom != currentRoom) {
			if (neighbourRoom->contains(p1) && neighbourRoom->contains(p2)) return neighbourRoom;
		}
		for (sf::FloatRect* room : roomBounds) {
			if (room->contains(p1) && room->contains(p2)) {
				if (room != currentRoom) return room;
			}
		}
		for (sf::FloatRect* corridor : corridorBounds) {
			if (corridor->contains(p1) && corridor->contains(p2)) {
				if (corridor != currentRoom) return corridor;
			}
		}
		return nullptr;
	}

	sf::FloatRect* findCurrentRoom2(sf::Vector2f& tr, sf::Vector2f& tl, sf::Vector2f& br, sf::Vector2f& bl) {
		if (currentRoom != nullptr) {
			if (currentRoom->contains(tr) || currentRoom->contains(tl) || currentRoom->contains(br) || currentRoom->contains(bl)) return currentRoom;
		}
		for (sf::FloatRect* corridor : corridorBounds) {
			if (corridor->contains(tr) || corridor->contains(tl) || corridor->contains(br) || corridor->contains(bl)) return corridor;
		}
		for (sf::FloatRect* room : roomBounds) {
			if (room->contains(tr) || room->contains(tl) || room->contains(br) || room->contains(bl)) return room;
		}
		currentRoom = nullptr;
	}

	void findNeighbourRoom2(sf::Vector2f& tr, sf::Vector2f& tl, sf::Vector2f& br, sf::Vector2f& bl) {
		for (sf::FloatRect* room : roomBounds) {
			if ((room->contains(tr) && room->contains(br)) || (room->contains(tl) && room->contains(bl))) {
				if (room != currentRoom) {
					horizontalNeighbourRoom = room;
					restrictVerticalMovement(currentRoom);
				}
			}
			if ((room->contains(tr) && room->contains(tl)) || (room->contains(br) && room->contains(bl))) {
				if (room != currentRoom) {
					horizontalNeighbourRoom = room;
					restrictHorizontalMovement(currentRoom);
				}
			}
		}
		for (sf::FloatRect* corridor : corridorBounds) {
			if ((corridor->contains(tr) && corridor->contains(br)) || (corridor->contains(tl) && corridor->contains(bl))) {
				if (corridor != currentRoom) {
					horizontalNeighbourRoom = corridor;
					restrictVerticalMovement(currentRoom);
				}
			}
			if ((corridor->contains(tr) && corridor->contains(tl)) || (corridor->contains(br) && corridor->contains(bl))) {
				if (corridor != currentRoom) {
					horizontalNeighbourRoom = corridor;
					restrictHorizontalMovement(currentRoom);
				}
			}
		}
		if (horizontalNeighbourRoom == nullptr) {
			restrictMovementInBounds(currentRoom, currentRoom);
		}
		else {
			//horizontalNeighbourRoom = nullptr;
		}
	}

public:
	~GameTracker() {
		for (sf::FloatRect* room : roomBounds) delete room;
		for (sf::FloatRect* corridor : corridorBounds) delete corridor;
	}
	void load(Player* _player, const std::vector<Room*> rooms, const std::vector<Corridor*> corridors) 
	{
		player = _player;
		playerBounds = player->getGlobalBounds();

		currentRoom = nullptr;
		horizontalNeighbourRoom = nullptr;
		verticalNeighbourRoom = nullptr;

		// Create bounds for rooms and corridors
		roomBounds.clear();
		for (const Room* room : rooms) {
			const sf::Vector2f position(room->x * tileSize.x, room->y * tileSize.y);
			const sf::Vector2f size(room->width * tileSize.x, room->height * tileSize.y);
			roomBounds.push_back(new sf::FloatRect(position, size));
		}

		corridorBounds.clear();
		for (const Corridor* corridor : corridors) {
			const sf::Vector2f position(corridor->x1 * tileSize.x, corridor->y1 * tileSize.y);
			const sf::Vector2f size(corridor->width * tileSize.x, corridor->height * tileSize.y);
			corridorBounds.push_back(new sf::FloatRect(position, size));
		}
	}

	void checkForPlayerCollision(sf::RenderWindow& window) 
	{
		// Get current player position
		playerPosition = player->getPosition();
		playerBounds = player->getGlobalBounds();

		// Define points for collision check
		sf::Vector2f topLeftCorner(playerPosition.x - playerBounds.width / 2, playerPosition.y - 16);
		sf::Vector2f topRightCorner(playerPosition.x + playerBounds.width / 2, playerPosition.y - 16);
		sf::Vector2f bottomLeftCorner(playerPosition.x - playerBounds.width / 2, playerPosition.y);
		sf::Vector2f bottomRightCorner(playerPosition.x + playerBounds.width / 2, playerPosition.y);

		currentRoom = findCurrentRoom2(topRightCorner, topLeftCorner, bottomRightCorner, bottomLeftCorner);
		findNeighbourRoom2(topRightCorner, topLeftCorner, bottomRightCorner, bottomLeftCorner);




		//sf::Vector2f movementDirection = player->getMovementVector();

		/*if (movementDirection.x > 0) {
			p1 = sf::Vector2f(topRightCorner.x + 6, topRightCorner.y);
			p2 = sf::Vector2f(bottomRightCorner.x + 6, bottomRightCorner.y);
		}
		else if (movementDirection.x < 0){
			p1 = sf::Vector2f(topLeftCorner.x - 6, topLeftCorner.y);
			p2 = sf::Vector2f(bottomLeftCorner.x - 6, bottomLeftCorner.y);
		}
		else {
			p1.y = topLeftCorner.y;
			p2.y = bottomLeftCorner.y;
		}

		if (movementDirection.y > 0) {
			p3 = sf::Vector2f(bottomRightCorner.x, bottomRightCorner.y + 6);
			p4 = sf::Vector2f(bottomLeftCorner.x, bottomLeftCorner.y + 6);
		}
		else if (movementDirection.y < 0) {
			p3 = sf::Vector2f(topRightCorner.x, topRightCorner.y - 6);
			p4 = sf::Vector2f(topLeftCorner.x, topLeftCorner.y - 6);
		}
		else {
			p3.x = topRightCorner.x;
			p4.x = topLeftCorner.x;
		}

		horizontalNeighbourRoom = findNeighbourRoom(p1, p2, horizontalNeighbourRoom);
		verticalNeighbourRoom = findNeighbourRoom(p3, p4, verticalNeighbourRoom);

		currentRoom = findCurrentRoom(topRightCorner, topLeftCorner, bottomRightCorner, bottomLeftCorner);


		sf::RectangleShape rect1(sf::Vector2f(1.f, 1.f));
		rect1.setPosition(sf::Vector2f(p3.x, p3.y));
		rect1.setFillColor(sf::Color::Red);
		window.draw(rect1);

		sf::RectangleShape rect2(sf::Vector2f(1.f, 1.f));
		rect2.setPosition(sf::Vector2f(p4.x, p4.y));
		rect2.setFillColor(sf::Color::Red);
		window.draw(rect2);

		sf::RectangleShape rect3(sf::Vector2f(1.f, 1.f));
		rect3.setPosition(sf::Vector2f(p1.x, p1.y));
		rect3.setFillColor(sf::Color::Red);
		window.draw(rect3);

		sf::RectangleShape rect4(sf::Vector2f(1.f, 1.f));
		rect4.setPosition(sf::Vector2f(p2.x, p2.y));
		rect4.setFillColor(sf::Color::Red);
		window.draw(rect4);
		*/

		if (currentRoom != nullptr) {
			sf::RectangleShape room_outline(sf::Vector2f(currentRoom->width, currentRoom->height));
			room_outline.setPosition(sf::Vector2f(currentRoom->left, currentRoom->top));
			room_outline.setOutlineColor(sf::Color::Green);
			room_outline.setFillColor(sf::Color::Transparent);
			room_outline.setOutlineThickness(1.f);
			window.draw(room_outline);
		}

		
		if (horizontalNeighbourRoom != nullptr) {
			sf::RectangleShape corridor_outline(sf::Vector2f(horizontalNeighbourRoom->width, horizontalNeighbourRoom->height));
			corridor_outline.setPosition(sf::Vector2f(horizontalNeighbourRoom->left, horizontalNeighbourRoom->top));
			corridor_outline.setOutlineColor(sf::Color::Blue);
			corridor_outline.setFillColor(sf::Color::Transparent);
			corridor_outline.setOutlineThickness(1.f);
			window.draw(corridor_outline);
		}
		/*
		if (verticalNeighbourRoom != nullptr) {
			sf::RectangleShape corridor_outline(sf::Vector2f(verticalNeighbourRoom->width, verticalNeighbourRoom->height));
			corridor_outline.setPosition(sf::Vector2f(verticalNeighbourRoom->left, verticalNeighbourRoom->top));
			corridor_outline.setOutlineColor(sf::Color::Cyan);
			corridor_outline.setFillColor(sf::Color::Transparent);
			corridor_outline.setOutlineThickness(2.f);
			window.draw(corridor_outline);
		}*/
	}
};