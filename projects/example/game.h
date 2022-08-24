#pragma once
#include "yoshix.h"

struct _player
{
	//PLayer Size
	const int X_WIDTH = 10;
	const int Y_HEIGHT = 1;

	//Player Coord
	float x =  0;
	float y = -24;
};

struct _constBorderValues {
	//Spielfelder Bedingungen etc
	const int   BORDER_X         = 30;
	const int   BORDER_Y         = 1;
	const float SPIELFELD_HEIGHT = 0;
};

struct _ball {
	bool startGame  = false;
	bool go			= false;
	bool gameOver	= false;

	const float START	= -24.0f;
	float xSpeed		= 0.4f;
	float ySpeed		= 0.4f;

	float positionXY[2] = { 0, START };

	bool moveUp		= false;		//flag für die aufwärst bewegung
	bool moveDown	= false;		//flag für die abwärts bewegung
	bool moveRight	= false;		//flag für die links bewegung
	bool moveLeft	= false;		//flag für die rechts bewegung
};

struct _const {
	const float SPEED = 1.0f;
	const int	BORDER = 37;

	const int BORDER_X = 30;
	const int BORDER_Y = 1;
	const float SPIELFELD_HEIGHT = 0;

	int currentPLayerPosition = 0;
	const int SPIELFELDBREITE = 26;

	const float BALLSPIELFELD_BREITE = 36;
	const float BALLSPIELFELD_HOEHE = 28;
};

//Variables
const static float CONST = 1.0f;
const static float ABWEICHUNG = 0.3f;
const static float ABWEICHUNG_Y_PLACEMENT = 1.0f;

//Functions
gfx::BHandle createBorder(gfx::BHandle _pMaterial);
gfx::BHandle createBall  (gfx::BHandle _pMaterial);
gfx::BHandle createPlayer(gfx::BHandle _pMaterial, _player& player);

gfx::BHandle createColoredMaterialInfo (gfx::BHandle _pVertexConstantBuffer, gfx::BHandle _pVertexShader, gfx::BHandle _pPixelShader);

int getRand();
bool startGame(_ball& ball);
void randomRichtung(_ball& ball);
void move(_ball& ball, _const& cValue, _player& player);
void collisionDetection(_ball& ball, _const& cValue, _player& player);

