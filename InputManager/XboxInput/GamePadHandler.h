#pragma once
#include "GamePad/GamePad.h"
#include <DirectXMath.h>
class GamePadHandler
{
private:
	static std::unique_ptr<DirectX::GamePad> m_gamePad;
	static DirectX::GamePad::ButtonStateTracker m_buttons;
	static DirectX::GamePad::State m_state;

	GamePadHandler();
	~GamePadHandler();

public:

	static GamePadHandler& Instance();

	static void Resume();
	static void Suspend();
	static bool UpdateState();
	static bool IsConnected();

	static bool IsAPressed();
	static bool IsBPressed();
	static bool IsYPressed();
	static bool IsXPressed();

	static float GetLeftStickYPosition();
	static float GetLeftStickXPosition();

	static float GetRightStickYPosition();
	static float GetRightStickXPosition();

	static DirectX::XMFLOAT2 GetLeftStickposition();
	static DirectX::XMFLOAT2 GetRightStickposition();

	static bool IsLeftStickPressed();

	static bool IsRightTriggerPressed();
	static bool IsLeftTriggerPressed();

	static bool IsRightDpadPressed();
	static bool IsLeftDpadPressed();
	static bool IsUpDpadPressed();
	static bool IsDownDpadPressed();

	static bool IsLeftShoulderPressed();
	static bool IsRightShoulderPressed();

};
