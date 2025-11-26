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
