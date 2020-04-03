#pragma once

typedef struct HWND__ *HWND;

void init_window();
HWND create_window(const char *title, int width, int height);
HWND create_log_window();
void append_log( HWND h, const char *data );
bool window_loop();
void close_window( HWND h );
void error_window( const char *msg );
