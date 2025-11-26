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
