#pragma once

#include <cstdint>

#if defined(_WIN32)
#define NOMINMAX
#include <Windows.h>

#elif defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>

#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/XInput2.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#else
#error Unsupported platform

#endif

namespace Input {

	enum class Key : uint16_t {

		A = 0x04, B, C, D, E, F, G, H, I, J,
		K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

		Num1 = 0x1E,
		Num2, Num3, Num4, Num5,
		Num6, Num7, Num8, Num9, Num0,

		Enter = 0x28,
		Escape,
		Backspace,
		Tab,
		Space,

		Right = 0x4F,
		Left,
		Down,
		Up,

		LeftControl = 0xE0,
		LeftShift,
		LeftAlt,
		LeftGUI,
		RightControl,
		RightShift,
		RightAlt,
		RightGUI

	};

	enum class MouseButton : uint8_t {

		Left,
		Right,
		Middle,
		X1,
		X2

	};

#pragma region Initialization

#if defined(__linux__)

	inline Display* display = nullptr;

	[[nodiscard]] inline bool initialize() {
		if (display)
			return true;

		display = XOpenDisplay(nullptr);
		return display != nullptr;
	}

	inline void shutdown() {
		if (display) {
			XCloseDisplay(display);
			display = nullptr;
		}
	}

#else

	[[nodiscard]] inline bool initialize() noexcept {
		return true;
	}

	inline void shutdown() noexcept {}

#endif

#pragma endregion

#pragma region Keyboard Mapping

#if defined(_WIN32)

	[[nodiscard]] inline WORD native_key(Key key) noexcept {
		const auto value = static_cast<uint16_t>(key);

		if (value >= 0x04 && value <= 0x1D)
			return static_cast<WORD>('A' + value - 0x04);

		if (value >= 0x1E && value <= 0x26)
			return static_cast<WORD>('1' + value - 0x1E);

		switch (key) {

		case Key::Num0:          return '0';
		case Key::Enter:         return VK_RETURN;
		case Key::Escape:        return VK_ESCAPE;
		case Key::Backspace:     return VK_BACK;
		case Key::Tab:           return VK_TAB;
		case Key::Space:         return VK_SPACE;

		case Key::Right:         return VK_RIGHT;
		case Key::Left:          return VK_LEFT;
		case Key::Down:          return VK_DOWN;
		case Key::Up:            return VK_UP;

		case Key::LeftControl:   return VK_LCONTROL;
		case Key::LeftShift:     return VK_LSHIFT;
		case Key::LeftAlt:       return VK_LMENU;
		case Key::LeftGUI:       return VK_LWIN;
		case Key::RightControl:  return VK_RCONTROL;
		case Key::RightShift:    return VK_RSHIFT;
		case Key::RightAlt:      return VK_RMENU;
		case Key::RightGUI:      return VK_RWIN;

		default:                 return 0;

		}
	}

#elif defined(__APPLE__)

	[[nodiscard]] inline CGKeyCode native_key(Key key) noexcept {
		switch (key) {

		case Key::A: return 0x00;
		case Key::B: return 0x0B;
		case Key::C: return 0x08;
		case Key::D: return 0x02;
		case Key::E: return 0x0E;
		case Key::F: return 0x03;
		case Key::G: return 0x05;
		case Key::H: return 0x04;
		case Key::I: return 0x22;
		case Key::J: return 0x26;
		case Key::K: return 0x28;
		case Key::L: return 0x25;
		case Key::M: return 0x2E;
		case Key::N: return 0x2D;
		case Key::O: return 0x1F;
		case Key::P: return 0x23;
		case Key::Q: return 0x0C;
		case Key::R: return 0x0F;
		case Key::S: return 0x01;
		case Key::T: return 0x11;
		case Key::U: return 0x20;
		case Key::V: return 0x09;
		case Key::W: return 0x0D;
		case Key::X: return 0x07;
		case Key::Y: return 0x10;
		case Key::Z: return 0x06;

		case Key::Num1: return 0x12;
		case Key::Num2: return 0x13;
		case Key::Num3: return 0x14;
		case Key::Num4: return 0x15;
		case Key::Num5: return 0x17;
		case Key::Num6: return 0x16;
		case Key::Num7: return 0x1A;
		case Key::Num8: return 0x1C;
		case Key::Num9: return 0x19;
		case Key::Num0: return 0x1D;

		case Key::Enter: return 0x24;
		case Key::Escape: return 0x35;
		case Key::Backspace: return 0x33;
		case Key::Tab: return 0x30;
		case Key::Space: return 0x31;

		case Key::Right: return 0x7C;
		case Key::Left: return 0x7B;
		case Key::Down: return 0x7D;
		case Key::Up: return 0x7E;

		case Key::LeftControl: return 0x3B;
		case Key::LeftShift: return 0x38;
		case Key::LeftAlt: return 0x3A;
		case Key::LeftGUI: return 0x37;
		case Key::RightControl: return 0x3E;
		case Key::RightShift: return 0x3C;
		case Key::RightAlt: return 0x3D;
		case Key::RightGUI: return 0x36;

		default: return 0;

		}
	}

#elif defined(__linux__)

