#pragma once
#include "State.h"
#include "SpriteNode.h"
#include "Common/d3dApp.h"
#include "ScreenTexture.h"
class TitleState : public State
{
public:
	TitleState(StateStack* stack, Context* context);

	virtual void		draw();
	virtual bool		update(const GameTimer& gt) override;
	virtual bool		handleEvent(Command& event) override;
	virtual void		buildState() override;

private:

	ScrenTexture* mScreenTex;
};

