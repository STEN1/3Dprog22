#pragma once
#include <unordered_map>

class Input
{
public:
	inline static std::unordered_map<int, bool> Keyboard;
	inline static std::unordered_map<int, bool> Mouse;
	inline static std::pair<int, int> MousePos{};
	inline static std::pair<float, float> MousePosDelta{};
	inline static int LastMouseWheelDelta{};
};