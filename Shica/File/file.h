#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "gc.h"

#ifndef WEBSHICA


void memoryWrite(char *path); // write memory to a file
void memoryWriteC(const char *path); // write memory to a C file

void memoryRead(char *path) ; //
int *memoryCastIntAddr(void);
int memoryCastIntSize(void);
void genBytes(int bytes);
void memoryClear(void) ;
#endif // WEBSHICA

#endif // FILE_H