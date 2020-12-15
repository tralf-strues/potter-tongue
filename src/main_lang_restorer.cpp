#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "tokenizer.h"
#include "parser.h"
#include "compiler.h"
#include "language_restore.h"
#include "../libs/file_manager.h"

#define UTB_DEFINITIONS
#include "../libs/utilib.h"

enum Error
{
    NO_ERROR,
    INPUT_UNSPECIFIED,
    OUTPUT_UNSPECIFIED,
    INPUT_LOAD_FAILED,
    RESTORE_FAILED,
};

const char* FLAG_OUTPUT    = "-o";
const char* DEFAULT_OUTPUT = "restored.txt";

int main(int argc, char* argv[])
{
    char* input  = nullptr;
    char* output = nullptr;

    if (argc > 1) { input = argv[1]; }

    for (size_t i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], FLAG_OUTPUT) == 0)
        {
            if (argc - i == 1)
            {
                printf("Output file unspecified!\n");
                return OUTPUT_UNSPECIFIED;
            }

            output = argv[i];
        }
    }

    if (input == nullptr)
    {
        printf("Input file unspecified!\n");
        return INPUT_UNSPECIFIED;
    }

    output = (output == nullptr) ? (char*) DEFAULT_OUTPUT : output;

    Node* readTree = readTreeFromFile(input);
    if (readTree == nullptr)
    {
        printf("Couldn't read tree from file.\n");
        return INPUT_LOAD_FAILED;
    }

    if (!restoreCode(readTree, output))
    {
        printf("Restoring program failed.\n");
        return RESTORE_FAILED;
    }

    return NO_ERROR;
}