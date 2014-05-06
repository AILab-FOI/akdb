/* 
 * File:   unique.h
 * Author: domagoj
 *
 * Created on May 23, 2012, 10:27 AM
 */



#ifndef UNIQUE
#define UNIQUE

#include "../../mm/memoman.h"
#include "../../file/id.h"


void Ak_set_constraint_unique(char* tableName, char* constraintName, char* attName);
int AK_read_constraint_unique(char* tableName, char *newvalue, char* attNamepar);
void AK_unique_test();

#endif
