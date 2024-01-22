#pragma once

#ifdef LEEP_OSM_ENABLE

#include "leep_osm_v2.h"

enum osm_enact_steps {
  OSM_NOOP,
  OSM_HOLD_CHECK,
  OSM_REGISTER_KEY,
  OSM_UNREGISTER_KEY,
  OSM_UNREGISTER_NEXT_ACTION,
  OSM_RELEASE_ON_UNPRESS,
  OSM_HOLD,
};

static uint8_t osm_step = OSM_NOOP;
static uint16_t osmed_key = 0;

void OSM_handled(uint16_t keycode, bool pressed) {
  switch (osm_step) {
  case OSM_NOOP:
    // Activate if pressing the relevant key
    if (pressed && keycode == OSM_shift_keycode) {
      register_code16(KC_RSFT);
      osm_step = OSM_HOLD_CHECK;
    }
    break;
  case OSM_HOLD_CHECK:
    // If we're pressing a different key first, then we are holding down the OSM key
    // in which case we simply need to release on unpress.
    if (pressed) {
      osm_step = OSM_RELEASE_ON_UNPRESS;
    }

    // If we're unpressing the OSM key first, then we're going to register a key
    if (!pressed && keycode == OSM_shift_keycode) {
      osm_step = OSM_REGISTER_KEY;
    }
    break;
  case OSM_RELEASE_ON_UNPRESS:
    if (!pressed && keycode == OSM_shift_keycode) {
      unregister_code16(KC_RSFT);
      osm_step = OSM_NOOP;
    }
    break;
  case OSM_REGISTER_KEY:
    if (pressed) {
      osmed_key = keycode;
      osm_step = OSM_UNREGISTER_KEY;
    }
    break;
  case OSM_UNREGISTER_KEY:
    if (!pressed && keycode == osmed_key) {
      // Unregister on the next action to ensure shift is held for all of this
      // unpress's logic.
      osm_step = OSM_UNREGISTER_NEXT_ACTION;
    }
    break;
  case OSM_UNREGISTER_NEXT_ACTION:
    unregister_code16(KC_RSFT);
    osm_step = OSM_NOOP;
    break;
  case OSM_HOLD:
    if (pressed && keycode == OSM_shift_keycode) {
      unregister_code16(KC_RSFT);
      osm_step = OSM_NOOP;
    }
    break;
  }
}

#endif
