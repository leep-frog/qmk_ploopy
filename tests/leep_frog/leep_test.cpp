#include "keyboard_report_util.hpp"
#include "keycode.h"
#include "test_common.hpp"
#include "action_tapping.h"
#include "test_keymap_key.hpp"
#include "leep_keyboard.h"
#include "../../../../users/leep-frog/main.h"
#include "users/leep-frog/keyboard-main/leep_tap_dance_kb.h"

using testing::_;
using testing::InSequence;
// See lib/googletest/docs/reference/assertions.md
using ::testing::IsEmpty;

// Define lots of transparent key aliases because LEEP_ADD_KEY creates
// a variable for each key created, so we need different names.
const uint8_t TK_0 = KC_TRANSPARENT;
const uint8_t TK_1 = KC_TRANSPARENT;
const uint8_t TK_2 = KC_TRANSPARENT;
const uint8_t TK_3 = KC_TRANSPARENT;
const uint8_t TK_4 = KC_TRANSPARENT;
const uint8_t TK_5 = KC_TRANSPARENT;
const uint8_t TK_6 = KC_TRANSPARENT;

// We need these go betweens because CK_ABCs are macros and the nested macros pass the initial string values around (not the final macro)
const uint16_t ck_test = CK_TEST;
const uint16_t ck_shft = CK_SHFT;

class LeepFrog : public TestFixture {};

#define NEW_ROW = QK_USER_MAX - 1

uint8_t leep_key_layer = 0;
uint8_t leep_key_col = 0;
#define LEEP_ADD_KEY(key) KeymapKey k_##key = KeymapKey(leep_key_layer, leep_key_col++, 0, key); add_key(k_##key);
#define LEEP_KEY_ROW(layer, n, ...) leep_key_layer = layer; leep_key_col = 0; REDUCE_##n(LEEP_ADD_KEY, , __VA_ARGS__)

#define CONFIRM_RESET()   \
k_ck_test.press();        \
EXPECT_NO_REPORT(driver); \
run_one_scan_loop();      \
/* Confirm the running string is set to confirm this value is actually changing on each run. */ \
EXPECT_STREQ(test_message, "Running tests (waiting for release)..."); \
k_ck_test.release();      \
EXPECT_NO_REPORT(driver); \
run_one_scan_loop();      \
EXPECT_STREQ(test_message, "Success!");

/***************
* Unlock tests *
***************/

TEST_F(LeepFrog, UnlockBehavior) {
    TestDriver driver;
    InSequence s;
    LEEP_KEY_ROW(0, 3,
      KC_A,
      KC_D,
      ck_test
    )

    // Ignore key before unlocking
    k_KC_A.press();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    k_KC_A.release();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    // D unlocks
    k_KC_D.press();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    k_KC_D.release();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    // A works after unlocking
    k_KC_A.press();
    EXPECT_REPORT(driver, (KC_A));
    run_one_scan_loop();

    k_KC_A.release();
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    // D unlocks
    k_KC_D.press();
    EXPECT_REPORT(driver, (KC_D));
    run_one_scan_loop();

    k_KC_D.release();
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    CONFIRM_RESET();
}

/************
* OSM tests *
************/

TEST_F(LeepFrog, Osm_TransparentKey) {
    TestDriver driver;
    InSequence s;
    LEEP_KEY_ROW(0, 3,
      KC_A,
      ck_shft,
      ck_test
    )

    LEEP_KEY_ROW(1, 3,
      TK_0,
      TK_1,
      TK_2
    )

    // Press and unpress the osm shift key
    k_ck_shft.press();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();
    k_ck_shft.release();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    // Press the A key, which should be shifted.
    k_KC_A.press();
    EXPECT_REPORT(driver, (KC_RSFT, KC_A));
    EXPECT_REPORT(driver, (KC_A));
    run_one_scan_loop();

    k_KC_A.release();
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    CONFIRM_RESET();
}

TEST_F(LeepFrog, Osm_DifferentKey) {
    TestDriver driver;
    InSequence s;
    LEEP_KEY_ROW(0, 3,
      KC_A,
      ck_shft,
      ck_test
    )

    LEEP_KEY_ROW(1, 3,
      KC_B,
      TK_0,
      TK_1
    )

    // Press and unpress the osm shift key
    k_ck_shft.press();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();
    k_ck_shft.release();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    // Press the A key, which should be shifted.
    k_KC_A.press();
    EXPECT_REPORT(driver, (KC_RSFT, KC_B));
    EXPECT_REPORT(driver, (KC_B));
    run_one_scan_loop();

    k_KC_A.release();
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    CONFIRM_RESET();
}

