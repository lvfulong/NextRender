#include "InputEvents.h"

InputEvent::InputEvent(Platform &platform, EventSource source) :
    platform{platform},
    source{source}
{
}

const Platform &InputEvent::get_platform() const
{
	return platform;
}

EventSource InputEvent::get_source() const
{
	return source;
}

KeyInputEvent::KeyInputEvent(Platform &platform, KeyCode code, KeyAction action) :
    InputEvent{platform, EventSource::Keyboard},
    code{code},
    action{action}
{
}

KeyCode KeyInputEvent::get_code() const
{
	return code;
}

KeyAction KeyInputEvent::get_action() const
{
	return action;
}

MouseButtonInputEvent::MouseButtonInputEvent(Platform &platform, MouseButton button, MouseAction action, float pos_x, float pos_y) :
    InputEvent{platform, EventSource::Mouse},
    button{button},
    action{action},
    pos_x{pos_x},
    pos_y{pos_y}
{
}

MouseButton MouseButtonInputEvent::get_button() const
{
	return button;
}

MouseAction MouseButtonInputEvent::get_action() const
{
	return action;
}

float MouseButtonInputEvent::get_pos_x() const
{
	return pos_x;
}

float MouseButtonInputEvent::get_pos_y() const
{
	return pos_y;
}

TouchInputEvent::TouchInputEvent(Platform &platform, int32_t pointer_id, std::size_t touch_points, TouchAction action, float pos_x, float pos_y) :
    InputEvent{platform, EventSource::Touchscreen},
    action{action},
    pointer_id{pointer_id},
    touch_points{touch_points},
    pos_x{pos_x},
    pos_y{pos_y}
{
}

TouchAction TouchInputEvent::get_action() const
{
	return action;
}

int32_t TouchInputEvent::get_pointer_id() const
{
	return pointer_id;
}

std::size_t TouchInputEvent::get_touch_points() const
{
	return touch_points;
}

float TouchInputEvent::get_pos_x() const
{
	return pos_x;
}

float TouchInputEvent::get_pos_y() const
{
	return pos_y;
}
