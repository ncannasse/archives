#pragma once

void read_config(const char *path);
int get_config(const char *key);
void set_config( const char *key, int value );
void free_config();