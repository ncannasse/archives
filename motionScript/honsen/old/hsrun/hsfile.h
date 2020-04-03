#pragma once
typedef struct HWND__ *HWND;

class MTW;

bool open_hs_file(MTW *hs, const char *filename, HWND hwnd, void callback(void *), void *cbparam );
const char *get_error_msg();
void free_error_msg();
