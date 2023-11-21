#include "Pacman.h"

#include <sstream>
#include <vector>
#include <ctgmath>
#include <iostream>


Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPlayerFrameTime(100), _cMunchieFrameTime(100), _cCherryFrameTime(700), _cMinotaurFrameTime(100), _cGearFrameTime(2000)
{
	_player = new Player();
	_enemyClass = new Enemies();
	_wavespawner = new WaveSpawner();
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i] = new Collectable();
		_munchie[i]->_munchieCurrentFrameTime = 0; 
		_munchie[i]->_frameCount = 0;
		_munchie[i]->_munchieFrame = 0;
		_munchie[i]->_frameTime = rand() % 500 + 50; 
	}

	_paused = false;
	_pKeyDown = false;
	_showStart = true; 
	_spaceKeyDown = false; 

	//player initialisation 
	_player->_playerDirection = 0;
	_player->_playerCurrentFrameTime = 0;
	_player->_playerFrame = 0;
	_player->dead = false; 
	_player->currentState = _player->Idle;
	_player->health = 150; 
	_player->damage = 5; 

	_cherryCurrentFrameTime = 0;
	_cherryFrame = 0;

	_pop = new SoundEffect();
	_Coin = new SoundEffect();

	//WaveSpawner initialisation 
	
	_wavespawner->state = _wavespawner->Spawning;

	_wavespawner->nextWave = 0;
	_wavespawner->timeBetweenWaves = 5;
	_wavespawner->WaveCountdown = _wavespawner->timeBetweenWaves;
	_wavespawner->SearchCountdown = 1;
	_wavespawner->spawnRate = 0.5f;
	_wavespawner->EnemyTypeCount = 2;

	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();									//					//16
	

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _player->_pacmanTexture;
	delete _player->_pacmanSourceRect;
	delete _player->_pacmanHitbox; 

	delete _menuBackground; 
	delete _StartBackground;
	delete _GameBackground;
	delete _GameShadow; 
	delete endGameScreenBackground; 

	delete _cherrySourceRect;
	delete _cherryTexture; 
	delete _player;
	delete _munchie;

	delete _munchie[0]->_munchieSheetTexture;
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		delete _munchie[i]->_munchieRect;
		delete _munchie[i]->_munchiePosition;
		delete _munchie[i];
	}
	delete[] _munchie;

	for (int i = 0; i < projectilesPlayer.size(); i++)
	{
		delete projectilesPlayer[i]->_projectilePosition; 
		delete projectilesPlayer[i]->_targetPosition; 
		delete projectilesPlayer[i]->_projectileRect; 
	}

	for (int i = 0; i < projectilesEnemies.size(); i++)
	{
		delete projectilesPlayer[i]->_projectilePosition;
		delete projectilesPlayer[i]->_targetPosition;
		delete projectilesPlayer[i]->_projectileRect;
		delete projectilesPlayer[i]->_centerPoint;
	}

	for (int i = 0; i < _wavespawner->Minotaurs.size(); i++)
	{
		delete _wavespawner->Minotaurs[i]->position;
		delete _wavespawner->Minotaurs[i]->sourceRect;
		
	}

	for (int i = 0; i < _wavespawner->gears.size(); i++)
	{
		delete _wavespawner->gears[i]->position;
		delete _wavespawner->gears[i]->sourceRect;

	}
	delete _pop; 
	delete _Coin; 
	delete _mousePos; 

	delete _ghostTex;
	delete _gearOuterTex;
	delete _gearInnerTex;
	delete _bulletTex;
	delete _attackTex;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_player->_pacmanTexture = new Texture2D();
	_player->_pacmanTexture->Load("Textures/PlayerSpritesheet.png", false);
	_player->_pacmanPosition = new Vector2(350.0f, 350.0f);
	_player->_pacmanSourceRect = new Rect(0.0f, 0.0f, 90, 90);
	_player->_pacmanHitbox = new Rect(24.0f, 12.0f, 42, 66); 
	_player->speedMuiltiply = 1.0f; 

	// Load Munchie

	Texture2D* munchieTex = new Texture2D(); 
	munchieTex->Load("Textures/Coin.png", true);
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i]->_munchieRect = new Rect(0.0f, 0.0f, 16, 16);
		_munchie[i]->_munchiePosition = new Vector2((rand() % Graphics:: GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		CheckEnemyPos(_munchie[i]->_munchiePosition, _munchie[i]->_munchieRect);
		_munchie[i]->_munchieSheetTexture = new Texture2D();
		_munchie[i]->_munchieSheetTexture = munchieTex;
	}

	_cherrySourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_cherryPosition = new Vector2(500.0f, 450.0f);
	_cherryTexture = new Texture2D();
	_cherryTexture->Load("Textures/CherrySpritesheet.png", true);


	// Set string position
	playerScore = 0; 
	_stringPosition = new Vector2(10.0f, 25.0f);
	_scoreTextPosition = new Vector2(Graphics::GetViewportWidth() / 5.0f * 4.5f, 25.0f);


	// Menu stuffs
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/PauseMenu.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	_StartBackground = new Texture2D();
	_StartBackground->Load("Textures/PacmanBackground.png", false);
	_StartRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_StartStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 3.0f);
	
	_GameBackground = new Texture2D();
	_GameBackground->Load("Textures/GameBackground.png", false);
	_GameShadow = new Texture2D();
	_GameShadow->Load("Textures/GameShadow.png", false);

	endGameScreenBackground = new Texture2D();
	endGameScreenBackground->Load("Textures/GameOver.png", false);


	//projectile textures
	_ghostTex = new Texture2D();
	_ghostTex->Load("Textures/MinotaurSpritesheet.png", false);
	_gearOuterTex = new Texture2D();
	_gearOuterTex->Load("Textures/CogOuter.png", false);
	_gearInnerTex = new Texture2D();
	_gearInnerTex->Load("Textures/CogInner.png", false);


	_bulletTex = new Texture2D();
	_bulletTex->Load("Textures/bullet.png", false);
	_attackTex = new Texture2D();
	_attackTex->Load("Textures/MinotaurAttack.png", false);

	
	//audio

	_pop->Load("Sounds/pop.wav");
	_Coin->Load("Sounds/coin.wav");
}

