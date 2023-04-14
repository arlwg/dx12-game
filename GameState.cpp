#include "GameState.hpp"
#include "Game.hpp"
#include <random>
#include <iostream>
#include "StateIdentifiers.h"
#include "ResourceIdentifiers.h"

GameState::GameState(StateStack* stack, Context* context)
	: State(stack, context)
	, mPlayerAircraft(nullptr)
	, mEnemy(nullptr)
	, mEnemy2(nullptr)
	, mBackground(nullptr)
	, mWorldBounds(-1.5f, 1.5f, 200.0f, 0.0f) //Left, Right, Down, Up
	, mSpawnPosition(0.f, 0.f)
	, mScrollSpeed(1.0f)
	, playerVelocity(0,0,0)

{
	buildState();
}

void GameState::draw()
{
	mSceneGraph->draw();
}

bool GameState::update(const GameTimer& gt)
{
	const float deltaTime = gt.DeltaTime();
	//mPlayerAircraft->setVelocity(mPlayerAircraft->getVelocity());

		mSceneGraph->update(gt);
		CheckAircraftBounds();
		BackGroundMovement(gt);
		EnemiesMovement(gt);
		Input(gt);
		mPlayerAircraft->move(XMFLOAT3(playerVelocity.x, playerVelocity.y, 0));
		mPlayerAircraft->setPosition(mPlayerAircraft->getWorldPosition().x, (mPlayerAircraft->getWorldPosition().y + ((playerVelocity.z * deltaTime) / 4)), mPlayerAircraft->getWorldPosition().z);


		//CommandQueue& commands = mWorld.getCommandQueue();
	/*mPlayer.handleEvent(commands);
	mPlayer.handleRealtimeInput(commands);*/
		while (!mCommandQueue.isEmpty())
			mSceneGraph->onCommand(mCommandQueue.pop(), gt);


		CommandQueue& commands = mCommandQueue;
		mContext->mPlayer->handleEvent(commands);
		mContext->mPlayer->handleRealtimeInput(commands);

		mSceneGraph->update(gt);
	
	return true;
}

bool GameState::handleEvent(WPARAM btnState)
{	

		if (btnState == 'P')
		{
			SaveCurrentPosition();
			requestStackPop();
			requestStackPush(States::Pause);
		}
	return true;
}

void GameState::buildState()
{
	/// //////////////////////////////////////////////////
	mContext->mGame->mAllRitems.clear();				//
	mContext->mGame->mOpaqueRitems.clear();				// Clear all items
	mContext->mGame->mFrameResources.clear();			//  
	mContext->mGame->BuildMaterials();					//
	/// //////////////////////////////////////////////////

	std::unique_ptr<Aircraft> player(new Aircraft(Aircraft::Eagle, mContext->mGame));
	mPlayerAircraft = player.get();
	mPlayerAircraft->setPosition(0, 0.1, -.5f);
	mPlayerAircraft->setScale(1.4f, 0.8f, 0.9f);
	mSceneGraph->attachChild(std::move(player));


	std::unique_ptr<Aircraft> enemy1(new Aircraft(Aircraft::Raptor, mContext->mGame));
	mEnemy = enemy1.get();
	mEnemy->setPosition(-1.5f, 0.1f, 2);
	mEnemy->setScale(0.9f, 0.9f, 0.9f);
	mEnemy->setWorldRotation(0, XM_PI, 0);
	mSceneGraph->attachChild(std::move(enemy1));

	std::unique_ptr<Aircraft> enemy2(new Aircraft(Aircraft::Raptor, mContext->mGame));
	mEnemy2 = enemy2.get();
	mEnemy2->setPosition(1.5f, 0.1, 2);
	mEnemy->setScale(0.9f, 0.9f, 0.9f);
	mEnemy2->setWorldRotation(0, XM_PI, 0);
	mSceneGraph->attachChild(std::move(enemy2));

	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(mContext->mGame));
	mBackground = backgroundSprite.get();
	//mBackground->setScale(10.0, 1.0, 350.0);
	mBackground->setScale(10.0, 1.0, 350.0);
	mBackground->setPosition(0, -0.3f, -mBackground->getWorldScale().z / 10);
	mSceneGraph->attachChild(std::move(backgroundSprite));
	LoadSavedPositions();


	//////////////////////////////////////////////////////////////////
	mSceneGraph->build();											//
	for (auto& e : mContext->mGame->mAllRitems)						//
		mContext->mGame->mOpaqueRitems.push_back(e.get());			// Build all items
	//
	mContext->mGame->BuildFrameResources();							//
	//////////////////////////////////////////////////////////////////
}

