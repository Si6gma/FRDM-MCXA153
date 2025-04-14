/*! ***************************************************************************
 *
 * \brief     Main application
 * \file      main.c
 * \author    Hugo Arends
 * \date      May 2024
 *
 * \see       NXP. (2024). MCX A153, A152, A143, A142 Reference Manual. Rev. 4,
 *            01/2024. From:
 *            https://www.nxp.com/docs/en/reference-manual/MCXAP64M96FS3RM.pdf
 *
 * \copyright 2024 HAN University of Applied Sciences. All Rights Reserved.
 *            \n\n
 *            Permission is hereby granted, free of charge, to any person
 *            obtaining a copy of this software and associated documentation
 *            files (the "Software"), to deal in the Software without
 *            restriction, including without limitation the rights to use,
 *            copy, modify, merge, publish, distribute, sublicense, and/or sell
 *            copies of the Software, and to permit persons to whom the
 *            Software is furnished to do so, subject to the following
 *            conditions:
 *            \n\n
 *            The above copyright notice and this permission notice shall be
 *            included in all copies or substantial portions of the Software.
 *            \n\n
 *            THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *            EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *            OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *            NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *            HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *            WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *            FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *            OTHER DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/
#include <MCXA153.h>
#include <stdio.h>
#include <string.h>

#include "gpio_input_interrupt.h"
#include "gpio_output.h"
#include "eeprom.h"
#include "encoder.h"
#include "hc595.h"
#include "lm35d_interrupt.h"
#include "lpuart0_interrupt.h"
#include "neopixel_flexpwm.h"
#include "ssd1306.h"

#include "fsm.h"

// -----------------------------------------------------------------------------
// Local type definitions
// -----------------------------------------------------------------------------
#ifdef DEBUG
#define TARGETSTR "Debug"
#else
#define TARGETSTR "Release"
#endif

#define EN (0)
#define NL (1)

typedef struct
{
    int len;
    char *str;
}language_t;

const language_t language[][2] =
{
    { // 0
        // "XXXXXXXXXXXXXXXXXXXXX"
        {6,"Waffle"}, // EN
        {5,"Wafel"},  // NL
    },
    { // 1
        {4,"bake"},
        {3,"bak"},
    },
    { // 2
        {5,"timer"},
        {5,"timer"},
    },
    { // 3
        {4,"with"},
        {3,"met"},
    },
    { // 4
        {3,"FSM"},
        {3,"FSM"},
    },
    { // 5
        {18,"Press SW2 to start"},
        {17,"Druk op SW2 om te"},
    },
    { // 6
        {6,"baking"},
        {18,"starten met bakken"},
    },
    { // 7
        {2,"OK"},
        {2,"OK"},
    },
    { // 8
        {6,"CANCEL"},
        {9,"ANNULEREN"},
    },
    { // 9
        {12,"Phase 1 of 3"},
        {12,"Fase 1 van 3"},
    },
    { // 10
        {12,"Phase 2 of 3"},
        {12,"Fase 2 van 3"},
    },
    { // 11
        {12,"Phase 3 of 3"},
        {12,"Fase 3 van 3"},
    },
    { // 12
        {5,"Done!"},
        {6,"Klaar!"},
    },
    { // 13
        {9,"Cancelled"},
        {11,"Geannuleerd"},
    },
    { // 14
        {10,"Nederlands"},
        {7,"English"},
    },
    { // 15
        {15,"turn the waffle"},
        {9,"draai het"},
    },
    { // 16
        {5,"maker"},
        {10,"wafelijzer"},
    },
    { // 17
        {15,"TURN THE WAFFLE"},
        {9,"DRAAI HET"},
    },
    { // 18
        {5,"MAKER"},
        {10,"WAFELIJZER"},
    },
};

char * eventEnumToText[] =
{
    "E_NO",
    "E_ANY",
    "E_CONTINUE",
    "E_TIMEOUT",
    "E_UPDATE",
    "E_SW2",
    "E_SW3",
};

char * stateEnumToText[] =
{
    "S_START",
    "S_INIT",
    "S_WAIT",
    "S_PHASE_1",
    "S_PHASE_2",
    "S_PHASE_3",
    "S_CANCEL",
    "S_DONE",
    "S_LANG",
};

// -----------------------------------------------------------------------------
// Local function prototypes
// -----------------------------------------------------------------------------

//void S_START_onExit(void);
void S_INIT_onEntry(void);
void S_INIT_onExit(void);
void S_WAIT_onEntry(void);
void S_WAIT_onExit(void);
void S_PHASE_1_onEntry(void);
void S_PHASE_1_onExit(void);
void S_PHASE_2_onEntry(void);
void S_PHASE_2_onExit(void);
void S_PHASE_3_onEntry(void);
void S_PHASE_3_onExit(void);
void S_CANCEL_onEntry(void);
void S_CANCEL_onExit(void);
void S_DONE_onEntry(void);
void S_DONE_onExit(void);
void S_LANG_onEntry(void);

// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------
static volatile uint32_t ms = 0;
static volatile uint32_t timeout_ms = 0xFFFFFFFF;
static volatile uint32_t previous_ms = 0xFFFFFFFF;
const uint32_t interval_update_ms = 250;

static uint32_t lang = EN;

state_t state;
event_t event;

//                        +-----------+
//                        |  S_START  |
//                        +-----+-----+
//                              |E_CONTINUE
//                              v
//                        +-----------+
//                        |  S_INIT   |
//                        +-----+-----+
//                              |E_CONTINUE
//                              |
//                              |    E_CONTINUE +-----------+
//                              |   +-----------+  S_LANG   |
//                              |   |           +-----------+
//                              |   |                 ^
//                              v   v   E_SW3         |
//                        +-----------+---------------+
//       +--------------->|  S_WAIT   |<--------------------+
//       |                +-----+-----+                     |
//       |                      |E_SW2                      |
//       |E_SW2                 v                           |E_SW2
// +-----+-----+   E_SW3  +-----------+<--+           +-----+-----+<--+
// | S_CANCEL  |<---------+ S_PHASE_1 +---+ E_UPDATE  |  S_DONE   +---+ E_UPDATE
// +-----------+          +-----+-----+               +-----------+
//       |                      |E_TIMEOUT                  ^
//       |                      v                           |
//       |         E_SW3  +-----------+<--+                 |
//       |<---------------+ S_PHASE_2 +---+ E_UPDATE        |
//       |                +-----+-----+                     |
//       |                      |E_TIMEOUT                  |
//       |                      v                           |
//       |         E_SW3  +-----------+<--+                 |
//       +----------------+ S_PHASE_3 +---+ E_UPDATE        |
//                        +-----+-----+                     |
//                              |                           |
//                              +---------------------------+


// -----------------------------------------------------------------------------
// Main application
// -----------------------------------------------------------------------------
int main(void)
{
    // 96 MHz FIRC clock selected
    SCG0->FIRCCFG = SCG_FIRCCFG_FREQ_SEL(0b101);

    lpuart0_init(115200);
    gpio_output_init();
    gpio_input_init();
    neopixel_init();
    ssd1306_init();

    // Generate an interrupt every 1 ms
    SysTick_Config(96000);

    printf("Waffle bake timer");
    printf(" - %s\r\n", TARGETSTR);
    printf("Build %s %s\r\n", __DATE__, __TIME__);

    printf("[%08lu] System init done\r\n", ms);
    printf("[%08lu] Add states\r\n", ms);

    //           State                        onEntry()          onExit()
    FSM_AddState(S_START  , &(state_funcs_t){ NULL             , NULL });
    FSM_AddState(S_INIT   , &(state_funcs_t){ S_INIT_onEntry   , S_INIT_onExit });
    FSM_AddState(S_WAIT   , &(state_funcs_t){ S_WAIT_onEntry   , S_WAIT_onExit });
    FSM_AddState(S_PHASE_1, &(state_funcs_t){ S_PHASE_1_onEntry, S_PHASE_1_onExit });
    FSM_AddState(S_PHASE_2, &(state_funcs_t){ S_PHASE_2_onEntry, S_PHASE_2_onExit });
    FSM_AddState(S_PHASE_3, &(state_funcs_t){ S_PHASE_3_onEntry, S_PHASE_3_onExit });
    FSM_AddState(S_CANCEL , &(state_funcs_t){ S_CANCEL_onEntry , S_CANCEL_onExit });
    FSM_AddState(S_DONE   , &(state_funcs_t){ S_DONE_onEntry   , S_DONE_onExit });
    FSM_AddState(S_LANG   , &(state_funcs_t){ S_LANG_onEntry   , NULL });

    printf("[%08lu] Add transitions\r\n", ms);

    //                                 From       Event       To
    FSM_AddTransition(&(transition_t){ S_START  , E_CONTINUE, S_INIT    });
    FSM_AddTransition(&(transition_t){ S_INIT   , E_CONTINUE, S_WAIT    });
    FSM_AddTransition(&(transition_t){ S_WAIT   , E_SW2     , S_PHASE_1 });
    FSM_AddTransition(&(transition_t){ S_WAIT   , E_SW3     , S_LANG    });
    FSM_AddTransition(&(transition_t){ S_LANG   , E_CONTINUE, S_WAIT    });
    FSM_AddTransition(&(transition_t){ S_PHASE_1, E_TIMEOUT , S_PHASE_2 });
    FSM_AddTransition(&(transition_t){ S_PHASE_1, E_UPDATE  , S_PHASE_1 });
    FSM_AddTransition(&(transition_t){ S_PHASE_2, E_TIMEOUT , S_PHASE_3 });
    FSM_AddTransition(&(transition_t){ S_PHASE_2, E_UPDATE  , S_PHASE_2 });
    FSM_AddTransition(&(transition_t){ S_PHASE_3, E_TIMEOUT , S_DONE    });
    FSM_AddTransition(&(transition_t){ S_PHASE_3, E_UPDATE  , S_PHASE_3 });
    FSM_AddTransition(&(transition_t){ S_PHASE_1, E_SW3     , S_CANCEL  });
    FSM_AddTransition(&(transition_t){ S_PHASE_2, E_SW3     , S_CANCEL  });
    FSM_AddTransition(&(transition_t){ S_PHASE_3, E_SW3     , S_CANCEL  });
    FSM_AddTransition(&(transition_t){ S_CANCEL , E_SW2     , S_WAIT    });
    FSM_AddTransition(&(transition_t){ S_DONE   , E_SW2     , S_WAIT    });
    FSM_AddTransition(&(transition_t){ S_DONE   , E_UPDATE  , S_DONE    });


    // Should unexpected events in a state be flushed or not?
    FSM_FlushEnexpectedEvents(true);

    // Set the finite state machine in the S_START state
    // and add a E_CONTINUE event into the event buffer
    state = S_START;
    FSM_AddEvent(E_CONTINUE);

    printf("[%08lu] Start statemachine\r\n", ms);

    while(1)
    {
        if(!FSM_NoEvents())
        {
            // Get the event and handle it
            event = FSM_GetEvent();

            // For debugging
            printf("[%08lu] %-10s -> %-10s", ms, stateEnumToText[state], eventEnumToText[event]);

            state = FSM_EventHandler(state, event);

            // For debugging
            printf(" -> %s\r\n", stateEnumToText[state]);
        }

        if(gpio_input_sw2_pressed())
        {
            FSM_AddEvent(E_SW2);
        }

        if(gpio_input_sw3_pressed())
        {
            FSM_AddEvent(E_SW3);
        }

        if(ms >= timeout_ms)
        {
            timeout_ms = 0xFFFFFFFF;
            FSM_AddEvent(E_TIMEOUT);
        }

        if(ms >= (previous_ms + interval_update_ms))
        {
            previous_ms = ms;
            FSM_AddEvent(E_UPDATE);
        }
    }
}

// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------
void SysTick_Handler(void)
{
    ms++;
}

void S_INIT_onEntry(void)
{
    // Show splash screen
    ssd1306_setorientation(1);
    ssd1306_setfont(Monospaced_plain_10);
    ssd1306_clearscreen();

    ssd1306_drawbitmap(waffles);

    ssd1306_putstring(0,  0, language[0][lang].str);
    ssd1306_putstring(0, 12, language[1][lang].str);
    ssd1306_putstring(0, 25, language[2][lang].str);
    ssd1306_putstring(127-(6*language[3][lang].len), 38, language[3][lang].str);
    ssd1306_putstring(127-(6*language[4][lang].len), 51, language[4][lang].str);

    ssd1306_update();

    while(ms < 3000)
    {}

    FSM_AddEvent(E_CONTINUE);
}

void S_INIT_onExit(void)
{
}

void S_WAIT_onEntry(void)
{
    timeout_ms = 0xFFFFFFFF;

    ssd1306_setfont(Monospaced_plain_10);
    ssd1306_clearscreen();

    ssd1306_putstring(64 - 3 - ((6 * language[5][lang].len) / 2),  0, language[5][lang].str);
    ssd1306_putstring(64 - 3 - ((6 * language[6][lang].len) / 2), 12, language[6][lang].str);
    ssd1306_putstring(0, 25, "");
    ssd1306_putstring(0, 38, "");
    ssd1306_putstring(0, 51, language[7][lang].str); // OK
    ssd1306_putstring(127-(6*language[14][lang].len), 51, language[14][lang].str); // Language

    ssd1306_update();

    rgb_t rgb[] = {{0,0,0},{0,0,0},{0,0,0}};
    neopixel_update(rgb);
}

void S_WAIT_onExit(void)
{

}

void S_PHASE_1_onEntry(void)
{
    const uint32_t interval_ms = 10000;

    if(timeout_ms == 0xFFFFFFFF)
    {
        timeout_ms = ms + interval_ms;
    }

    ssd1306_clearscreen();

    char str[8];
    sprintf(str, "%lu", (timeout_ms - ms)/1000);
    ssd1306_setfont(Monospaced_bold_24);
    ssd1306_putstring(64 - ((14 * strlen(str)) / 2), 12, str);

    ssd1306_setfont(Monospaced_plain_10);

    ssd1306_putstring(64 - ((6 * language[9][lang].len) / 2),  0, language[9][lang].str);
    ssd1306_putstring(0, 12, "");
    ssd1306_putstring(0, 25, "");
    ssd1306_putstring(0, 38, "");
    ssd1306_putstring(127-(6*language[8][lang].len), 51, language[8][lang].str); // CANCEL

    ssd1306_update();

    rgb_t rgb[] = {{10,0,0},{0,0,0},{0,0,0}};
    neopixel_update(rgb);
}

void S_PHASE_1_onExit(void)
{

}

void S_PHASE_2_onEntry(void)
{
    const uint32_t interval_ms = 120000;
//    const uint32_t interval_ms = 20000;

    static uint32_t cnt = 0;

    if(timeout_ms == 0xFFFFFFFF)
    {
        timeout_ms = ms + interval_ms;
        cnt = 0;
    }

    cnt++;

    ssd1306_clearscreen();

    if((timeout_ms - ms) > (interval_ms - 3000))
    {
        ssd1306_setfont(Monospaced_plain_10);

        if((cnt % 2) == 0)
        {
            ssd1306_putstring(64 - ((6 * language[15][lang].len) / 2),  0, language[15][lang].str);
            ssd1306_putstring(64 - ((6 * language[16][lang].len) / 2), 12, language[16][lang].str);
            ssd1306_putstring(0, 25, "");
            ssd1306_putstring(0, 38, "");
            ssd1306_putstring(127-(6*language[8][lang].len), 51, language[8][lang].str); // CANCEL

            rgb_t rgb[] = {{0,0,0},{0,0,0},{0,0,0}};
            neopixel_update(rgb);
        }
        else
        {
            ssd1306_putstring(64 - ((6 * language[17][lang].len) / 2),  0, language[17][lang].str);
            ssd1306_putstring(64 - ((6 * language[18][lang].len) / 2), 12, language[18][lang].str);
            ssd1306_putstring(0, 25, "");
            ssd1306_putstring(0, 38, "");
            ssd1306_putstring(127-(6*language[8][lang].len), 51, language[8][lang].str); // CANCEL

            rgb_t rgb[] = {{0,0,0},{10,10,0},{0,0,0}};
            neopixel_update(rgb);
        }
    }
    else
    {
        char str[8];
        sprintf(str, "%lu", (timeout_ms - ms)/1000);
        ssd1306_setfont(Monospaced_bold_24);
        ssd1306_putstring(64 - ((14 * strlen(str)) / 2), 12, str);

        ssd1306_setfont(Monospaced_plain_10);

        ssd1306_putstring(64 - ((6 * language[10][lang].len) / 2),  0, language[10][lang].str);
        ssd1306_putstring(0, 12, "");
        ssd1306_putstring(0, 25, "");
        ssd1306_putstring(0, 38, "");
        ssd1306_putstring(127-(6*language[8][lang].len), 51, language[8][lang].str); // CANCEL

        rgb_t rgb[] = {{0,0,0},{10,10,0},{0,0,0}};
        neopixel_update(rgb);
    }

    ssd1306_update();
}

void S_PHASE_2_onExit(void)
{

}

void S_PHASE_3_onEntry(void)
{
    const uint32_t interval_ms = 90000;
//    const uint32_t interval_ms = 5000;

    static uint32_t cnt = 0;

    if(timeout_ms == 0xFFFFFFFF)
    {
        timeout_ms = ms + interval_ms;
        cnt = 0;
    }

    cnt++;

    ssd1306_clearscreen();

    if((timeout_ms - ms) > (interval_ms - 3000))
    {
        ssd1306_setfont(Monospaced_plain_10);

        if((cnt % 2) == 0)
        {
            ssd1306_putstring(64 - ((6 * language[15][lang].len) / 2),  0, language[15][lang].str);
            ssd1306_putstring(64 - ((6 * language[16][lang].len) / 2), 12, language[16][lang].str);
            ssd1306_putstring(0, 25, "");
            ssd1306_putstring(0, 38, "");
            ssd1306_putstring(127-(6*language[8][lang].len), 51, language[8][lang].str); // CANCEL

            rgb_t rgb[] = {{0,0,0},{0,0,0},{0,0,0}};
            neopixel_update(rgb);
        }
        else
        {
            ssd1306_putstring(64 - ((6 * language[17][lang].len) / 2),  0, language[17][lang].str);
            ssd1306_putstring(64 - ((6 * language[18][lang].len) / 2), 12, language[18][lang].str);
            ssd1306_putstring(0, 25, "");
            ssd1306_putstring(0, 38, "");
            ssd1306_putstring(127-(6*language[8][lang].len), 51, language[8][lang].str); // CANCEL

            rgb_t rgb[] = {{0,0,0},{0,0,0},{0,10,0}};
            neopixel_update(rgb);
        }
    }
    else
    {
        char str[8];
        sprintf(str, "%lu", (timeout_ms - ms)/1000);
        ssd1306_setfont(Monospaced_bold_24);
        ssd1306_putstring(64 - ((14 * strlen(str)) / 2), 12, str);

        ssd1306_setfont(Monospaced_plain_10);

        ssd1306_putstring(64 - ((6 * language[11][lang].len) / 2),  0, language[11][lang].str);
        ssd1306_putstring(0, 12, "");
        ssd1306_putstring(0, 25, "");
        ssd1306_putstring(0, 38, "");
        ssd1306_putstring(127-(6*language[8][lang].len), 51, language[8][lang].str); // CANCEL

        rgb_t rgb[] = {{0,0,0},{0,0,0},{0,10,0}};
        neopixel_update(rgb);
    }

    ssd1306_update();
}

void S_PHASE_3_onExit(void)
{

}

void S_CANCEL_onEntry(void)
{
    ssd1306_setfont(Monospaced_plain_10);
    ssd1306_clearscreen();

    ssd1306_putstring(64 - ((6 * language[13][lang].len) / 2),  0, language[13][lang].str);
    ssd1306_putstring(0, 12, "");
    ssd1306_putstring(0, 25, "");
    ssd1306_putstring(0, 38, "");
    ssd1306_putstring(0, 51, language[7][lang].str); // OK

    ssd1306_update();

    rgb_t rgb[] = {{0,0,0},{0,0,0},{0,0,0}};
    neopixel_update(rgb);
}

void S_CANCEL_onExit(void)
{

}

void S_DONE_onEntry(void)
{
    static uint32_t cnt = 0;

    ssd1306_setfont(Monospaced_plain_10);
    ssd1306_clearscreen();

    if((cnt % 2) == 0)
    {
        ssd1306_drawbitmap(waffles);
    }

    cnt++;

    ssd1306_putstring(0,  0, language[12][lang].str);
    ssd1306_putstring(0, 12, "");
    ssd1306_putstring(0, 25, "");
    ssd1306_putstring(0, 38, "");
    ssd1306_putstring(0, 51, language[7][lang].str); // OK

    ssd1306_update();

    rgb_t rgb[] = {{0,0,0},{0,0,0},{0,0,0}};
    neopixel_update(rgb);
}

void S_DONE_onExit(void)
{

}

void S_LANG_onEntry(void)
{
    lang = (lang == EN) ? NL : EN;
    FSM_AddEvent(E_CONTINUE);
}
