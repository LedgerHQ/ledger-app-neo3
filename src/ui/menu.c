/*****************************************************************************
 *   Ledger App Boilerplate.
 *   (c) 2020 Ledger SAS.
 *       2021 COZ Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *****************************************************************************/

#include "os.h"
#include "ux.h"
#include "glyphs.h"

#include "globals.h"
#include "shared_context.h"
#include "menu.h"
#include "utils.h"

#ifdef HAVE_NBGL
#include "nbgl_use_case.h"
#endif

#ifdef HAVE_BAGL

static void ui_menu_about();

static void display_settings(const ux_flow_step_t* const start_step);
static void switch_settings_contract_scripts(void);

UX_STEP_NOCB(ux_menu_ready_step, pn, {&C_badge_neo, "Wake up NEO.."});
UX_STEP_NOCB(ux_menu_version_step, bn, {"Version", APPVERSION});
UX_STEP_CB(ux_menu_settings_step, pb, display_settings(NULL), {&C_icon_eye, "Settings"});
UX_STEP_CB(ux_menu_about_step, pb, ui_menu_about(), {&C_icon_certificate, "About"});
UX_STEP_VALID(ux_menu_exit_step, pb, os_sched_exit(-1), {&C_icon_dashboard_x, "Quit"});
// FLOW for the main menu:
// #1 screen: ready
// #2 screen: version of the app
// #3 screen: settings
// #4 screen: about submenu
// #5 screen: quit
UX_FLOW(ux_menu_main_flow,
        &ux_menu_ready_step,
        &ux_menu_version_step,
        &ux_menu_settings_step,
        &ux_menu_about_step,
        &ux_menu_exit_step,
        FLOW_LOOP);

#if defined(TARGET_NANOS)
// clang-format off
UX_STEP_CB(
    ux_settings_contract_scripts,
    bnnn_paging,
    switch_settings_contract_scripts(),
    {
        .title = "Contract data",
        .text = strings.scriptsAllowed
    });

#else
UX_STEP_CB(
    ux_settings_contract_scripts,
    bnnn,
    switch_settings_contract_scripts(),
    {
        "Contract scripts",
        "Allow contract scripts",
        "in transactions",
        strings.scriptsAllowed
    });
#endif

UX_STEP_CB(
    ux_settings_back_step,
    pb,
    ui_menu_main(),
    {
      &C_icon_back_x,
      "Back",
    });

// clang-format on
UX_FLOW(ux_settings_flow, &ux_settings_contract_scripts, &ux_settings_back_step);

static void display_settings(const ux_flow_step_t* const start_step) {
    strlcpy(strings.scriptsAllowed, (N_storage.scriptsAllowed ? "Allowed" : "NOT Allowed"), 12);
    ux_flow_init(0, ux_settings_flow, start_step);
}

static void switch_settings_contract_scripts() {
    uint8_t value = (N_storage.scriptsAllowed ? 0 : 1);
    nvm_write((void*) &N_storage.scriptsAllowed, (void*) &value, sizeof(uint8_t));
    display_settings(&ux_settings_contract_scripts);
}

UX_STEP_NOCB(ux_menu_info_step, bn, {"NEO N3 App", "(c) 2021 COZ Inc"});
UX_STEP_CB(ux_menu_back_step, pb, ui_menu_main(), {&C_icon_back, "Back"});

// FLOW for the about submenu:
// #1 screen: app info
// #2 screen: back button to main menu
UX_FLOW(ux_menu_about_flow, &ux_menu_info_step, &ux_menu_back_step, FLOW_LOOP);

static void ui_menu_about() {
    ux_flow_init(0, ux_menu_about_flow, NULL);
}

#else

static const char* const info_types[] = {"Version", DISPLAYABLE_APPNAME};
static const char* const info_contents[] = {APPVERSION, "(c) 2021 COZ Inc"};

static void quit_app_callback(void) {
    os_sched_exit(-1);
}

#define NB_SETTINGS_SWITCHES 1
static nbgl_layoutSwitch_t G_switches[NB_SETTINGS_SWITCHES];

enum {
    SWITCH_CONTRACT_DATA_SET_TOKEN = FIRST_USER_TOKEN,
};

static bool settings_nav_callback(uint8_t page, nbgl_pageContent_t* content) {
    if (page == 0) {
        content->type = INFOS_LIST;
        content->infosList.nbInfos = ARRAY_COUNT(info_types);
        content->infosList.infoTypes = info_types;
        content->infosList.infoContents = info_contents;
    } else if (page == 1) {
        content->type = SWITCHES_LIST;
        content->switchesList.nbSwitches = NB_SETTINGS_SWITCHES;
        content->switchesList.switches = G_switches;
    } else {
        return false;
    }

    return true;
}

static void ui_menu_main_nbgl(void);
void ui_menu_settings(void);

static void settings_controls_callback(int token, uint8_t index) {
    bool new_setting;
    UNUSED(index);
    switch (token) {
        case SWITCH_CONTRACT_DATA_SET_TOKEN:
            G_switches[0].initState = !(G_switches[0].initState);
            new_setting = (G_switches[0].initState == ON_STATE);
            nvm_write((void*) &N_storage.scriptsAllowed, &new_setting, 1);
            break;
        default:
            PRINTF("Should not happen !");
            break;
    }
}

void ui_menu_settings(void) {
    G_switches[0].text = "Contract scripts";
    G_switches[0].subText = "Allow contract scripts";
    G_switches[0].token = SWITCH_CONTRACT_DATA_SET_TOKEN;
    G_switches[0].tuneId = TUNE_TAP_CASUAL;
    if (N_storage.scriptsAllowed) {
        G_switches[0].initState = ON_STATE;
    } else {
        G_switches[0].initState = OFF_STATE;
    }
    nbgl_useCaseSettings(DISPLAYABLE_APPNAME " settings",
                         0,
                         2,
                         false,
                         ui_menu_main_nbgl,
                         settings_nav_callback,
                         settings_controls_callback);
}

static void ui_menu_main_nbgl(void) {
    nbgl_useCaseHome(DISPLAYABLE_APPNAME, &C_icon_neo_n3_64x64, NULL, true, ui_menu_settings, quit_app_callback);
}
#endif

void ui_menu_main() {
#ifdef HAVE_BAGL
    if (G_ux.stack_count == 0) {
        ux_stack_push();
    }
    ux_flow_init(0, ux_menu_main_flow, NULL);
#else
    ui_menu_main_nbgl();
#endif
}
