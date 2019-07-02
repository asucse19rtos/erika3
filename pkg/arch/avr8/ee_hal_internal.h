/* ###*B*###
 * Erika Enterprise, version 3
 * 
 * Copyright (C) 2017 - 2018 Evidence s.r.l.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License, version 2, for more details.
 * 
 * You should have received a copy of the GNU General Public License,
 * version 2, along with this program; if not, see
 * < www.gnu.org/licenses/old-licenses/gpl-2.0.html >.
 * 
 * This program is distributed to you subject to the following
 * clarifications and special exceptions to the GNU General Public
 * License, version 2.
 * 
 * THIRD PARTIES' MATERIALS
 * 
 * Certain materials included in this library are provided by third
 * parties under licenses other than the GNU General Public License. You
 * may only use, copy, link to, modify and redistribute this library
 * following the terms of license indicated below for third parties'
 * materials.
 * 
 * In case you make modified versions of this library which still include
 * said third parties' materials, you are obligated to grant this special
 * exception.
 * 
 * The complete list of Third party materials allowed with ERIKA
 * Enterprise version 3, together with the terms and conditions of each
 * license, is present in the file THIRDPARTY.TXT in the root of the
 * project.
 * ###*E*### */

/** \file   ee_hal_internal.h
 *  \brief  HAL internal.
 *
 *  This files contains all HAL Internals for a specific Architecture in
 *  Erika Enterprise.
 *
 *  \note TO BE DOCUMENTED!!!
 *
 *  \author Errico Guidieri
 *  \author Giuseppe Serano
 *  \date   2016
 */

#ifndef OSEE_HAL_INTERNAL_H
#define OSEE_HAL_INTERNAL_H

#include "ee_utils.h"
#include "ee_platform_types.h"
#include "ee_hal.h"
#include "ee_avr8_irq.h"
#include "ee_hal_internal_types.h"
#include "ee_kernel_types.h"
#include "ee_std_change_context.h"
//#include "ee_atmega_internal.h"

#if (defined(OSEE_HAS_SYSTEM_TIMER))
#include "ee_atmega_timer0ctc.h"
#include "ee_atmega_timer1ctc.h"
#endif	/* OSEE_HAS_SYSTEM_TIMER */