TEST_F(LeepFrog, Osm_OverlappingKeyPresses) {
    TestDriver driver;
    InSequence s;

    uint16_t td_a = TD_A;

    LEEP_KEY_ROW(0, 4,
      KC_H,
      KC_I,
      ck_shft,
      ck_test
    )

    LEEP_KEY_ROW(1, 4,
      TK_0,
      TK_1,
      TK_2,
      TK_3
    )

    // Press and unpress the osm shift key
    k_ck_shft.press();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();
    k_ck_shft.release();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    // Press H and then press I before releasing H.
    k_KC_H.press();
    EXPECT_REPORT(driver, (KC_RSFT, KC_H));
    EXPECT_REPORT(driver, (KC_H));
    run_one_scan_loop();

    k_KC_I.press();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    k_KC_H.release();
    EXPECT_REPORT(driver, (KC_H, KC_I));
    EXPECT_REPORT(driver, (KC_I));
    run_one_scan_loop();

    k_KC_I.release();
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    CONFIRM_RESET();
}

TEST_F(LeepFrog, Osm_OverlappingTapDanceKeyPresses) {
    TestDriver driver;
    InSequence s;

    uint16_t td_i = TD_I;

    LEEP_KEY_ROW(0, 4,
      KC_H,
      td_i, // regular KC_I in the other layer
      ck_shft,
      ck_test
    )

    LEEP_KEY_ROW(1, 4,
      TK_0,
      KC_I,
      TK_2,
      TK_3
    )

    // Press and unpress the osm shift key
    k_ck_shft.press();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();
    k_ck_shft.release();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    // Press H and then press I before releasing H.
    k_KC_H.press();
    EXPECT_NO_REPORT(driver);
    EXPECT_REPORT(driver, (KC_RSFT, KC_H));
    EXPECT_REPORT(driver, (KC_H));
    run_one_scan_loop();

    k_td_i.press();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    k_KC_H.release();
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    k_td_i.release();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    EXPECT_REPORT(driver, (KC_I));
    EXPECT_EMPTY_REPORT(driver);
    idle_for(10 * TAPPING_TERM);

    CONFIRM_RESET();
}

TEST_F(LeepFrog, Osm_Hold) {
    TestDriver driver;
    InSequence s;

    uint16_t td_a = TD_A;

    LEEP_KEY_ROW(0, 7,
      td_a, // Tap dance key
      KC_H, // Regular key
      KC_I, // Regular key
      KC_D, // Combo key
      KC_F, // Combo key
      ck_shft,
      ck_test
    )

    LEEP_KEY_ROW(1, 7,
      TK_0,
      TK_1,
      TK_2,
      TK_3,
      TK_4,
      TK_5,
      TK_6
    )

    // Press and hold the osm shift key
    k_ck_shft.press();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();

    // Press the H key, which should be shifted.
    k_KC_H.press();
    EXPECT_REPORT(driver, (KC_RSFT, KC_H));
    run_one_scan_loop();

    k_KC_H.release();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();

    // Press the I key, which should be shifted.
    k_KC_I.press();
    EXPECT_REPORT(driver, (KC_RSFT, KC_I));
    run_one_scan_loop();

    k_KC_I.release();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();

    // Overlap the key presses
    k_KC_H.press();
    EXPECT_REPORT(driver, (KC_RSFT, KC_H));
    run_one_scan_loop();

    k_KC_I.press();
    EXPECT_REPORT(driver, (KC_RSFT, KC_H, KC_I));
    run_one_scan_loop();

    k_KC_H.release();
    EXPECT_REPORT(driver, (KC_RSFT, KC_I));
    run_one_scan_loop();

    k_KC_I.release();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();

    // Press a tap dance key
    k_td_a.press();
    run_one_scan_loop();

    k_td_a.release();
    run_one_scan_loop();

    EXPECT_REPORT(driver, (KC_RSFT, KC_A));
    EXPECT_REPORT(driver, (KC_RSFT));
    idle_for(TAPPING_TERM);

    // Press a combo key with no combo
    k_KC_F.press();
    EXPECT_REPORT(driver, (KC_RSFT, KC_F));
    run_one_scan_loop();

    k_KC_F.release();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();

    // Press a combo key with combo
    k_KC_F.press();
    k_KC_D.press();
    run_one_scan_loop();

    k_KC_F.release();
    k_KC_D.release();
    EXPECT_EMPTY_REPORT(driver);
    EXPECT_REPORT(driver, (KC_QUOTE));
    EXPECT_EMPTY_REPORT(driver);
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();

    // Release the osm shift key
    k_ck_shft.release();
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    CONFIRM_RESET();
}

