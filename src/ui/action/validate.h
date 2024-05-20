#pragma once

#include <stdbool.h>  // bool

/**
 * Action for public key validation and export.
 *
 * @param[in] approved
 *   User approved or rejected.
 * @param[in] go_back_to_menu
 *   If the function must explicitly go back to the menu
 *
 */
void ui_action_validate_pubkey(bool approved, bool go_back_to_menu);

/**
 * Action for transaction information validation.
 *
 * @param[in] approved
 *   User approved or rejected.
 * @param[in] go_back_to_menu
 *   If the function must explicitly go back to the menu
 *
 */
void ui_action_validate_transaction(bool approved, bool go_back_to_menu);
