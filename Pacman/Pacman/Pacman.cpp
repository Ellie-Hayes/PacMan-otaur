#include "Pacman.h"

#include <sstream>
#include <vector>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPlayerFrameTime(100), _cMunchieFrameTime(500), _cCherryFrameTime(700), _cMinotaurFrameTime(100)
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


	/*for (int i = 0; i < ENEMYCOUNT; i++)
	{
		_Enemies[i] = new Enemies::MovingEnemy();
		_Enemies[i]->_CurrentFrameTime = 0;
		_Enemies[i]->_frameCount = 0;
		_Enemies[i]->_Frame = 0;
		_Enemies[i]->_frameTime = rand() % 500 + 50;
		_Enemies[i]->speed = 0.1; 
		_Enemies[i]->direction = 0; 
	}*/
	//_munchie = new Collectable();
	_paused = false;
	_pKeyDown = false;
	_showStart = true; 
	_spaceKeyDown = false; 

	_player->_playerDirection = 0;
	_player->_playerCurrentFrameTime = 0;
	_player->_playerFrame = 0;
	_player->dead = false; 
	_player->currentState = _player->Idle;

	_cherryCurrentFrameTime = 0;
	_cherryFrame = 0;

	_pop = new SoundEffect();
	

	//WaveSpawner initialisation 
	
	_wavespawner->state = _wavespawner->Spawning;

	_wavespawner->nextWave = 0;
	_wavespawner->timeBetweenWaves = 5;
	_wavespawner->WaveCountdown = _wavespawner->timeBetweenWaves;
	_wavespawner->SearchCountdown = 1;
	_wavespawner->spawnRate = 0.5f;
	_wavespawner->EnemyTypeCount = 2;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();
	Audio::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _player->_pacmanTexture;
	delete _player->_pacmanSourceRect;
	delete _menuBackground; 
	delete _StartBackground;
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

	/*for (int i = 0; i < ENEMYCOUNT; i++)
	{
		delete _Enemies[i]->sourceRect;
		delete _Enemies[i]->position;
		delete _Enemies[i];
	}
	delete[] _Enemies;*/

	delete _pop; 
}

void Pacman::LoadContent()
{
	// Load Pacman
	_player->_pacmanTexture = new Texture2D();
	_player->_pacmanTexture->Load("Textures/PlayerSpritesheet.png", false);
	_player->_pacmanPosition = new Vector2(350.0f, 350.0f);
	_player->_pacmanSourceRect = new Rect(0.0f, 0.0f, 30, 30);
	_player->speedMuiltiply = 1.0f; 

	// Load Munchie

	Texture2D* munchieTex = new Texture2D(); 
	munchieTex->Load("Textures/MuchieSpritesheet.png", true);
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i]->_munchieRect = new Rect(0.0f, 0.0f, 12, 12);
		_munchie[i]->_munchiePosition = new Vector2((rand() % Graphics:: GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_munchie[i]->_munchieSheetTexture = new Texture2D();
		_munchie[i]->_munchieSheetTexture = munchieTex;
	}

	/*Texture2D* ghostTex = new Texture2D();
	ghostTex->Load("Textures/GhostBlue.png", true);
	for (int i = 0; i < ENEMYCOUNT; i++)
	{
		_Enemies[i]->sourceRect = new Rect(0.0f, 0.0f, 19, 19);
		_Enemies[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_Enemies[i]->texture = new Texture2D();
		_Enemies[i]->texture = ghostTex;
	}*/

	_cherrySourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_cherryPosition = new Vector2(500.0f, 450.0f);
	_cherryTexture = new Texture2D();
	_cherryTexture->Load("Textures/CherrySpritesheet.png", true);

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	// Menu stuffs
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	_StartBackground = new Texture2D();
	_StartBackground->Load("Textures/PacmanBackground.jpg", false);
	_StartRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_StartStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 3.0f);

	_pop->Load("Sounds/pop.wav");
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
			

			for (int i = 0; i < MUNCHIECOUNT; i++)
			{
				UpdateMunchie(_munchie[i], elapsedTime);
			}

			UpdateCherry(elapsedTime);

			for (int i = 0; i < _wavespawner->Minotaurs.size(); i++)
			{
				UpdateGhost(_wavespawner->Minotaurs[i], elapsedTime);
			}
		}
	}
	
}


