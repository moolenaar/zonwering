/* kernel.h
 * Copyright 2015 - 2019 Herman Moolenaar
 * This file is part of zonwering.
 *
 * zonwering is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * zonwering is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with zonwering. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KERNEL_H
#define KERNEL_H

#include "general.h"

#define NRTASKS 5
#define MINIMUMSTACKSIZE 39

typedef void (*TaskFunction)(void);

enum TaskState_t
{
   StateStopped = 0x10,
   StateWaiting = 0x20,
   StatePriority = 0x30,
   StateRunable = 0x40
};

void KernelSetup(void);
uint8_t InitTask(uint16_t stackSize, uint8_t* stackBuffer, TaskFunction function);
void StartKernel(TaskFunction function);
void TaskSleep(uint16_t time);
void TaskStart(uint8_t index);
void TaskStop(uint8_t index);
void EnableTaskSwitching(void);

#endif // KERNEL_H
