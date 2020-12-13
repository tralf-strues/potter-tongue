#include <assert.h>
#include <string.h>
#include "compiler.h"

// TODOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
#define HERE printf("===HERE?===\n");

#define ASSERT_COMPILER(compiler) assert(compiler        != nullptr); \
                                  assert(compiler->table != nullptr); \
                                  assert(compiler->file  != nullptr); \

#define OUTPUT   compiler->file
#define CUR_FUNC compiler->curFunction

const size_t HORIZONTAL_LINE_LENGTH = 50;

void compileError        (Compiler* compiler, CompilerError error);

void writeHorizontalLine (Compiler* compiler);
void writeFunctionHeader (Compiler* compiler);

void writeFunction       (Compiler* compiler, Node* node);
void writeBlock          (Compiler* compiler, Node* node);
void writeStatement      (Compiler* compiler, Node* node);

void writeCondition      (Compiler* compiler, Node* node);
void writeLoop           (Compiler* compiler, Node* node);
void writeAssignment     (Compiler* compiler, Node* node);
void writeReturn         (Compiler* compiler, Node* node);

void writeExpression     (Compiler* compiler, Node* node);
void writeMath           (Compiler* compiler, Node* node);
void writeCompare        (Compiler* compiler, Node* node);
void writeNumber         (Compiler* compiler, Node* node);
void writeVar            (Compiler* compiler, Node* node);

void writeCall           (Compiler* compiler, Node* node);
bool writeStdCall        (Compiler* compiler, Node* node);

void construct(Compiler* compiler, Node* tree, SymbolTable* table)
{
    assert(compiler != nullptr);
    assert(tree     != nullptr);
    assert(table    != nullptr);

    compiler->table = table; 
    compiler->tree  = tree;
}

void destroy(Compiler* compiler)
{
    assert(compiler != nullptr);

    compiler->table = nullptr;
    compiler->tree  = nullptr;
}

const char* errorString(CompilerError error)
{
    switch (error)
    {
        case COMPILER_NO_ERROR:                return "no error";

        case COMPILER_ERROR_FILE_OPEN_FAILURE: return "couldn't open file to write output to";
        case COMPILER_ERROR_NO_MAIN_FUNCTION:  return "main function ('love') wasn't found";
        case COMPILER_CALL_UNDEFINED_FUNCTION: return "calling undefined function";
    }

    return "UNDEFINED error";
}

void compileError(Compiler* compiler, CompilerError error)
{
    ASSERT_COMPILER(compiler);

    compiler->status = error;

    printf("COMPILATION ERROR: %s\n", errorString(error));
}

CompilerError compile(Compiler* compiler, const char* outputFile)
{
    assert(compiler   != nullptr);
    assert(outputFile != nullptr);

    OUTPUT = fopen(outputFile, "w");
    if (OUTPUT == nullptr)
    {
        compileError(compiler, COMPILER_ERROR_FILE_OPEN_FAILURE);
        return compiler->status;
    }

    if (getFunction(compiler->table, MAIN_FUNCTION_NAME) == nullptr)
    {
        compileError(compiler, COMPILER_ERROR_NO_MAIN_FUNCTION);
        return compiler->status;
    }

    CUR_FUNC = compiler->table->functions;

    Node* curDeclaration = compiler->tree;
    while (curDeclaration != nullptr)
    {
        writeFunction(compiler, curDeclaration->right);
        curDeclaration = curDeclaration->left;
        CUR_FUNC++;
    }

    fclose(OUTPUT);

    return compiler->status;
}

void writeHorizontalLine(Compiler* compiler)
{
    ASSERT_COMPILER(compiler);

    fprintf(OUTPUT, "; ");

    for (size_t i = 0; i < HORIZONTAL_LINE_LENGTH; i++)
    {
        fprintf(OUTPUT, "=");
    }

    fprintf(OUTPUT, "\n");
}

