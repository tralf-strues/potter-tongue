#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "tokenizer.h"
#include "parser.h"
#include "compiler.h"
#include "../libs/file_manager.h"

#define UTB_DEFINITIONS
#include "../libs/utilib.h"

enum Error
{
    NO_ERROR,
    INPUT_UNSPECIFIED,
    OUTPUT_UNSPECIFIED,
    INPUT_LOAD_FAILED,
    COMPILATION_FAILED
};

struct FlagManager
{
    bool isTokenDump;
    bool isGraphDump;
    bool isTreeDump;
    bool isSymbTableDump;
    bool useNumerics;
};

const char* FLAG_TOKEN_DUMP      = "-token-dump";
const char* FLAG_GRAPH_DUMP      = "-graph-dump";
const char* FLAG_TREE_DUMP       = "-tree-dump";
const char* FLAG_SYMB_TABLE_DUMP = "-symb-table-dump";
const char* FLAG_USE_NUMERICS    = "-numeric";
const char* FLAG_OUTPUT          = "-o";

const char* DEFAULT_OUTPUT       = "a.asy";

Error compile(char* input, char* output, FlagManager flags);

int main(int argc, char* argv[])
{
    char* input  = nullptr;
    char* output = nullptr;

    FlagManager flags = {};

    if (argc > 1) { input = argv[1]; }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], FLAG_TOKEN_DUMP) == 0)
        {
            flags.isTokenDump = true;
        }
        else if (strcmp(argv[i], FLAG_GRAPH_DUMP) == 0)
        {
            flags.isGraphDump = true;
        }
        else if (strcmp(argv[i], FLAG_TREE_DUMP) == 0)
        {
            flags.isTreeDump = true;
        }
        else if (strcmp(argv[i], FLAG_SYMB_TABLE_DUMP) == 0)
        {
            flags.isSymbTableDump = true;
        }
        else if (strcmp(argv[i], FLAG_USE_NUMERICS) == 0)
        {
            flags.useNumerics = true;
        }
        else if (strcmp(argv[i], FLAG_OUTPUT) == 0)
        {
            if (argc - i == 1)
            {
                printf("Output file unspecified!\n");
                return OUTPUT_UNSPECIFIED;
            }

            output = argv[i + 1];
        }
    }

    if (input == nullptr)
    {
        printf("Input file unspecified!\n");
        return INPUT_UNSPECIFIED;
    }

    output = (output == nullptr) ? (char*) DEFAULT_OUTPUT : output;

    return compile(input, output, flags);
}

Error compile(char* input, char* output, FlagManager flags)
{
    assert(input  != nullptr);
    assert(output != nullptr);

    char*  buffer     = nullptr;
    size_t bufferSize = 0;

    if (!loadFile(input, &buffer, &bufferSize))
    {
        printf("Couldn't load file '%s'\n", input);
        return INPUT_LOAD_FAILED;
    }

    Node* tree = nullptr;

    Tokenizer tokenizer = {};
    construct(&tokenizer, buffer, bufferSize, flags.useNumerics);
    tokenizeBuffer(&tokenizer);

    if (flags.isTokenDump)
    {
        dumpTokens(tokenizer.tokens, tokenizer.tokensCount);
    }

    SymbolTable table = {};
    construct(&table);

    Parser parser = {};
    construct(&parser, &tokenizer);
    if (parseProgram(&parser, &table, &tree) != PARSE_NO_ERROR)
    {
        printf("Couldn't compile the program.\n");
        return COMPILATION_FAILED;
    }

    if (flags.isGraphDump)
    {
        graphDump(tree);
    }

    if (flags.isSymbTableDump)
    {
        dump(&table);
    }

    if (flags.isTreeDump)
    {
        FILE* file = fopen("dumped_tree.txt", "w");
        assert(file != nullptr);

        dumpToFile(file, tree);

        fclose(file);
    }   

    Compiler compiler = {};
    construct(&compiler, tree, &table);
    if (compile(&compiler, output) != COMPILER_NO_ERROR)
    {
        printf("Couldn't compile the program.\n");
        return COMPILATION_FAILED;
    }

    destroy(&table);
    destroy(&tokenizer);
    destroy(&parser);
    destroy(&compiler);

    return NO_ERROR;
}