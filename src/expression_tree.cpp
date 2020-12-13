#include <assert.h>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "expression_tree.h"
#include "../libs/utilib.h"

const size_t MAX_FILENAME_LENGTH = 128;
const size_t MAX_COMMAND_LENGTH  = 256;

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

    node->type    = type;
    node->data    = data;

    node->parent  = nullptr;
    node->left    = left;
    node->right   = right;

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
        fscanf(counterFile, "%u", &count);
        fseek(counterFile, 0, SEEK_SET);
    }

    fprintf(counterFile, "%u", count + 1);
    fclose(counterFile);

    return count;
}

void graphDump(Node* root)
{
    assert(root != nullptr);

    int count = counterFileUpdate("log/tree_dumps/graph/count.cnt");

    char textFilename[MAX_FILENAME_LENGTH] = {};
    snprintf(textFilename, sizeof(textFilename), "%s%u.txt", "log/tree_dumps/graph/text/tree", count);

    char imageFilename[MAX_FILENAME_LENGTH] = {};
    snprintf(imageFilename, sizeof(imageFilename), "%s%u.svg", "log/tree_dumps/graph/img/tree", count);

    FILE* file = fopen(textFilename, "w");
    assert(file != nullptr);

    fprintf(file,
            "digraph structs {\n"
            "\tnode [shape=\"circle\", style=\"filled\", color=\"#52527A\", fontcolor=\"#52527A\", fillcolor=\"#E1E1EA\"];\n\n");

    if (root == nullptr) { fprintf(file, "\t\"ROOT\" [label = \"Empty database\"];\n"); }
    else                 { graphDumpSubtree(file, root); }

    fprintf(file, "}");

    fclose(file);

    char dotCmd[MAX_COMMAND_LENGTH] = {};

    snprintf(dotCmd, sizeof(dotCmd), "dot -Tsvg %s -o %s", textFilename, imageFilename);
    system(dotCmd);

    snprintf(dotCmd, sizeof(dotCmd), "start %s", imageFilename);
    system(dotCmd);
}

void graphDumpSubtree(FILE* file, Node* node)
{
    assert(file != nullptr);
    
    if (node == nullptr) { return; }

    fprintf(file, "\t\"%p\" [label=", node);

    NodeType    type      = node->type; 
    const char* constName = nullptr;
    switch (type)
    {
        // case TYPE_NUMBER:
        //     constName = getConstantName(node->data.number);
        //     if (constName != nullptr)
        //     {
        //         fprintf(file, "<<B><I>%s</I></B>>, color=\"#75A673\", fillcolor=\"#EDFFED\", fontcolor=\"#75A673\"];\n", constName);
        //     }
        //     else
        //     {
        //         fprintf(file, "\"%lg\", color=\"#D2691E\", fillcolor=\"#FFFAEF\", fontcolor=\"#FF7F50\"];\n", node->data.number);
        //     }

        //     break;

        // case TYPE_VAR:
        //     fprintf(file, "<<B><I>%c</I></B>>, color=\"#367ACC\", fillcolor=\"#E0F5FF\", fontcolor=\"#4881CC\"];\n", node->data.var);
        //     break;

        // case TYPE_OP:
        //     fprintf(file, "\"%s\", color=\"#000000\", fillcolor=\"#FFFFFF\", fontcolor=\"#000000\"];\n", OPERATIONS[node->data.op]);
        //     break;

        case DECL_TYPE: { fprintf(file, "\"D\", color=\"#000000\", "
                                        "fillcolor=\"#FFFFFF\", "
                                        "fontcolor=\"#000000\"];\n");                        break; }

        case BLCK_TYPE: { fprintf(file, "\"Block\", color=\"#000000\", "
                                        "fillcolor=\"#FFFFFF\", "
                                        "fontcolor=\"#000000\"];\n");                        break; }

        case STAT_TYPE: { fprintf(file, "\"S\", color=\"#000000\", "
                                        "fillcolor=\"#FFFFFF\", "
                                        "fontcolor=\"#000000\"];\n");                        break; }

        case COND_TYPE: { fprintf(file, "\"if\", color=\"#367ACC\", "
                                        "fillcolor=\"#E0F5FF\", "
                                        "fontcolor=\"#4881CC\"];\n");                        break; } 

        case IFEL_TYPE: { fprintf(file, "\"if-else\", color=\"#367ACC\", "
                                        "fillcolor=\"#E0F5FF\", "
                                        "fontcolor=\"#4881CC\"];\n");                        break; } 

        case LOOP_TYPE: { fprintf(file, "\"while\", color=\"#367ACC\", "
                                        "fillcolor=\"#E0F5FF\", "
                                        "fontcolor=\"#4881CC\"];\n");                        break; } 

        case ASSG_TYPE: { fprintf(file, "\"=\"];\n");                                        break; } 

        case CALL_TYPE: { fprintf(file, "\"call\"];\n");                                     break; } 
        case LIST_TYPE: { fprintf(file, "\"param\"];\n");                                    break; } 
        case JUMP_TYPE: { fprintf(file, "\"return\"];\n");                                   break; } 

        case MATH_TYPE: { fprintf(file, "\"%s\"];\n", mathOpToString(node->data.operation)); break; } 

        case NUMB_TYPE: { fprintf(file, "\"%lg\", color=\"#D2691E\", "
                                        "fillcolor=\"#FFFAEF\", "
                                        "fontcolor=\"#FF7F50\"];\n", node->data.number);     break; } 

        case NAME_TYPE: { fprintf(file, "\"%s\", color=\"#75A673\", "
                                        "fillcolor=\"#EDFFED\", "
                                        "fontcolor=\"#75A673\"];\n", node->data.id);         break; } 

        default:        { assert(! "VALID TYPE");                                            break; } 
    }

    if (node->parent != nullptr)
    {
        if (isLeft(node))
        {
            fprintf(file, "\t\"%p\":sw->\"%p\";\n", node->parent, node);
        }
        else
        {
            fprintf(file, "\t\"%p\":se->\"%p\";\n", node->parent, node);
        }
    }   

    graphDumpSubtree(file, node->left);
    graphDumpSubtree(file, node->right);
}