#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
    
#include "language_restore.h"
#include "../libs/utilib.h"

#define OUTPUT     restorer->file
#define NEW_LINE() write(restorer, "\n")

struct Restorer
{
    size_t curIndent;
    FILE*  file;
};  

void writeIndented        (Restorer* restorer, const char* format, ...);
void write                (Restorer* restorer, const char* format, ...);

void restoreFunction      (Restorer* restorer, Node* node);
void restoreBlock         (Restorer* restorer, Node* block);
void restoreStatement     (Restorer* restorer, Node* statement);
void restoreCondition     (Restorer* restorer, Node* condition);
void restoreLoop          (Restorer* restorer, Node* loop);
void restoreCmdLine       (Restorer* restorer, Node* line);
void restoreVDeclaration  (Restorer* restorer, Node* vdecl);
void restoreAssignment    (Restorer* restorer, Node* assignment);
void restoreJump          (Restorer* restorer, Node* jump);
void restoreExpression    (Restorer* restorer, Node* expression);
void restoreNumber        (Restorer* restorer, Node* number);
void restoreDerefVar      (Restorer* restorer, Node* var);
void restoreCall          (Restorer* restorer, Node* call);

bool isOperationNode      (Node* node, MathOp operation);
bool firstBracketsNeeded  (Node* operation);
bool secondBracketsNeeded (Node* operation);
void restoreMathOp        (Restorer* restorer, Node* operation);

bool restoreCode(Node* root, const char* filename)
{   
    assert(root     != nullptr);
    assert(filename != nullptr);

    FILE* file = fopen(filename, "w");
    if (file == nullptr) { return false; }

    Restorer restorer  = { 0, file };

    write(&restorer, "%s %.*s\n\n", KEYWORDS[PROG_START_KEYWORD].name, strchr(filename, '.') - filename, filename);

    Node* curDeclaration = root;
    while (curDeclaration != nullptr)
    {
        restoreFunction(&restorer, curDeclaration->right);
        curDeclaration = curDeclaration->left;
    }

    write(&restorer, "%s", KEYWORDS[PROG_END_KEYWORD].name);

    fclose(file);

    return true;
}   

void writeIndented(Restorer* restorer, const char* format, ...)
{
    assert(restorer != nullptr);
    assert(format   != nullptr);

    va_list args = {};
    va_start(args, &format);

    for (size_t i = 0; i < restorer->curIndent; i++)
    {
        fprintf(restorer->file, "    ");
    }

    vfprintf(restorer->file, format, args);

    va_end(args);
}

void write(Restorer* restorer, const char* format, ...)
{
    assert(restorer != nullptr);
    assert(format   != nullptr);

    va_list args = {};
    va_start(args, &format);

    vfprintf(restorer->file, format, args);

    va_end(args);
}

void restoreFunction(Restorer* restorer, Node* node)
{
    assert(restorer != nullptr);
    assert(node     != nullptr);

    write(restorer, "%s %s ", getKeywordString(FDECL_KEYWORD), node->data.id);

    Node* curArg = node->right;
    if (curArg == nullptr)
    {
        write(restorer, "%s", getKeywordString(ZERO_KEYWORD));
    }

    while (curArg != nullptr)
    {
        write(restorer, curArg->data.id);

        if (curArg->right != nullptr)
        {
            write(restorer, ", ");
        }

        curArg = curArg->right;
    }

    NEW_LINE();
    restoreBlock(restorer, node->left);
    NEW_LINE();
}

void restoreBlock(Restorer* restorer, Node* block)
{
    assert(restorer != nullptr);
    assert(block    != nullptr);

    writeIndented(restorer, "%s\n", getKeywordString(OPEN_BRACE_KEYWORD));
    restorer->curIndent++;

    Node* curStatement = block->right;
    while (curStatement != nullptr)
    {
        restoreStatement(restorer, curStatement);
        curStatement = curStatement->right;
    }

    restorer->curIndent--;
    writeIndented(restorer, "%s\n", getKeywordString(CLOSE_BRACE_KEYWORD));
}

void restoreStatement(Restorer* restorer, Node* statement)
{
    assert(restorer  != nullptr);
    assert(statement != nullptr);

    switch (statement->left->type)
    {
        case COND_TYPE: { restoreCondition (restorer, statement->left); break; }
        case LOOP_TYPE: { restoreLoop      (restorer, statement->left); break; }
        default:        { restoreCmdLine   (restorer, statement->left); break; }
    }
}

void restoreCondition(Restorer* restorer, Node* condition)
{
    assert(restorer  != nullptr);
    assert(condition != nullptr);

    writeIndented(restorer, "%s ", getKeywordString(IF_KEYWORD));

    write(restorer, "%s ", getKeywordString(BRACKET_KEYWORD));
    restoreExpression(restorer, condition->left);
    write(restorer, " %s", getKeywordString(BRACKET_KEYWORD));
    NEW_LINE();

    restoreBlock(restorer, condition->right->left);

    if (condition->right->right != nullptr)
    {
        writeIndented(restorer, "%s\n", getKeywordString(ELSE_KEYWORD));
        restoreBlock(restorer, condition->right->right);
    }

    NEW_LINE();
}

void restoreLoop(Restorer* restorer, Node* loop)
{
    assert(restorer != nullptr);
    assert(loop     != nullptr);

    writeIndented(restorer, "%s ", getKeywordString(LOOP_KEYWORD));
    write(restorer, "%s ", getKeywordString(BRACKET_KEYWORD));
    restoreExpression(restorer, loop->left);
    write(restorer, " %s",getKeywordString(BRACKET_KEYWORD));

    NEW_LINE();
    restoreBlock(restorer, loop->right);
    NEW_LINE();
}

