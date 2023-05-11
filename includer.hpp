#pragma once

// Libraries

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <random>
#include <chrono>

// Game parameters

static const unsigned int defaultFPS = 60;

static const float cameraSizeX = 500.f;
static const float cameraSizeY = 300.f;

static const sf::Vector2u tileSize(16, 16);

// Dungeon parameters

static const unsigned int minRoomSize = 14;
static const unsigned int maxRoomSize = 16;
static const unsigned int roomMargin = 2;
static const unsigned int corridorWidth = 2;

static const unsigned int dungeon1width = 40;
static const unsigned int dungeon1height = 60;

static const unsigned int dungeon2width = 60;
static const unsigned int dungeon2height = 80;

static const unsigned int dungeon3width = 80;
static const unsigned int dungeon3height = 100;

// Asset file locations

static const std::string mapTileset = "./assets/tileset/tileset.png";
static const std::string backgroundTileset = "./assets/background/background.png";

// Functions

int getRandomInRange(int min, int max) {
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);

	std::uniform_int_distribution<int> distribution(min, max);

	return distribution(generator);
}

// Header files

#include "dungeon_generator.hpp"
#include "map_renderer.hpp"
#include "animation.hpp"
#include "player.hpp"
#include "game_tracker.hpp"