#if (defined(__cplusplus))
extern "C" {
#endif

/*==============================================================================
                                        Macros
 =============================================================================*/

/*
 * Provide global symbol and declare an assembly label.
 */
#define OSEE_AVR8_PROVIDE_SYM(symbol) \
  __asm__ __volatile__(               \
    ".global " OSEE_S(symbol) "\n"    \
    OSEE_S(symbol) ":\n" ::           \
  )

/*
 * Access a global symbol.
 */
#define OSEE_AVR8_REQUEST_SYM(symbol)   \
  __asm__ __volatile__(                 \
    ".global " OSEE_S(symbol)           \
    "\n ldi r30, lo8(" OSEE_S(symbol)   \
    ")\n" ::                            \
  )

/*==============================================================================
                        Interrupt handling utilities
 =============================================================================*/

/* Disable/Enable Interrupts */
OSEE_STATIC_INLINE void osEE_hal_disableIRQ( void )
{
  cli();
}

OSEE_STATIC_INLINE void osEE_hal_enableIRQ( void )
{
  sei();
}

/* Suspend/Resume Interrupts */
OSEE_STATIC_INLINE OsEE_reg osEE_hal_suspendIRQ ( void )
{
  register OsEE_reg sr = SREG;
  osEE_hal_disableIRQ();
  return sr;
}

OSEE_STATIC_INLINE void osEE_hal_disableIRQsource(OsEE_isr_src_id source_id)
{
    switch(source_id)
  {
  /* INT0 interrupt vector */
  case OSEE_AVR8_INT0_ISR_ID:
	  EIMSK &= ~(1U << INT0);
	  break;
  /* INT1 interrupt vector */
  case OSEE_AVR8_INT1_ISR_ID:
	  EIMSK &= ~(1U << INT1);
	  break;
  #if (defined(__AVR_ATmega128__)) || (defined(__AVR_ATmega1281__))
  /* INT2 interrupt vector */
  case OSEE_AVR8_INT2_ISR_ID:
	  EIMSK &= ~(1U << INT2);
	  break;
  /* INT3 interrupt vector */
  case OSEE_AVR8_INT3_ISR_ID:
	  EIMSK &= ~(1U << INT3);
	  break;

  /* INT4 interrupt vector */
  case OSEE_AVR8_INT4_ISR_ID:
	  EIMSK &= ~(1U << INT4);
	  break;
  /* INT5 interrupt vector */
  case OSEE_AVR8_INT5_ISR_ID:
	  EIMSK &= ~(1U << INT5);
	  break;
  /* INT6 interrupt vector */
  case OSEE_AVR8_INT6_ISR_ID:
	  EIMSK &= ~(1U << INT6);
	  break;
  /* INT7 interrupt vector */
  case OSEE_AVR8_INT7_ISR_ID:
	  EIMSK &= ~(1U << INT7);
	  break;
  #endif  /* __AVR_ATmega128__ || __AVR_ATmega1281__ */

  #if (                            \
    defined(__AVR_ATmega328__)  || \
    defined(__AVR_ATmega328P__) || \
    defined(__AVR_ATmega1281__)    \
  )
  /* PCINT0 interrupt vector */
  case OSEE_AVR8_PCINT0_ISR_ID:
	  PCMSK0 &= ~(1U << PCINT0);
	  break;
  /* PCINT1 interrupt vector */
  case OSEE_AVR8_PCINT1_ISR_ID:
	  PCMSK0 &= ~(1U << PCINT1);
	  break;
  /* PCINT2 interrupt vector */
  case OSEE_AVR8_PCINT2_ISR_ID:
	  PCMSK0 &= ~(1U << PCINT2);
	  break;
  /* WDT interrupt vector */
  case OSEE_AVR8_WDT_ISR_ID:
	  WDTCSR &= ~(1U << PCINT2);
	  break;
  #endif  /* __AVR_ATmega328__ || __AVR_ATmega328P__ || __AVR_ATmega1281__ */

  #if (defined(__AVR_ATmega128__))
  /* TIMER2_COMP interrupt vector */
  case OSEE_AVR8_TIMER2_COMP_ISR_ID:

  #endif  /* __AVR_ATmega128__ */

  #if (                            \
    defined(__AVR_ATmega328__)  || \
    defined(__AVR_ATmega328P__) || \
    defined(__AVR_ATmega1281__)    \
  )
  /* TIMER2_COMPA interrupt vector */
  case OSEE_AVR8_TIMER2_COMPA_ISR_ID:
	  TIMSK2 &= ~(1U << OCIE2A);
	  break;
  /* TIMER2_COMPB interrupt vector */
  case OSEE_AVR8_TIMER2_COMPB_ISR_ID:
	  TIMSK2 &= ~(1U << OCIE2B);
	  break;
  #endif  /* __AVR_ATmega328__ || __AVR_ATmega328P__ || __AVR_ATmega1281__ */

  /* TIMER2_OVF interrupt vector */
  case OSEE_AVR8_TIMER2_OVF_ISR_ID:

  /* TIMER1_CAPT interrupt vector */
  case OSEE_AVR8_TIMER1_CAPT_ISR_ID:
	  TIMSK1 &= ~(1U << OCIE1A);
  /* TIMER1_COMPA interrupt vector */
  case OSEE_AVR8_TIMER1_COMPA_ISR_ID:
	  TIMSK1 &= ~(1U << OCIE1A);
	  break;
  /* TIMER1_COMPB interrupt vector */
  case OSEE_AVR8_TIMER1_COMPB_ISR_ID:
	  TIMSK1 &= ~(1U << OCIE1B);
	  break;
  #if (defined(__AVR_ATmega128__)) || (defined(__AVR_ATmega1281__))
  /* TIMER1_COMPC interrupt vector */
  case OSEE_AVR8_TIMER1_COMPC_ISR_ID:
	  TIMSK1 &= ~(1U << OCIE1C);
	  break;
  #endif  /* __AVR_ATmega128__ || __AVR_ATmega1281__ */

  /* TIMER1_OVF interrupt vector */
  case OSEE_AVR8_TIMER1_OVF_ISR_ID:

#if (defined(__AVR_ATmega128__))
/* TIMER0_COMP interrupt vector */
  case OSEE_AVR8_TIMER0_COMP_ISR_ID:

#endif  /* __AVR_ATmega128__ */

#if (                            \
  defined(__AVR_ATmega328__)  || \
  defined(__AVR_ATmega328P__) || \
  defined(__AVR_ATmega1281__)    \
)
/* TIMER2_COMPA interrupt vector */
 case OSEE_AVR8_TIMER0_COMPA_ISR_ID:

/* TIMER0_COMPB interrupt vector */
 case OSEE_AVR8_TIMER0_COMPB_ISR_ID:

