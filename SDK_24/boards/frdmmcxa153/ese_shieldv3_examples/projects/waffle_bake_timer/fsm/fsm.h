/*! ***************************************************************************
 *
 * \brief     Finate statemachine
 * \file      fsm.h
 * \author    Hugo Arends
 * \date      June 2021
 *
 * \copyright 2021 HAN University of Applied Sciences. All Rights Reserved.
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
#ifndef FSM_H_
#define FSM_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_STATES           ( 9)
#define MAX_TRANSITIONS      (24)
#define MAX_EVENTS_IN_BUFFER (32) // 2,4,8,16,32,64,128 or 256

typedef enum
{
    S_START = 0, // S_START must be 0!
    S_INIT,
    S_WAIT,
    S_PHASE_1,
    S_PHASE_2,
    S_PHASE_3,
    S_CANCEL,
    S_DONE,
    S_LANG,

}state_t;

typedef enum
{
    E_NO = 0,
    E_ANY,
    E_CONTINUE,
    E_TIMEOUT,
    E_UPDATE,
    E_SW2,
    E_SW3,

}event_t;

typedef struct
{
    void (*onEntry)(void);
    void (*onExit)(void);
}state_funcs_t;

typedef struct
{
    state_t from;
    event_t event;
    state_t to;

}transition_t;

// Function prototypes
state_t FSM_EventHandler(const state_t state, const event_t event);
void    FSM_FlushEnexpectedEvents(const bool flush);
void    FSM_AddState(const state_t state, const state_funcs_t *funcs);
void    FSM_AddTransition(const transition_t *transition);
void    FSM_AddEvent(const event_t event);
event_t FSM_GetEvent(void);
event_t FSM_WaitForEvent(void);
event_t FSM_PeekForEvent(void);
bool    FSM_NoEvents(void);
uint8_t FSM_NofEvents(void);

#endif // FSM_H_