void Pacman::Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState)
{
	if (state->IsKeyDown(Input::Keys::LEFTSHIFT)) { _player->speedMuiltiply = 2.0; ;
	}
	else {
		_player->speedMuiltiply = 1.0;
		
	}

	if (_player->currentState == _player->Hurt || _player->currentState == _player->Attacking){ return; }
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



	//Mouse
	if (mouseState ->LeftButton == Input::ButtonState::PRESSED)
	{
		_cherryPosition->X = mouseState->X; 
		_cherryPosition->Y = mouseState->Y;
	}
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(pauseKey) && !_pKeyDown)
	{
		_pKeyDown = true;
		Audio::Play(_pop); 
		_wavespawner->Minotaurs.clear();
		_wavespawner->gears.clear();
		//_paused = !_paused;
	}

	if (state->IsKeyUp(pauseKey))
		_pKeyDown = false;
}

void Pacman::CheckViewportCollision()
{
	//Wrapping
	if (_player->_pacmanPosition->X + _player->_pacmanSourceRect->Width > Graphics::GetViewportWidth()) { _player->_pacmanPosition->X = 0; }
	if (_player->_pacmanPosition->X < 0) { _player->_pacmanPosition->X = Graphics::GetViewportWidth() - _player->_pacmanSourceRect->Width; }
	if (_player->_pacmanPosition->Y + _player->_pacmanSourceRect->Height > Graphics::GetViewportHeight()) { _player->_pacmanPosition->Y = 0; }
	if (_player->_pacmanPosition->Y < 0) { _player->_pacmanPosition->Y = Graphics::GetViewportHeight() - _player->_pacmanSourceRect->Height; }
}