	[[nodiscard]] inline KeySym native_key(Key key) noexcept {
		const auto value = static_cast<uint16_t>(key);

		if (value >= 0x04 && value <= 0x1D)
			return XK_a + value - 0x04;

		if (value >= 0x1E && value <= 0x26)
			return XK_1 + value - 0x1E;

		switch (key) {

		case Key::Num0: return XK_0;
		case Key::Enter: return XK_Return;
		case Key::Escape: return XK_Escape;
		case Key::Backspace: return XK_BackSpace;
		case Key::Tab: return XK_Tab;
		case Key::Space: return XK_space;

		case Key::Right: return XK_Right;
		case Key::Left: return XK_Left;
		case Key::Down: return XK_Down;
		case Key::Up: return XK_Up;

		case Key::LeftControl: return XK_Control_L;
		case Key::LeftShift: return XK_Shift_L;
		case Key::LeftAlt: return XK_Alt_L;
		case Key::LeftGUI: return XK_Super_L;
		case Key::RightControl: return XK_Control_R;
		case Key::RightShift: return XK_Shift_R;
		case Key::RightAlt: return XK_Alt_R;
		case Key::RightGUI: return XK_Super_R;

		default: return NoSymbol;

		}
	}

#endif

#pragma endregion

#pragma region Keyboard Events

	[[nodiscard]] inline bool key_down(Key key) {
#if defined(_WIN32)

		const WORD code = native_key(key);
		if (!code) return false;

		INPUT input{};
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = code;

		return SendInput(1, &input, sizeof(INPUT)) == 1;

#elif defined(__APPLE__)

		const CGKeyCode code = native_key(key);
		if (!code && key != Key::A) return false;

		CGEventRef event = CGEventCreateKeyboardEvent(
			nullptr, code, true);

		if (!event) return false;

		CGEventPost(kCGHIDEventTap, event);
		CFRelease(event);

		return true;

#elif defined(__linux__)

		if (!display) return false;

		const KeySym symbol = native_key(key);
		if (symbol == NoSymbol) return false;

		const KeyCode code = XKeysymToKeycode(display, symbol);
		if (!code) return false;

		const bool result =
			XTestFakeKeyEvent(display, code, True, CurrentTime);

		XFlush(display);
		return result != 0;

#endif
	}

	[[nodiscard]] inline bool key_up(Key key) {
#if defined(_WIN32)

		const WORD code = native_key(key);
		if (!code) return false;

		INPUT input{};
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = code;
		input.ki.dwFlags = KEYEVENTF_KEYUP;

		return SendInput(1, &input, sizeof(INPUT)) == 1;

#elif defined(__APPLE__)

		const CGKeyCode code = native_key(key);
		if (!code && key != Key::A) return false;

		CGEventRef event = CGEventCreateKeyboardEvent(
			nullptr, code, false);

		if (!event) return false;

		CGEventPost(kCGHIDEventTap, event);
		CFRelease(event);

		return true;

#elif defined(__linux__)

		if (!display) return false;

		const KeySym symbol = native_key(key);
		if (symbol == NoSymbol) return false;

		const KeyCode code = XKeysymToKeycode(display, symbol);
		if (!code) return false;

		const bool result =
			XTestFakeKeyEvent(display, code, False, CurrentTime);

		XFlush(display);
		return result != 0;

#endif
	}

	[[nodiscard]] inline bool is_mouse_down(MouseButton button) {
		const int code = native_button_state(button);
		return code && (GetAsyncKeyState(code) & 0x8000) != 0;
	}

	[[nodiscard]] inline bool press_key(Key key) {
		if (!key_down(key))
			return false;

		return key_up(key);
	}

#pragma endregion

#pragma region Mouse Events

#if defined(_WIN32)

	[[nodiscard]] inline DWORD native_button_down(MouseButton button) noexcept {
		switch (button) {
		case MouseButton::Left:   return MOUSEEVENTF_LEFTDOWN;
		case MouseButton::Right:  return MOUSEEVENTF_RIGHTDOWN;
		case MouseButton::Middle: return MOUSEEVENTF_MIDDLEDOWN;
		case MouseButton::X1:
		case MouseButton::X2:     return MOUSEEVENTF_XDOWN;
		default:                  return 0;
		}
	}

