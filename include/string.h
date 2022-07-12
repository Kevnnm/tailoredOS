#pragma once

#include "stdint.h"
#include "stddef.h"

void* memset(void* src, int c, size_t count);
void* memcpy(void* dest, const void* src, size_t count);
int memcmp(const void* s1, const void* s2, size_t n);

size_t strlen(const char *s);

char* itoa(unsigned long long num, char *str, int base);

void reverse(char *str, size_t length);