void Pacman::Update(int elapsedTime)
{

	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();

	if (_showStart)
	{
		if (keyboardState->IsKeyDown(Input::Keys::SPACE))
			_showStart = false;
	}
	else
	{
		CheckPaused(keyboardState, Input::Keys::P);

		if (!_paused)
		{
			Input(elapsedTime, keyboardState, mouseState);
			CheckViewportCollision();
			UpdatePacman(elapsedTime, _player->currentState);
			SpawnWave();
			CheckWaveComplete();
			HandleAllCollision();

			for (int i = 0; i < MUNCHIECOUNT; i++)
			{
				UpdateMunchie(_munchie[i], elapsedTime);
			}

			UpdateCherry(elapsedTime);

			if (_player->health <= 0)
			{
				_player->currentState == Player::Dead; 
				endGameScreen = true; 
			}

			for (int i = 0; i < _wavespawner->Minotaurs.size(); i++)
			{
				UpdateGhost(_wavespawner->Minotaurs[i], elapsedTime); //update each enemy

				
				if (CheckHealth(_wavespawner->Minotaurs[i]->health)) // check if enemy dead
				{
					_wavespawner->Minotaurs.erase(_wavespawner->Minotaurs.begin() + i);
					
					playerScore += 10; //add score
				}
			}
			

			for (int i = 0; i < projectilesPlayer.size(); i++)
			{
				MoveProjectiles(projectilesPlayer[i]); //moves projectiles based on angle
				if (deleteProjectile(projectilesPlayer[i])) //deletes them if hit the walls
				{
					projectilesPlayer.erase(projectilesPlayer.begin() + i);
					break;
				}
				
			}

			for (int i = 0; i < projectilesEnemies.size(); i++)
			{
				MoveProjectiles(projectilesEnemies[i]);
				if (deleteProjectile(projectilesEnemies[i]))
				{
					projectilesEnemies.erase(projectilesEnemies.begin() + i);
					break;
				}

			}
			
			for (int i = _wavespawner->gears.size() - 1; i >= 0; i--)
			{
				UpdateGear(_wavespawner->gears[i], elapsedTime);
				if (CheckHealth(_wavespawner->gears[i]->health))
				{
					_wavespawner->gears.erase(_wavespawner->gears.begin() + i);
					playerScore += 15;
				}
			}
		}
	}
	
}


