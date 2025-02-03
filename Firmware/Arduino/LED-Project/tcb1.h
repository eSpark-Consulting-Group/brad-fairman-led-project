#ifndef TCB1_H_INCLUDED
#define TCB1_H_INCLUDED

#include <stdint.h>
#include <Arduino.h>
// #include "../system/utils/compiler.h"
#include "timer_interface.h"
#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const struct TMR_INTERFACE TCB1_Interface;


/**
 * @ingroup tcb1
 * @typedef void *TCB1_cb_t
 * @brief Function pointer to callback function called by the TCB. The default value is set to NULL which means that no callback function will be used.
 */
typedef void (*TCB1_cb_t)(void);

/**
 * @ingroup tcb1
 * @brief Registers a callback function to be called at capture event.
 * @param TCB1_cb_t cb - Callback function for capture event.
 * @return None.
 */
void TCB1_CaptureCallbackRegister(TCB1_cb_t cb);


/**
 * @ingroup tcb1
 * @brief Initializes the TCB module
 * @param None.
 * @return None.
 */
void TCB1_Initialize(void);
/**
 * @ingroup tcb1
 * @brief Starts the TCB counter.
 * @param None.
 * @return None.
 */
void TCB1_Start(void);
/**
 * @ingroup tcb1
 * @brief Stops the TCB counter.
 * @param None.
 * @return None.
 */
void TCB1_Stop(void);
/**
 * @ingroup tcb1
 * @brief Enables the capture interrupt for the TCB.
 * @param None.
 * @return None.
 */

bool TCB1_IsRunning(void);

void TCB1_EnableCaptInterrupt(void);
/**
 * @ingroup tcb1
 * @brief Disables the capture interrupt for the TCB.
 * @param None.
 * @return None.
 */
void TCB1_DisableCaptInterrupt(void);
/**
 * @ingroup tcb1
 * @brief Reads the 16-bit timer value of the TCB.
 * @param None.
 * @return uint16_t
 */
uint16_t TCB1_Read(void);
/**
 * @ingroup tcb1
 * @brief Writes the 16-bit timer value to the TCB. 
 * @param uint16_t timerVal - 16-bit Timer value to write for TCB interface.
 * @return None.
 */
void TCB1_Write(uint16_t timerVal);
/**
 * @ingroup tcb1
 * @brief Checks the Overflow Interrupt flag.
 * @param None.
 * @return bool.
 */
bool TCB1_IsOvfInterruptFlag(void);
/**
 * @ingroup tcb1
 * @brief Checks the Capture Interrupt flag.
 * @param None.
 * @return bool.
 */
bool TCB1_IsCaptInterruptFlag(void);

/**
 * @ingroup tcb1
 * @brief Clears the Capture Interrupt flag.
 * @param None.
 * @return None.
 */
void TCB1_ClearCaptInterruptFlag(void);
/**
 * @ingroup tcb1
 * @brief Checks if the capture interrupt is enabled.
 * @param None.
 * @return None.
 */
bool TCB1_IsCaptInterruptEnabled(void);

/**
 * @ingroup tcb1_normal
 * @brief Performs the tasks to be executed on timer events.
 * @param None.
 * @return None.
 */
void TCB1_Tasks(void);



#ifdef __cplusplus
}
#endif

#endif /* TCB1_H_INCLUDED */