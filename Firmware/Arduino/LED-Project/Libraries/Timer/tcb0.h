#ifndef TCB0_H_INCLUDED
#define TCB0_H_INCLUDED

#include <stdint.h>
#include <Arduino.h>
// #include "../system/utils/compiler.h"
#include "timer_interface.h"
#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const struct TMR_INTERFACE TCB0_Interface;


/**
 * @ingroup tcb0
 * @typedef void *TCB0_cb_t
 * @brief Function pointer to callback function called by the TCB. The default value is set to NULL which means that no callback function will be used.
 */
typedef void (*TCB0_cb_t)(void);

/**
 * @ingroup tcb0
 * @brief Registers a callback function to be called at capture event.
 * @param TCB0_cb_t cb - Callback function for capture event.
 * @return None.
 */
void TCB0_CaptureCallbackRegister(TCB0_cb_t cb);


/**
 * @ingroup tcb0
 * @brief Initializes the TCB module
 * @param None.
 * @return None.
 */
void TCB0_Initialize(void);
/**
 * @ingroup tcb0
 * @brief Starts the TCB counter.
 * @param None.
 * @return None.
 */
void TCB0_Start(void);
/**
 * @ingroup tcb0
 * @brief Stops the TCB counter.
 * @param None.
 * @return None.
 */
void TCB0_Stop(void);
/**
 * @ingroup tcb0
 * @brief Enables the capture interrupt for the TCB.
 * @param None.
 * @return None.
 */
void TCB0_EnableCaptInterrupt(void);
/**
 * @ingroup tcb0
 * @brief Disables the capture interrupt for the TCB.
 * @param None.
 * @return None.
 */
void TCB0_DisableCaptInterrupt(void);
/**
 * @ingroup tcb0
 * @brief Reads the 16-bit timer value of the TCB.
 * @param None.
 * @return uint16_t
 */
uint16_t TCB0_Read(void);
/**
 * @ingroup tcb0
 * @brief Writes the 16-bit timer value to the TCB. 
 * @param uint16_t timerVal - 16-bit Timer value to write for TCB interface.
 * @return None.
 */
void TCB0_Write(uint16_t timerVal);
/**
 * @ingroup tcb0
 * @brief Checks the Overflow Interrupt flag.
 * @param None.
 * @return bool.
 */
bool TCB0_IsOvfInterruptFlag(void);
/**
 * @ingroup tcb0
 * @brief Checks the Capture Interrupt flag.
 * @param None.
 * @return bool.
 */
bool TCB0_IsCaptInterruptFlag(void);

/**
 * @ingroup tcb0
 * @brief Clears the Capture Interrupt flag.
 * @param None.
 * @return None.
 */
void TCB0_ClearCaptInterruptFlag(void);
/**
 * @ingroup tcb0
 * @brief Checks if the capture interrupt is enabled.
 * @param None.
 * @return None.
 */
bool TCB0_IsCaptInterruptEnabled(void);

/**
 * @ingroup tcb0_normal
 * @brief Performs the tasks to be executed on timer events.
 * @param None.
 * @return None.
 */
void TCB0_Tasks(void);



#ifdef __cplusplus
}
#endif

#endif /* TCB0_H_INCLUDED */