void Pacman::Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState)
{
	
	//stops the player moving if they are hurt or dead
	if (_player->currentState == _player->Hurt || _player->currentState == _player->Attacking || _player->currentState == _player->Dead){ return; } 
	else { _player->currentState = _player->Idle; }

	if (state->IsKeyDown(Input::Keys::D))
	{
		_player->_pacmanPosition->X += _cPacmanSpeed * elapsedTime *_player->speedMuiltiply; //Moves Pacman across X axis
		_player->_playerDirection = 0;
		_player->currentState = _player->Walking;
	}
	if (state->IsKeyDown(Input::Keys::A))
	{
		_player->_pacmanPosition->X += -_cPacmanSpeed * elapsedTime * _player->speedMuiltiply;
		_player->_playerDirection = 2;
		_player->currentState = _player->Walking;
	}
	if (state->IsKeyDown(Input::Keys::W))
	{
		_player->_pacmanPosition->Y += -_cPacmanSpeed * elapsedTime * _player->speedMuiltiply; //Moves Pacman across X axis
		_player->_playerDirection = 3;
		_player->currentState = _player->Walking;
	}
	if (state->IsKeyDown(Input::Keys::S))
	{
		_player->_pacmanPosition->Y += _cPacmanSpeed * elapsedTime * _player->speedMuiltiply;
		_player->_playerDirection = 1;
		_player->currentState = _player->Walking;
	}
	if (state->IsKeyDown(Input::Keys::O))
	{
		_player->_playerFrame = 0; 
		_player->currentState = _player->Attacking;
	}

	if (endGameScreen)
	{
		if (state->IsKeyDown(Input::Keys::SPACE))
			exit(0); //exit program 
	}


	//Mouse
	if (mouseState ->LeftButton == Input::ButtonState::PRESSED && !_mouseDown) //If player clicks spawn projectile
	{
		_mouseDown = true; 
		_mousePos = new Vector2(mouseState->X, mouseState->Y); //pass position
		SpawnProjectile(_player->_pacmanSourceRect, _player->_pacmanPosition, _mousePos, 0.0f, false, true);
	}
	if (mouseState->LeftButton == Input::ButtonState::RELEASED)
	{
		_mouseDown = false;
	}
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(pauseKey) && !_pKeyDown)
	{
		_pKeyDown = true;
		Audio::Play(_pop); 
		_paused = !_paused;
	}

	if (state->IsKeyUp(pauseKey))
		_pKeyDown = false;
}

void Pacman::CheckViewportCollision() //checks if player hits the walls and stops them
{
	if (_player->_pacmanPosition->X + _player->_pacmanSourceRect->Width + 64 > Graphics::GetViewportWidth()) { _player->_pacmanPosition->X = Graphics::GetViewportWidth() - _player->_pacmanSourceRect->Width - 64; }
	if (_player->_pacmanPosition->X < 64) { _player->_pacmanPosition->X = 64; }
	if (_player->_pacmanPosition->Y + _player->_pacmanSourceRect->Height + 96 > Graphics::GetViewportHeight()) { _player->_pacmanPosition->Y = Graphics::GetViewportHeight() - _player->_pacmanSourceRect->Height - 96; }
	if (_player->_pacmanPosition->Y < 32) { _player->_pacmanPosition->Y = 32; }
}