#endif  /* __AVR_ATmega328__ || __AVR_ATmega328P__ || __AVR_ATmega1281__ */

/* TIMER0_OVF interrupt vector */
 case OSEE_AVR8_TIMER0_OVF_ISR_ID:

/* SPI_STC interrupt vector */
 case OSEE_AVR8_SPI_STC_ISR_ID:

#if (defined(__AVR_ATmega128__)) || (defined(__AVR_ATmega1281__))
/* USART0_RX interrupt vector */
 case OSEE_AVR8_USART0_RX_ISR_ID:

/* USART0_UDRE interrupt vector */
 case OSEE_AVR8_USART0_UDRE_ISR_ID:

/* USART0_TX interrupt vector */
 case OSEE_AVR8_USART0_TX_ISR_ID:

#endif  /* __AVR_ATmega128__ || __AVR_ATmega1281__ */

#if (defined(__AVR_ATmega328__)) || (defined(__AVR_ATmega328P__))
/* USART_RX interrupt vector */
 case OSEE_AVR8_USART_RX_ISR_ID:

/* USART_UDRE interrupt vector */
 case OSEE_AVR8_USART_UDRE_ISR_ID:

/* USART_TX interrupt vector */
 //case OSEE_AVR8_USART_TX_ISR_ID:

#endif  /* __AVR_ATmega328__ || __AVR_ATmega328P__ */

/* ADC interrupt vector */
 case OSEE_AVR8_ADC_ISR_ID:

/* EE_READY interrupt vector */
 case OSEE_AVR8_EE_READY_ISR_ID:

/* ANALOG_COMP interrupt vector */
 case OSEE_AVR8_ANALOG_COMP_ISR_ID:

#if (defined(__AVR_ATmega128__)) || (defined(__AVR_ATmega1281__))
/* TIMER3_CAPT interrupt vector */
 case OSEE_AVR8_TIMER3_CAPT_ISR_ID:

/* TIMER3_COMPA interrupt vector */
 case OSEE_AVR8_TIMER3_COMPA_ISR_ID:

/* TIMER3_COMPB interrupt vector */
 case OSEE_AVR8_TIMER3_COMPB_ISR_ID:

/* TIMER3_COMPC interrupt vector */
 case OSEE_AVR8_TIMER3_COMPC_ISR_ID:

/* TIMER3_OVF interrupt vector */
 case OSEE_AVR8_TIMER3_OVF_ISR_ID:

/* USART1_RX interrupt vector */
 case OSEE_AVR8_USART1_RX_ISR_ID:

/* USART1_UDRE interrupt vector */
 case OSEE_AVR8_USART1_UDRE_ISR_ID:

/* USART1_TX interrupt vector */
 case OSEE_AVR8_USART1_TX_ISR_ID:

#endif  /* __AVR_ATmega128__ || __AVR_ATmega1281__ */

/* TWI interrupt vector */
 case OSEE_AVR8_TWI_ISR_ID:

/* SPM_READY interrupt vector */
 case OSEE_AVR8_SPM_READY_ISR_ID:

#if (defined(__AVR_ATmega1281__))
/* TIMER4_CAPT interrupt vector */
 case OSEE_AVR8_TIMER4_CAPT_ISR_ID:

/* TIMER4_COMPA interrupt vector */
 case OSEE_AVR8_TIMER4_COMPA_ISR_ID:

/* TIMER4_COMPB interrupt vector */
 case OSEE_AVR8_TIMER4_COMPB_ISR_ID:

/* TIMER4_COMPC interrupt vector */
 case OSEE_AVR8_TIMER4_COMPC_ISR_ID:

/* TIMER4_OVF interrupt vector */
 case OSEE_AVR8_TIMER4_OVF_ISR_ID:

/* TIMER5_CAPT interrupt vector */
 case OSEE_AVR8_TIMER5_CAPT_ISR_ID:

/* TIMER5_COMPA interrupt vector */
 case OSEE_AVR8_TIMER5_COMPA_ISR_ID:

/* TIMER5_COMPB interrupt vector */
 case OSEE_AVR8_TIMER5_COMPB_ISR_ID:

/* TIMER5_COMPC interrupt vector */
 case OSEE_AVR8_TIMER5_COMPC_ISR_ID:

/* TIMER5_OVF interrupt vector */
 case OSEE_AVR8_TIMER5_OVF_ISR_ID:

/* USART2_RX interrupt vector */
 case OSEE_AVR8_USART2_RX_ISR_ID:

