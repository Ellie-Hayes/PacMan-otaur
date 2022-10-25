#include "Pacman.h"

#include <sstream>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPlayerFrameTime(250), _cMunchieFrameTime(500), _cCherryFrameTime(700)
{
	_frameCount = 0;
	_paused = false;
	_pKeyDown = false;
	_showStart = true; 
	_spaceKeyDown = false; 
	_playerDirection = 0; 

	_playerCurrentFrameTime = 0; 
	_playerFrame = 0;
	_munchieCurrentFrameTime = 0;
	_munchieFrame = 0;
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
	delete _pacmanTexture;
	delete _pacmanSourceRect;
	delete _munchieRect;
	delete _menuBackground; 
	delete _StartBackground;
	delete _munchieSheetTexture;
	delete _cherrySourceRect;
	delete _cherryTexture; 
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacmanTexture = new Texture2D();
	_pacmanTexture->Load("Textures/Pacman.tga", false);
	_pacmanPosition = new Vector2(350.0f, 350.0f);
	_pacmanSourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Munchie
	_munchieRect = new Rect(0.0f, 0.0f, 12, 12);
	_munchiePosition = new Vector2(100.0f, 450.0f);
	_munchieSheetTexture = new Texture2D();
	_munchieSheetTexture->Load("Textures/MuchieSpritesheet.png", true);

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

	//Movement 
	if (!_paused && !_showStart)
	{
		if (keyboardState->IsKeyDown(Input::Keys::D))
		{
			_pacmanPosition->X += _cPacmanSpeed * elapsedTime; //Moves Pacman across X axis
			_playerDirection = 0;
		}
		if (keyboardState->IsKeyDown(Input::Keys::A))
		{
			_pacmanPosition->X += -_cPacmanSpeed * elapsedTime;
			_playerDirection = 2;
		}
		if (keyboardState->IsKeyDown(Input::Keys::W))
		{
			_pacmanPosition->Y += -_cPacmanSpeed * elapsedTime; //Moves Pacman across X axis
			_playerDirection = 3;
		}
		if (keyboardState->IsKeyDown(Input::Keys::S))
		{
			_pacmanPosition->Y += _cPacmanSpeed * elapsedTime;
			_playerDirection = 1;
		}

	}

	//Menus

	if (keyboardState->IsKeyDown(Input::Keys::P) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
	}
	
	if (keyboardState->IsKeyUp(Input::Keys::P))
		_pKeyDown = false;

	if (keyboardState->IsKeyDown(Input::Keys::SPACE))
		_showStart = false;

	_playerCurrentFrameTime += elapsedTime;

	if (_playerCurrentFrameTime > _cPlayerFrameTime) //if frameTime is greater than 250 increment the sprite by one, if its greater than 2 loop back
	{
		_playerFrame++; 

		if (_playerFrame >= 2) //only two frames change this for more sprites 
		{
			_playerFrame = 0;
		}
		_playerCurrentFrameTime = 0; 
	}

	//munchie
	_munchieCurrentFrameTime += elapsedTime;

	if (_munchieCurrentFrameTime > _cMunchieFrameTime)
	{
		_munchieFrame++;

		if (_munchieFrame >= 2)
		{
			_munchieFrame = 0;
		}
		_munchieCurrentFrameTime = 0;
	}

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

	//rotating character
	_pacmanSourceRect->Y = _pacmanSourceRect->Height * _playerDirection;
	_pacmanSourceRect->X = _pacmanSourceRect->Width * _playerFrame; //uses spritesheets then changes the rect to show the right texture
	_munchieRect->X = _munchieRect->Width * _munchieFrame;

	//Wrapping
	if (_pacmanPosition->X + _pacmanSourceRect->Width > Graphics::GetViewportWidth()) { _pacmanPosition->X = 0; }
	if (_pacmanPosition->X < 0){ _pacmanPosition->X = Graphics::GetViewportWidth() - _pacmanSourceRect->Width; }
	if (_pacmanPosition->Y + _pacmanSourceRect->Height > Graphics::GetViewportHeight()) { _pacmanPosition->Y = 0; }
	if (_pacmanPosition->Y < 0) { _pacmanPosition->Y = Graphics::GetViewportHeight() - _pacmanSourceRect->Height; }
	
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacmanPosition->X << " Y: " << _pacmanPosition->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	SpriteBatch::Draw(_pacmanTexture, _pacmanPosition, _pacmanSourceRect); // Draws Pacman
	SpriteBatch::Draw(_munchieSheetTexture, _munchiePosition, _munchieRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
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