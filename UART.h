/******************************************************************************/
/* Filename: UART.h
 * Author: Abdullah Alhadlaq
 * Date: 25 Sep 2017
 * Purpose: This file contains the definitions,
 *          externals, and prototypes of the UART
 *          module.
 * Disclaimer: A large portion of this file is taken from the ECED4402 course
 *             website.
 * Edits:
 * 27 Sep 2017: -Included UART_state
 *              -Added subroutine prototypes
 * 24 Oct 2017: -Added print_char and print_str prototypes
 */


/******************************************************************************/
/* -------------------------------------------------------------------------- *
 * Author: Abdulrahman  Aljedaibi
 * Author: Abdullah     Alhadlaq
 * Course: Real time systems
 * ECED 4402
 * Date assigned :   26  Sept  2017
 * Date created  :   24  Oct  2017
 * Editing       :   15  Sept - Disable interrupt upon entry and enable upon
 *                                  leaving
 * Submission date : 15 Nov 2017
 * File name : Queue.c
 * Purpose: Implement a static circular queue in order to organize interrupts
 *              According to their type (UART or SYSTICK)
 * ------------------------------------------------------------------------- */
#ifndef UART_H_
#define UART_H_

/* UART0/UART1 & PORTA/PORTB Registers */

// GPIOA Alternate Function Select Register
#define GPIO_PORTA_AFSEL_R  (*((volatile unsigned long *)0x40058420))
#define GPIO_PORTB_AFSEL_R  (*((volatile unsigned long *)0x40059420))

// GPIO Digital Enable Register
#define GPIO_PORTA_DEN_R    (*((volatile unsigned long *)0x4005851C))
#define GPIO_PORTB_DEN_R    (*((volatile unsigned long *)0x4005951C))

// GPIO Port Control Register
#define GPIO_PORTA_PCTL_R   (*((volatile unsigned long *)0x4005852C))
#define GPIO_PORTB_PCTL_R   (*((volatile unsigned long *)0x4005952C))

// UART Data Registers
#define UART0_DR_R          (*((volatile unsigned long *)0x4000C000))
#define UART1_DR_R          (*((volatile unsigned long *)0x4000D000))


// UART Integer Baud-Rate Divisor Register
#define UART0_IBRD_R        (*((volatile unsigned long *)0x4000C024))
#define UART1_IBRD_R        (*((volatile unsigned long *)0x4000D024))

// UART Fractional Baud-Rate Divisor Register
#define UART0_FBRD_R        (*((volatile unsigned long *)0x4000C028))
#define UART1_FBRD_R        (*((volatile unsigned long *)0x4000D028))

// UART Line Control Register
#define UART0_LCRH_R        (*((volatile unsigned long *)0x4000C02C))
#define UART1_LCRH_R        (*((volatile unsigned long *)0x4000D02C))

// UART Control Register
#define UART0_CTL_R         (*((volatile unsigned long *)0x4000C030))
#define UART1_CTL_R         (*((volatile unsigned long *)0x4000D030))

// UART Interrupt FIFO Level Select Register
#define UART0_IFLS_R        (*((volatile unsigned long *)0x4000C034))
#define UART1_IFLS_R        (*((volatile unsigned long *)0x4000D034))

// UART Interrupt Mask Register
#define UART0_IM_R          (*((volatile unsigned long *)0x4000C038))
#define UART1_IM_R          (*((volatile unsigned long *)0x4000D038))

// UART Masked Interrupt Status Register
#define UART0_MIS_R         (*((volatile unsigned long *)0x4000C040))
#define UART1_MIS_R         (*((volatile unsigned long *)0x4000D040))

// UART Interrupt Clear Register
#define UART0_ICR_R         (*((volatile unsigned long *)0x4000C044))
#define UART1_ICR_R         (*((volatile unsigned long *)0x4000D044))

// UART Clock Control Register
#define UART0_CC_R          (*((volatile unsigned long *)0x4000CFC8))
#define UART1_CC_R          (*((volatile unsigned long *)0x4000DFC8))

// Clock Gating Registers
#define SYSCTL_RCGCGPIO_R      (*((volatile unsigned long *)0x400FE608))
#define SYSCTL_RCGCUART_R      (*((volatile unsigned long *)0x400FE618))

#define SYSCTL_RCGCGPIO_UART0      0x00000001  // UART0 Clock Gating Control
#define SYSCTL_RCGCUART_GPIOA      0x00000001  // PortA Clock Gating Control
#define SYSCTL_RCGCGPIO_UART1      0x00000002  // UART1 Clock Gating Control
#define SYSCTL_RCGCUART_GPIOB      0x00000002  // PortB Clock Gating Control


// UART0 Rx and Tx interrupt index (decimal)
#define INT_VEC_UART0           5
#define INT_VEC_UART1           6
#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000010  // UART Receive FIFO Empty
// UART Receive FIFO Interrupt Level at >= 1/8
#define UART_RX_FIFO_ONE_EIGHT  0x00000038
// UART Transmit FIFO Interrupt Level at <= 7/8
#define UART_TX_FIFO_SVN_EIGHT  0x00000007
#define UART_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_CTL_UARTEN         0x00000301  // UART RX/TX Enable
#define UART_INT_TX             0x020       // Transmit Interrupt Mask
#define UART_INT_RX             0x010       // Receive Interrupt Mask
#define UART_INT_RT             0x040       // Receive Timeout Interrupt Mask
#define UART_CTL_EOT            0x00000010  // UART End of Transmission Enable
#define EN_RX_PA0               0x00000001  // Enable Receive Function on PA0
#define EN_TX_PA1               0x00000002  // Enable Transmit Function on PA1
#define EN_DIG_PA0              0x00000001  // Enable Digital I/O on PA0
#define EN_DIG_PA1              0x00000002  // Enable Digital I/O on PA1




// Clock Configuration Register
#define SYSCTRL_RCC_R           (*((volatile unsigned long *)0x400FE0B0))

#define CLEAR_USRSYSDIV     0xF83FFFFF  // Clear USRSYSDIV Bits
#define SET_BYPASS          0x00000800  // Set BYPASS Bit

// Interrupt 0-31 Set Enable Register
#define NVIC_EN0_R      (*((volatile unsigned long *)0xE000E100))
// Interrupt 32-54 Set Enable Register
#define NVIC_EN1_R      (*((volatile unsigned long *)0xE000E104))

#define BUSY    1
#define IDLE    0

/* Globals */
extern volatile int UART_state;             /* BUSY|IDLE */
extern volatile int UART1_state;             /* BUSY|IDLE */
extern struct UART_entry current_msg;       /* message currently being printed */

extern struct frame recv;
extern struct frame send;
/* function prototypes */
void UART0_Init(void);
void InterruptEnable(unsigned long InterruptIndex);
void UART0_IntEnable(unsigned long flags);
void UART0_IntHandler(void);
void InterruptMasterEnable(void);
void InterruptMasterDisable (void);
void print_char (char prnt);
void print_str (char *str);
void init_UART (void);

#endif /* UART_H_ */
