#pragma once

#include <stdio.h>
#include "symbol_table.h"
#include "expression_tree.h"

enum CompilerError
{
    COMPILER_NO_ERROR,
    COMPILER_ERROR_FILE_OPEN_FAILURE,
    COMPILER_ERROR_NO_MAIN_FUNCTION,
    COMPILER_CALL_UNDEFINED_FUNCTION
};

struct Compiler
{
    SymbolTable*  table;
    Node*         tree;
    FILE*         file;
    Function*     curFunction;

    size_t        curCondLabel;
    size_t        curLoopLabel;
    size_t        curCmpLabel;

    CompilerError status;
};

void          construct   (Compiler* compiler, Node* tree, SymbolTable* table);
void          destroy     (Compiler* compiler);
const char*   errorString (CompilerError error);
CompilerError compile     (Compiler* compiler, const char* outputFile);