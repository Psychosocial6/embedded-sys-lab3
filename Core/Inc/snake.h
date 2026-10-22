/*
 * snake.h
 *
 *  Created on: Oct 21, 2026
 *      Author: andre
 */

#ifndef SNAKE_H_
#define SNAKE_H_

#include <stdint.h>

void Snake_Init(void);
void Snake_Step(void);
void Snake_HandleInput(char key);
uint32_t Snake_GetDelay(void);

#endif /* INC_SNAKE_H_ */
