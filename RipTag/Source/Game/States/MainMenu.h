#pragma once
#include "State.h"

class Quad;
class Circle;
class TextInput;
class MainMenu : public State
{
private:
	Quad * playButton;
	Quad * quitButton;
	TextInput * m_textInput;


	Circle * c;
	long float cTimer;
public:
	MainMenu(RenderingManager * rm);
	~MainMenu();

	void Update(double deltaTime) override;

	void Draw() override;
};