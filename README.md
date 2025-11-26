# Arduino_STM32 Priviledge violation and Fault cause
## 1. STM32 priviledge and execute mode
STM32 has several execute mode.

### STM32 Privilege Mode
STM32 microcontrollers are based on the Arm Cortex-M core and support two execution access levels: "Privileged Mode" and "Unprivileged Mode."

### Privileged Mode Overview
In privileged mode, software has unrestricted access to all resources in the system (all instructions, all memory areas, all peripheral registers, etc.).

This mode is typically reserved for the operating system (OS) kernel and other trusted system components that perform core system functions.

### Differences Between Privileged and Unprivileged Modes
The primary difference lies in the scope of access permissions.

| Function          | Privileged Mode | Unprivileged Mode             |
| ---               | ---             | ---                           |
| System Resources  | All Accessible  | Restricted Access             |
| Special Registers | All Accessible  | Restricted Access Except APSR |
| System Timer      | Accessible      | Inaccessible                  |
| NVIC, SCB         | Accessible      | Inaccessible                  |
| MPU Settings      | Changeable      | Immutable                     |

Attempts to access restricted resources in unprivileged mode are typically ignored or **generate a fault exception (error)**.

## 2. Automatic Mode Switching
### At startup:
The processor starts in privileged Thread mode by default.

### Handler mode:
When an interrupt or exception occurs, the processor automatically switches to Handler mode, which always runs in privileged mode.

### Software Control:
Privileged software can change the access level of Thread mode from privileged to unprivileged, or from unprivileged to privileged, by manipulating the CONTROL register.

Unprivileged software can indirectly request privileged operations by issuing a Supervisor Call (SVC) instruction and transferring control to privileged software.

While most STM32 microcontrollers typically operate exclusively in privileged mode by default, 
using these modes becomes important when using a real-time operating system (RTOS) or when using a memory protection unit (MPU) for enhanced security.

## 3. Operating Modes
Access levels (privileged/non-privileged) are used in conjunction with the following two operating modes:

### Handler mode:
This mode is executed when an **exception (such as an interrupt) occurs**.\
It always runs at **privileged level**.

### Thread mode:
This mode is executed in normal application code, not exception handling.\
It can be configured to run at **either privileged or non-privileged level**.

### Usage
The privileged mode mechanism allows system designers to assign different permissions to different parts of software. 
This improves system robustness and security by preventing user applications from changing critical settings that could affect overall system stability.

By default, the processor starts in privileged thread mode at boot time. Switching to non-privileged mode is accomplished by setting the CONTROL register,
allowing non-privileged software to call privileged software functions using the SVC (Supervisor Call) instruction.

## 4. Simple test program fot Arduino IDE 1.8.8 + Arduino_STM32
### `"BluePill-Pliviladge-Test.ino" for STM32F103C8T6`
```
#define LED_BUILTIN         PC13        // Define onBoard LED port

#include "libmaple/scb.h"               // for SCB register access

//--------------------
void setup() {

    pinMode(LED_BUILTIN,OUTPUT);        // For onBoard LED drive

    Serial.begin(9600);
    while (!Serial) { delay(100); }
    Serial.println("BluePill-Priviledge-Test");

    if (is_in_thread_mode()) {
        Serial.println("** Now execute in Thread mode.");
    } else {
        Serial.println("** Now execute in Handler mode.");        
    }
    if (is_in_privileged_thread_mode()) {
        Serial.println("** Now in Previlaged mode.");
    } else {
        Serial.println("** Now in Unrevilaged mode.");
    }
    if (is_use_which_stack_pointer()) {
        Serial.println("** Now use MSP.");
    } else {
        Serial.println("** Now use PSP.");
    }
    //Serial.print("** SP = 0x"); Serial.println(get_xsp(),HEX);
    //Serial.print("** PC = 0x"); Serial.println(get_pc(),HEX);
    Serial.print("** PRIMASK = 0x"); Serial.println(get_primask(),HEX);
    Serial.print("** BASEPRI = 0x"); Serial.println(get_basepri(),HEX);
}

//--------------------
void loop() {
    Serial.print(".");
    delay(500);
}

uint32_t is_in_thread_mode(void) {
    // Read the ICSR to get the VECTACTIVE field value.
    // The VECTACTIVE bits are at position 0-8 in the ICSR.
    // Use the CMSIS definition for SCB->ICSR to ensure portability.
    return (SCB_BASE->ICSR & SCB_ICSR_VECTACTIVE) == 0;
}

/**
 * @brief  Return the Control Register value
 * @return Control value
 * Return the content of the control register
 */
uint32_t __get_CONTROL(void) {
  asm volatile("mrs r0, control");
  asm volatile("bx lr");
}

int is_in_privileged_thread_mode(void) {
    // In Thread mode, check the nPRIV bit of the CONTROL register.
    // (CONTROL & 0x01) == 0 means nPRIV is 0, so we are privileged.
    return !(__get_CONTROL() & 0x01);
}

int is_use_which_stack_pointer(void) {
    // (CONTROL & 0x02) == 0 means use MSP and other PSP.
    return !(__get_CONTROL() & 0x02);
}

uint32_t __get_PRIMASK(void) {
  asm volatile("mrs r0, primask");
  asm volatile("bx lr");
}

uint32_t get_primask(void) {
    return (__get_PRIMASK() & 0x01);
}

uint32_t __get_BASEPRI(void) {
  asm volatile("mrs r0, basepri");
  asm volatile("bx lr");
}

uint32_t get_basepri(void) {
    return (__get_BASEPRI() & 0xFF);
}

uint32_t __get_xSP(void) {
//  asm volatile("mov r0, sp");
//  asm volatile("mrs r0, sp");
  asm volatile("mov r0, r13");
  asm volatile("bx lr");
}
uint32_t get_xsp(void) {
    return (__get_xSP() & 0xFFFFFFFF);
}

uint32_t __get_PC(void) {
//  asm volatile("mov r0, pc");
//  asm volatile("mrs r0, r15");
  asm volatile("mov r0, r15");
  asm volatile("bx lr");
}
uint32_t get_pc(void) {
    return (__get_PC() & 0xFFFFFFFF);
}
```
### Execute log
`"BluePill-Priviledge-Test.log"`
```
BluePill-Priviledge-Test
** Now execute in Thread mode.
** Now in Unrevilaged mode.
** Now use PSP.
** PRIMASK = 0x0
** BASEPRI = 0x0
....................................
```
