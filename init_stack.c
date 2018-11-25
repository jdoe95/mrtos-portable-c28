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
 * The C28x Stack Frame
 * Grows from low memory to high memory
 *
 * Since the compiler does not support __attribute__((__packed__)),
 * the members are exactly 1 byte to make the compiler less interested
 * in inserting padding bytes.
 */

typedef struct {

    /*
     * Registers managed by hardware
     */
    uint16_t st0;
    uint16_t t;

    uint16_t al;
    uint16_t ah;

    uint16_t pl;
    uint16_t ph;

    uint16_t ar0;
    uint16_t ar1;

    uint16_t st1;
    uint16_t dp;

    uint16_t ier;
    uint16_t dbgstat;

    uint16_t pc_low;
    uint16_t pc_high;

    /*
     * Registers managed by the context switcher
     */

    uint16_t rpc_low;
    uint16_t rpc_high;

    uint16_t ar0h;
    uint16_t ar1h;

    uint16_t ar2;
    uint16_t ar2h;

    uint16_t ar3;
    uint16_t ar3h;

    uint16_t ar4;
    uint16_t ar4h;

    uint16_t ar5;
    uint16_t ar5h;

    uint16_t ar6;
    uint16_t ar6h;

    uint16_t ar7;
    uint16_t ar7h;

    /* this is actually register T being pushed again, because there is
     * no instruction to push only TH. Renamed to dummy2 here to avoid
     * confusion.
     */
    uint16_t dummy2;
    uint16_t th;

    /* floating point registers */
    uint16_t rb_low;
    uint16_t rb_high;

    uint16_t stf_low;
    uint16_t stf_high;

    uint16_t r0h_low;
    uint16_t r0h_high;

    uint16_t r1h_low;
    uint16_t r1h_high;

    uint16_t r2h_low;
    uint16_t r2h_high;

    uint16_t r3h_low;
    uint16_t r3h_high;

    uint16_t r4h_low;
    uint16_t r4h_high;

    uint16_t r5h_low;
    uint16_t r5h_high;

    uint16_t r6h_low;
    uint16_t r6h_high;

    uint16_t r7h_low;
    uint16_t r7h_high;
} stack_frame_t;

void* osport_init_stack( void *p_stack, OSPORT_UINT_T size,
void (*p_start_from)(void), void (*p_return_to)(void) )
{
    stack_frame_t *p_stack_frame = (stack_frame_t*) p_stack;

    p_stack_frame->st0 = 0x0080;
    p_stack_frame->st1 = 0x8A08;

    /*
     * PC will control where thread will resume
     * when context is loaded
     *
     * RPC will control where thread will return. The
     * operating system will install a routine here
     * to let the thread kill itself and release the
     * resources.
     */

    /* PC */
    p_stack_frame->pc_low = ((uint32_t)(p_start_from)) & 0xFFFF;
    p_stack_frame->pc_high = (((uint32_t)(p_start_from)) >> 16) & 0xFFFF;

    /* RPC */
    p_stack_frame->rpc_low = ((uint32_t)(p_return_to)) & 0xFFFF;
    p_stack_frame->rpc_high = (((uint32_t)(p_return_to)) >> 16) & 0xFFFF;

    /* DP */
    p_stack_frame->dp = 0x0000;

    /* IER */
    p_stack_frame->ier = 0xFFFF;

    /* T, TH */
    p_stack_frame->t = 0x0000;
    p_stack_frame->th = 0x0000;

    /* PL, PH */
    p_stack_frame->pl = 0;
    p_stack_frame->ph = 0;

    /* STF */
    p_stack_frame->stf_low = 0x0000;
    p_stack_frame->stf_high = 0x0000;

    /* RB */
    p_stack_frame->rb_low = 0x0000;
    p_stack_frame->rb_high = 0x0000;


#if OSPORT_ENABLE_DEBUG
    /*
     * Initialize the registers with dummy values
     * to see if the stack frame is loaded onto
     * the CPU currectly.
     */
    p_stack_frame->al = 0xAAAA;
    p_stack_frame->ah = 0xBBBB;

    p_stack_frame->ar0 = 0x0000;
    p_stack_frame->ar1 = 0x0001;
    p_stack_frame->ar2 = 0x0002;
    p_stack_frame->ar3 = 0x0003;
    p_stack_frame->ar4 = 0x0004;
    p_stack_frame->ar5 = 0x0005;
    p_stack_frame->ar6 = 0x0006;
    p_stack_frame->ar7 = 0x0007;

    p_stack_frame->ar0h = 0x1000;
    p_stack_frame->ar1h = 0x1001;
    p_stack_frame->ar2h = 0x1002;
    p_stack_frame->ar3h = 0x1003;
    p_stack_frame->ar4h = 0x1004;
    p_stack_frame->ar5h = 0x1005;
    p_stack_frame->ar6h = 0x1006;
    p_stack_frame->ar7h = 0x1007;

    p_stack_frame->r0h_low = 0xB000;
    p_stack_frame->r1h_low = 0xB001;
    p_stack_frame->r2h_low = 0xB002;
    p_stack_frame->r3h_low = 0xB003;
    p_stack_frame->r4h_low = 0xB004;
    p_stack_frame->r5h_low = 0xB005;
    p_stack_frame->r6h_low = 0xB006;
    p_stack_frame->r7h_low = 0xB007;

    p_stack_frame->r0h_high = 0xA000;
    p_stack_frame->r1h_high = 0xA001;
    p_stack_frame->r2h_high = 0xA002;
    p_stack_frame->r3h_high = 0xA003;
    p_stack_frame->r4h_high = 0xA004;
    p_stack_frame->r5h_high = 0xA005;
    p_stack_frame->r6h_high = 0xA006;
    p_stack_frame->r7h_high = 0xA007;

#endif

    return (uint16_t*)p_stack_frame + sizeof(stack_frame_t) + 1;
}
