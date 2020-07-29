#pragma once

#define ESC "\x1b"
#define CSI "\x1b["
int enable_terminal();
void save_position();
void last_position();
void set_window_size(int left, int top, int right, int bottom); 
void move_xy(int x, int y);
