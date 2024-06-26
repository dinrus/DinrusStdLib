#define GUIPLATFORM_KEYCODES
K_BACK       = 8,
K_BACKSPACE  = 8,

K_TAB        = 9,

K_RETURN     = 13,
K_ENTER      = 13,
K_ESCAPE     = 27,

K_SPACE      = 32,

K_DELETE     = XK_Delete + K_DELTA,

K_SHIFT_KEY  = XK_Shift_L + K_DELTA,
K_CTRL_KEY   = XK_Control_L + K_DELTA,
K_ALT_KEY    = XK_Meta_L + K_DELTA,
K_CAPSLOCK   = XK_Caps_Lock + K_DELTA,
K_PRIOR      = XK_Page_Up + K_DELTA,
K_PAGEUP     = XK_Page_Up + K_DELTA,
K_NEXT       = XK_Page_Down + K_DELTA,
K_PAGEDOWN   = XK_Page_Down + K_DELTA,
K_END        = XK_End + K_DELTA,
K_HOME       = XK_Home + K_DELTA,
K_LEFT       = XK_Left + K_DELTA,
K_UP         = XK_Up + K_DELTA,
K_RIGHT      = XK_Right + K_DELTA,
K_DOWN       = XK_Down + K_DELTA,
K_INSERT     = XK_Insert + K_DELTA,

K_NUMPAD0    = XK_KP_0 + K_DELTA,
K_NUMPAD1    = XK_KP_1 + K_DELTA,
K_NUMPAD2    = XK_KP_2 + K_DELTA,
K_NUMPAD3    = XK_KP_3 + K_DELTA,
K_NUMPAD4    = XK_KP_4 + K_DELTA,
K_NUMPAD5    = XK_KP_5 + K_DELTA,
K_NUMPAD6    = XK_KP_6 + K_DELTA,
K_NUMPAD7    = XK_KP_7 + K_DELTA,
K_NUMPAD8    = XK_KP_8 + K_DELTA,
K_NUMPAD9    = XK_KP_9 + K_DELTA,
K_MULTIPLY   = XK_KP_Multiply + K_DELTA,
K_ADD        = XK_KP_Add + K_DELTA,
K_SEPARATOR  = XK_KP_Separator + K_DELTA,
K_SUBTRACT   = XK_KP_Subtract + K_DELTA,
K_DECIMAL    = XK_KP_Decimal + K_DELTA,
K_DIVIDE     = XK_KP_Divide + K_DELTA,

K_SCROLL     = XK_Scroll_Lock + K_DELTA,

K_PLUS       = XK_plus|K_DELTA,
K_MINUS      = XK_minus|K_DELTA,
K_COMMA      = 0x2c|K_DELTA,
K_PERIOD     = XK_period|K_DELTA,
K_SEMICOLON  = XK_semicolon|K_DELTA,
K_SLASH      = 0x5f|K_DELTA,
K_GRAVE      = XK_grave|K_DELTA,
K_LBRACKET   = XK_bracketleft|K_DELTA,
K_BACKSLASH  = 0x5c|K_DELTA,
K_RBRACKET   = XK_bracketright|K_DELTA,
K_QUOTEDBL   = XK_quotedbl|K_DELTA,

K_F1         = XK_F1 + K_DELTA,
K_F2         = XK_F2 + K_DELTA,
K_F3         = XK_F3 + K_DELTA,
K_F4         = XK_F4 + K_DELTA,
K_F5         = XK_F5 + K_DELTA,
K_F6         = XK_F6 + K_DELTA,
K_F7         = XK_F7 + K_DELTA,
K_F8         = XK_F8 + K_DELTA,
K_F9         = XK_F9 + K_DELTA,
K_F10        = XK_F10 + K_DELTA,
K_F11        = XK_F11 + K_DELTA,
K_F12        = XK_F12 + K_DELTA,

K_A          = 'A' + K_DELTA,
K_B          = 'B' + K_DELTA,
K_C          = 'C' + K_DELTA,
K_D          = 'D' + K_DELTA,
K_E          = 'E' + K_DELTA,
K_F          = 'F' + K_DELTA,
K_G          = 'G' + K_DELTA,
K_H          = 'H' + K_DELTA,
K_I          = 'I' + K_DELTA,
K_J          = 'J' + K_DELTA,
K_K          = 'K' + K_DELTA,
K_L          = 'L' + K_DELTA,
K_M          = 'M' + K_DELTA,
K_N          = 'N' + K_DELTA,
K_O          = 'O' + K_DELTA,
K_P          = 'P' + K_DELTA,
K_Q          = 'Q' + K_DELTA,
K_R          = 'R' + K_DELTA,
K_S          = 'S' + K_DELTA,
K_T          = 'T' + K_DELTA,
K_U          = 'U' + K_DELTA,
K_V          = 'V' + K_DELTA,
K_W          = 'W' + K_DELTA,
K_X          = 'X' + K_DELTA,
K_Y          = 'Y' + K_DELTA,
K_Z          = 'Z' + K_DELTA,
K_0          = '0' + 128 + K_DELTA,
K_1          = '1' + 128 + K_DELTA,
K_2          = '2' + 128 + K_DELTA,
K_3          = '3' + 128 + K_DELTA,
K_4          = '4' + 128 + K_DELTA,
K_5          = '5' + 128 + K_DELTA,
K_6          = '6' + 128 + K_DELTA,
K_7          = '7' + 128 + K_DELTA,
K_8          = '8' + 128 + K_DELTA,
K_9          = '9' + 128 + K_DELTA,

K_CTRL_LBRACKET  = K_CTRL|XK_bracketleft|K_DELTA,
K_CTRL_RBRACKET  = K_CTRL|XK_bracketright|K_DELTA,
K_CTRL_MINUS     = K_CTRL|0x2d|K_DELTA,
K_CTRL_GRAVE     = K_CTRL|XK_grave|K_DELTA,
K_CTRL_SLASH     = K_CTRL|0x5f|K_DELTA,
K_CTRL_BACKSLASH = K_CTRL|0x5c|K_DELTA,
K_CTRL_COMMA     = K_CTRL|0x2c|K_DELTA,
K_CTRL_PERIOD    = K_CTRL|XK_period|K_DELTA,
K_CTRL_SEMICOLON = K_CTRL|XK_semicolon|K_DELTA,
K_CTRL_EQUAL     = K_CTRL|0x3d|K_DELTA,
K_CTRL_APOSTROPHE= K_CTRL|0x27|K_DELTA,

K_BREAK      = XK_Pause|K_DELTA,
