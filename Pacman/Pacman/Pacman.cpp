#include "Pacman.h"

#include <sstream>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPlayerFrameTime(250), _cMunchieFrameTime(500), _cCherryFrameTime(700)
{
	_player = new Player();

	//int i;
	//for (i = 0; i < 50; i++)
	//{
	//	_munchie[i] = new Collectable();
	//	_munchie[i]->_munchieCurrentFrameTime = 0; 
	//}

	_munchie = new Collectable();
	_munchie->_frameCount = 0;
	_paused = false;
	_pKeyDown = false;
	_showStart = true; 
	_spaceKeyDown = false; 
	_player->_playerDirection = 0;

	_player->_playerCurrentFrameTime = 0;
	_player->_playerFrame = 0;
	_munchie->_munchieCurrentFrameTime = 0;
	_munchie->_munchieFrame = 0;
	_cherryCurrentFrameTime = 0;
	_cherryFrame = 0;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _player->_pacmanTexture;
	delete _player->_pacmanSourceRect;
	delete _munchie->_munchieRect;
	delete _menuBackground; 
	delete _StartBackground;
	delete _munchie->_munchieSheetTexture;
	delete _cherrySourceRect;
	delete _cherryTexture; 
	delete _player;
	delete _munchie;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_player->_pacmanTexture = new Texture2D();
	_player->_pacmanTexture->Load("Textures/Pacman.tga", false);
	_player->_pacmanPosition = new Vector2(350.0f, 350.0f);
	_player->_pacmanSourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Munchie
	_munchie->_munchieRect = new Rect(0.0f, 0.0f, 12, 12);
	_munchie->_munchiePosition = new Vector2(100.0f, 450.0f);
	_munchie->_munchieSheetTexture = new Texture2D();
	_munchie->_munchieSheetTexture->Load("Textures/MuchieSpritesheet.png", true);

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

}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

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
			Input(elapsedTime, keyboardState);
			CheckViewportCollision();
			UpdatePacman(elapsedTime);
			UpdateMunchie(elapsedTime);
			UpdateCherry(elapsedTime);
		}
	}
	
}


void Pacman::Input(int elapsedTime, Input::KeyboardState* state)
{
	if (state->IsKeyDown(Input::Keys::D))
	{
		_player->_pacmanPosition->X += _cPacmanSpeed * elapsedTime; //Moves Pacman across X axis
		_player->_playerDirection = 0;
	}
	if (state->IsKeyDown(Input::Keys::A))
	{
		_player->_pacmanPosition->X += -_cPacmanSpeed * elapsedTime;
		_player->_playerDirection = 2;
	}
	if (state->IsKeyDown(Input::Keys::W))
	{
		_player->_pacmanPosition->Y += -_cPacmanSpeed * elapsedTime; //Moves Pacman across X axis
		_player->_playerDirection = 3;
	}
	if (state->IsKeyDown(Input::Keys::S))
	{
		_player->_pacmanPosition->Y += _cPacmanSpeed * elapsedTime;
		_player->_playerDirection = 1;
	}
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(pauseKey) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
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

void Pacman::UpdatePacman(int elapsedTime)
{
	_player->_playerCurrentFrameTime += elapsedTime;

	if (_player->_playerCurrentFrameTime > _cPlayerFrameTime) //if frameTime is greater than 250 increment the sprite by one, if its greater than 2 loop back
	{
		_player->_playerFrame++;

		if (_player->_playerFrame >= 2) //only two frames change this for more sprites 
		{
			_player->_playerFrame = 0;
		}
		_player->_playerCurrentFrameTime = 0;
	}

	//rotates pacman
	_player->_pacmanSourceRect->Y = _player->_pacmanSourceRect->Height * _player->_playerDirection;
	_player->_pacmanSourceRect->X = _player->_pacmanSourceRect->Width * _player->_playerFrame;  //uses spritesheets then changes the rect to show the right texture
}

void Pacman::UpdateMunchie(int elapsedTime)
{
	//munchie
	_munchie->_munchieCurrentFrameTime += elapsedTime;

	if (_munchie->_munchieCurrentFrameTime > _cMunchieFrameTime)
	{
		_munchie->_munchieFrame++;
		if (_munchie->_munchieFrame >= 2) { _munchie->_munchieFrame = 0; }
		_munchie->_munchieCurrentFrameTime = 0;
	}

	_munchie->_munchieRect->X = _munchie->_munchieRect->Width * _munchie->_munchieFrame;
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

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _player->_pacmanPosition->X << " Y: " << _player->_pacmanPosition->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	SpriteBatch::Draw(_player->_pacmanTexture, _player->_pacmanPosition, _player->_pacmanSourceRect); // Draws Pacman
	SpriteBatch::Draw(_munchie->_munchieSheetTexture, _munchie->_munchiePosition, _munchie->_munchieRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
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


