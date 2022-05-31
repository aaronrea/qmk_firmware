#include QMK_KEYBOARD_H

#include "oneshot.h"
#include "swapper.h"

#define HOME G(KC_LEFT)
#define END G(KC_RGHT)
#define FWD G(KC_RBRC)
#define BACK G(KC_LBRC)
#define TABL G(S(KC_LBRC))
#define TABR G(S(KC_RBRC))
#define SPCL A(G(KC_LEFT))
#define SPC_R A(G(KC_RGHT))
#define LA_SYM MO(SYM)
#define LA_NAV MO(NAV)

enum layers {
    DEF,
    SYM,
    NAV,
    NUM,
};

enum keycodes {
    // Custom oneshot mod implementation with no timers.
    OS_SHFT = SAFE_RANGE,
    OS_CTRL,
    OS_ALT,
    OS_CMD,

    SW_WIN,  // Switch to next window         (cmd-tab)
    SW_LANG, // Switch to next input language (ctl-spc)
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [DEF] = LAYOUT_moonlander(  
        _______, KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    _______, _______, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    _______,
        _______, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    _______, _______, KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, _______,
        _______, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    _______, _______, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_ENT,  _______,
        _______, _______, _______, _______, LA_NAV,  KC_LSFT,                   KC_SPC,  LA_SYM,  _______, _______, _______, _______,
        _______, _______, _______, _______, _______,          _______, _______,          _______, _______, _______, _______, _______,  
                                            _______, _______, _______, _______, _______, _______                      
    ),

    [SYM] = LAYOUT_moonlander( 
        _______, KC_ESC,  KC_LCBR, KC_LBRC, KC_LPRN, KC_TILD, _______, _______, KC_GRV,  KC_RPRN, KC_RBRC, KC_RCBR, KC_PIPE, _______,
        _______, KC_EQL,  KC_PLUS, KC_MINS, KC_UNDS, KC_BSLS, _______, _______, KC_SLSH, OS_CMD,  OS_ALT,  OS_CTRL, OS_SHFT, _______,
        _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, _______, _______, KC_CIRC, KC_AMPR, KC_ASTR, KC_QUOT, KC_DQUO, _______,
        _______, _______, _______, _______, _______, KC_LSFT,                   KC_SPC,  _______, _______, _______, _______, _______, 
        _______, _______, _______, _______, _______,          _______, _______,          _______, _______, _______, _______, _______,  
                                            _______, _______, _______, _______, _______, _______
    ),

    [NAV] = LAYOUT_moonlander(
        _______, KC_TAB,  SW_WIN,  TABL,    TABR,    KC_VOLU, _______, _______, RESET,   HOME,    KC_UP,   END,     KC_DEL,  _______,
        _______, OS_SHFT, OS_CTRL, OS_ALT,  OS_CMD,  KC_VOLD, _______, _______, KC_CAPS, KC_LEFT, KC_DOWN, KC_RGHT, KC_BSPC, _______,
        _______, SPCL,    SPC_R,   BACK,    FWD,     KC_MPLY, _______, _______, RGB_MOD, KC_PGDN, KC_PGUP, SW_LANG, KC_ENT,  _______,
        _______, _______, _______, _______, _______, KC_LSFT,                   KC_SPC,  _______, _______, _______, _______, _______, 
        _______, _______, _______, _______, _______,          _______, _______,          _______, _______, _______, _______, _______,  
                                            _______, _______, _______, _______, _______, _______
    ),

    [NUM] = LAYOUT_moonlander(  
        _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    _______, _______, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    _______,
        _______, OS_SHFT, OS_CTRL, OS_ALT,  OS_CMD,  KC_F11,  _______, _______, KC_F10,  OS_CMD,  OS_ALT,  OS_CTRL, OS_SHFT, _______,
        _______, KC_F7,   KC_F5,   KC_F3,   KC_F1,   KC_F9,   _______, _______, KC_F8,   KC_F12,  KC_F2,   KC_F4,   KC_F6,   _______,
        _______, _______, _______, _______, _______, KC_LSFT,                   KC_SPC,  _______, _______, _______, _______, _______, 
        _______, _______, _______, _______, _______,          _______, _______,          _______, _______, _______, _______, _______,  
                                            _______, _______, _______, _______, _______, _______
    ),
};

bool is_oneshot_cancel_key(uint16_t keycode) {
    switch (keycode) {
    case LA_SYM:
    case LA_NAV:
        return true;
    default:
        return false;
    }
}

bool is_oneshot_ignored_key(uint16_t keycode) {
    switch (keycode) {
    case LA_SYM:
    case LA_NAV:
    case KC_LSFT:
    case OS_SHFT:
    case OS_CTRL:
    case OS_ALT:
    case OS_CMD:
        return true;
    default:
        return false;
    }
}

bool sw_win_active = false;
bool sw_lang_active = false;

oneshot_state os_shft_state = os_up_unqueued;
oneshot_state os_ctrl_state = os_up_unqueued;
oneshot_state os_alt_state = os_up_unqueued;
oneshot_state os_cmd_state = os_up_unqueued;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    update_swapper(
        &sw_win_active, KC_LGUI, KC_TAB, SW_WIN,
        keycode, record
    );
    update_swapper(
        &sw_lang_active, KC_LCTL, KC_SPC, SW_LANG,
        keycode, record
    );

    update_oneshot(
        &os_shft_state, KC_LSFT, OS_SHFT,
        keycode, record
    );
    update_oneshot(
        &os_ctrl_state, KC_LCTL, OS_CTRL,
        keycode, record
    );
    update_oneshot(
        &os_alt_state, KC_LALT, OS_ALT,
        keycode, record
    );
    update_oneshot(
        &os_cmd_state, KC_LCMD, OS_CMD,
        keycode, record
    );

    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, SYM, NAV, NUM);
}