void Pacman::UpdatePacman(int elapsedTime, Player::PlayerState state)
{
	_player->_playerCurrentFrameTime += elapsedTime;

	if (_player->currentState == Player::Hurt)
	{
		_player->_pacmanPosition->X += _player->KnockbackPower * cos(_player->KnockbackAngle);
		_player->_pacmanPosition->Y += _player->KnockbackPower * sin(_player->KnockbackAngle);
	}

	if (_player->_playerCurrentFrameTime > _cPlayerFrameTime) //if frameTime is greater than 250 increment the sprite by one, if its greater than 2 loop back
	{
		_player->_playerFrame++;

		if (_player->_playerFrame >= 8) //cycles through 8 frames
		{
			_player->_playerFrame = 0;
				
			switch (state)
			{
			case Player::Dead: //changes the states so the player can move again and show right animation
				endGameScreen = true; 
				break;
			case Player::Hurt:
				_player->currentState = _player->Idle;
				break;
			default:
				break;
			}
		}
		_player->_playerCurrentFrameTime = 0; 
	}

	//rotates pacman
	_player->_pacmanSourceRect->Y = _player->_pacmanSourceRect->Height * (_player->_playerDirection * 5 + static_cast<std::underlying_type<Player::PlayerState>::type>(state));
	_player->_pacmanSourceRect->X = _player->_pacmanSourceRect->Width * _player->_playerFrame;  //uses spritesheets then changes the rect to show the right texture
}

void Pacman::UpdateMunchie(Collectable* munchie, int elapsedTime)
{
	//munchie
	munchie->_munchieCurrentFrameTime += elapsedTime;

	if (munchie->_munchieCurrentFrameTime > _cMunchieFrameTime)
	{
		munchie->_munchieFrame++;
		if (munchie->_munchieFrame >= 5) { munchie->_munchieFrame = 0; }
		munchie->_munchieCurrentFrameTime = 0;
	}

	munchie->_munchieRect->X = munchie->_munchieRect->Width * munchie->_munchieFrame;
	
}

void Pacman::UpdateCherry(int elapsedTime)
{
	_cherryCurrentFrameTime += elapsedTime;

	if (_cherryCurrentFrameTime > _cCherryFrameTime)
	{
		_cherryFrame++;

		if (_cherryFrame >= 2)
		{
			_cherryFrame = 0;
		}
		_cherryCurrentFrameTime = 0;
	}

	_cherrySourceRect->X = _cherrySourceRect->Width * _cherryFrame;
}

