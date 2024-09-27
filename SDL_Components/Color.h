#pragma once

struct Color {
    unsigned char r, g, b, a;

    Color();
    Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255);

    Color& operator*=(float x);
};

const Color BLACK      = {  0,   0,   0};
const Color DARK_GRAY  = { 51,  51,  51};
const Color GRAY       = {153, 153, 153};
const Color LIGHT_GRAY = {204, 204, 204};
const Color WHITE      = {255, 255, 255};
const Color RED        = {255,   0,   0};
const Color GREEN      = {  0, 255,   0};
const Color BLUE       = {  0,   0, 255};
const Color YELLOW     = {255, 255,   0};
const Color PURPLE     = {255,   0, 255};
const Color CYAN       = {  0, 255, 255};
const Color ORANGE     = {255, 153,   0};
const Color PINK       = {255,   0, 128};
const Color NO_COLOR   = {  0,   0,   0,   0};
