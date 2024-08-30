#pragma once

namespace Atlas
{
	typedef enum class KeyCode : uint16_t
	{
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

// From glfw3.h
#define ATLAS_KEY_SPACE           ::Atlas::Key::Space
#define ATLAS_KEY_APOSTROPHE      ::Atlas::Key::Apostrophe    /* ' */
#define ATLAS_KEY_COMMA           ::Atlas::Key::Comma         /* , */
#define ATLAS_KEY_MINUS           ::Atlas::Key::Minus         /* - */
#define ATLAS_KEY_PERIOD          ::Atlas::Key::Period        /* . */
#define ATLAS_KEY_SLASH           ::Atlas::Key::Slash         /* / */
#define ATLAS_KEY_0               ::Atlas::Key::D0
#define ATLAS_KEY_1               ::Atlas::Key::D1
#define ATLAS_KEY_2               ::Atlas::Key::D2
#define ATLAS_KEY_3               ::Atlas::Key::D3
#define ATLAS_KEY_4               ::Atlas::Key::D4
#define ATLAS_KEY_5               ::Atlas::Key::D5
#define ATLAS_KEY_6               ::Atlas::Key::D6
#define ATLAS_KEY_7               ::Atlas::Key::D7
#define ATLAS_KEY_8               ::Atlas::Key::D8
#define ATLAS_KEY_9               ::Atlas::Key::D9
#define ATLAS_KEY_SEMICOLON       ::Atlas::Key::Semicolon     /* ; */
#define ATLAS_KEY_EQUAL           ::Atlas::Key::Equal         /* = */
#define ATLAS_KEY_A               ::Atlas::Key::A
#define ATLAS_KEY_B               ::Atlas::Key::B
#define ATLAS_KEY_C               ::Atlas::Key::C
#define ATLAS_KEY_D               ::Atlas::Key::D
#define ATLAS_KEY_E               ::Atlas::Key::E
#define ATLAS_KEY_F               ::Atlas::Key::F
#define ATLAS_KEY_G               ::Atlas::Key::G
#define ATLAS_KEY_H               ::Atlas::Key::H
#define ATLAS_KEY_I               ::Atlas::Key::I
#define ATLAS_KEY_J               ::Atlas::Key::J
#define ATLAS_KEY_K               ::Atlas::Key::K
#define ATLAS_KEY_L               ::Atlas::Key::L
#define ATLAS_KEY_M               ::Atlas::Key::M
#define ATLAS_KEY_N               ::Atlas::Key::N
#define ATLAS_KEY_O               ::Atlas::Key::O
#define ATLAS_KEY_P               ::Atlas::Key::P
#define ATLAS_KEY_Q               ::Atlas::Key::Q
#define ATLAS_KEY_R               ::Atlas::Key::R
#define ATLAS_KEY_S               ::Atlas::Key::S
#define ATLAS_KEY_T               ::Atlas::Key::T
#define ATLAS_KEY_U               ::Atlas::Key::U
#define ATLAS_KEY_V               ::Atlas::Key::V
#define ATLAS_KEY_W               ::Atlas::Key::W
#define ATLAS_KEY_X               ::Atlas::Key::X
#define ATLAS_KEY_Y               ::Atlas::Key::Y
#define ATLAS_KEY_Z               ::Atlas::Key::Z
#define ATLAS_KEY_LEFT_BRACKET    ::Atlas::Key::LeftBracket   /* [ */
#define ATLAS_KEY_BACKSLASH       ::Atlas::Key::Backslash     /* \ */
#define ATLAS_KEY_RIGHT_BRACKET   ::Atlas::Key::RightBracket  /* ] */
#define ATLAS_KEY_GRAVE_ACCENT    ::Atlas::Key::GraveAccent   /* ` */
#define ATLAS_KEY_WORLD_1         ::Atlas::Key::World1        /* non-US #1 */
#define ATLAS_KEY_WORLD_2         ::Atlas::Key::World2        /* non-US #2 */

/* Function keys */
#define ATLAS_KEY_ESCAPE          ::Atlas::Key::Escape
#define ATLAS_KEY_ENTER           ::Atlas::Key::Enter
#define ATLAS_KEY_TAB             ::Atlas::Key::Tab
#define ATLAS_KEY_BACKSPACE       ::Atlas::Key::Backspace
#define ATLAS_KEY_INSERT          ::Atlas::Key::Insert
#define ATLAS_KEY_DELETE          ::Atlas::Key::Delete
#define ATLAS_KEY_RIGHT           ::Atlas::Key::Right
#define ATLAS_KEY_LEFT            ::Atlas::Key::Left
#define ATLAS_KEY_DOWN            ::Atlas::Key::Down
#define ATLAS_KEY_UP              ::Atlas::Key::Up
#define ATLAS_KEY_PAGE_UP         ::Atlas::Key::PageUp
#define ATLAS_KEY_PAGE_DOWN       ::Atlas::Key::PageDown
#define ATLAS_KEY_HOME            ::Atlas::Key::Home
#define ATLAS_KEY_END             ::Atlas::Key::End
#define ATLAS_KEY_CAPS_LOCK       ::Atlas::Key::CapsLock
#define ATLAS_KEY_SCROLL_LOCK     ::Atlas::Key::ScrollLock
#define ATLAS_KEY_NUM_LOCK        ::Atlas::Key::NumLock
#define ATLAS_KEY_PRINT_SCREEN    ::Atlas::Key::PrintScreen
#define ATLAS_KEY_PAUSE           ::Atlas::Key::Pause
#define ATLAS_KEY_F1              ::Atlas::Key::F1
#define ATLAS_KEY_F2              ::Atlas::Key::F2
#define ATLAS_KEY_F3              ::Atlas::Key::F3
#define ATLAS_KEY_F4              ::Atlas::Key::F4
#define ATLAS_KEY_F5              ::Atlas::Key::F5
#define ATLAS_KEY_F6              ::Atlas::Key::F6
#define ATLAS_KEY_F7              ::Atlas::Key::F7
#define ATLAS_KEY_F8              ::Atlas::Key::F8
#define ATLAS_KEY_F9              ::Atlas::Key::F9
#define ATLAS_KEY_F10             ::Atlas::Key::F10
#define ATLAS_KEY_F11             ::Atlas::Key::F11
#define ATLAS_KEY_F12             ::Atlas::Key::F12
#define ATLAS_KEY_F13             ::Atlas::Key::F13
#define ATLAS_KEY_F14             ::Atlas::Key::F14
#define ATLAS_KEY_F15             ::Atlas::Key::F15
#define ATLAS_KEY_F16             ::Atlas::Key::F16
#define ATLAS_KEY_F17             ::Atlas::Key::F17
#define ATLAS_KEY_F18             ::Atlas::Key::F18
#define ATLAS_KEY_F19             ::Atlas::Key::F19
#define ATLAS_KEY_F20             ::Atlas::Key::F20
#define ATLAS_KEY_F21             ::Atlas::Key::F21
#define ATLAS_KEY_F22             ::Atlas::Key::F22
#define ATLAS_KEY_F23             ::Atlas::Key::F23
#define ATLAS_KEY_F24             ::Atlas::Key::F24
#define ATLAS_KEY_F25             ::Atlas::Key::F25

/* Keypad */
#define ATLAS_KEY_KP_0            ::Atlas::Key::KP0
#define ATLAS_KEY_KP_1            ::Atlas::Key::KP1
#define ATLAS_KEY_KP_2            ::Atlas::Key::KP2
#define ATLAS_KEY_KP_3            ::Atlas::Key::KP3
#define ATLAS_KEY_KP_4            ::Atlas::Key::KP4
#define ATLAS_KEY_KP_5            ::Atlas::Key::KP5
#define ATLAS_KEY_KP_6            ::Atlas::Key::KP6
#define ATLAS_KEY_KP_7            ::Atlas::Key::KP7
#define ATLAS_KEY_KP_8            ::Atlas::Key::KP8
#define ATLAS_KEY_KP_9            ::Atlas::Key::KP9
#define ATLAS_KEY_KP_DECIMAL      ::Atlas::Key::KPDecimal
#define ATLAS_KEY_KP_DIVIDE       ::Atlas::Key::KPDivide
#define ATLAS_KEY_KP_MULTIPLY     ::Atlas::Key::KPMultiply
#define ATLAS_KEY_KP_SUBTRACT     ::Atlas::Key::KPSubtract
#define ATLAS_KEY_KP_ADD          ::Atlas::Key::KPAdd
#define ATLAS_KEY_KP_ENTER        ::Atlas::Key::KPEnter
#define ATLAS_KEY_KP_EQUAL        ::Atlas::Key::KPEqual

#define ATLAS_KEY_LEFT_SHIFT      ::Atlas::Key::LeftShift
#define ATLAS_KEY_LEFT_CONTROL    ::Atlas::Key::LeftControl
#define ATLAS_KEY_LEFT_ALT        ::Atlas::Key::LeftAlt
#define ATLAS_KEY_LEFT_SUPER      ::Atlas::Key::LeftSuper
#define ATLAS_KEY_RIGHT_SHIFT     ::Atlas::Key::RightShift
#define ATLAS_KEY_RIGHT_CONTROL   ::Atlas::Key::RightControl
#define ATLAS_KEY_RIGHT_ALT       ::Atlas::Key::RightAlt
#define ATLAS_KEY_RIGHT_SUPER     ::Atlas::Key::RightSuper
#define ATLAS_KEY_MENU            ::Atlas::Key::Menu