void writeFunctionHeader(Compiler* compiler)
{
    ASSERT_COMPILER(compiler);

    writeHorizontalLine(compiler);
    fprintf(OUTPUT, "; %s\n;\n; args: ", CUR_FUNC->name);

    for (size_t i = 0; i < CUR_FUNC->argsCount; i++)
    {
        fprintf(OUTPUT, "%s", CUR_FUNC->vars[i]);

        if (i < CUR_FUNC->argsCount - 1)
        {
            fprintf(OUTPUT, ", ");
        }
    }

    fprintf(OUTPUT, "\n; vars: ");

    for (size_t i = CUR_FUNC->argsCount; i < CUR_FUNC->varsCount - CUR_FUNC->argsCount; i++)
    {
        fprintf(OUTPUT, "%s", CUR_FUNC->vars[i]);

        if (i < CUR_FUNC->varsCount - CUR_FUNC->argsCount - 1)
        {
            fprintf(OUTPUT, ", ");
        }
    }

    fprintf(OUTPUT, "\n");
    writeHorizontalLine(compiler);
}

void writeFunction(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node != nullptr);

    writeFunctionHeader(compiler);

    for (size_t i = 0; i < CUR_FUNC->argsCount; i++)
    {
        fprintf(OUTPUT, "pop [rax + %u]\n", 2 + i);
    }

    fprintf(OUTPUT, "\n");
    writeBlock(compiler, node->left);
    fprintf(OUTPUT, "\n");
}

void writeBlock(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node != nullptr);

    Node* curStatement = node->right;
    while (curStatement != nullptr)
    {
        writeStatement(compiler, curStatement);
        curStatement = curStatement->right;
    }
}

void writeStatement(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node       != nullptr);
    assert(node->left != nullptr);

    switch (node->left->type)
    {
        case COND_TYPE: { writeCondition  (compiler, node->left); break; }
        case LOOP_TYPE: { writeLoop       (compiler, node->left); break; }
        case ASSG_TYPE: { writeAssignment (compiler, node->left); break; }
        case JUMP_TYPE: { writeReturn     (compiler, node->left); break; }
        default:        { writeExpression (compiler, node->left); break; }
    }
}

void writeCondition(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node != nullptr);

    fprintf(OUTPUT, "; IF statement\n");

    writeExpression(compiler, node->left);

    size_t label = compiler->curCondLabel++;

    fprintf(OUTPUT, "push 0\n"
                    "je :IF_END_%u\n\n",
                    label);

    writeBlock(compiler, node->right->left);    

    fprintf(OUTPUT, "jmp :IF_ELSE_END_%u\n"
                    "IF_END_%u:\n", 
                    label,
                    label);

    if (node->right->right != nullptr)
    {
        writeBlock(compiler, node->right->right);
    }

    fprintf(OUTPUT, "IF_ELSE_END_%u:\n\n", label);
}

void writeLoop(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node != nullptr);

    size_t label = compiler->curLoopLabel++;

    fprintf(OUTPUT, "\nWHILE_%u:\n", label);
    writeExpression(compiler, node->left);

    fprintf(OUTPUT, "push 0\n"
                    "je: WHILE_END_%u\n"
                    "WHILE_BODY_%u:\n",
                    label,
                    label);

    writeBlock(compiler, node->right);

    fprintf(OUTPUT, "jmp :WHILE_%u\n"
                    "WHILE_END_%u\n\n",
                    label,
                    label);
}

void writeAssignment(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node != nullptr);

    writeExpression(compiler, node->right);

    fprintf(OUTPUT, "pop [rax + %u]\n\n", 2 + getVarOffset(CUR_FUNC, node->left->data.id));
}

void writeReturn(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node != nullptr);

    writeExpression(compiler, node->right);

    fprintf(OUTPUT, "push rax\n"
                    "push [rax]\n"
                    "sub\n"
                    "pop rax\n"
                    "ret\n\n");
}

