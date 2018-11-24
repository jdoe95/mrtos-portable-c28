# Microcontroller Real Time Operating System

This is the portable code of MRTOS for the Texas Instruments C28x platform. To use this
platform, please see [the main mRTOS repository](https://github.com/jdoe95/mrtos). 

The portable code uses the following interrupts
 * ``USER1``, (interrupt #20, ``ISR_USER1``), used for loading the first thread
 * ``RTOSINT``,  (interrupt #16, ``ISR_RTOSINT``), implemented as the context switcher
 * ``INT14``, (interrupt #14, ``ISR_CPUTIM2``), CPUTIM2, used for generating system heartbeats
  
 The portable code does not configure CPUTIM2. The configuration of CPUTIM2 must be 
 set before ``os_start()``, and __the timer must be disabled__. When the operating
 system starts, it will enable the timer automatically.
 
Do not use ``EINT`` and ``DINT`` instructions in the application. Use the nested
version provided by the operating system instead (``os_enter_critical()`` and 
``os_exit_critical()``). Be sure to replace the macros in TI's peripheral library,  
too, if it's being used. Manipulating interrupts directly in the application can
really mess up the operating system's shared variable access locks.

 
## Features

 * FPU support
 * Low power mode (when idle)
 * Thread starts with interrupts enabled and EALLOW disabled
 
## Settings

|Setting|Value|
|---|---|
|CPUTIM2 interrupt frequency| 100Hz (recommended)|
|RTOSINT interrupt priority| lowest* (recommended) |
|CPUTIM2 interrupt priority| highest* (recommended) |
|USER1 interrupt priority| highest* (recommended)|

*: The interrupt priorities on the C28x core are hard-coded onto the silicon,
but the manual provides several ways to implement priorities in software.
The setting is recommended but not required. It is good enough to use
the hardware priorities but the interrupt latency might be slightly longer.
 
## Code Example

```C

// defined in rtos portable code
extern interrupt void ISR_RTOSINT(void);
extern interrupt void ISR_CPUTIM2(void);
extern interrupt void ISR_USER1(void);

// These variables are defined by the linker 
// as instructed by the linker command file
/* 
 * Example linker section:
 * 	.esysmem :> RAM, PAGE=0,
 *			LOAD_START(_esysmem_start),
 *			LOAD_SIZE(_esysmem_size)
 *
 * .esysmem is the "heap" section generated by
 * the compiler. Modify linker settings to 
 * change the size of this section.  
 */
extern int esysmem_start;
extern int esysmem_size;

int main(void)
{
    // initialize hardware, configure CPUTIM2 (do not enable it)
	
    // ...
	
    // Fill PIE vector table
    PIE_VECT_TABLE.RTOSINT = ISR_RTOSINT;
    PIE_VECT_TABLE.TINT2 = ISR_CPUTIM2;
    PIE_VECT_TABLE.USER1 = ISR_USER1;

    // enable PIE
    PIECTRL.PIECTRL.bit.ENPIE = 1;
    
    // initialize operating system
    os_config_t config;
    config.p_pool_mem = &esysmem_start;

	// suppress warning #70: "integer conversion results in truncation"
	// because we know that the size of the heap will be smaller than 
	// the maximum value of OSPORT_UINT_T. If you want to use a longer
	// OSPORT_UINT_T, that's fine.
	// WARNING: esysmem_size must be a multiple of OSPORT_MEM_ALIGN
#pragma diag_push
#pragma diag_suppress 70
    config.pool_size = (uintptr_t)(&esysmem_size);
#pragma diag_pop

    os_init(&config);
    
    // create some operating system objects here: threads, semaphores, etc.
    
    // ... 
    
    // start the operating system, run the first highest priority thread
    os_start();
    
    // reaches the point of no return. The execution should
    // never get here. If it does, something went wrong.
    
    for( ; ; )
   		;
    
    return 0;
}

```
