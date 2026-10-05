#pragma once
#include <srl.hpp>

inline const char *transitionMessage[] = {
    "Idle",
    "Out",
    "Loading",
    "In"
};

inline MenuItem_t debugItems[] = {
    { .label = "Debug Display:", .type = MENU_ITEM_BOOL, .action = nullptr, .boolVal = &PPE::CoreOptions.debugDisplay },
    { .label = "Debug Mode:   ", .type = MENU_ITEM_BOOL, .action = nullptr, .boolVal = &PPE::CoreOptions.debugMode },
    { .label = "Back          ", .type = MENU_ITEM_BACK, .action = nullptr, .boolVal = nullptr },
};

inline Menu_t debugMenu = {
    "Debug Menu:", debugItems, sizeof(debugItems) / sizeof(debugItems[0]), 0, nullptr
};