	[[nodiscard]] inline DWORD native_button_up(MouseButton button) noexcept {
		switch (button) {
		case MouseButton::Left:   return MOUSEEVENTF_LEFTUP;
		case MouseButton::Right:  return MOUSEEVENTF_RIGHTUP;
		case MouseButton::Middle: return MOUSEEVENTF_MIDDLEUP;
		case MouseButton::X1:
		case MouseButton::X2:     return MOUSEEVENTF_XUP;
		default:                  return 0;
		}
	}

	[[nodiscard]] inline DWORD native_button_data(MouseButton button) noexcept {
		switch (button) {
		case MouseButton::X1: return XBUTTON1;
		case MouseButton::X2: return XBUTTON2;
		default:              return 0;
		}
	}

	inline int native_button_state(MouseButton button) noexcept {
		switch (button) {
		case MouseButton::Left:   return VK_LBUTTON;
		case MouseButton::Right:  return VK_RBUTTON;
		case MouseButton::Middle: return VK_MBUTTON;
		case MouseButton::X1:     return VK_XBUTTON1;
		case MouseButton::X2:     return VK_XBUTTON2;
		default:                  return 0;
		}
	}

	[[nodiscard]] inline bool is_mouse_down(MouseButton button) {
		const int code = native_button_state(button);
		return code && (GetAsyncKeyState(code) & 0x8000) != 0;
	}

#elif defined(__APPLE__)

	[[nodiscard]] inline CGMouseButton native_button(MouseButton button) noexcept {
		switch (button) {
		case MouseButton::Left:   return kCGMouseButtonLeft;
		case MouseButton::Right:  return kCGMouseButtonRight;
		case MouseButton::Middle: return kCGMouseButtonCenter;
		case MouseButton::X1:     return static_cast<CGMouseButton>(3);
		case MouseButton::X2:     return static_cast<CGMouseButton>(4);
		default:                  return kCGMouseButtonLeft;
		}
	}

	[[nodiscard]] inline bool is_mouse_down(MouseButton button) {
		return CGEventSourceButtonState(
			kCGEventSourceStateCombinedSessionState,
			native_button(button));
	}

#elif defined(__linux__)

	[[nodiscard]] inline unsigned int native_button(MouseButton button) noexcept {
		switch (button) {
		case MouseButton::Left:   return 1;
		case MouseButton::Middle: return 2;
		case MouseButton::Right:  return 3;
		case MouseButton::X1:     return 8;
		case MouseButton::X2:     return 9;
		default:                  return 0;
		}
	}

	[[nodiscard]] inline bool is_mouse_up(MouseButton button) {
		return !is_mouse_down(button);
	}

#endif

	[[nodiscard]] inline bool mouse_down(MouseButton button) {
#if defined(_WIN32)

		INPUT input{};
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = native_button_down(button);
		input.mi.mouseData = native_button_data(button);

		return input.mi.dwFlags &&
			SendInput(1, &input, sizeof(INPUT)) == 1;

#elif defined(__APPLE__)

		const CGMouseButton code = native_button(button);

		const CGEventType type =
			button == MouseButton::Left ? kCGEventLeftMouseDown :
			button == MouseButton::Right ? kCGEventRightMouseDown :
			kCGEventOtherMouseDown;

		CGEventRef current = CGEventCreate(nullptr);
		if (!current) return false;

		const CGPoint position = CGEventGetLocation(current);
		CFRelease(current);

		CGEventRef event = CGEventCreateMouseEvent(
			nullptr, type, position, code);

		if (!event) return false;

		CGEventPost(kCGHIDEventTap, event);
		CFRelease(event);

		return true;

#elif defined(__linux__)

		if (!display) return false;

		const unsigned int code = native_button(button);
		if (!code) return false;

		const bool result =
			XTestFakeButtonEvent(display, code, True, CurrentTime);

		XFlush(display);
		return result != 0;

#endif
	}

	[[nodiscard]] inline bool mouse_up(MouseButton button) {
#if defined(_WIN32)

		INPUT input{};
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = native_button_up(button);
		input.mi.mouseData = native_button_data(button);

		return input.mi.dwFlags &&
			SendInput(1, &input, sizeof(INPUT)) == 1;

#elif defined(__APPLE__)

		const CGMouseButton code = native_button(button);

		const CGEventType type =
			button == MouseButton::Left ? kCGEventLeftMouseUp :
			button == MouseButton::Right ? kCGEventRightMouseUp :
			kCGEventOtherMouseUp;

		CGEventRef current = CGEventCreate(nullptr);
		if (!current) return false;

		const CGPoint position = CGEventGetLocation(current);
		CFRelease(current);

		CGEventRef event = CGEventCreateMouseEvent(
			nullptr, type, position, code);

		if (!event) return false;

		CGEventPost(kCGHIDEventTap, event);
		CFRelease(event);

		return true;

#elif defined(__linux__)

		if (!display) return false;

		const unsigned int code = native_button(button);
		if (!code) return false;

		const bool result =
			XTestFakeButtonEvent(display, code, False, CurrentTime);

		XFlush(display);
		return result != 0;

#endif
	}