/* USART2_UDRE interrupt vector */
 case OSEE_AVR8_USART2_UDRE_ISR_ID:

/* USART2_TX interrupt vector */
 case OSEE_AVR8_USART2_TX_ISR_ID:

/* USART3_RX interrupt vector */
 case OSEE_AVR8_USART3_RX_ISR_ID:

/* USART3_UDRE interrupt vector */
 case OSEE_AVR8_USART3_UDRE_ISR_ID:

/* USART3_TX interrupt vector */
  case OSEE_AVR8_USART3_TX_ISR_ID:

#endif  /* __AVR_ATmega1281__ */
      break;

  }
}

OSEE_STATIC_INLINE void osEE_hal_enableIRQsource(OsEE_isr_src_id source_id)
{
	  switch(source_id)
	  {
	  /* INT0 interrupt vector */
	  case OSEE_AVR8_INT0_ISR_ID:
		  EIMSK |= (1U << INT0);
	  /* INT1 interrupt vector */
	  case OSEE_AVR8_INT1_ISR_ID:
		  EIMSK |= (1U << INT1);
	  #if (defined(__AVR_ATmega128__)) || (defined(__AVR_ATmega1281__))
	  /* INT2 interrupt vector */
	  case OSEE_AVR8_INT2_ISR_ID:

	  /* INT3 interrupt vector */
	  case OSEE_AVR8_INT3_ISR_ID:

	  /* INT4 interrupt vector */
	  case OSEE_AVR8_INT4_ISR_ID:

	  /* INT5 interrupt vector */
	  case OSEE_AVR8_INT5_ISR_ID:

	  /* INT6 interrupt vector */
	  case OSEE_AVR8_INT6_ISR_ID:

	  /* INT7 interrupt vector */
	  case OSEE_AVR8_INT7_ISR_ID:

	  #endif  /* __AVR_ATmega128__ || __AVR_ATmega1281__ */

	  #if (                            \
	    defined(__AVR_ATmega328__)  || \
	    defined(__AVR_ATmega328P__) || \
	    defined(__AVR_ATmega1281__)    \
	  )
	  /* PCINT0 interrupt vector */
	  case OSEE_AVR8_PCINT0_ISR_ID:

	  /* PCINT1 interrupt vector */
	  case OSEE_AVR8_PCINT1_ISR_ID:

	  /* PCINT2 interrupt vector */
	  case OSEE_AVR8_PCINT2_ISR_ID:

	  /* WDT interrupt vector */
	  case OSEE_AVR8_WDT_ISR_ID:

	  #endif  /* __AVR_ATmega328__ || __AVR_ATmega328P__ || __AVR_ATmega1281__ */

	  #if (defined(__AVR_ATmega128__))
	  /* TIMER2_COMP interrupt vector */
	  case OSEE_AVR8_TIMER2_COMP_ISR_ID:

	  #endif  /* __AVR_ATmega128__ */

	  #if (                            \
	    defined(__AVR_ATmega328__)  || \
	    defined(__AVR_ATmega328P__) || \
	    defined(__AVR_ATmega1281__)    \
	  )
	  /* TIMER2_COMPA interrupt vector */
	  case OSEE_AVR8_TIMER2_COMPA_ISR_ID:

	  /* TIMER2_COMPB interrupt vector */
	  case OSEE_AVR8_TIMER2_COMPB_ISR_ID:

	  #endif  /* __AVR_ATmega328__ || __AVR_ATmega328P__ || __AVR_ATmega1281__ */

	  /* TIMER2_OVF interrupt vector */
	  case OSEE_AVR8_TIMER2_OVF_ISR_ID:

	  /* TIMER1_CAPT interrupt vector */
	  case OSEE_AVR8_TIMER1_CAPT_ISR_ID:

	  /* TIMER1_COMPA interrupt vector */
	  case OSEE_AVR8_TIMER1_COMPA_ISR_ID:
		  TIMSK1 |= (1U << OCIE1A);
      break;
	  /* TIMER1_COMPB interrupt vector */
	  case OSEE_AVR8_TIMER1_COMPB_ISR_ID:

	  #if (defined(__AVR_ATmega128__)) || (defined(__AVR_ATmega1281__))
	  /* TIMER1_COMPC interrupt vector */
	  case OSEE_AVR8_TIMER1_COMPC_ISR_ID:

	  #endif  /* __AVR_ATmega128__ || __AVR_ATmega1281__ */

	  /* TIMER1_OVF interrupt vector */
	  case OSEE_AVR8_TIMER1_OVF_ISR_ID:
	#if (defined(__AVR_ATmega128__))
	/* TIMER0_COMP interrupt vector */
	  case OSEE_AVR8_TIMER0_COMP_ISR_ID:

	#endif  /* __AVR_ATmega128__ */

	#if (                            \
	  defined(__AVR_ATmega328__)  || \
	  defined(__AVR_ATmega328P__) || \
	  defined(__AVR_ATmega1281__)    \
	)
	/* TIMER2_COMPA interrupt vector */
	 case OSEE_AVR8_TIMER0_COMPA_ISR_ID:

	/* TIMER0_COMPB interrupt vector */
	 case OSEE_AVR8_TIMER0_COMPB_ISR_ID:

	#endif  /* __AVR_ATmega328__ || __AVR_ATmega328P__ || __AVR_ATmega1281__ */

	/* TIMER0_OVF interrupt vector */
	 case OSEE_AVR8_TIMER0_OVF_ISR_ID:

	/* SPI_STC interrupt vector */
	 case OSEE_AVR8_SPI_STC_ISR_ID:

	#if (defined(__AVR_ATmega128__)) || (defined(__AVR_ATmega1281__))
	/* USART0_RX interrupt vector */
	 case OSEE_AVR8_USART0_RX_ISR_ID:

	/* USART0_UDRE interrupt vector */
	 case OSEE_AVR8_USART0_UDRE_ISR_ID:

	/* USART0_TX interrupt vector */
	 case OSEE_AVR8_USART0_TX_ISR_ID:

	#endif  /* __AVR_ATmega128__ || __AVR_ATmega1281__ */

	#if (defined(__AVR_ATmega328__)) || (defined(__AVR_ATmega328P__))
	/* USART_RX interrupt vector */
	 case OSEE_AVR8_USART_RX_ISR_ID:

	/* USART_UDRE interrupt vector */
	 case OSEE_AVR8_USART_UDRE_ISR_ID:

	/* USART_TX interrupt vector */
	 //case OSEE_AVR8_USART_TX_ISR_ID:

	#endif  /* __AVR_ATmega328__ || __AVR_ATmega328P__ */

	/* ADC interrupt vector */
	 case OSEE_AVR8_ADC_ISR_ID:

	/* EE_READY interrupt vector */
	 case OSEE_AVR8_EE_READY_ISR_ID:

	/* ANALOG_COMP interrupt vector */
	 case OSEE_AVR8_ANALOG_COMP_ISR_ID:

	#if (defined(__AVR_ATmega128__)) || (defined(__AVR_ATmega1281__))
	/* TIMER3_CAPT interrupt vector */
	 case OSEE_AVR8_TIMER3_CAPT_ISR_ID:

	/* TIMER3_COMPA interrupt vector */
	 case OSEE_AVR8_TIMER3_COMPA_ISR_ID:

	/* TIMER3_COMPB interrupt vector */
	 case OSEE_AVR8_TIMER3_COMPB_ISR_ID:

	/* TIMER3_COMPC interrupt vector */
	 case OSEE_AVR8_TIMER3_COMPC_ISR_ID:

	/* TIMER3_OVF interrupt vector */
	 case OSEE_AVR8_TIMER3_OVF_ISR_ID:

	/* USART1_RX interrupt vector */
	 case OSEE_AVR8_USART1_RX_ISR_ID:

	/* USART1_UDRE interrupt vector */
	 case OSEE_AVR8_USART1_UDRE_ISR_ID:

	/* USART1_TX interrupt vector */
	 case OSEE_AVR8_USART1_TX_ISR_ID:

	#endif  /* __AVR_ATmega128__ || __AVR_ATmega1281__ */

	/* TWI interrupt vector */
	 case OSEE_AVR8_TWI_ISR_ID:

	/* SPM_READY interrupt vector */
	 case OSEE_AVR8_SPM_READY_ISR_ID:

	#if (defined(__AVR_ATmega1281__))
	/* TIMER4_CAPT interrupt vector */
	 case OSEE_AVR8_TIMER4_CAPT_ISR_ID:

	/* TIMER4_COMPA interrupt vector */
	 case OSEE_AVR8_TIMER4_COMPA_ISR_ID:

	/* TIMER4_COMPB interrupt vector */
	 case OSEE_AVR8_TIMER4_COMPB_ISR_ID:

	/* TIMER4_COMPC interrupt vector */
	 case OSEE_AVR8_TIMER4_COMPC_ISR_ID:

	/* TIMER4_OVF interrupt vector */
	 case OSEE_AVR8_TIMER4_OVF_ISR_ID:

	/* TIMER5_CAPT interrupt vector */
	 case OSEE_AVR8_TIMER5_CAPT_ISR_ID:

	/* TIMER5_COMPA interrupt vector */
	 case OSEE_AVR8_TIMER5_COMPA_ISR_ID:

	/* TIMER5_COMPB interrupt vector */
	 case OSEE_AVR8_TIMER5_COMPB_ISR_ID:

	/* TIMER5_COMPC interrupt vector */
	 case OSEE_AVR8_TIMER5_COMPC_ISR_ID:

	/* TIMER5_OVF interrupt vector */
	 case OSEE_AVR8_TIMER5_OVF_ISR_ID:

	/* USART2_RX interrupt vector */
	 case OSEE_AVR8_USART2_RX_ISR_ID:

	/* USART2_UDRE interrupt vector */
	 case OSEE_AVR8_USART2_UDRE_ISR_ID:

	/* USART2_TX interrupt vector */
	 case OSEE_AVR8_USART2_TX_ISR_ID:

	/* USART3_RX interrupt vector */
	 case OSEE_AVR8_USART3_RX_ISR_ID:

	/* USART3_UDRE interrupt vector */
	 case OSEE_AVR8_USART3_UDRE_ISR_ID:

	/* USART3_TX interrupt vector */
	  case OSEE_AVR8_USART3_TX_ISR_ID:

	#endif  /* __AVR_ATmega1281__ */
	      break;
}
}

