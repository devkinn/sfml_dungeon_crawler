#pragma once

enum orientation { left, right };

class Player {
private:
    Animation idle_animation;
    Animation run_animation;
    Animation* current_animation;
    sf::Sprite sprite;
    bool isRunning;
    float movement_spd = 90.0f;
    orientation orient = right;

    sf::Vector2f movement;

public:
    Player();
    void update(float deltaTime, sf::RenderWindow& window);
    void getInput(float deltaTime);
    void playIdleAnimation();
    void playRunAnimation();
    sf::Vector2f getPosition() const { return sprite.getPosition(); }
    sf::Vector2f getMovementVector() const { return movement; }
    sf::FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }
    void setPosition(sf::Vector2f position) { sprite.setPosition(position); }
};

Player::Player() : idle_animation(0.1f, 4, true), run_animation(0.1f, 4, true), isRunning(false)
{
    // Load textures for idle animation
    idle_animation.load("./assets/heroes/lizard/lizard_f_idle_anim_f");

    // Load textures for run animation
    run_animation.load("./assets/heroes/lizard/lizard_f_run_anim_f");

    // Start with idle animation
    current_animation = &idle_animation;
    current_animation->play();

    // Set initial position and size of the sprite

    // Set sprite's origin to it's center on x axis
    sf::Texture texture = current_animation->getCurrentFrame();
    sf::Vector2f spriteSize(texture.getSize().x, texture.getSize().y);
    sprite.setOrigin(sf::Vector2f(spriteSize.x * 0.5f, spriteSize.y));
}

void Player::playIdleAnimation()
{
    // Update current animation to idle animation
    if (current_animation != &idle_animation) current_animation->stop();
    current_animation = &idle_animation;
    current_animation->play();
}

void Player::playRunAnimation()
{
    // Update current animation to run animation
    if (current_animation != &run_animation) current_animation->stop();
    current_animation = &run_animation;
    current_animation->play();
}

void Player::getInput(float deltaTime)
{
    // Get inputs for movement
    movement = sf::Vector2f(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        // Move player and update his orientation
        movement.x -= 1.f;
        isRunning = true;
        if (orient == right) {
            orient = left;
            sprite.setScale(-1.f, 1.f);
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        movement.x += 1.f;
        isRunning = true;
        if (orient == left) {
            orient = right;
            sprite.setScale(1.f, 1.f);
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        movement.y -= 1.f;
        isRunning = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        movement.y += 1.f;
        isRunning = true;
    }

    if (movement.x != 0.f && movement.y != 0.f)
    {
        // Calculate the length of the vector
        float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);

        // Divide the vector by its length to normalize it
        movement /= length;
    }

    movement *= movement_spd;
    sprite.move(movement * deltaTime);
}

void Player::update(float deltaTime, sf::RenderWindow& window)
{
    isRunning = false;

    getInput(deltaTime);

    // Manage animations
    if (isRunning) {
        playRunAnimation();
    }
    else {
        playIdleAnimation();
    }

    // Set sprite texture based on animation currently playing
    current_animation->update(deltaTime);
    sprite.setTexture(current_animation->getCurrentFrame());

    // Draw the sprite
    window.draw(sprite);
}