	[[nodiscard]] inline bool click(MouseButton button) {
		if (!mouse_down(button))
			return false;

		return mouse_up(button);
	}

	[[nodiscard]] inline bool move_mouse(int x, int y) {
#if defined(_WIN32)

		const int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		const int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

		if (width <= 0 || height <= 0)
			return false;

		INPUT input{};
		input.type = INPUT_MOUSE;
		input.mi.dx = static_cast<LONG>(
			(x - GetSystemMetrics(SM_XVIRTUALSCREEN)) * 65535LL /
			(width - 1));

		input.mi.dy = static_cast<LONG>(
			(y - GetSystemMetrics(SM_YVIRTUALSCREEN)) * 65535LL /
			(height - 1));

		input.mi.dwFlags =
			MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE |
			MOUSEEVENTF_VIRTUALDESK;

		return SendInput(1, &input, sizeof(INPUT)) == 1;

#elif defined(__APPLE__)

		CGEventRef event = CGEventCreateMouseEvent(
			nullptr, kCGEventMouseMoved,
			CGPointMake(x, y), kCGMouseButtonLeft);

		if (!event) return false;

		CGEventPost(kCGHIDEventTap, event);
		CFRelease(event);

		return true;

#elif defined(__linux__)

		if (!display) return false;

		const bool result =
			XTestFakeMotionEvent(display, -1, x, y, CurrentTime);

		XFlush(display);
		return result != 0;

#endif
	}

	[[nodiscard]] inline bool move_mouse_relative(int dx, int dy) {
#if defined(_WIN32)

		INPUT input{};
		input.type = INPUT_MOUSE;
		input.mi.dx = dx;
		input.mi.dy = dy;
		input.mi.dwFlags = MOUSEEVENTF_MOVE;

		return SendInput(1, &input, sizeof(INPUT)) == 1;

#elif defined(__APPLE__)

		CGEventRef current = CGEventCreate(nullptr);
		if (!current) return false;

		const CGPoint position = CGEventGetLocation(current);
		CFRelease(current);

		return move_mouse(
			static_cast<int>(position.x) + dx,
			static_cast<int>(position.y) + dy);

#elif defined(__linux__)

		if (!display) return false;

		const bool result =
			XTestFakeRelativeMotionEvent(
				display, dx, dy, CurrentTime);

		XFlush(display);
		return result != 0;

#endif
	}

#pragma endregion

#pragma region Key Status

#if defined(_WIN32)

	[[nodiscard]] inline bool is_key_down(Key key) {
		const WORD code = native_key(key);
		if (!code) return false;

		return (GetAsyncKeyState(code) & 0x8000) != 0;
	}

#elif defined(__APPLE__)

	[[nodiscard]] inline bool is_key_down(Key key) {
		return CGEventSourceKeyState(
			kCGEventSourceStateCombinedSessionState,
			native_key(key));
	}

#elif defined (__linux__)

	[[nodiscard]] inline bool is_key_down(Key key) {
		if (!display) return false;

		const KeySym symbol = native_key(key);
		if (symbol == NoSymbol) return false;

		const KeyCode code = XKeysymToKeycode(display, symbol);
		if (!code) return false;

		char keys[32];
		XQueryKeymap(display, keys);

		return (keys[code >> 3] & (1 << (code & 7))) != 0;
	}

#endif

	[[nodiscard]] bool is_key_up(Key key) {
		return !is_key_down(key);
	}

#pragma endregion

#pragma region Mouse Status

	//[[nodiscard]] bool is_mouse_down(MouseButton button);
	//[[nodiscard]] bool is_mouse_up(MouseButton button);

	struct MousePosition {
		int x;
		int y;
	};

#if defined(_WIN32)

	[[nodiscard]] inline MousePosition get_mouse_position() {
		POINT point{};

		GetCursorPos(&point);

		return { point.x, point.y };
	}

#elif defined(__APPLE__)

	[[nodiscard]] inline MousePosition get_mouse_position() {
		CGEventRef event = CGEventCreate(nullptr);

		if (!event)
			return {};

		const CGPoint point = CGEventGetLocation(event);
		CFRelease(event);

		return {
			static_cast<int>(point.x),
			static_cast<int>(point.y)
		};
	}

#elif defined (__linux__)

	[[nodiscard]] inline MousePosition get_mouse_position() {
		if (!display)
			return {};

		Window root = DefaultRootWindow(display);
		Window child{};

		int rootX{}, rootY{};
		int windowX{}, windowY{};

		unsigned int mask{};

		XQueryPointer(
			display, root, &root, &child,
			&rootX, &rootY,
			&windowX, &windowY,
			&mask);

		return { rootX, rootY };
	}

#endif

#pragma endregion

#pragma endregion

}