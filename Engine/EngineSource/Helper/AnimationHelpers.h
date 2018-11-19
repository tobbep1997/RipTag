#pragma once

namespace Animation
{
	float constexpr lerp(float from, float to, float fraction)
	{
		return from + fraction * (to - from);
	}

	float constexpr getNewValueInNewRange(float minOld, float maxOld, float minNew, float maxNew, float value)
	{
		return ((minOld - maxOld) * ((value - maxNew) / (minNew - maxOld))) + maxNew;
	}
}