#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "expression_tree.h"
#include "../libs/utilib.h"
#include "../libs/file_manager.h"

const size_t MAX_COMMAND_LENGTH   = 256;

const char*  DECL_GRAPH_STYLE     = ", color=\"#000000\", fillcolor=\"#FFFFFF\", fontcolor=\"#000000\"";
const char*  BLCK_GRAPH_STYLE     = ", color=\"#000000\", fillcolor=\"#FFFFFF\", fontcolor=\"#000000\"";
const char*  STAT_GRAPH_STYLE     = ", color=\"#000000\", fillcolor=\"#FFFFFF\", fontcolor=\"#000000\"";
const char*  COND_GRAPH_STYLE     = ", color=\"#367ACC\", fillcolor=\"#E0F5FF\", fontcolor=\"#4881CC\"";
const char*  IFEL_GRAPH_STYLE     = ", color=\"#367ACC\", fillcolor=\"#E0F5FF\", fontcolor=\"#4881CC\"";
const char*  LOOP_GRAPH_STYLE     = ", color=\"#367ACC\", fillcolor=\"#E0F5FF\", fontcolor=\"#4881CC\"";
const char*  ASSG_GRAPH_STYLE     = "";
const char*  CALL_GRAPH_STYLE     = "";
const char*  LIST_GRAPH_STYLE     = "";
const char*  JUMP_GRAPH_STYLE     = "";
const char*  MATH_GRAPH_STYLE     = "";
const char*  NUMB_GRAPH_STYLE     = ", color=\"#D2691E\", fillcolor=\"#FFFAEF\", fontcolor=\"#FF7F50\"";
const char*  NAME_GRAPH_STYLE     = ", color=\"#75A673\", fillcolor=\"#EDFFED\", fontcolor=\"#75A673\"";

const char*  UNIVERSAL_MAIN_NAME  = "main";
const char*  UNIVERSAL_PRINT_NAME = "print";
const char*  UNIVERSAL_SCAN_NAME  = "scan";
const char*  UNIVERSAL_FLOOR_NAME = "floor";
const char*  UNIVERSAL_SQRT_NAME  = "sqrt";

const size_t FIRST_DECL_LENGTH    = strlen("0 | 0");
const size_t LEAF_CHILDREN_LENGTH = strlen("{ } { }");
const size_t LEFT_CHILD_LENGTH    = strlen("{ } { ");

#define CHECK_NULL(value, action) if (value == nullptr) { action; }

int  counterFileUpdate (const char* filename);
void graphDumpSubtree  (FILE* file, Node* node);

void destroySubtree(Node* root)
{
    CHECK_NULL(root, return);

    destroySubtree(root->left);
    destroySubtree(root->right);

    deleteNode(root);
}

Node* newNode()
{
    return (Node*) calloc(1, sizeof(Node));
}

Node* newNode(NodeType type, NodeData data, Node* left, Node* right)
{
    Node* node = newNode();
    CHECK_NULL(node, return nullptr);

    node->type   = type;
    node->data   = data;

    node->parent = nullptr;
    node->left   = left;
    node->right  = right;

    if (left  != nullptr) { left->parent  = node; }
    if (right != nullptr) { right->parent = node; }

    return node;
}

void deleteNode(Node* node)
{
    assert(node != nullptr);

    node->parent = nullptr;
    node->left   = nullptr;
    node->right  = nullptr;

    free(node);
}

void setLeft(Node* root, Node* left)
{
    assert(root != nullptr);

    root->left = left;
    if (left != nullptr) { left->parent = root; } 
}

void setRight(Node* root, Node* right)
{
    assert(root != nullptr);

    root->right = right;
    if (right != nullptr) { right->parent = root; } 
}

void copyNode(Node* dest, const Node* src)
{
    assert(dest != nullptr);
    assert(src  != nullptr);

    dest->type   = src->type;
    dest->data   = src->data;
    dest->parent = src->parent;
    dest->left   = src->left;
    dest->right  = src->right;

    if (src->left  != nullptr) { src->left->parent  = dest; }
    if (src->right != nullptr) { src->right->parent = dest; }
}

Node* copyTree(const Node* node)
{
    if (node == nullptr) { return nullptr; }

    return newNode(node->type, node->data, copyTree(node->left), copyTree(node->right));
}

bool isLeft(const Node* node)
{
    assert(node         != nullptr);
    assert(node->parent != nullptr);

    return node == node->parent->left;
}

void setData(Node* node, NodeType type, NodeData data)
{
    assert(node != nullptr);

    node->type = type;
    node->data = data;
}

void setData(Node* node, double number)
{
    assert(node != nullptr);

    node->type        = NUMB_TYPE;
    node->data.number = number;
}

