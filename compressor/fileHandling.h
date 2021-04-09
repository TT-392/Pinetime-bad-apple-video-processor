#pragma once
#include "struct.h"

int writeBlock(struct dataBlock data, FILE *file);

struct dataBlock readBlock(FILE *file);

