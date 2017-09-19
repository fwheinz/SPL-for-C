/*
 * File: gtimer.c
 * --------------
 * This file implements the gtimer.h interface.  The functions that
 * implement receiving timer from the environment are implemented in
 * the platform package.
 */

/*************************************************************************/
/* Stanford Portable Library                                             */
/* Copyright (C) 2013 by Eric Roberts <eroberts@cs.stanford.edu>         */
/*                                                                       */
/* This program is free software: you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation, either version 3 of the License, or     */
/* (at your option) any later version.                                   */
/*                                                                       */
/* This program is distributed in the hope that it will be useful,       */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/* GNU General Public License for more details.                          */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */
/*************************************************************************/

#include <stdio.h>
#include "cslib.h"
#include "gtimer.h"
#include "platform.h"

/*
 * Type GTimerCDT
 * --------------
 * This type is the concrete type for the timer.
 */

struct GTimerCDT {
   double duration;
};

GTimer newGTimer(double milliseconds) {
   GTimer timer;

   timer = newBlock(GTimer);
   timer->duration = milliseconds;
   createTimerOp(timer, milliseconds);
   return timer;
}

void freeGTimer(GTimer timer) {
   deleteTimerOp(timer);
}

void startTimer(GTimer timer) {
   startTimerOp(timer);
}

void stopTimer(GTimer timer) {
   stopTimerOp(timer);
}
