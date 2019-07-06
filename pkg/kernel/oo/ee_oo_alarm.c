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

/** \file   ee_oo_alarm.c
 *  \brief  OSEK AlarmsImplementation.
 *
 *  This files contains all OSEK Kernel Alarms Implementation in Erika
 *  Enterprise.
 *
 *  \author Errico Guidieri
 *  \date   2017
 */

#include "ee_internal.h"

FUNC(StatusType, OS_CODE)
  osEE_alarm_set_rel
(
  P2VAR(OsEE_CounterDB, AUTOMATIC, OS_APPL_DATA)  p_counter_db,
  P2VAR(OsEE_AlarmDB, AUTOMATIC, OS_APPL_DATA)    p_alarm_db,
  VAR(TickType,   AUTOMATIC)                      increment,
  VAR(TickType,   AUTOMATIC)                      cycle
)
{
  VAR(StatusType, AUTOMATIC) ev;
  // get pointer to alarm control block of current alarm DB
  CONSTP2VAR(OsEE_AlarmCB, AUTOMATIC, OS_APPL_DATA)
    p_alarm_cb = osEE_alarm_get_cb(p_alarm_db);
  // get pointer to trigger descriptor block of current alarm DB
  CONSTP2VAR(OsEE_TriggerDB, AUTOMATIC, OS_APPL_DATA)
    p_trigger_db = osEE_alarm_get_trigger_db(p_alarm_db);
  // get pointer to trigger control block
  CONSTP2VAR(OsEE_TriggerCB, AUTOMATIC, OS_APPL_DATA)
    p_trigger_cb = p_trigger_db->p_trigger_cb;
  /** if multicore lock the current core that counter is using*/
#if (!defined(OSEE_SINGLECORE))
  CONST(CoreIdType, AUTOMATIC)
    counter_core_id = p_counter_db->core_id;
  /* Lock the Core Lock witch the counter is tied */
  osEE_lock_core_id(counter_core_id);
#endif /* OSEE_SINGLECORE */

  /** check whether alarm is already in use, return E_OS_STATE*/
  if (p_trigger_cb->status > OSEE_TRIGGER_CANCELED) {
    /**if not status is inactive or canceled, then alarm is in use*/
    ev = E_OS_STATE;
  } else if (p_trigger_cb->status == OSEE_TRIGGER_CANCELED) {
    p_alarm_cb->cycle = cycle;
    /* Re-turn on the trigger, that is in handling, since is handling I'll set
       'here' when based on increment */
    p_trigger_cb->when   = osEE_counter_eval_when(p_counter_db, increment);
    p_trigger_cb->status = OSEE_TRIGGER_REENABLED;

    ev = E_OK;
  } else {
    p_alarm_cb->cycle = cycle;
    /* Turn On the Trigger */
    p_trigger_cb->status = OSEE_TRIGGER_ACTIVE;
    /* insert rel trigger*/
    osEE_counter_insert_rel_trigger(
      p_counter_db, p_trigger_db, increment
    );
 
    ev = E_OK;
  }
#if (!defined(OSEE_SINGLECORE))
  osEE_unlock_core_id(counter_core_id);
#endif /* OSEE_SINGLECORE */
  return ev;
}

/**
 * @brief set alarm to fire at abs value
 * @param[in] p_counter_db pointer to counter descriptor block
 * @param[in] p_alarm_db pointer to alarm descriptor block
 * @param[in] start start Absolute value in ticks
 * @param[in] cycle Cycle value in case of cyclic alarm. In case of single alarms, cycle shall be zero.
 * @return StatusType
 */
FUNC(StatusType, OS_CODE)
  osEE_alarm_set_abs
(
  P2VAR(OsEE_CounterDB, AUTOMATIC, OS_APPL_DATA)  p_counter_db,
  P2VAR(OsEE_AlarmDB, AUTOMATIC, OS_APPL_DATA)    p_alarm_db,
  VAR(TickType,   AUTOMATIC)                      start,
  VAR(TickType,   AUTOMATIC)                      cycle
)
{
  VAR(StatusType, AUTOMATIC) ev;
  CONSTP2VAR(OsEE_AlarmCB, AUTOMATIC, OS_APPL_DATA)
    p_alarm_cb = osEE_alarm_get_cb(p_alarm_db); /**< get pointer to alarm control block*/
  CONSTP2VAR(OsEE_TriggerDB, AUTOMATIC, OS_APPL_DATA)
    p_trigger_db = osEE_alarm_get_trigger_db(p_alarm_db); /**< pointer to trigger descriptor block*/
  CONSTP2VAR(OsEE_TriggerCB, AUTOMATIC, OS_APPL_DATA)
    p_trigger_cb = p_trigger_db->p_trigger_cb;/**< pointer to trigger control block*/
#if (!defined(OSEE_SINGLECORE))/** if multicore*/
  CONST(CoreIdType, AUTOMATIC)
    counter_core_id = p_counter_db->core_id;
/* Lock the Core Lock to whom the counter is tied */
  osEE_lock_core_id(counter_core_id);
#endif /* OSEE_SINGLECORE */

  /** check if the trigger status is not inactive or cancelled, which means alarm is in use, then return E_OS_STATE */
  if (p_trigger_cb->status > OSEE_TRIGGER_CANCELED) {
    ev = E_OS_STATE;
  } else if (p_trigger_cb->status == OSEE_TRIGGER_CANCELED) {//if the trigger status is canceled, set when= start and reenable the alarm, and set return status to E_OK
    p_alarm_cb->cycle = cycle;
    /* Re-turn on the trigger, that is in handling, since is handling I'll set
       here 'when' based on start */
    p_trigger_cb->when   = start;
    p_trigger_cb->status = OSEE_TRIGGER_REENABLED;

    ev = E_OK;
  } else {
    p_alarm_cb->cycle = cycle;
    /* Turn On the Trigger */
    p_trigger_cb->status = OSEE_TRIGGER_ACTIVE;
    /** insert the new trigger in the triggers queue*/
    osEE_counter_insert_abs_trigger(
      p_counter_db, p_trigger_db, start
    );

    ev = E_OK;
  }
#if (!defined(OSEE_SINGLECORE))
  osEE_unlock_core_id(counter_core_id);
#endif /* OSEE_SINGLECORE */
  return ev;
}

