#pragma once
#include "GamePad/GamePad.h"
#include <DirectXMath.h>
#include <LuaTalker.h>
#include <tuple>


//LUA DEFINES
#define LUA_GAMEPAD "Gamepad"
#define LUA_GAMEPAD_RESUME "Resume"
#define LUA_GAMEPAD_SUSPEND "Suspend"
#define LUA_GAMEPAD_IS_CONNECTED "IsConnected"
#define LUA_GAMEPAD_IS_A_PRESSED "IsAPressed"
#define LUA_GAMEPAD_IS_B_PRESSED "IsBPressed"
#define LUA_GAMEPAD_IS_Y_PRESSED "IsYPressed"
#define LUA_GAMEPAD_IS_X_PRESSED "IsXPressed"
#define LUA_GAMEPAD_GET_LEFT_STICK_Y "GetLeftStickY"
#define LUA_GAMEPAD_GET_LEFT_STICK_X "GetLeftStickX"
#define LUA_GAMEPAD_GET_RIGHT_STICK_Y "GetRightStickY"
#define LUA_GAMEPAD_GET_RIGHT_STICK_X "GetRightStickX"
#define LUA_GAMEPAD_GET_LEFT_STICK_POS "GetLeftStickPos"
#define LUA_GAMEPAD_GET_RIGHT_STICK_POS "GetRightStickPos"
#define LUA_GAMEPAD_IS_LEFT_STICK_PRESSED "IsLeftStickPressed"
#define LUA_GAMEPAD_IS_RIGHT_STICK_PRESSED "IsRightStickPressed"
#define LUA_GAMEPAD_IS_LT_PRESSED "IsLTPressed"
#define LUA_GAMEPAD_IS_RT_PRESSED "IsRTPressed"
#define LUA_GAMEPAD_GET_LT_VALUE "GetLTValue"
#define LUA_GAMEPAD_GET_RT_VALUE "GetRTValue"
#define LUA_GAMEPAD_IS_DP_LEFT_PRESSED "IsDPLeftPressed"
#define LUA_GAMEPAD_IS_DP_RIGHT_PRESSED "IsDPRightPressed"
#define LUA_GAMEPAD_IS_DP_UP_PRESSED "IsDPUpPressed"
#define LUA_GAMEPAD_IS_DP_DOWN_PRESSED "IsDPDownPressed"
#define LUA_GAMEPAD_IS_LB_PRESSED "IsLBPressed"
#define LUA_GAMEPAD_IS_RB_PRESSED "IsRBPressed"
#define LUA_GAMEPAD_SET_LEFT_VIBRATION "SetLeftVibration"
#define LUA_GAMEPAD_SET_RIGHT_VIBRATION "SetRightVibration"
#define LUA_GAMEPAD_SET_VIBRATION "SetVibration"
#define LUA_GAMEPAD_SET_VIBRATION_LT_RT "SetVibrationLTRT"


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

	static float GetLeftStickYPosition();
	static float GetLeftStickXPosition();

	static float GetRightStickYPosition();
	static float GetRightStickXPosition();

	static DirectX::XMFLOAT2 GetLeftStickposition();
	static std::tuple<float, float> GetLeftStickPosLUA();
	static DirectX::XMFLOAT2 GetRightStickposition();
	static std::tuple<float, float> GetRightStickPosLUA();

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

	static void SetLeftVibration(const float & left);
	static void SetRightVibration(const float & left);
	static void SetVibration(const float & left, const float & right);
	static void SetVibration(const float & left, const float & right, const float & leftTrigger, const float & rightTrigger);

	static void REGISTER_TO_LUA();
};