void Pacman::HandleAllCollision()
{
	//Collision between gears enemy and player projectiles
	for (int j = 0; j < _wavespawner->gears.size(); j++)
	{
		for (int i = 0; i < projectilesPlayer.size(); i++)
		{
			if (CheckCollisions(projectilesPlayer[i]->_projectilePosition->X, projectilesPlayer[i]->_projectilePosition->Y, projectilesPlayer[i]->_projectileRect->Width, projectilesPlayer[i]->_projectileRect->Height, _wavespawner->gears[j]->position->X, _wavespawner->gears[j]->position->Y, _wavespawner->gears[j]->sourceRect->Width, _wavespawner->gears[j]->sourceRect->Height))
			{
				_wavespawner->gears[j]->health -= _player->damage;
				projectilesPlayer.erase(projectilesPlayer.begin() + i);
			}
		}
	}

	//Collision between minotaur enemy and player projectiles
	for (int j = 0; j < _wavespawner->Minotaurs.size(); j++)
	{
		for (int i = 0; i < projectilesPlayer.size(); i++)
		{
			if (CheckCollisions(projectilesPlayer[i]->_projectilePosition->X, projectilesPlayer[i]->_projectilePosition->Y, projectilesPlayer[i]->_projectileRect->Width, projectilesPlayer[i]->_projectileRect->Height, _wavespawner->Minotaurs[j]->position->X, _wavespawner->Minotaurs[j]->position->Y, _wavespawner->Minotaurs[j]->sourceRect->Width, _wavespawner->Minotaurs[j]->sourceRect->Height))
			{
				_wavespawner->Minotaurs[j]->currentState = Enemies::MovingEnemy::Hurt;
				_wavespawner->Minotaurs[j]->_Frame = 4;
				_wavespawner->Minotaurs[j]->health -= _player->damage;
				_wavespawner->Minotaurs[j]->knockbackAngle = projectilesPlayer[i]->angle;
				projectilesPlayer.erase(projectilesPlayer.begin() + i);
			}
		}
	}

	//collision between player and minotaur to check to attack

	for (int i = 0; i < _wavespawner->Minotaurs.size(); i++)
	{
		if (CheckCollisions(_player->_pacmanPosition->X, _player->_pacmanPosition->Y, _player->_pacmanSourceRect->Width, _player->_pacmanSourceRect->Height, _wavespawner->Minotaurs[i]->position->X, _wavespawner->Minotaurs[i]->position->Y, _wavespawner->Minotaurs[i]->sourceRect->Width, _wavespawner->Minotaurs[i]->sourceRect->Height))
		{
			_wavespawner->Minotaurs[i]->currentState = Enemies::MovingEnemy::Attacking;
		}
		
	}

	
	for (int i = 0; i < projectilesEnemies.size(); i++)
	{
		if (CheckCollisions(_player->_pacmanPosition->X + 24, _player->_pacmanPosition->Y + 12, _player->_pacmanHitbox->Width, _player->_pacmanHitbox->Height, projectilesEnemies[i]->_projectilePosition->X, projectilesEnemies[i]->_projectilePosition->Y, projectilesEnemies[i]->_projectileRect->Width, projectilesEnemies[i]->_projectileRect->Height))
		{
			if (_player->currentState == Player::Dead) { return; }
			_player->currentState = Player::Hurt;
			_player->_playerFrame = 6;
			_player->health -= 1;
			int diffX = projectilesEnemies[i]->_projectilePosition->X - _player->_pacmanPosition->X;
			int diffY = projectilesEnemies[i]->_projectilePosition->Y - _player->_pacmanPosition->Y;

			_player->KnockbackAngle = (float)atan2(diffY, diffX);
			projectilesEnemies.erase(projectilesEnemies.begin() + i);
		}
	}

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		if (CheckCollisions(_player->_pacmanPosition->X + 24, _player->_pacmanPosition->Y + 12, _player->_pacmanHitbox->Width, _player->_pacmanHitbox->Height, _munchie[i]->_munchiePosition->X, _munchie[i]->_munchiePosition->Y, _munchie[i]->_munchieRect->Width, _munchie[i]->_munchieRect->Height))
		{
			playerScore += 25; 
			_munchie[i]->_munchiePosition->X = -500;
			_munchie[i]->_munchiePosition->Y = -500;
			Audio::Play(_Coin);
		}
	}
	
	
}