void setData(Node* node, MathOp op)
{
    assert(node != nullptr);

    node->type           = MATH_TYPE;
    node->data.operation = op;
}

void setData(Node* node, const char* id)
{
    assert(node != nullptr);

    node->type    = NAME_TYPE;
    node->data.id = id;
}

int counterFileUpdate(const char* filename)
{
    assert(filename != nullptr);

    int count = 0;

    FILE* counterFile = fopen(filename, "r+");
    if (counterFile == nullptr)
    {
        counterFile = fopen(filename, "w");
        count = -1;
    }
    else
    {
        fscanf(counterFile, "%d", &count);
        fseek(counterFile, 0, SEEK_SET);
    }

    fprintf(counterFile, "%u", count + 1);
    fclose(counterFile);

    return count;
}

void graphDump(Node* root, const char* treeFilename, const char* outputFilename)
{
    assert(root           != nullptr);
    assert(treeFilename   != nullptr);
    assert(outputFilename != nullptr);

    FILE* file = fopen(treeFilename, "w");
    assert(file != nullptr);

    fprintf(file, "digraph structs {\n"
                  "\tnode [shape=\"circle\", "
                  "style=\"filled\", "
                  "color=\"#52527A\", "
                  "fontcolor=\"#52527A\", "
                  "fillcolor=\"#E1E1EA\"];\n\n");

    if (root == nullptr) { fprintf(file, "\t\"ROOT\" [label = \"Empty database\"];\n"); }
    else                 { graphDumpSubtree(file, root); }

    fprintf(file, "}");

    fclose(file);

    char dotCmd[MAX_COMMAND_LENGTH] = {};

    snprintf(dotCmd, sizeof(dotCmd), "dot -Tsvg %s -o %s", treeFilename, outputFilename);
    system(dotCmd);
}

void graphDumpSubtree(FILE* file, Node* node)
{
    assert(file != nullptr);
    
    if (node == nullptr) { return; }

    fprintf(file, "\t\"%p\" [label=", (void*) node);

    switch (node->type)
    {
        case DECL_TYPE:  { fprintf(file, "\"D\"%s];\n",                 DECL_GRAPH_STYLE); break; }
        case VDECL_TYPE: { fprintf(file, "\"=\"%s];\n",                 ASSG_GRAPH_STYLE); break; }
        case NAME_TYPE:  { fprintf(file, "\"%s\"%s];\n", node->data.id, NAME_GRAPH_STYLE); break; } 
        case LIST_TYPE:  { fprintf(file, "\"param\"%s];\n",             LIST_GRAPH_STYLE); break; } 
        
        case BLCK_TYPE:  { fprintf(file, "\"Block\"%s];\n",             BLCK_GRAPH_STYLE); break; }
        case STAT_TYPE:  { fprintf(file, "\"S\"%s];\n",                 STAT_GRAPH_STYLE); break; }

        case COND_TYPE:  { fprintf(file, "\"if\"%s];\n",                COND_GRAPH_STYLE); break; } 
        case IFEL_TYPE:  { fprintf(file, "\"if-else\"%s];\n",           IFEL_GRAPH_STYLE); break; } 
        case LOOP_TYPE:  { fprintf(file, "\"while\"%s];\n",             LOOP_GRAPH_STYLE); break; } 
        case ASSG_TYPE:  { fprintf(file, "\"=\"%s];\n",                 ASSG_GRAPH_STYLE); break; } 
        
        case CALL_TYPE:  { fprintf(file, "\"call\"%s];\n",              CALL_GRAPH_STYLE); break; } 
        case JUMP_TYPE:  { fprintf(file, "\"return\"%s];\n",            JUMP_GRAPH_STYLE); break; } 

        case MATH_TYPE:  { fprintf(file, "\"%s\"%s];\n", 
                                   mathOpToString(node->data.operation), 
                                   MATH_GRAPH_STYLE); break; } 

        case NUMB_TYPE:  { fprintf(file, "\"%lg\"%s];\n", 
                                   node->data.number, 
                                   NUMB_GRAPH_STYLE); break; } 

        default:         { assert(! "VALID TYPE"); break; } 
    }

    if (node->parent != nullptr)
    {
        if (isLeft(node))
        {
            fprintf(file, "\t\"%p\":sw->\"%p\";\n", (void*) node->parent, (void*) node);
        }
        else
        {
            fprintf(file, "\t\"%p\":se->\"%p\";\n", (void*) node->parent, (void*) node);
        }
    }   

    graphDumpSubtree(file, node->left);
    graphDumpSubtree(file, node->right);
}

