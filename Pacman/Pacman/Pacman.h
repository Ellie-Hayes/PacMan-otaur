#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file


#include "S2D/S2D.h"
#include <iostream>
#include <vector>
#define MUNCHIECOUNT 50
#define ENEMYCOUNT 2
// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;



// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.

struct Player
{
	enum PlayerState { Idle, Walking, Attacking, Dead, Hurt };
	PlayerState currentState; 
	bool dead; 
	Vector2* _pacmanPosition;
	Rect* _pacmanSourceRect;
	Texture2D* _pacmanTexture;
	
	int _playerDirection;
	int _playerFrame;
	int _playerCurrentFrameTime;
	float speedMuiltiply; 

};

struct Collectable
{
	Vector2* _munchiePosition;
	Rect* _munchieRect;
	Texture2D* _munchieSheetTexture;

	int _frameCount;
	int _munchieFrame;
	int _munchieCurrentFrameTime;
	int _frameTime;
};

struct projectile
{
	
};

class Enemies
{
	public:
		enum EnemyType {Minotaur, gear, vines};

	class MovingEnemy
	{
		public:
		enum EnemyState { Idle, Walking, Attacking, Dead, Hurt };
		Vector2* position;
		Rect* sourceRect;
		Texture2D* texture;

		int direction;
		int _frameCount;
		int _Frame;
		int _CurrentFrameTime;
		int _frameTime;
		float speed;
	};

	class gearEnemy
	{
		public:
		Vector2* position;
		Rect* sourceRect;
		Texture2D* textureOuter;
		Texture2D* textureinner;
	};
};

struct WaveSpawner
{
	enum SpawnState { Spawning, Waiting, Counting };
	SpawnState state;

	const int enemyArray[20]{
		1, 3, 4, 6,
		8, 9, 10, 12,
		15, 18, 19, 20,
		19, 28, 39, 50,
		50, 50, 60, 60,
	};

	int nextWave;
	float timeBetweenWaves;
	float WaveCountdown;
	float SearchCountdown;
	float spawnRate;
	int EnemyTypeCount;

	vector<Enemies::MovingEnemy*> Minotaurs;
	vector<Enemies::gearEnemy*> gears;
};



class Pacman : public Game
{
private:

	Enemies* _enemyClass;
	WaveSpawner *_wavespawner;
	void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckViewportCollision();
	void UpdatePacman(int elapsedTime, Player::PlayerState state);
	void UpdateMunchie(Collectable* munchie, int elapsedTime);
	void UpdateCherry(int elapsedTime);
	void CheckGhostCollision();
	void UpdateGhost(Enemies::MovingEnemy* ghost, int elapsedTime);
	void SpawnWave();
	void CheckWaveComplete(); 
	bool EnemiesAlive();
	void NewEnemy(Enemies::EnemyType enemyType, Texture2D* texture1, Texture2D* texture2);
	Player* _player; 
	
	
	const float _cPacmanSpeed;
	const int _cPlayerFrameTime;

	Collectable* _munchie[MUNCHIECOUNT];
	const int _cMunchieFrameTime;

	/*Enemies::MovingEnemy* _Enemies[ENEMYCOUNT];*/
	const int _cMinotaurFrameTime;

	// Position for String
	Vector2* _stringPosition;

	// Data for Menu
	Texture2D* _menuBackground;
	Rect* _menuRectangle; 
	Vector2* _menuStringPosition; 
	bool _paused;
	bool _pKeyDown;

	//Start Menu data
	Texture2D* _StartBackground;
	Rect* _StartRectangle;
	Vector2* _StartStringPosition;
	bool _showStart;
	bool _spaceKeyDown;

	int _cherryFrame;
	int _cherryCurrentFrameTime;
	const int _cCherryFrameTime;

	//cherry
	Vector2* _cherryPosition;
	Rect* _cherrySourceRect;
	Texture2D* _cherryTexture;

	//Audio

	SoundEffect* _pop; 

public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);

	
};