bool Pacman::CheckCollisions(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
{
	int left1 = x1;
	int left2 = x2;
	int right1 = x1 + width1;
	int right2 = x2 + width2;
	int top1 = y1;
	int top2 = y2;
	int bot1 = y1 + height1;
	int bot2 = y2 + height2;
	if (bot1 < top2)
		return false;
	else if (top1 > bot2)
		return false;
	else if (right1 < left2)
		return false;
	else if (left1 > right2)
		return false;

	return true;
}

void Pacman::UpdateGhost(Enemies::MovingEnemy* ghost, int elapsedTime)
{
	//gets position of player and calculates movement angle based on position
	int diffX = _player->_pacmanPosition->X - ghost->position->X;
	int diffY = _player->_pacmanPosition->Y - ghost->position->Y;

	float angle = (float)atan2(diffY, diffX);

	if (ghost->currentState == Enemies::MovingEnemy::Hurt)
	{
		angle = ghost->knockbackAngle; //makes the enemy have knockback 
	}
	else
	{
		//checks angle to tell which anim to play up down left or right based on player position
		//std::cout << angle << endl; 
		if (angle >= 0 && angle < 1.8) { ghost->direction = 0; }
		else if (angle >= 1.8 && angle < 3.6) { ghost->direction = 1; }
		else if (angle >= -1.8 && angle < 0) { ghost->direction = 3; }
		else if (angle >= -3.6 && angle <= -1.8) { ghost->direction = 2; }
	}

	//moves minotaur
	if (ghost->currentState != Enemies::MovingEnemy::Attacking && ghost->currentState != Enemies::MovingEnemy::Dead)
	{
		ghost->position->X += ghost->speed * cos(angle);
		ghost->position->Y += ghost->speed * sin(angle);
	}
	
	//Anim cycling
	ghost->_CurrentFrameTime += elapsedTime;

	if (ghost->_CurrentFrameTime > _cMinotaurFrameTime)
	{
		ghost->_Frame++;

		if (ghost->_Frame == 4 && ghost->currentState == Enemies::MovingEnemy::Attacking)
		{
			SpawnProjectile(ghost->sourceRect, ghost->position, NULL, ghost->direction * 55, true, false);
		}
		if (ghost->_Frame >= 8) 
		{ 
			ghost->_Frame = 0; 

			if (ghost->currentState == Enemies::MovingEnemy::Hurt || ghost->currentState == Enemies::MovingEnemy::Attacking)
			{
				ghost->currentState = Enemies::MovingEnemy::Walking; 
				ghost->speed = 1; 
			}
			else if (ghost->currentState == Enemies::MovingEnemy::Dead)
			{
				ghost->deleteEnemy = true; 
			}
		
		}
		ghost->_CurrentFrameTime = 0;
	}

	//Updates what sprite is being displayed 
	ghost->sourceRect->Y = ghost->sourceRect->Height * (ghost->direction * 5 + static_cast<std::underlying_type<Player::PlayerState>::type>(ghost->currentState));
	ghost->sourceRect->X = ghost->sourceRect->Width * ghost->_Frame;

}

void Pacman::SpawnWave()
{
	if (_wavespawner->state == _wavespawner->Spawning)
	{
		for (int i = 0; i < _wavespawner->EnemyTypeCount; i++)
		{
			if (_wavespawner->nextWave >= 20) { _wavespawner->nextWave = 19; } // if hit the end of the array cycle back so it doesnt go out of range
			
			if (i == 0)
			{
				for (int i = 0; i < _wavespawner->enemyArray[_wavespawner->nextWave]; i++)
				{
					NewEnemy(Enemies::Minotaur, _ghostTex, NULL); 
				}
			}
			else
			{
				for (int i = 0; i < _wavespawner->enemyArray[_wavespawner->nextWave]; i++)
				{
					NewEnemy(Enemies::gear, _gearOuterTex, _gearInnerTex);
				}
				
			}

			_wavespawner->nextWave++; //increments the wave
		}
		_wavespawner->state = _wavespawner->Waiting;

	}
}

void Pacman::CheckWaveComplete()
{
	if (_wavespawner->state == _wavespawner->Waiting)
	{
		if (_wavespawner->Minotaurs.size() + _wavespawner->gears.size() == 0)
		{
			_wavespawner->state = _wavespawner->Spawning;
		}
	}
}

void Pacman::NewEnemy(Enemies::EnemyType enemyType, Texture2D* texture1, Texture2D* texture2)
{
	 
	if (enemyType == Enemies::Minotaur)
	{
		Enemies::MovingEnemy* tempObject = new Enemies::MovingEnemy();
		_wavespawner->Minotaurs.push_back(tempObject);

		tempObject->_CurrentFrameTime = 0;
		tempObject->_frameCount = 0;
		tempObject->_Frame = 0;
		tempObject->_frameTime = rand() % 500 + 50;
		tempObject->speed = 1;
		tempObject->direction = 0;
		tempObject->currentState = tempObject->EnemyState::Walking; 
		tempObject->sourceRect = new Rect(0.0f, 0.0f, 90, 90);
		tempObject->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));

		//check pos is valid 
		CheckEnemyPos(tempObject->position, tempObject->sourceRect);

		tempObject->texture = texture1;

		tempObject->health = 15;
		tempObject->damage = 15; 
	}
	else if (enemyType == Enemies::gear)
	{
		Enemies::gearEnemy* tempObject = new Enemies::gearEnemy();
		_wavespawner->gears.push_back(tempObject);

		tempObject->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		tempObject->sourceRect = new Rect(0.0f, 0.0f, 42, 42);
		tempObject->textureOuter = texture1;
		tempObject->textureinner = texture2;

		CheckEnemyPos(tempObject->position, tempObject->sourceRect);

		tempObject->health = 35;
		tempObject->damage = 5;
	}
}