void restoreCmdLine(Restorer* restorer, Node* line)
{
    assert(restorer != nullptr);
    assert(line     != nullptr);

    writeIndented(restorer, " - ");

    switch (line->type)
    {
        case VDECL_TYPE: { restoreVDeclaration (restorer, line); break; }
        case ASSG_TYPE:  { restoreAssignment   (restorer, line); break; }
        case JUMP_TYPE:  { restoreJump         (restorer, line); break; }
        default:         { restoreExpression   (restorer, line); break; }
    }

    NEW_LINE();
}

void restoreVDeclaration(Restorer* restorer, Node* vdecl)
{
    assert(restorer != nullptr);
    assert(vdecl    != nullptr);

    write(restorer, "%s ", getKeywordString(VDECL_KEYWORD));
    restoreAssignment(restorer, vdecl);
}

void restoreAssignment(Restorer* restorer, Node* assignment)
{
    assert(restorer   != nullptr);
    assert(assignment != nullptr);

    write(restorer, "%s %s ", assignment->left->data.id, getKeywordString(ASSGN_KEYWORD));
    restoreExpression(restorer, assignment->right);
}

void restoreJump(Restorer* restorer, Node* jump)
{
    assert(restorer != nullptr);
    assert(jump     != nullptr);

    write(restorer, "%s ", getKeywordString(RETURN_KEYWORD));
    restoreExpression(restorer, jump->right);
}

void restoreExpression(Restorer* restorer, Node* expression)
{
    assert(restorer   != nullptr);
    assert(expression != nullptr);

    switch (expression->type)
    {
        case NUMB_TYPE: { restoreNumber   (restorer, expression); break; }
        case NAME_TYPE: { restoreDerefVar (restorer, expression); break; }
        case CALL_TYPE: { restoreCall     (restorer, expression); break; }
        default:        { restoreMathOp   (restorer, expression); break; }
    }
}

void restoreNumber(Restorer* restorer, Node* number)
{
    assert(restorer != nullptr);
    assert(number   != nullptr);

    write(restorer, "%lg", number->data.number);
}

void restoreDerefVar(Restorer* restorer, Node* var)
{
    assert(restorer != nullptr);
    assert(var      != nullptr);

    write(restorer, "%s %s", getKeywordString(DEREF_KEYWORD), var->data.id);
}

void restoreCall(Restorer* restorer, Node* call)
{
    assert(restorer != nullptr);
    assert(call     != nullptr);

    const char* function = call->left->data.id;
    if (strcmp(function, getKeywordString(SCAN_KEYWORD)) == 0)
    {
        write(restorer, function);
        return;
    }

    if (strcmp(function, getKeywordString(PRINT_KEYWORD)) == 0)
    {
        write(restorer, "%s ", function);
        restoreExpression(restorer, call->right->left);
        return;
    }

    if (strcmp(function, getKeywordString(FLOOR_KEYWORD)) != 0 && 
        strcmp(function, getKeywordString(SQRT_KEYWORD))  != 0)
    {
        write(restorer, "%s ", getKeywordString(CALL_KEYWORD));
    }

    write(restorer, "%s %s ", function, getKeywordString(BRACKET_KEYWORD));

    Node* curParam = call->right;
    while (curParam != nullptr)
    {
        restoreExpression(restorer, curParam->left);

        if (curParam->right != nullptr)
        {
            write(restorer, ", ");
        }

        curParam = curParam->right;
    }

    write(restorer, " %s", getKeywordString(BRACKET_KEYWORD));
}

bool isOperationNode(Node* node, MathOp operation)
{
    assert(node != nullptr);

    return node->type == MATH_TYPE && node->data.operation == operation;
}

bool firstBracketsNeeded(Node* operation)
{
    assert(operation != nullptr);

    return (isOperationNode(operation,       MUL_OP) || isOperationNode(operation,       DIV_OP)) &&
           (isOperationNode(operation->left, ADD_OP) || isOperationNode(operation->left, SUB_OP));
}

bool secondBracketsNeeded(Node* operation)
{
    assert(operation != nullptr);

    return (isOperationNode(operation,       MUL_OP) || 
            isOperationNode(operation,       DIV_OP) || 
            isOperationNode(operation,       SUB_OP)) &&

           (isOperationNode(operation->left, ADD_OP) || 
            isOperationNode(operation->left, SUB_OP));
}

void restoreMathOp(Restorer* restorer, Node* operation)
{
    assert(restorer  != nullptr);
    assert(operation != nullptr);

    if (firstBracketsNeeded(operation))
    {
        write(restorer, "%s ", getKeywordString(BRACKET_KEYWORD));
        restoreExpression(restorer, operation->left);
        write(restorer, " %s", getKeywordString(BRACKET_KEYWORD));
    }
    else
    {
        restoreExpression(restorer, operation->left);
    }

    write(restorer, " %s ", KEYWORDS[PLUS_KEYWORD + operation->data.operation].name);

    if (secondBracketsNeeded(operation))
    {
        write(restorer, "%s ", getKeywordString(BRACKET_KEYWORD));
        restoreExpression(restorer, operation->right);
        write(restorer, " %s", getKeywordString(BRACKET_KEYWORD));
    }
    else
    {
        restoreExpression(restorer, operation->right);
    }
}
