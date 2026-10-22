#include "snake.h"
#include "oled.h"
#include "fonts.h"
#include "main.h"

#define GRID_W          28
#define GRID_H          12
#define MAX_SNAKE_LEN   120

#define OFFSET_X        8
#define OFFSET_Y        14

typedef enum {
    DIR_UP = 0,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction_t;

typedef enum {
    STATE_START = 0,
    STATE_PLAYING,
    STATE_GAMEOVER
} GameState_t;

typedef struct {
    int8_t x;
    int8_t y;
} Point_t;

static Point_t s_snake[MAX_SNAKE_LEN];
static int s_snake_len = 3;
static Direction_t s_current_dir = DIR_RIGHT;
static Direction_t s_next_dir = DIR_RIGHT;
static Point_t s_food;
static int s_score = 0;
static int s_high_score = 0;
static GameState_t s_state = STATE_START;
static uint32_t s_rng_state = 1234567;

static uint32_t rng_next(void) {
    s_rng_state = s_rng_state * 1664525u + 1013904223u;
    return s_rng_state;
}

static void int_to_str(int val, char *buf) {
    if (val < 0) val = 0;
    char temp[10];
    int i = 0;
    if (val == 0) {
        temp[i++] = '0';
    } else {
        while (val > 0 && i < 9) {
            temp[i++] = '0' + (val % 10);
            val /= 10;
        }
    }
    int j = 0;
    while (i > 0) {
        buf[j++] = temp[--i];
    }
    buf[j] = '\0';
}

static void draw_filled_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, OLED_COLOR color) {
    for (uint8_t i = 0; i < h; i++) {
        oled_DrawHLine(x, x + w - 1, y + i, color);
    }
}

static void spawn_food(void) {
    int valid = 0;
    while (!valid) {
        s_food.x = rng_next() % GRID_W;
        s_food.y = rng_next() % GRID_H;
        valid = 1;
        for (int i = 0; i < s_snake_len; i++) {
            if (s_snake[i].x == s_food.x && s_snake[i].y == s_food.y) {
                valid = 0;
                break;
            }
        }
    }
}

void Snake_Init(void) {
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);


    s_snake_len = 4;
    s_snake[0].x = 8;  s_snake[0].y = 6;
    s_snake[1].x = 7;  s_snake[1].y = 6;
    s_snake[2].x = 6;  s_snake[2].y = 6;
    s_snake[3].x = 5;  s_snake[3].y = 6;

    s_current_dir = DIR_RIGHT;
    s_next_dir = DIR_RIGHT;
    s_score = 0;
    spawn_food();
}

uint32_t Snake_GetDelay(void) {
    if (s_state != STATE_PLAYING) {
        return 100;
    }
    // Скорость змейки увеличивается по мере набора очков (от 160 мс до 70 мс)
    int delay = 160 - (s_score / 10) * 10;
    if (delay < 60) delay = 60;
    return (uint32_t)delay;
}


void Snake_HandleInput(char key) {
    s_rng_state ^= (uint32_t)key + 0x9E3779B9;

    if (s_state == STATE_START || s_state == STATE_GAMEOVER) {
        if (key != 0) {
            Snake_Init();
            s_state = STATE_PLAYING;
        }
        return;
    }

    if (s_state == STATE_PLAYING) {
        if (key == '2' && s_current_dir != DIR_DOWN) {
            s_next_dir = DIR_UP;
        } else if (key == '8' && s_current_dir != DIR_UP) {
            s_next_dir = DIR_DOWN;
        } else if (key == '4' && s_current_dir != DIR_RIGHT) {
            s_next_dir = DIR_LEFT;
        } else if (key == '6' && s_current_dir != DIR_LEFT) {
            s_next_dir = DIR_RIGHT;
        }
    }
}