/* not supported in AVR */
OSEE_STATIC_INLINE void osEE_hal_clearpend_int (OsEE_isr_src_id source_id)
{
	/* Touch unused paramters */
	(void)source_id;
}

OSEE_STATIC_INLINE void osEE_hal_resumeIRQ ( OsEE_reg flags )
{
  OSEE_BARRIER();
  SREG = flags;
}

OSEE_STATIC_INLINE void osEE_hal_set_ipl(TaskPrio virt_prio) {
  (void)virt_prio; /* No IPL support for AVR8 arch */
}

OSEE_STATIC_INLINE OsEE_reg
  osEE_hal_prepare_ipl(OsEE_reg flags, TaskPrio virt_prio)
{
  /* No IPL support for AVR8 arch */
  (void)virt_prio;
  return flags;
}

/*==============================================================================
                    HAL For Primitives Synchronization
 =============================================================================*/

/* Called as _first_ function of a primitive that can be called from within
 * an IRQ and from within a task. */
OSEE_STATIC_INLINE OsEE_reg osEE_hal_begin_nested_primitive( void )
{
  return osEE_hal_suspendIRQ();
}

/* Called as _last_ function of a primitive that can be called from
 * within an IRQ or a task. */
OSEE_STATIC_INLINE void osEE_hal_end_nested_primitive( OsEE_reg flag )
{
  osEE_hal_resumeIRQ(flag);
}

