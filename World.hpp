#pragma once
#include "SceneNode.hpp"
#include "Aircraft.hpp"
#include "SpriteNode.h"
#include <list>
class World 
{
public:
	explicit							World(Game* window);
	void								update(const GameTimer& gt);
	void								draw();
	void								Input(const GameTimer& gt);
	void								BackGroundMovement(const GameTimer& gt);
	void								EnemiesMovement(const GameTimer& gt);

	//void								loadTextures();
	void								buildScene();


private:
	enum Layer
	{
		Background,
		Air,
		LayerCount
	};


private:
	Game*								mGame;

	SceneNode*							mSceneGraph;
	std::array<SceneNode*, LayerCount>	mSceneLayers;

	XMFLOAT4							mWorldBounds;
	XMFLOAT2		    				mSpawnPosition;
	float								mScrollSpeed;
	Aircraft*							mPlayerAircraft;
	SpriteNode*							mBackground;
	Aircraft*							mEnemy;
	Aircraft*							mEnemy2;

private:
	float playerSpeed = 3.0f;
	float enemySpeed = 5.0f;
	float backgroundSpeed = 10.0f;
};