static void draw_game(void) {
    oled_Fill(Black);

    // 1. Верхняя панель счета (Y = 1..10)
    char s_buf[10];
    oled_SetCursor(4, 1);
    oled_WriteString("SC:", Font_7x10, White);
    int_to_str(s_score, s_buf);
    oled_WriteString(s_buf, Font_7x10, White);

    oled_SetCursor(68, 1);
    oled_WriteString("HI:", Font_7x10, White);
    int_to_str(s_high_score, s_buf);
    oled_WriteString(s_buf, Font_7x10, White);

    // 2. Рамка игрового поля (X: 7..120, Y: 13..62) - симметрично по 7 пикселей по краям
    oled_DrawSquare(7, 120, 13, 62, White);

    // 3. Отрисовка еды (ромбик 3x3)
    uint8_t fx = OFFSET_X + s_food.x * 4;
    uint8_t fy = OFFSET_Y + s_food.y * 4;
    oled_DrawPixel(fx + 1, fy,     White);
    oled_DrawPixel(fx,     fy + 1, White);
    oled_DrawPixel(fx + 2, fy + 1, White);
    oled_DrawPixel(fx + 1, fy + 2, White);

    // 4. Отрисовка змейки (квадраты 3x3 внутри ячеек 4x4)
    for (int i = 0; i < s_snake_len; i++) {
        uint8_t sx = OFFSET_X + s_snake[i].x * 4;
        uint8_t sy = OFFSET_Y + s_snake[i].y * 4;
        draw_filled_rect(sx, sy, 3, 3, White);
    }

    oled_UpdateScreen();
}

static void draw_menu(void) {
    oled_Fill(Black);

    // Заголовок (10 симв * 11 px = 110 px), центрирование: (128 - 110) / 2 = 9
    oled_SetCursor(9, 6);
    oled_WriteString("SNAKE", Font_11x18, White);

    // Управление (14 симв * 7 px = 98 px), центрирование: (128 - 98) / 2 = 15
    oled_SetCursor(15, 28);
    oled_WriteString("2,4,6,8 : MOVE", Font_7x10, White);

    // Подсказка старта (15 симв * 7 px = 105 px), центрирование: (128 - 105) / 2 = 11
    oled_SetCursor(11, 44);
    oled_WriteString("PRESS 5 TO PLAY", Font_7x10, White);

    oled_UpdateScreen();
}

static void draw_gameover(void) {
    oled_Fill(Black);
    char buf[10];

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

    // Заголовок (9 симв * 11 px = 99 px), центрирование: (128 - 99) / 2 = 14
    oled_SetCursor(14, 6);
    oled_WriteString("GAME OVER", Font_11x18, White);

    // Счет: "SCORE: " (7 симв) + число (до 3 симв) = 10 симв (70 px), центрирование: (128 - 70) / 2 = 29
    oled_SetCursor(29, 28);
    oled_WriteString("SCORE: ", Font_7x10, White);
    int_to_str(s_score, buf);
    oled_WriteString(buf, Font_7x10, White);

    // Подсказка рестарта (15 симв * 7 px = 105 px), центрирование: (128 - 105) / 2 = 11
    oled_SetCursor(11, 44);
    oled_WriteString("PRESS 5 TO PLAY", Font_7x10, White);

    oled_UpdateScreen();
}

void Snake_Step(void) {
    if (s_state == STATE_START) {
        draw_menu();
        return;
    }

    if (s_state == STATE_GAMEOVER) {
        draw_gameover();
        return;
    }

    // Состояние STATE_PLAYING: расчет шага змейки
    s_current_dir = s_next_dir;
    Point_t new_head = s_snake[0];

    switch (s_current_dir) {
        case DIR_UP:    new_head.y--; break;
        case DIR_DOWN:  new_head.y++; break;
        case DIR_LEFT:  new_head.x--; break;
        case DIR_RIGHT: new_head.x++; break;
    }

    // Столкновение со стеной
    if (new_head.x < 0 || new_head.x >= GRID_W ||
        new_head.y < 0 || new_head.y >= GRID_H) {
        s_state = STATE_GAMEOVER;
        // Зажигаем красный светодиод (PD14) при проигрыше
        //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
        //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
        draw_gameover();
        return;
    }

    // Столкновение с собственным телом
    for (int i = 0; i < s_snake_len - 1; i++) {
        if (s_snake[i].x == new_head.x && s_snake[i].y == new_head.y) {
            s_state = STATE_GAMEOVER;
            //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
            //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
            draw_gameover();
            return;
        }
    }

    // Сдвиг сегментов тела
    for (int i = s_snake_len; i > 0; i--) {
        s_snake[i] = s_snake[i - 1];
    }
    s_snake[0] = new_head;

    // Проверка поедания яблока
    if (new_head.x == s_food.x && new_head.y == s_food.y) {
        s_score += 10;
        if (s_score > s_high_score) {
            s_high_score = s_score;
        }
        if (s_snake_len < MAX_SNAKE_LEN - 1) {
            s_snake_len++;
        }
        spawn_food();
        // Мигаем синим светодиодом (PD15) при поедании яблока
        //HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
    }

    draw_game();
}
