#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "tokenizer.h"
#include "parser.h"
#include "compiler.h"
#include "../libs/file_manager.h"

#define UTB_DEFINITIONS
#include "../libs/utilib.h"

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
    if (parseProgram(&parser, &table, &tree) != PARSE_NO_ERROR)
    {
        printf("Couldn't compile the program.\n");
        return -1;
    }

    Compiler compiler = {};
    construct(&compiler, tree, &table);
    if (compile(&compiler, "program.asy") != COMPILER_NO_ERROR)
    {
        printf("Couldn't compile the program.\n");
        return -1;
    }

    // graphDump(tree);
    dump(&table);

    FILE* file = fopen("dumped_tree.txt", "w");
    assert(file != nullptr);

    dumpToFile(file, tree);

    fclose(file);

    destroy(&table);
    destroy(&tokenizer);
    destroy(&parser);

    Node* readTree = readTreeFromFile("dumped_tree.txt");
    assert(readTree != nullptr);
    graphDump(readTree);

    return 0;
}