#pragma once

class PlayerCharacter;

class Weapon {
private:

	unsigned int damage;
	float attack_cooldown;

	sf::Sprite sprite;
    sf::Texture texture;

    float elapsedAnimationTime = 0.0f;
    bool animationComplete = false;

    const float originalRotation = 0.0f;
    float targetRotation = 45.0f;

public:

    Weapon(unsigned int _damage, float _attack_cooldown, std::string _weaponTexturePath) : damage(_damage), attack_cooldown(_attack_cooldown)
    {
        // Load textures for idle animation
        texture.loadFromFile(_weaponTexturePath);

        sprite.setTexture(texture);

        // Set sprite's origin to it's center on x axis
        sf::Vector2f spriteSize(texture.getSize().x, texture.getSize().y);
        sprite.setOrigin(sf::Vector2f(spriteSize.x * 0.5f, spriteSize.y));
    }

    void setPosition(sf::Vector2f position) 
    {
        sprite.setPosition(position);
    }

    sf::Sprite& getSprite() { return sprite; }

    sf::FloatRect getBounds() { return sprite.getGlobalBounds(); }

    unsigned int getDamage() const { return damage; }

    float getAttackCooldown() const { return attack_cooldown; }

    sf::FloatRect getHitbox() const
    {
        sf::FloatRect defaultBounds = sprite.getGlobalBounds();
        return sf::FloatRect(defaultBounds.left, defaultBounds.top, defaultBounds.height, defaultBounds.height);
    }

    void resetAnim() {
        animationComplete = false;
        elapsedAnimationTime = 0.0f;
    }

    void playAttackAnimation(const float& deltaTime)
    {
        if (!animationComplete) {
            elapsedAnimationTime += deltaTime;

            if (elapsedAnimationTime >= attack_cooldown) {
                elapsedAnimationTime = attack_cooldown;
                animationComplete = true;
            }

            // Calculate the interpolation factor (0.0 to 1.0)
            float t = elapsedAnimationTime / attack_cooldown;

            // Interpolate the position and rotation
            float interpolatedRotation = originalRotation + t * targetRotation;

            // Update the sprite's position and rotation
            //sprite.setPosition(interpolatedPosition);
            sprite.setRotation(interpolatedRotation);
        }
    }

    void setTargetRotation(const float rotation) { targetRotation = rotation; }
};

class WeaponContainer {
private:

    std::vector<Weapon*> activeWeapons;

public:

    ~WeaponContainer()
    {
        reset();
    }

    void reset() {
        for (Weapon* weapon : activeWeapons) {
            delete weapon;
        }
        activeWeapons.clear();
    }

    Weapon* removeByIndex(int index) 
    {
        Weapon* ptr = activeWeapons[index];
        activeWeapons.erase(activeWeapons.begin() + index);
        return ptr;
    }

    unsigned int getCurrentSize() const { return activeWeapons.size(); }

    void addWeapon(Weapon* wp) { activeWeapons.push_back(wp); }

    void update(PlayerCharacter* player);

    void load(const std::string& directoryPath)
    {
        reset();

        for (const auto& entry : fs::directory_iterator(directoryPath))
        {
            if (fs::is_directory(entry.path()))
            {
                std::string directoryName = entry.path().filename().string();

                if (directoryName == "fast") {
                    for (const auto& entry : fs::directory_iterator(directoryPath + "/" + directoryName))
                    {
                        activeWeapons.push_back(new Weapon(fastWeaponDamage, fastWeaponAttackCooldown, directoryPath + directoryName + "/" + entry.path().filename().string()));
                    }
                }
                else if (directoryName == "medium") {
                    for (const auto& entry : fs::directory_iterator(directoryPath + "/" + directoryName))
                    {
                        activeWeapons.push_back(new Weapon(mediumWeaponDamage, mediumWeaponAttackCooldown, directoryPath + directoryName + "/" + entry.path().filename().string()));
                    }
                }
                else if (directoryName == "slow") {
                    for (const auto& entry : fs::directory_iterator(directoryPath + "/" + directoryName))
                    {
                        activeWeapons.push_back(new Weapon(slowWeaponDamage, slowWeaponAttackCooldown, directoryPath + directoryName + "/" + entry.path().filename().string()));
                    }
                }
            }
        }
    }

    std::vector<sf::Sprite> getWeaponSprites()
    {
        std::vector<sf::Sprite> v;
        for (Weapon* weapon : activeWeapons) {
            v.push_back(weapon->getSprite());
        }
        return v;
    }
};