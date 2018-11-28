#pragma once
#include "GamePad/GamePad.h"
#include <DirectXMath.h>
class GamePadHandler
{
private:
	static std::unique_ptr<DirectX::GamePad> m_gamePad;
	static DirectX::GamePad::ButtonStateTracker m_buttons;
	static DirectX::GamePad::State m_state;

	static float m_leftMotorVibration;
	static float m_rightMotorVibration;
	static float m_rightTriggerMotorVibration;
	static float m_leftTriggerMotorVibration;



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

	static bool IsAReleased();
	static bool IsBReleased();
	static bool IsYReleased();
	static bool IsXReleased();

	static float GetLeftStickYPosition();
	static float GetLeftStickXPosition();

	static float GetRightStickYPosition();
	static float GetRightStickXPosition();

	static DirectX::XMFLOAT2 GetLeftStickposition();
	static DirectX::XMFLOAT2 GetRightStickposition();

	static bool IsLeftStickPressed();
	static bool IsRightStickPressed();

	static bool IsRightTriggerPressed();
	static bool IsLeftTriggerPressed();

	static float RightTrigger();
	static float LeftTrigger();

	static bool IsRightDpadPressed();
	static bool IsLeftDpadPressed();
	static bool IsUpDpadPressed();
	static bool IsDownDpadPressed();

	static bool IsLeftShoulderPressed();
	static bool IsRightShoulderPressed();

	static bool IsLeftShoulderReleased();
	static bool IsRightShoulderReleased();

	static void SetLeftVibration(const float & left);
	static void SetRightVibration(const float & left);
	static void SetVibration(const float & left, const float & right);
	static void SetVibration(const float & left, const float & right, const float & leftTrigger, const float & rightTrigger);

	static bool IsSelectPressed();
	static bool IsSelectReleased();
	static bool IsStartPressed();
};
