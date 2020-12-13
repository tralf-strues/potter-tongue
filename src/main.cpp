#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "tokenizer.h"
#include "parser.h"
#include "compiler.h"
#include "../libs/file_manager.h"

#define UTB_DEFINITIONS
#include "../libs/utilib.h"

bool loadFile(const char* filename, char** buffer, size_t* bufferSize);

int main(int argc, char* argv[])
{
    if (argc <= 1) { printf("Filename unspecified\n"); return -1; }

    char*  buffer     = nullptr;
    size_t bufferSize = 0;

    if (!loadFile(argv[1], &buffer, &bufferSize))
    {
        printf("Couldn't load file '%s'\n", argv[1]);
        return -1;
    }

    Node* tree = nullptr;

    Tokenizer tokenizer = {};
    construct(&tokenizer, buffer, bufferSize);
    tokenizeBuffer(&tokenizer);
    dumpTokens(tokenizer.tokens, tokenizer.tokensCount);

    SymbolTable table = {};
    construct(&table);

    Parser parser = {};
    construct(&parser, &tokenizer);
    parseProgram(&parser, &table, &tree);

    Compiler compiler = {};
    construct(&compiler, tree, &table);
    if (compile(&compiler, "program.asy") != COMPILER_NO_ERROR)
    {
        printf("Couldn't compile the program.\n");
    }

    // graphDump(tree);
    dump(&table);

    destroy(&table);
    destroy(&tokenizer);
    destroy(&parser);
}

bool loadFile(const char* filename, char** buffer, size_t* bufferSize)
{
    assert(filename   != nullptr);
    assert(buffer     != nullptr);
    assert(bufferSize != nullptr);

    FILE* file = fopen(filename, "r");
    if (file == nullptr) { return false; }

    size_t fileSize = getFileSize(filename);

    *buffer = (char*) calloc(fileSize + 1, sizeof(char));
    assert(*buffer != nullptr);

    *bufferSize = fread(*buffer, sizeof(char), fileSize, file);
    if (*bufferSize == 0) 
    {
        fclose(file);
        free(*buffer);

        *buffer = nullptr;

        return false;
    }

    *(*buffer + *bufferSize) = '\0';

    fclose(file);

    return true;
}