void dumpToFile(FILE* file, Node* node)
{
    assert(file != nullptr);
    if (node == nullptr) { return; }

    fprintf(file, "%d | ", node->type);

    if (node->type == NUMB_TYPE)
    {
        fprintf(file, "%lg ", node->data.number);
    }
    else if (node->type == NAME_TYPE)
    {
        if (strcmp(node->data.id, MAIN_FUNCTION_NAME) == 0)
        {
            fprintf(file, "%s ", UNIVERSAL_MAIN_NAME);
        }
        else if (strcmp(node->data.id, KEYWORDS[PRINT_KEYWORD].name) == 0)
        {
            fprintf(file, "%s ", UNIVERSAL_PRINT_NAME);   
        }
        else if (strcmp(node->data.id, KEYWORDS[SCAN_KEYWORD].name) == 0)
        {
            fprintf(file, "%s ", UNIVERSAL_SCAN_NAME);   
        }
        else if (strcmp(node->data.id, KEYWORDS[FLOOR_KEYWORD].name) == 0)
        {
            fprintf(file, "%s ", UNIVERSAL_FLOOR_NAME);   
        }
        else if (strcmp(node->data.id, KEYWORDS[SQRT_KEYWORD].name) == 0)
        {
            fprintf(file, "%s ", UNIVERSAL_SQRT_NAME);   
        }
        else
        {
            fprintf(file, "%s ", node->data.id);
        }
    }
    else 
    {
        fprintf(file, "%d ", node->data.operation);
    }

    fprintf(file, "{ ");
    dumpToFile(file, node->left);
    fprintf(file, "} ");

    fprintf(file, "{ ");
    dumpToFile(file, node->right);
    fprintf(file, "} ");
}

Node* readTreeFromFile(const char* filename)
{
    char*  buffer     = nullptr;
    size_t bufferSize = 0;
    size_t ofs        = 0;

    if (!loadFile(filename, &buffer, &bufferSize))
    {
        return nullptr;
    }

    Node* node = newNode(DECL_TYPE, {}, nullptr, nullptr);
    ofs += FIRST_DECL_LENGTH; // to skip the first decl info

    while (ofs < bufferSize)
    {
        while (buffer[ofs] == ' ') { ofs++; }

        if (buffer[ofs] == '{')
        {
            if (ofs + LEAF_CHILDREN_LENGTH < bufferSize && strncmp(buffer + ofs, "{ } { }", LEAF_CHILDREN_LENGTH) == 0)
            {
                ofs += LEAF_CHILDREN_LENGTH;
                continue;
            }

            if (ofs + LEFT_CHILD_LENGTH < bufferSize && strncmp(buffer + ofs, "{ } { ", LEFT_CHILD_LENGTH) == 0)
            {
                setRight(node, newNode());
                node = node->right;
                ofs += LEFT_CHILD_LENGTH;
                continue;
            }

            if (node->left == nullptr)
            {
                setLeft(node, newNode());
                node = node->left;
            }
            else if (buffer[ofs + 2] == '}') // in case '{ }'
            {
                ofs += 3; // to skip '{ }'
                continue;
            }
            else
            {
                setRight(node, newNode());
                node = node->right;
            }

            ofs++;
            continue;
        } 

        if (buffer[ofs] == '}')
        {
            node = node->parent;
            ofs++;
            continue;
        }

        int len = 0;
        sscanf(buffer + ofs, "%d | %n", (int*) &node->type, &len);
        ofs += len;

        if (node->type == NUMB_TYPE)
        {
            sscanf(buffer + ofs, "%lg %n", &(node->data.number), &len);
        }
        else if (node->type == NAME_TYPE)
        {
            len = strspn(buffer + ofs, LETTERS);

            if (strncmp(buffer + ofs, UNIVERSAL_MAIN_NAME, len) == 0)
            {
                node->data.id = MAIN_FUNCTION_NAME;
            }
            else if (strncmp(buffer + ofs, UNIVERSAL_PRINT_NAME, len) == 0)
            {
                node->data.id = KEYWORDS[PRINT_KEYWORD].name;
            }
            else if (strncmp(buffer + ofs, UNIVERSAL_SCAN_NAME, len) == 0)
            {
                node->data.id = KEYWORDS[SCAN_KEYWORD].name;
            }
            else if (strncmp(buffer + ofs, UNIVERSAL_FLOOR_NAME, len) == 0)
            {
                node->data.id = KEYWORDS[FLOOR_KEYWORD].name;
            }
            else if (strncmp(buffer + ofs, UNIVERSAL_SQRT_NAME, len) == 0)
            {
                node->data.id = KEYWORDS[SQRT_KEYWORD].name;
            }
            else
            {
                node->data.id = copyString(buffer + ofs, len);
            }
        }
        else
        {
            sscanf(buffer + ofs, "%d %n", (int*) &(node->data.operation), &len);
        }

        ofs += len;
    }

    free(buffer);

    return node;
}