/*==============================================================================
                              Start-up and ISR2
 =============================================================================*/

extern void osEE_atmega_intvect(void);

#if (defined(OSEE_HAS_SYSTEM_TIMER))
extern void osEE_avr8_system_timer_init(void);
#endif /* OSEE_HAS_SYSTEM_TIMER */

#define OSEE_CPU_STARTOS_INLINE OSEE_STATIC_INLINE

/* Nothing to do. All the initialiazation is done in osEE_os_init */
OSEE_CPU_STARTOS_INLINE OsEE_bool osEE_cpu_startos ( void )
{
  /* Interrupt Vector Table Recall. */
  OsEE_void_cb volatile ivt             =  &osEE_atmega_intvect;
  OsEE_bool    const    cpu_startos_ok  = osEE_std_cpu_startos();
#if (defined(OSEE_HAS_SYSTEM_TIMER))
  if (cpu_startos_ok) {
    osEE_avr8_system_timer_init();
  }
#endif /* OSEE_HAS_SYSTEM_TIMER */
  return cpu_startos_ok;
}

OSEE_STATIC_INLINE FUNC(void, OS_CODE)
  osEE_change_context_from_isr2_end
(
  P2VAR(OsEE_TDB, AUTOMATIC, OS_APPL_CONST) p_from,
  P2VAR(OsEE_TDB, AUTOMATIC, OS_APPL_CONST) p_to
)
{
  osEE_change_context_from_task_end(p_from, p_to);
}
#if (defined(__cplusplus))
}
#endif

#endif /* !OSEE_HAL_INTERNAL_H */
