#pragma once

// standard
#include <windows.h>
#include <vector>
#include <fstream>

// interface
#include "start/imgui/imgui.h"
#include "start/imgui/backend/ImGui_impl_dx11.h"
#include "start/imgui/backend/ImGui_impl_win32.h"
#include "start/imgui/imgui_internal.h"

#include "start/bytes.hxx"

static std::string key_code_to_text(int key_code) 
{
	switch (key_code)
	{
	case 0: return "none";
	case VK_BACK: return "backspace";
	case VK_TAB: return "tab";
	case VK_RETURN: return "enter";
	case VK_SHIFT: return "shift";
	case VK_CONTROL: return "ctrl";
	case VK_MENU: return "alt";
	case VK_PAUSE: return "pause";
	case VK_CAPITAL: return "capslock";
	case VK_ESCAPE: return "escape";
	case VK_SPACE: return "space";
	case VK_PRIOR: return "pageup";
	case VK_NEXT: return "pagedown";
	case VK_END: return "end";
	case VK_HOME: return "home";
	case VK_LEFT: return "left";
	case VK_UP: return "up";
	case VK_RIGHT: return "right";
	case VK_DOWN: return "down";
	case VK_SELECT: return "select";
	case VK_PRINT: return "print";
	case VK_EXECUTE: return "execute";
	case VK_SNAPSHOT: return "printscreen";
	case VK_INSERT: return "insert";
	case VK_DELETE: return "delete";
	case VK_HELP: return "help";
	case VK_LWIN: return "leftwindows";
	case VK_RWIN: return "rightwindows";
	case VK_APPS: return "application";
	case VK_SLEEP: return "sleep";
	case VK_NUMPAD0: return "numpad0";
	case VK_NUMPAD1: return "numpad1";
	case VK_NUMPAD2: return "numpad2";
	case VK_NUMPAD3: return "numpad3";
	case VK_NUMPAD4: return "numpad4";
	case VK_NUMPAD5: return "numpad5";
	case VK_NUMPAD6: return "numpad6";
	case VK_NUMPAD7: return "numpad7";
	case VK_NUMPAD8: return "numpad8";
	case VK_NUMPAD9: return "numpad9";
	case VK_MULTIPLY: return "multiply";
	case VK_ADD: return "add";
	case VK_SEPARATOR: return "separator";
	case VK_SUBTRACT: return "subtract";
	case VK_DECIMAL: return "decimal";
	case VK_DIVIDE: return "divide";
	case VK_F1: return "f1";
	case VK_F2: return "f2";
	case VK_F3: return "f3";
	case VK_F4: return "f4";
	case VK_F5: return "f5";
	case VK_F6: return "f6";
	case VK_F7: return "f7";
	case VK_F8: return "f8";
	case VK_F9: return "f9";
	case VK_F10: return "f10";
	case VK_F11: return "f11";
	case VK_F12: return "f12";
	case VK_F13: return "f13";
	case VK_F14: return "f14";
	case VK_F15: return "f15";
	case VK_F16: return "f16";
	case VK_F17: return "f17";
	case VK_F18: return "f18";
	case VK_F19: return "f19";
	case VK_F20: return "f20";
	case VK_F21: return "f21";
	case VK_F22: return "f22";
	case VK_F23: return "f23";
	case VK_F24: return "f24";
	case VK_NUMLOCK: return "numlock";
	case VK_SCROLL: return "scrolllock";
	case VK_LSHIFT: return "leftshift";
	case VK_RSHIFT: return "rightshift";
	case VK_LCONTROL: return "leftctrl";
	case VK_RCONTROL: return "rightctrl";
	case VK_LMENU: return "leftalt";
	case VK_RMENU: return "rightalt";
	case VK_BROWSER_BACK: return "browserback";
	case VK_BROWSER_FORWARD: return "browserforward";
	case VK_BROWSER_REFRESH: return "browserrefresh";
	case VK_BROWSER_STOP: return "browserstop";
	case VK_BROWSER_SEARCH: return "browsersearch";
	case VK_BROWSER_FAVORITES: return "browserfavorites";
	case VK_BROWSER_HOME: return "browserhome";
	case VK_VOLUME_MUTE: return "volumemute";
	case VK_VOLUME_DOWN: return "volumedown";
	case VK_VOLUME_UP: return "volumeup";
	case VK_MEDIA_NEXT_TRACK: return "medianexttrack";
	case VK_MEDIA_PREV_TRACK: return "mediaprevtrack";
	case VK_MEDIA_STOP: return "mediastop";
	case VK_MEDIA_PLAY_PAUSE: return "mediaplaypause";
	case VK_LAUNCH_MAIL: return "launchmail";
	case VK_LAUNCH_MEDIA_SELECT: return "launchmediaselect";
	case VK_LAUNCH_APP1: return "launchapp1";
	case VK_LAUNCH_APP2: return "launchapp2";
	case VK_OEM_1: return "semicolon";
	case VK_OEM_PLUS: return "equals";
	case VK_OEM_COMMA: return "comma";
	case VK_OEM_MINUS: return "minus";
	case VK_OEM_PERIOD: return "period";
	case VK_OEM_2: return "slash";
	case VK_OEM_3: return "tilde";
	case VK_OEM_4: return "leftbracket";
	case VK_OEM_5: return "backslash";
	case VK_OEM_6: return "rightbracket";
	case VK_OEM_7: return "quote";
	case VK_OEM_8: return "oem8";
	case VK_OEM_AX: return "oemax";
	case VK_OEM_102: return "oem102";
	case VK_PROCESSKEY: return "processkey";
	case VK_PACKET: return "packet";
	case VK_ATTN: return "attn";
	case VK_CRSEL: return "crsel";
	case VK_EXSEL: return "exsel";
	case VK_EREOF: return "ereof";
	case VK_PLAY: return "play";
	case VK_ZOOM: return "zoom";
	case VK_NONAME: return "noname";
	case VK_PA1: return "pa1";
	case VK_OEM_CLEAR: return "oemclear";
	case VK_RBUTTON: return "rightmouse";
	case VK_LBUTTON: return "leftmouse";
	case VK_MBUTTON: return "middlemouse";
	case VK_XBUTTON1: return "xbutton1";
	case VK_XBUTTON2: return "xbutton2";
	case 'A': return "A";
	case 'B': return "B";
	case 'C': return "C";
	case 'D': return "D";
	case 'E': return "E";
	case 'F': return "F";
	case 'G': return "G";
	case 'H': return "H";
	case 'I': return "I";
	case 'J': return "J";
	case 'K': return "K";
	case 'L': return "L";
	case 'M': return "M";
	case 'N': return "N";
	case 'O': return "O";
	case 'P': return "P";
	case 'Q': return "Q";
	case 'R': return "R";
	case 'S': return "S";
	case 'T': return "T";
	case 'U': return "U";
	case 'V': return "V";
	case 'W': return "W";
	case 'X': return "X";
	case 'Y': return "Y";
	case 'Z': return "Z";

	case '1': return "1";
	case '2': return "2";
	case '3': return "3";
	case '4': return "4";
	case '5': return "5";
	case '6': return "6";
	case '7': return "7";
	case '8': return "8";
	case '9': return "9";
	case '0': return "0";

	default: return "Unknown Key";
	}
}