void Pacman::CheckEnemyPos(Vector2* position, Rect* sourceRect)
{
	if (position->X + sourceRect->Width + 96 > Graphics::GetViewportWidth()) { position->X = Graphics::GetViewportWidth() - sourceRect->Width - 96; }
	if (position->X < 96) { position->X = 96; }
	if (position->Y + sourceRect->Height + 96 > Graphics::GetViewportHeight()) { position->Y = Graphics::GetViewportHeight() - sourceRect->Height - 96; }
	if (position->Y < 96) { position->Y = 96; }
}

void Pacman::UpdateGear(Enemies::gearEnemy* gearObj, int elapsedTime)
{
	gearObj->_CurrentFrameTime += elapsedTime;

	if (gearObj->_CurrentFrameTime >  _cGearFrameTime)
	{
		gearObj->_Frame++;

		float angle = 0; // on first rotation will set projectiles to move in plus shape
		if (gearObj->_Frame >= 2)
		{
			gearObj->_Frame = 0;
			angle = 22.5; // on second rotation projectiles move in cross shape
		}

		for (int i = 0; i < 4; i++)
		{
			SpawnProjectile(gearObj->sourceRect, gearObj->position, NULL, angle, false, false);
			angle += 55;
		}
		gearObj->_CurrentFrameTime = 0;
	}

}

void Pacman::MoveProjectiles(Projectile* projectileUpdating)
{
	projectileUpdating->_projectilePosition->X += projectileUpdating->speed * cos(projectileUpdating->angle);
	projectileUpdating->_projectilePosition->Y += projectileUpdating->speed * sin(projectileUpdating->angle);
}

void Pacman::SpawnProjectile(Rect* passedRect, Vector2* passedPosition, Vector2* targetPosition, float passedAngle, bool enlarged, bool playerSpawned)
{
	Projectile* spawnedProjectile = new Projectile();
	spawnedProjectile->_projectilePosition = new Vector2(passedRect->Width / 2.0f + passedPosition->X, passedRect->Height / 2.0f + passedPosition->Y);

	if (playerSpawned) //spawns an instance of player bullet to ignore collision later so player doesnt die from their own shots 
	{ 
		projectilesPlayer.push_back(spawnedProjectile); 
		spawnedProjectile->_targetPosition = new Vector2(targetPosition->X, targetPosition->Y);
		int diffX = spawnedProjectile->_targetPosition->X - spawnedProjectile->_projectilePosition->X;
		int diffY = spawnedProjectile->_targetPosition->Y - spawnedProjectile->_projectilePosition->Y;

		spawnedProjectile->angle = (float)atan2(diffY, diffX); // calculates angle needed to shoot towards where the player mouse isd
	}
	else { projectilesEnemies.push_back(spawnedProjectile);
		spawnedProjectile->angle = passedAngle; // passes whether the angle needs to be in + or x direction set in for loop 
		cout << spawnedProjectile->angle << endl; 
	}
	
	//for minotaur attacking mechanic 
	if (!enlarged || playerSpawned)
	{ 
		spawnedProjectile->_projectileRect = new Rect(0.0f, 0.0f, 8, 8);
		spawnedProjectile->_projectileSheetTexture = _bulletTex;
		spawnedProjectile->_centerPoint = new Vector2(spawnedProjectile->_projectileRect->Width / 2.0f, spawnedProjectile->_projectileRect->Height / 2.0f);
	}
	else 
	{ 
		float lalala = passedAngle / 55; 
		if (lalala == 0) { spawnedProjectile->rotation = 90; }
		else if (lalala == 1) { spawnedProjectile->rotation = 0; 
		spawnedProjectile->angle = 165;
		}
		else if (lalala == 2) { spawnedProjectile->rotation = 270; }
		else if (lalala == 3) { spawnedProjectile->rotation = 180;
		spawnedProjectile->angle = 55;
		}

		spawnedProjectile->_projectileRect = new Rect(0.0f, 0.0f, 116, 36);
		spawnedProjectile->_projectileSheetTexture = _attackTex;
		
		spawnedProjectile->_centerPoint = new Vector2(spawnedProjectile->_projectileRect->Width / 2.0f, spawnedProjectile->_projectileRect->Height / 2.0f);
		spawnedProjectile->_projectilePosition->X -= spawnedProjectile->_projectileRect->X; 
	} 
	
	spawnedProjectile->speed = 4; 
}

