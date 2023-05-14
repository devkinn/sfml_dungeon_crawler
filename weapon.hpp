#pragma once

class Weapon {
private:
	unsigned int damage;
	unsigned int attack_speed;
	sf::Sprite sprite;
    sf::Texture texture;

public:

    Weapon(unsigned int _damage, unsigned int _attack_speed, std::string _weaponTexturePath) : damage(_damage), attack_speed(_attack_speed)
    {
        // Load textures for idle animation
        texture.loadFromFile(_weaponTexturePath);

        sprite.setTexture(texture);

        // Set sprite's origin to it's center on x axis
        sf::Vector2f spriteSize(texture.getSize().x, texture.getSize().y);
        sprite.setOrigin(sf::Vector2f(spriteSize.x * 0.5f, spriteSize.y));
    }

    void setPosition(sf::Vector2f position) {
        sprite.setPosition(position);
    }

    sf::Sprite& getSprite() { return sprite; }

    sf::FloatRect getBounds() { return sprite.getGlobalBounds(); }
};