void writeExpression(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node != nullptr);

    switch (node->type)
    {
        case MATH_TYPE: { writeMath   (compiler, node); break; }
        case NUMB_TYPE: { writeNumber (compiler, node); break; }
        case NAME_TYPE: { writeVar    (compiler, node); break; }
        case CALL_TYPE: { writeCall   (compiler, node); break; }
    }
}

void writeMath(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node != nullptr);

    MathOp operation = node->data.operation;

    if (operation > DIV_OP)
    {
        writeCompare(compiler, node);
        return;
    }

    writeExpression(compiler, node->left);
    writeExpression(compiler, node->right);

    switch (node->data.operation)
    {
        case ADD_OP: { fprintf(OUTPUT, "add\n\n"); break; }
        case SUB_OP: { fprintf(OUTPUT, "sub\n\n"); break; }
        case MUL_OP: { fprintf(OUTPUT, "mul\n\n"); break; }
        case DIV_OP: { fprintf(OUTPUT, "div\n\n"); break; }
    }
}

void writeCompare(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node != nullptr);

    MathOp operation = node->data.operation;
    size_t label = compiler->curCmpLabel++;

    writeExpression(compiler, node->left);
    writeExpression(compiler, node->right);

    switch (node->data.operation)
    {
        case EQUAL_OP:         { fprintf(OUTPUT, "je");  break; }
        case NOT_EQUAL_OP:     { fprintf(OUTPUT, "jne"); break; }
        case LESS_OP:          { fprintf(OUTPUT, "jb");  break; }
        case GREATER_OP:       { fprintf(OUTPUT, "ja");  break; }
        case LESS_EQUAL_OP:    { fprintf(OUTPUT, "jbe"); break; }
        case GREATER_EQUAL_OP: { fprintf(OUTPUT, "jae"); break; }
    }

    fprintf(OUTPUT, " :COMPARISON_%u\n"
                    "push 0\n"
                    "jmp :COMPARISON_END_%u\n"
                    "COMPARISON_%u:\n"
                    "push 1\n"
                    "COMPARISON_END_%u:\n\n", 
                    label,
                    label,
                    label,
                    label);
}

void writeNumber(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node != nullptr);

    fprintf(OUTPUT, "push %lg\n", node->data.number);
}

void writeVar(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node != nullptr);

    fprintf(OUTPUT, "push [rax + %u]\n", 2 + getVarOffset(CUR_FUNC, node->data.id));
}

void writeCall(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node != nullptr);

    if (writeStdCall(compiler, node)) { return; }

    Function* function = getFunction(compiler->table, node->left->data.id);
    if (function == nullptr) 
    {
        compileError(compiler, COMPILER_CALL_UNDEFINED_FUNCTION);
        return; 
    }

    Node* curParamExpr = node->right;
    while (curParamExpr != nullptr)
    {
        writeExpression(compiler, curParamExpr->left);
        curParamExpr = curParamExpr->right;
    }

    fprintf(OUTPUT, "; calling %s\n"
                    "push [rax + 1]\n"
                    "push rax\n"
                    "push [rax + 1]\n"
                    "add\n"
                    "pop rax\n"
                    "pop [rax]\n"
                    "push %u\n"
                    "pop [rax + 1]\n"
                    "call :%s\n\n",
                    function->name,
                    function->varsCount + 2,
                    function->name);
}

bool writeStdCall(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node != nullptr);

    const char* name = node->left->data.id;
    if (strcmp(name, KEYWORDS[PRINT_KEYWORD].name) == 0)
    {
        writeExpression(compiler, node->right->left);
        fprintf(OUTPUT, "out\n");
    } 
    else if (strcmp(name, KEYWORDS[SCAN_KEYWORD].name) == 0)
    {
        fprintf(OUTPUT, "in\n");
    } 
    else if (strcmp(name, KEYWORDS[FLOOR_KEYWORD].name) == 0)
    {
        writeExpression(compiler, node->right->left);
        fprintf(OUTPUT, "flr\n");
    }  
    else 
    {
        return false;
    }

    return true;
}