float GameState::GenerateRandomNumber(float lower, float upper)
{
	std::mt19937 generator(static_cast<unsigned int>(std::time(0)));
	std::uniform_real_distribution<float> distribution(lower, upper);

	return distribution(generator);
}

void GameState::ResetEnemyPositions(Aircraft* aircraft, float lower, float upper)
{
	if (aircraft->getWorldPosition().z <= -5)
	{
		aircraft->setPosition(GenerateRandomNumber(lower, upper), 0.1f, GenerateRandomNumber(8, 10));
	}
}

void GameState::BackGroundMovement(const GameTimer& gt)
{
	const float deltaTime = gt.DeltaTime();
	mBackground->move(0, 0, -backgroundSpeed * deltaTime);

	//once it below a certain point reset it to desired position
	if (mBackground->getWorldPosition().z <= -(mBackground->getWorldScale().z / 10 * 2))
	{
		mBackground->setPosition(0, -0.3f, -mBackground->getWorldScale().z / 10);
	}
}

void GameState::EnemiesMovement(const GameTimer& gt)
{
	const float deltaTime = gt.DeltaTime();

	mEnemy->move(0, 0, -enemySpeed * deltaTime);
	mEnemy2->move(0, 0, -enemySpeed * deltaTime);


	ResetEnemyPositions(mEnemy, -4, 1);
	ResetEnemyPositions(mEnemy2, 0, 4);
}

void GameState::CheckAircraftBounds()
{
	auto pos = mPlayerAircraft->getWorldPosition();

	float pushPowerX = 0.0f;
	float pushPowerY = 0.0f;
	float pushPowerZ = 0.0f;
	if (pos.x > 1.93356)
	{
		//push to the left
		pushPowerX = -5.0f;
	}
	if (pos.x < -1.80393)
	{
		//push to the right
		pushPowerX = 5.0f;
	}

	if (pos.y > 1)
	{
		//push back
		pushPowerY = -5.0f;
	}
	if (pos.y < -0.1)
	{
		//push forward
		pushPowerY = 5.0f;
	}

	if (pos.z > 1)
	{
		//push back
		pushPowerZ = -5.0f;
	}
	if (pos.z < -1.3977)
	{
		//push forward
		pushPowerZ = 5.0f;
	}
	XMFLOAT3 pusher = XMFLOAT3(pushPowerX, pushPowerY, pushPowerZ);
	mPlayerAircraft->move(pusher);
}


void GameState::Input(const GameTimer& gt)
{
	/*const float deltaTime = gt.DeltaTime();
	*/


	
}

void GameState::SaveCurrentPosition()
{
	mContext->mGame->bgPos = mBackground->getWorldPosition();
	mContext->mGame->playerPos = mPlayerAircraft->getWorldPosition();
	mContext->mGame->enemy1Pos = mEnemy->getWorldPosition();
	mContext->mGame->enemy2Pos = mEnemy2->getWorldPosition();
	mContext->mGame->ReloadGameState = true;
}

void GameState::LoadSavedPositions()
{
	if (mContext->mGame->ReloadGameState)
	{
		mBackground->setPosition(mContext->mGame->bgPos.x, mContext->mGame->bgPos.y, mContext->mGame->bgPos.z);
		mPlayerAircraft->setPosition(mContext->mGame->playerPos.x, mContext->mGame->playerPos.y, mContext->mGame->playerPos.z);
		mEnemy->setPosition(mContext->mGame->enemy1Pos.x, mContext->mGame->enemy1Pos.y, mContext->mGame->enemy1Pos.z);
		mEnemy2->setPosition(mContext->mGame->enemy2Pos.x, mContext->mGame->enemy2Pos.y, mContext->mGame->enemy2Pos.z);
		mContext->mGame->ReloadGameState = false;
	}
}