bool Pacman::deleteProjectile(Projectile* projectile) //delete when hit wall
{
	if (projectile->_projectilePosition->X + projectile->_projectileRect->Width + 64 > Graphics::GetViewportWidth()) { return true; }
	if (projectile->_projectilePosition->X < 64) { return true; }
	if (projectile->_projectilePosition->Y + projectile->_projectileRect->Height + 96 > Graphics::GetViewportHeight()) { return true; }
	if (projectile->_projectilePosition->Y < 32) { return true; }

	return false; 
}

bool Pacman::CheckHealth(int health)
{
	if (health <= 0) { return true; }
	
	return false;
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	

	SpriteBatch::BeginDraw(); // Starts Drawing
	SpriteBatch::Draw(_GameBackground, _StartRectangle, nullptr);

	std::stringstream stream;
	stream << "Health: " << _player->health;
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::White);

	std::stringstream scoreStream; scoreStream << "Score: " << playerScore;
	SpriteBatch::DrawString(scoreStream.str().c_str(), _scoreTextPosition, Color::White);

	for (int i = 0; i < projectilesEnemies.size(); i++)
	{
		SpriteBatch::Draw(projectilesEnemies[i]->_projectileSheetTexture, projectilesEnemies[i]->_projectilePosition, projectilesEnemies[i]->_projectileRect, projectilesEnemies[i]->_centerPoint, 1.0f, projectilesEnemies[i]->rotation, Color::White, SpriteEffect::NONE);
	}

	if (!_player->dead)
	{
		SpriteBatch::Draw(_player->_pacmanTexture, _player->_pacmanPosition, _player->_pacmanSourceRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE); // Draws Pacman
	}
	
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		SpriteBatch::Draw(_munchie[i]->_munchieSheetTexture, _munchie[i]->_munchiePosition, _munchie[i]->_munchieRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
	}
	
	for (int i = 0; i < _wavespawner->Minotaurs.size(); i++)
	{
		SpriteBatch::Draw(_wavespawner->Minotaurs[i]->texture, _wavespawner->Minotaurs[i]->position, _wavespawner->Minotaurs[i]->sourceRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
	}

	for (int i = 0; i < _wavespawner->gears.size(); i++)
	{
		SpriteBatch::Draw(_wavespawner->gears[i]->textureOuter, _wavespawner->gears[i]->position, _wavespawner->gears[i]->sourceRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
		SpriteBatch::Draw(_wavespawner->gears[i]->textureinner, _wavespawner->gears[i]->position, _wavespawner->gears[i]->sourceRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
	}

	for (int i = 0; i < projectilesPlayer.size(); i++)
	{
		SpriteBatch::Draw(projectilesPlayer[i]->_projectileSheetTexture, projectilesPlayer[i]->_projectilePosition, projectilesPlayer[i]->_projectileRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
	}

	//SpriteBatch::Draw(_cherryTexture, _cherryPosition, _cherrySourceRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
	SpriteBatch::Draw(_GameShadow, _StartRectangle, nullptr);

	//draws menu
	if (_paused)
	{
		//std::stringstream menuStream; menuStream << "PAUSED!";
		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);// SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	if (_showStart)
	{
		SpriteBatch::Draw(_StartBackground, _StartRectangle, nullptr); 
	}

	if (endGameScreen)
	{
		SpriteBatch::Draw(endGameScreenBackground, _StartRectangle, nullptr);
	}

	
	
	// Draws String
	
	SpriteBatch::EndDraw(); // Ends Drawing

	
}






