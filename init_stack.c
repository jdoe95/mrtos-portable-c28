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
#include <rtos_module.h>
#include "rtos_portable.h"

/*
 * The C28x stack grows from low memory
 * to high memory
 *
 * ACC:32       (AH:16 AL:16) accumulator
 * XAR0-XAR7:32 (AR0-AR7:16)  auxiliary registers
 * DP:16                      data page pointer
 * IFR:16                     interrupt flag
 * IER:16                     interrupt enable
 * DBGIER:16                  debug interrupt enable
 * P:32         (PH:16 PL:16) product register
 * PC:22                      program counter
 * RPC:22                     return program counter
 * SP:16                      stack pointer
 * ST0:16                     status register 0
 * ST1:16                     status register 1
 * XT:32        (T:16  TL:16) multiplicand register
 *
 * The struct should be __attribute__((__packed__))
 * but the compiler doesn't support this keyword.
 * We assume that the compiler will generate a
 * packed struct with no padding.
 */
struct stack_frame_s
{
    /* Pushed automatically by the CPU */
    uint16_t st0;
    uint16_t t;

    uint32_t acc;

    uint32_t p;

    uint16_t ar0;
    uint16_t ar1;

    uint16_t st1;
    uint16_t dp;

    uint16_t ier;
    uint16_t dbgstat;

    uint32_t pc;

    /* managed by the context switcher */
    uint32_t rpc;

    uint16_t ar0h;
    uint16_t ar1h;

    uint32_t xar2;
    uint32_t xar3;
    uint32_t xar4;
    uint32_t xar5;
    uint32_t xar6;
    uint32_t xar7;

    uint32_t xt;

    /* floating point registers */
    uint32_t rb;
    uint32_t stf;
    uint32_t r0h;
    uint32_t r1h;
    uint32_t r2h;
    uint32_t r3h;
    uint32_t r4h;
    uint32_t r5h;
    uint32_t r6h;
    uint32_t r7h;

};

typedef struct stack_frame_s stack_frame_t;

void* osport_init_stack( void *p_stack, OSPORT_UINT_T size,
void (*p_start_from)(void), void (*p_return_to)(void) )
{
    stack_frame_t *p_stack_frame = (stack_frame_t*)p_stack;

    p_stack_frame->st0 = 0;
    p_stack_frame->st1 = 0x0A18;

    /*
     * PC will control where thread will resume
     * when context is loaded
     *
     * RPC will control where thread will return. The
     * operating system will install a routine here
     * to let the thread kill itself and release the
     * resources.
     */
    p_stack_frame->pc = (uint32_t)(p_start_from);
    p_stack_frame->rpc = (uint32_t)(p_return_to);

    p_stack_frame->dp = 0x0000;
    p_stack_frame->ier = 0xFFFF;

    p_stack_frame->t = 0x0000;
    p_stack_frame->xt = 0x00000000;
    p_stack_frame->p = 0x00000000;

    p_stack_frame->stf = 0x00000000;
    p_stack_frame->rb = 0x00000000;

#if OSPORT_ENABLE_DEBUG
    /*
     * Initialize the registers with dummy values
     * to see if the stack frame is loaded onto
     * the CPU currectly.
     */
    p_stack_frame->acc = 0xAAAAAAAA;
    p_stack_frame->ar0h = 0x1000;
    p_stack_frame->ar1h = 0x1000;
    p_stack_frame->ar0 = 0x0000;
    p_stack_frame->ar1 = 0x0001;
    p_stack_frame->xar2 = 0x10000002;
    p_stack_frame->xar3 = 0x10000003;
    p_stack_frame->xar4 = 0x10000004;
    p_stack_frame->xar5 = 0x10000005;
    p_stack_frame->xar6 = 0x10000006;
    p_stack_frame->xar7 = 0x10000007;

    p_stack_frame->r0h = 0x20000000;
    p_stack_frame->r1h = 0x20000001;
    p_stack_frame->r2h = 0x20000002;
    p_stack_frame->r3h = 0x20000003;
    p_stack_frame->r4h = 0x20000004;
    p_stack_frame->r5h = 0x20000005;
    p_stack_frame->r6h = 0x20000006;
    p_stack_frame->r7h = 0x20000007;
#endif

    return (uint16_t*)p_stack_frame + sizeof(stack_frame_t);
}