TEST_F(LeepFrog, Osm_StickyHold) {
    TestDriver driver;
    InSequence s;

    uint16_t td_a = TD_A;

    LEEP_KEY_ROW(0, 7,
      td_a, // Tap dance key
      KC_H, // Regular key
      KC_I, // Regular key
      KC_D, // Combo key
      KC_F, // Combo key
      ck_shft,
      ck_test
    )

    LEEP_KEY_ROW(1, 7,
      TK_0,
      TK_1,
      TK_2,
      TK_3,
      TK_4,
      TK_5,
      TK_6
    )

    // Press and release the osm shift key twice
    k_ck_shft.press();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();
    k_ck_shft.release();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    k_ck_shft.press();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();
    k_ck_shft.release();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    // Press the H key, which should be shifted.
    k_KC_H.press();
    EXPECT_REPORT(driver, (KC_RSFT, KC_H));
    run_one_scan_loop();

    k_KC_H.release();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();

    // Press the I key, which should be shifted.
    k_KC_I.press();
    EXPECT_REPORT(driver, (KC_RSFT, KC_I));
    run_one_scan_loop();

    k_KC_I.release();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();

    // Overlap the key presses
    k_KC_H.press();
    EXPECT_REPORT(driver, (KC_RSFT, KC_H));
    run_one_scan_loop();

    k_KC_I.press();
    EXPECT_REPORT(driver, (KC_RSFT, KC_H, KC_I));
    run_one_scan_loop();

    k_KC_H.release();
    EXPECT_REPORT(driver, (KC_RSFT, KC_I));
    run_one_scan_loop();

    k_KC_I.release();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();

    // Press a tap dance key
    k_td_a.press();
    run_one_scan_loop();

    k_td_a.release();
    run_one_scan_loop();

    EXPECT_REPORT(driver, (KC_RSFT, KC_A));
    EXPECT_REPORT(driver, (KC_RSFT));
    idle_for(TAPPING_TERM);

    // Press a combo key with no combo
    k_KC_F.press();
    EXPECT_REPORT(driver, (KC_RSFT, KC_F));
    run_one_scan_loop();

    k_KC_F.release();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();

    // Press a combo key with combo
    k_KC_F.press();
    k_KC_D.press();
    run_one_scan_loop();

    k_KC_F.release();
    k_KC_D.release();
    EXPECT_EMPTY_REPORT(driver);
    EXPECT_REPORT(driver, (KC_QUOTE));
    EXPECT_EMPTY_REPORT(driver);
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();

    // Press again to deactivate osm mode.
    k_ck_shft.press();
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();
    k_ck_shft.release();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    CONFIRM_RESET();
}

/**************
* Combo tests *
**************/

TEST_F(LeepFrog, ComboBehavior) {
    TestDriver driver;
    InSequence s;
    LEEP_KEY_ROW(0, 5,
      KC_D,
      KC_F,
      KC_LSFT,
      KC_RSFT,
      ck_test
    )

    // Press and release the D and F keys simultaneously
    k_KC_D.press();
    k_KC_F.press();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    k_KC_D.release();
    k_KC_F.release();
    EXPECT_REPORT(driver, (KC_LSFT));
    EXPECT_REPORT(driver, (KC_LSFT, KC_QUOTE));
    EXPECT_REPORT(driver, (KC_LSFT));
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    // Press and release the D and F keys close together
    k_KC_D.press();
    run_one_scan_loop();

    k_KC_F.press();
    run_one_scan_loop();

    k_KC_D.release();
    EXPECT_REPORT(driver, (KC_LSFT));
    EXPECT_REPORT(driver, (KC_LSFT, KC_QUOTE));
    EXPECT_REPORT(driver, (KC_LSFT));
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    k_KC_F.release();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    // When left shift is held, it should do un-shifted quote
    k_KC_LSFT.press();
    EXPECT_REPORT(driver, (KC_LSFT));
    run_one_scan_loop();

    k_KC_D.press();
    k_KC_F.press();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    k_KC_D.release();
    k_KC_F.release();
    EXPECT_EMPTY_REPORT(driver);
    EXPECT_REPORT(driver, (KC_QUOTE));
    EXPECT_EMPTY_REPORT(driver);
    EXPECT_REPORT(driver, (KC_LSFT));

    run_one_scan_loop();

    k_KC_LSFT.release();
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    // When right shift is held, it should do un-shifted quote
    k_KC_RSFT.press();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();

    k_KC_D.press();
    k_KC_F.press();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    k_KC_D.release();
    k_KC_F.release();
    EXPECT_EMPTY_REPORT(driver);
    EXPECT_REPORT(driver, (KC_QUOTE));
    EXPECT_EMPTY_REPORT(driver);
    EXPECT_REPORT(driver, (KC_RSFT));

    run_one_scan_loop();

    k_KC_RSFT.release();
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    CONFIRM_RESET();
}

