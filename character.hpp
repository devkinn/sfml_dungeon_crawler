#pragma once

class Character {
protected:
    Animation idle_animation;
    Animation run_animation;
    Animation* current_animation;

    sf::Sprite sprite;

    bool isRunning;
    float movement_spd;

    int hitPoints;

    void playIdleAnimation()
    {
        // Update current animation to idle animation
        if (current_animation != &idle_animation) current_animation->stop();
        current_animation = &idle_animation;
        current_animation->play();
    }

    void playRunAnimation()
    {
        // Update current animation to run animation
        if (current_animation != &run_animation) current_animation->stop();
        current_animation = &run_animation;
        current_animation->play();
    }

    void move(sf::Vector2f movementVector, const float& deltaTime)
    {
        if (movementVector.x != 0.f && movementVector.y != 0.f)
        {
            // Calculate the length of the vector
            float length = std::sqrt(movementVector.x * movementVector.x + movementVector.y * movementVector.y);

            // Divide the vector by its length to normalize it
            movementVector /= length;
        }

        if (movementVector.x != 0 || movementVector.y != 0) isRunning = true;
        else isRunning = false;

        if (movementVector.x > 0) sprite.setScale(1.f, 1.f);
        else if (movementVector.x < 0) sprite.setScale(-1.f, 1.f);

        movementVector *= movement_spd;
        sprite.move(movementVector * movement_spd * deltaTime);
    }

    void handleAnimations() 
    {
        if (isRunning) playRunAnimation();
        else playIdleAnimation();
    }

public:
    Character(std::string _idleAnim, std::string _runAnim, float _movement_spd, int _hitPoints) 
        : idle_animation(0.1f, 4, true), run_animation(0.1f, 4, true), movement_spd(_movement_spd), hitPoints(_hitPoints), isRunning(false)
    {
        // Load textures for idle animation
        idle_animation.load(_idleAnim);

        // Load textures for run animation
        run_animation.load(_runAnim);

        // Start with idle animation
        current_animation = &idle_animation;
        current_animation->play();

        // Set sprite's origin to it's center on x axis
        sf::Texture texture = current_animation->getCurrentFrame();
        sf::Vector2f spriteSize(texture.getSize().x, texture.getSize().y);
        sprite.setOrigin(sf::Vector2f(spriteSize.x * 0.5f, spriteSize.y));
    }

    sf::Vector2f getPosition() const { return sprite.getPosition(); }

    sf::FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }

    void setPosition(const sf::Vector2f& position) { sprite.setPosition(position); }

    sf::Sprite& getSprite() { return sprite; }
};

class PlayerCharacter : public Character {
protected:
    Weapon* currentWeapon;

    void getInputs(const float& deltaTime) {
        // Get inputs for movement
        sf::Vector2f movement = sf::Vector2f(0.f, 0.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) movement.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) movement.x += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) movement.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) movement.y += 1.f;

        move(movement, deltaTime);

        // Get inputs for attack
        //if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    }

public:
    PlayerCharacter(std::string _idleAnim, std::string _runAnim, float _movement_spd, int _hitPoints) 
        : Character(_idleAnim, _runAnim, _movement_spd, _hitPoints), currentWeapon(nullptr) {}

    void equipWeapon(Weapon* weapon) { currentWeapon = weapon; }

    sf::Sprite& getWeaponSprite() { return currentWeapon->getSprite(); }

    void update(const float& deltaTime)
    {
        isRunning = false;

        getInputs(deltaTime);
        handleAnimations();

        // Set sprite texture based on animation currently playing
        current_animation->update(deltaTime);
        sprite.setTexture(current_animation->getCurrentFrame());

        currentWeapon->setPosition(sf::Vector2f(sprite.getPosition().x + 5, sprite.getPosition().y));
    }
};

class EnemyCharacter : public Character {
protected:
    sf::Clock attackClock;
    sf::Time attackCooldown;

    sf::Clock moveClock;
    sf::Time moveTime;
    sf::Time idleTime;

    void calculateMoveDirection(const sf::Vector2f& playerPosition, const float& dt) 
    {
        sf::Vector2f position = getPosition();
        sf::Vector2f direction = playerPosition - position;

        move(direction, dt);
    }

    bool canAttack() { return attackClock.getElapsedTime() >= attackCooldown; }

    bool canMove() 
    { 
        if (moveClock.getElapsedTime() <= moveTime) {
            return true;
        }
        else if (moveClock.getElapsedTime() >= moveTime + idleTime) {
            moveClock.restart();
            return true;
        }
        return false;
    }

    bool playerDetected(const sf::FloatRect& playerBounds) { return playerBounds.intersects(getGlobalBounds()); }

public:
    EnemyCharacter(std::string _idleAnim, std::string _runAnim, float _movement_spd, int _hitPoints, float _attack_cooldown, float _move_cooldown)
        : Character(_idleAnim, _runAnim, _movement_spd, _hitPoints), attackCooldown(sf::seconds(_attack_cooldown)), moveTime(sf::seconds(_move_cooldown)), idleTime(sf::seconds(2.f)) {}

    void update(const float& deltaTime, const sf::Vector2f& playerPosition, const sf::FloatRect& playerBounds)
    {
        isRunning = false;

        //calculateMoveDirection(playerPosition, deltaTime);

        if (canMove()) {
            calculateMoveDirection(playerPosition, deltaTime);
        }


        if (canAttack()) {
            if (playerDetected(playerBounds)) {
                std::cout << "Player has been hit by the enemy" << std::endl;
                attackClock.restart();
            }
        }

        handleAnimations();

        // Set sprite texture based on animation currently playing
        current_animation->update(deltaTime);
        sprite.setTexture(current_animation->getCurrentFrame());
    }
};