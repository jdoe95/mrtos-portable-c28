/** ************************************************************************
 * @file portable.h
 * @brief Platform dependent functions for C2000
 * @author John Yu buyi.yu@wne.edu
 *
 * This file is part of mRTOS.
 *
 * Copyright (C) 2018 John Buyi Yu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *****************************************************************************/
#ifndef MRTOS_PORTABLE_RTOS_PORTABLE_H_
#define MRTOS_PORTABLE_RTOS_PORTABLE_H_

/***********************************************
 * Operating system configurations
 ***********************************************/
#define OSPORT_BYTE_T       uint16_t
#define OSPORT_UINT_T       uint16_t
#define OSPORT_UINTPTR_T    uintptr_t
#define OSPORT_BOOL_T       bool

#define OSPORT_IDLE_STACK_SIZE  (52)
#define OSPORT_NUM_PRIOS        (8) /* 0 to 6, 7 reserved for idle */
#define OSPORT_MEM_ALIGN        (2)
#define OSPORT_MEM_SMALLEST     (0)
#define OSPORT_ENABLE_DEBUG     (1)

#define OSPORT_IDLE_FUNC \
    osport_idle

#define OSPORT_START() \
    osport_start()

#define OSPORT_INIT_STACK(p_stack, size, p_start, p_return) \
    osport_init_stack(p_stack, size, p_start, p_return)

#define OSPORT_BREAKPOINT() \
    osport_breakpoint()

#define OSPORT_DISABLE_INT() \
    osport_disable_int()

#define OSPORT_ENABLE_INT() \
    osport_enable_int()

#define OSPORT_CONTEXTSW_REQ() \
    osport_contextsw_req()

/***********************************************
 * End of operating system configurations
 ***********************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>

void osport_start( void );
void osport_idle( void ) __attribute__((noreturn));
void* osport_init_stack( void *p_stack, OSPORT_UINT_T size,
void (*p_start_from)(void), void (*p_return_to)(void) );

void osport_breakpoint(void);
void osport_disable_int(void);
void osport_enable_int(void);
void osport_contextsw_req(void);

#endif /* MRTOS_PORTABLE_RTOS_PORTABLE_H_ */