TEST_F(LeepFrog, ComboAndOSMTap) {
    TestDriver driver;
    InSequence s;
    LEEP_KEY_ROW(0, 6,
      KC_A,
      KC_B,
      KC_D,
      KC_F,
      ck_shft,
      ck_test
    )

    uint8_t kc_d = KC_D;
    uint8_t kc_f = KC_F;

    LEEP_KEY_ROW(1, 6,
      TK_0,
      KC_C,
      kc_d,
      kc_f,
      TK_1,
      TK_2
    )

    // Press and unpress the osm shift key
    k_ck_shft.press();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();
    k_ck_shft.release();
    run_one_scan_loop();

    // Press and release the D and F keys simultaneously
    k_KC_D.press();
    k_KC_F.press();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    k_KC_D.release();
    k_KC_F.release();
    EXPECT_EMPTY_REPORT(driver);
    EXPECT_REPORT(driver, (KC_QUOTE));
    EXPECT_EMPTY_REPORT(driver);
    EXPECT_REPORT(driver, (KC_RSFT));
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    CONFIRM_RESET();
}

/******************
* Tap dance tests *
******************/

TEST_F(LeepFrog, TapDance_CLICK_KC_HOLD_LAYER) {
    TestDriver driver;
    InSequence s;

    uint16_t to_shct = TO_SHCT;
    LEEP_KEY_ROW(0, 3,
      to_shct,
      KC_A,
      ck_test
    )

    LEEP_KEY_ROW(LR_SHORTCUTS, 3,
      TK_0,
      KC_B,
      TK_1
    )

    // Single tap dance just presses the key.
    k_to_shct.press();
    run_one_scan_loop();
    k_to_shct.release();
    EXPECT_REPORT(driver, (KC_LSFT));
    EXPECT_REPORT(driver, (KC_LSFT, KC_LEFT_BRACKET));
    EXPECT_REPORT(driver, (KC_LSFT));
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    EXPECT_NO_REPORT(driver);
    idle_for(TAPPING_TERM);

    CONFIRM_RESET();

    // Interrupted tap dance
    k_to_shct.press();
    run_one_scan_loop();

    k_KC_A.press();
    EXPECT_REPORT(driver, (KC_B));
    run_one_scan_loop();

    k_KC_A.release();
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    k_to_shct.release();
    run_one_scan_loop();

    EXPECT_NO_REPORT(driver);
    idle_for(TAPPING_TERM);

    CONFIRM_RESET();

    // Interrupted tap dance with inter-woven release
    k_to_shct.press();
    run_one_scan_loop();

    k_KC_A.press();
    EXPECT_REPORT(driver, (KC_B));
    run_one_scan_loop();

    k_to_shct.release();
    run_one_scan_loop();

    k_KC_A.release();
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    EXPECT_NO_REPORT(driver);
    idle_for(TAPPING_TERM);

    CONFIRM_RESET();
}

/****************
* Holding tests *
*****************/

TEST_F(LeepFrog, Osm_HoldJustShyOfTappingTerm) {
    TestDriver driver;
    InSequence s;
    LEEP_KEY_ROW(0, 3,
      KC_A,
      ck_shft,
      ck_test
    )

    LEEP_KEY_ROW(1, 3,
      TK_0,
      TK_1,
      TK_2
    )

    // Press the osm shift key
    k_ck_shft.press();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();

    // Wait to register as hold
    idle_for(TAPPING_TERM-1);

    // Unpress the osm shift key
    k_ck_shft.release();
    EXPECT_NO_REPORT(driver);
    run_one_scan_loop();

    // Press the A key, which should NOT be shifted.
    k_KC_A.press();
    EXPECT_REPORT(driver, (KC_RSFT, KC_A));
    EXPECT_REPORT(driver, (KC_A));
    run_one_scan_loop();

    k_KC_A.release();
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    CONFIRM_RESET();
}

TEST_F(LeepFrog, Osm_HoldLongerThanTappingTerm) {
    TestDriver driver;
    InSequence s;
    LEEP_KEY_ROW(0, 3,
      KC_A,
      ck_shft,
      ck_test
    )

    LEEP_KEY_ROW(1, 3,
      TK_0,
      TK_1,
      TK_2
    )

    // Press the osm shift key
    k_ck_shft.press();
    EXPECT_REPORT(driver, (KC_RSFT));
    run_one_scan_loop();

    // Wait to register as hold
    idle_for(TAPPING_TERM);

    // Unpress the osm shift key
    k_ck_shft.release();
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    // Press the A key, which should NOT be shifted.
    k_KC_A.press();
    EXPECT_REPORT(driver, (KC_A));
    run_one_scan_loop();

    k_KC_A.release();
    EXPECT_EMPTY_REPORT(driver);
    run_one_scan_loop();

    CONFIRM_RESET();
}
