#pragma once
#include "struct.h"

int writeBlock_compressed (struct dataBlock data, FILE *file);

int writeBlock(struct dataBlock data, FILE *file);

struct dataBlock readBlock(FILE *file);

struct dataBlock readBlock_compressed(FILE *file);

