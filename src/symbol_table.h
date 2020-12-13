#pragma once

#include <stdlib.h>

struct Function
{
    const char* name;

    char**      vars;
    size_t      varsCapacity;
    size_t      varsCount;
    size_t      argsCount;
};

struct SymbolTable
{
    Function* functions;
    size_t    functionsCapacity;
    size_t    functionsCount;
};

void      construct    (SymbolTable* table);
void      destroy      (SymbolTable* table);

Function* pushFunction (SymbolTable* table, const char* function);
Function* getFunction  (SymbolTable* table, const char* function);

void      pushArgument (Function* function, const char* argument);
void      pushVariable (Function* function, const char* variable);
int       getVarOffset (Function* function, const char* variable);

void      dump         (SymbolTable* table);