/**
 * brief cancel an alarm
 * param[in] p_alarm_db pointer to the alarm descriptor block
 * @return status of the call
 */
FUNC(StatusType, OS_CODE)
  osEE_alarm_cancel
(
  P2VAR(OsEE_AlarmDB, AUTOMATIC, OS_APPL_DATA)    p_alarm_db
)
{
  VAR(StatusType, AUTOMATIC) ev;/**< statustype that will be returned*/
  //get pointer to trigger descriptor block
  CONSTP2VAR(OsEE_TriggerDB, AUTOMATIC, OS_APPL_DATA)
    p_trigger_db = osEE_alarm_get_trigger_db(p_alarm_db);
  //get pointer to trigger control block
  CONSTP2VAR(OsEE_TriggerCB, AUTOMATIC, OS_APPL_DATA)
    p_trigger_cb = p_trigger_db->p_trigger_cb;
  //get pointer to counter descriptor block
  CONSTP2VAR(OsEE_CounterDB, AUTOMATIC, OS_APPL_DATA)
    p_counter_db = p_trigger_db->p_counter_db;
#if (!defined(OSEE_SINGLECORE))
//get core id and lock it
  CONST(CoreIdType, AUTOMATIC)
    counter_core_id = p_counter_db->core_id;
/* Lock the Core Lock to whom the counter is tied */
  osEE_lock_core_id(counter_core_id);
#endif /* OSEE_SINGLECORE */

  /*check whether the alarm is already canceled or inactive, if so return E_OS_NOFUNC status*/
  if (p_trigger_cb->status <= OSEE_TRIGGER_CANCELED) {
    ev = E_OS_NOFUNC;
  } 
  /*if the alarm status is expired or reenabled, set it's status canceled and return E_OK*/
  else if (p_trigger_cb->status >= OSEE_TRIGGER_EXPIRED) {
    p_trigger_cb->status = OSEE_TRIGGER_CANCELED;
    ev = E_OK;
  } 
  /*if the status is active, cancel the counter and return E_OK status*/
  else {
    p_trigger_cb->status = OSEE_TRIGGER_INACTIVE;
    osEE_counter_cancel_trigger(p_counter_db, p_trigger_db);
    ev = E_OK;
  }
  //unlock current core
#if (!defined(OSEE_SINGLECORE))
  osEE_unlock_core_id(counter_core_id);
#endif /* OSEE_SINGLECORE */
  return ev;
}

/* get ticks before alarm expires */
FUNC(StatusType, OS_CODE)
  osEE_alarm_get
(
  P2VAR(OsEE_AlarmDB, AUTOMATIC, OS_APPL_DATA)    p_alarm_db,
  P2VAR(TickType, AUTOMATIC, OS_APPL_DATA)        p_tick
)
{
  VAR(StatusType, AUTOMATIC) ev;
  CONSTP2VAR(OsEE_TriggerDB, AUTOMATIC, OS_APPL_DATA)
    p_trigger_db = osEE_alarm_get_trigger_db(p_alarm_db); /**<pointer to trigger descriptor block*/
  CONSTP2CONST(OsEE_TriggerCB, AUTOMATIC, OS_APPL_DATA)
    p_trigger_cb = p_trigger_db->p_trigger_cb;  /**< pointer to trigger control block */
  CONSTP2VAR(OsEE_CounterDB, AUTOMATIC, OS_APPL_DATA)
    p_counter_db = p_trigger_db->p_counter_db;  /**< pointer counter descriptor block */
#if (!defined(OSEE_SINGLECORE))//multicore
/* get the current core id */
  CONST(CoreIdType, AUTOMATIC)
    counter_core_id = p_counter_db->core_id;
/* Lock the Core Lock to whom the counter is tied */
  osEE_lock_core_id(counter_core_id);
#endif /* OSEE_SINGLECORE */
  /* check if the trigger status is inactive or canceled, then return E_OS_NOFUNC status*/
  if (p_trigger_cb->status <= OSEE_TRIGGER_CANCELED) {
    ev = E_OS_NOFUNC;
  } else {/* trigger is active, get the ticks before expiration and return E_OK status */
    *p_tick = osEE_counter_eval_delta(p_counter_db, p_trigger_cb->when);
    ev = E_OK;
  }
#if (!defined(OSEE_SINGLECORE))
  /* if multicore unlock core */
  osEE_unlock_core_id(counter_core_id);
#endif /* OSEE_SINGLECORE */
  return ev;
}