void Pacman::UpdatePacman(int elapsedTime, Player::PlayerState state)
{
	_player->_playerCurrentFrameTime += elapsedTime;

	if (_player->_playerCurrentFrameTime > _cPlayerFrameTime) //if frameTime is greater than 250 increment the sprite by one, if its greater than 2 loop back
	{
		_player->_playerFrame++;

		if (_player->_playerFrame >= 8) //only two frames change this for more sprites 
		{
			_player->_playerFrame = 0;
				
			switch (state)
			{
			case Player::Idle:
				break;
			case Player::Walking:
				break;
			case Player::Attacking:
				//spawn the projectile
				_player->currentState = _player->Idle;
				break;
			case Player::Dead:
				//end game
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
		if (munchie->_munchieFrame >= 2) { munchie->_munchieFrame = 0; }
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

void Pacman::CheckGhostCollision()
{
	
}

void Pacman::UpdateGhost(Enemies::MovingEnemy* ghost, int elapsedTime)
{
	if (ghost->direction == 0) { ghost->position->X += ghost->speed * elapsedTime; }
	else if (ghost->direction == 1)
	{
		ghost->position->X -= ghost->speed * elapsedTime;
	}

	if (ghost->position->X + ghost->sourceRect->Width >= Graphics::GetViewportWidth())
	{
		ghost->direction = 1;
	}
	else if (ghost->position->X <= 0)
	{
		ghost->direction = 0; 
	}
	
}

void Pacman::SpawnWave()
{
	if (_wavespawner->state == _wavespawner->Spawning)
	{
		for (int i = 0; i < _wavespawner->EnemyTypeCount; i++)
		{
			//runs for loop twice for each enemy
			Texture2D* ghostTex = new Texture2D();
			ghostTex->Load("Textures/MinotaurSpritesheet.png", true);
			
			Texture2D* GearOuterText = new Texture2D();
			GearOuterText->Load("Textures/CogOuter.png", true);

			Texture2D* GearInnerTex = new Texture2D();
			GearInnerTex->Load("Textures/CogInner.png", true);
			if (i == 0)
			{
				for (int i = 0; i < _wavespawner->enemyArray[_wavespawner->nextWave]; i++)
				{
					NewEnemy(Enemies::Minotaur, ghostTex, NULL);
				}
			}
			else
			{
				for (int i = 0; i < _wavespawner->enemyArray[_wavespawner->nextWave]; i++)
				{
					NewEnemy(Enemies::gear, GearOuterText, GearInnerTex);
				}
				
			}

			_wavespawner->nextWave++; 
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
		tempObject->speed = 0.1;
		tempObject->direction = 0;

		tempObject->sourceRect = new Rect(0.0f, 0.0f, 30, 30);
		tempObject->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		tempObject->texture = new Texture2D();
		tempObject->texture = texture1;
	}
	else if (enemyType == Enemies::gear)
	{
		Enemies::gearEnemy* tempObject = new Enemies::gearEnemy();
		_wavespawner->gears.push_back(tempObject);

		tempObject->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		tempObject->sourceRect = new Rect(0.0f, 0.0f, 14, 14);
		tempObject->textureOuter = new Texture2D();
		tempObject->textureOuter = texture1;
		tempObject->textureinner = new Texture2D();
		tempObject->textureinner = texture2;
	}
}

bool Pacman::EnemiesAlive()
{
	return false; 
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _player->_pacmanPosition->X << " Y: " << _player->_pacmanPosition->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	if (!_player->dead)
	{
		SpriteBatch::Draw(_player->_pacmanTexture, _player->_pacmanPosition, _player->_pacmanSourceRect, Vector2::Zero, 3.0f, 0.0f, Color::White, SpriteEffect::NONE); // Draws Pacman
	}
	
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		//SpriteBatch::Draw(_munchie[i]->_munchieSheetTexture, _munchie[i]->_munchiePosition, _munchie[i]->_munchieRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
	}
	
	SpriteBatch::Draw(_cherryTexture, _cherryPosition, _cherrySourceRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);

	/*for (int i = 0; i < ENEMYCOUNT; i++)
	{
		SpriteBatch::Draw(_Enemies[i]->texture, _Enemies[i]->position, _Enemies[i]->sourceRect, Vector2::Zero, 6.0f, 0.0f, Color::White, SpriteEffect::NONE);
	}*/

	for (int i = 0; i < _wavespawner->Minotaurs.size(); i++)
	{
		SpriteBatch::Draw(_wavespawner->Minotaurs[i]->texture, _wavespawner->Minotaurs[i]->position, _wavespawner->Minotaurs[i]->sourceRect, Vector2::Zero, 3.0f, 0.0f, Color::White, SpriteEffect::NONE);
	}

	for (int i = 0; i < _wavespawner->gears.size(); i++)
	{
		SpriteBatch::Draw(_wavespawner->gears[i]->textureOuter, _wavespawner->gears[i]->position, _wavespawner->gears[i]->sourceRect, Vector2::Zero, 3.0f, 0.0f, Color::White, SpriteEffect::NONE);
		SpriteBatch::Draw(_wavespawner->gears[i]->textureinner, _wavespawner->gears[i]->position, _wavespawner->gears[i]->sourceRect, Vector2::Zero, 3.0f, 0.0f, Color::White, SpriteEffect::NONE);
	}

	SpriteBatch::Draw(_cherryTexture, _cherryPosition, _cherrySourceRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);

	//draws menu
	if (_paused)
	{
		std::stringstream menuStream; menuStream << "PAUSED!";
		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr); SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	if (_showStart)
	{
		std::stringstream menuStream; menuStream << "Press space to start!";
		SpriteBatch::Draw(_StartBackground, _StartRectangle, nullptr); 
		SpriteBatch::DrawString(menuStream.str().c_str(), _StartStringPosition, Color::Magenta);
	}

	
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	SpriteBatch::EndDraw(); // Ends Drawing

	
}






