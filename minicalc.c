// File: minicalc.c
#include <stdlib.h>
#include "raylib.h"

#define NUM_BUTTONS 20
#define DISPLAY_LEN 15
#define DISPLAY_MAX 9999999999999
#define DISPLAY_MIN -9999999999999

typedef __int128 calc_int;

typedef struct {
    char * label;
    int x;
    int y;
} button;

void set_display_num(char * display_num, calc_int num) {
    int i = 0;
    if (num < 0) {
        num *= -1;
        display_num[i] = '-';
        i++;
    }
    calc_int power = 1;
    while (power * 10 <= num) power *= 10;
    while (power > 0) {
        display_num[i] = (num / power) % 10 + '0';
        i++;
        power /= 10;
    }
    display_num[i] = '\0';
}

calc_int append_digit(calc_int active, calc_int digit) {
    active *= 10;
    if (active < 0) {
        active -= digit;
        if (active < DISPLAY_MIN) active = DISPLAY_MIN;
    }
    else {
        active += digit;
        if (active > DISPLAY_MAX) active = DISPLAY_MAX;
    }
    return active;
}

calc_int do_math(calc_int a, char op, calc_int b) {
    calc_int result = a;
    switch (op) {
        case '+':
            result += b;
            break;
        case '-':
            result -= b;
            break;
        case '*':
            result *= b;
            break;
        case '/':
            if (b == 0) {
                if (result < 0) result = DISPLAY_MIN;
                else result = DISPLAY_MAX;
            }
            else result /= b;
            break;
        case '%':
            if (b == 0) {
                if (result < 0) result = DISPLAY_MIN;
                else result = DISPLAY_MAX;
            }
            else result %= b;
            break;
        case '^':
            if (b < 0) result = 0;
            else if (b == 0) result = 1;
            else {
                for (calc_int i = b; i > 1; i--) {
                    result *= a;
                    if (result <= DISPLAY_MIN || result >= DISPLAY_MAX) {
                        if (a < 0 && b % 2 == 1) result = DISPLAY_MIN;
                        else result = DISPLAY_MAX;
                        break;
                    }
                }
            }
            break;
    }
    if (result < DISPLAY_MIN) result = DISPLAY_MIN;
    else if (result > DISPLAY_MAX) result = DISPLAY_MAX;
    return result;
}

int main() {
    const int B_SIZE = 100;
    const int W_WIDTH = B_SIZE * 4;
    const int W_HEIGHT = B_SIZE * 6;
    const int FONT_PX = 40;
    const int PADDING_PX = 2;
    const int TEXT_OFFSET_Y = (B_SIZE - FONT_PX) / 2 - PADDING_PX;
    const int TEXT_OFFSET_X = TEXT_OFFSET_Y + FONT_PX / 10;
    const float LINE_PX = 2.0;
    const Rectangle VIEWBOX = {PADDING_PX, PADDING_PX,
    W_WIDTH - PADDING_PX * 2, B_SIZE - PADDING_PX * 2};

    char * labels[NUM_BUTTONS] = {
        "C", "^", "%", "/",
        "7", "8", "9", "*",
        "4", "5", "6", "-",
        "1", "2", "3", "+",
        "<", "0", "~", "="
    };

    button * calc_buttons = malloc(NUM_BUTTONS * sizeof(button));
    for (int i = 0; i < NUM_BUTTONS; i++) {
        calc_buttons[i].label = labels[i];
        calc_buttons[i].x = B_SIZE * (i % 4) + PADDING_PX;
        calc_buttons[i].y = B_SIZE * (i / 4 + 1) + PADDING_PX;
    }

    calc_int active = 0;
    calc_int memory = 0;
    char operation = '+';

    char * display_num = malloc(DISPLAY_LEN * sizeof(char));
    set_display_num(display_num, active);

    int mouse_x = 0;
    int mouse_y = 0;
    int selected_button;
    Rectangle buffer = {0, 0, B_SIZE - PADDING_PX * 2, B_SIZE - PADDING_PX * 2};

    InitWindow(W_WIDTH, W_HEIGHT, "Minicalc");
    SetTargetFPS(30);

    while (!WindowShouldClose()) {
        mouse_x = GetMouseX();
        mouse_y = GetMouseY();
        selected_button = -1;
        for (int i = 0; i < NUM_BUTTONS; i++) {
            if (mouse_x > calc_buttons[i].x && mouse_y > calc_buttons[i].y
            && mouse_x < calc_buttons[i].x + B_SIZE - PADDING_PX * 2
            && mouse_y < calc_buttons[i].y + B_SIZE - PADDING_PX * 2) {
                selected_button = i;
                break;
            }
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && selected_button > -1) {
            switch (*(calc_buttons[selected_button].label)) {
            case '0':
                active = append_digit(active, 0);
                break;
            case '1':
                active = append_digit(active, 1);
                break;
            case '2':
                active = append_digit(active, 2);
                break;
            case '3':
                active = append_digit(active, 3);
                break;
            case '4':
                active = append_digit(active, 4);
                break;
            case '5':
                active = append_digit(active, 5);
                break;
            case '6':
                active = append_digit(active, 6);
                break;
            case '7':
                active = append_digit(active, 7);
                break;
            case '8':
                active = append_digit(active, 8);
                break;
            case '9':
                active = append_digit(active, 9);
                break;
            case 'C':
                active = 0;
                memory = 0;
                operation = '+';
                break;
            case '<':
                active /= 10;
                break;
            case '~':
                active *= -1;
                break;
            case '=':
                active = do_math(memory, operation, active);
                memory = 0;
                operation = '+';
                break;
            default:
                // (+, -, *, /, %, ^)
                memory = do_math(memory, operation, active);
                active = 0;
                operation = *(calc_buttons[selected_button].label);
                break;
            }
            set_display_num(display_num, active);
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) selected_button = -1;

        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleLinesEx(VIEWBOX, LINE_PX, WHITE);
        DrawText(display_num, TEXT_OFFSET_X, TEXT_OFFSET_Y, FONT_PX, WHITE);
        for (int i = 0; i < NUM_BUTTONS; i++) {
            buffer.x = calc_buttons[i].x;
            buffer.y = calc_buttons[i].y;
            if (selected_button == i) {
                DrawRectangleRec(buffer, WHITE);
                DrawText(calc_buttons[i].label, calc_buttons[i].x
                + TEXT_OFFSET_X, calc_buttons[i].y + TEXT_OFFSET_Y,
                FONT_PX, BLACK);
            }
            else {
                DrawRectangleLinesEx(buffer, LINE_PX, WHITE);
                DrawText(calc_buttons[i].label, calc_buttons[i].x
                + TEXT_OFFSET_X, calc_buttons[i].y + TEXT_OFFSET_Y,
                FONT_PX, WHITE);
            }
        }
        EndDrawing();
    }

    CloseWindow();
    free(calc_buttons);
    free(display_num);
    return 0;
}
