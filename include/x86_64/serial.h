#pragma once

void init_serial();

void serial_writec(const char c);
void serial_writen(const char *s, unsigned n);
void serial_write(const char *s);
