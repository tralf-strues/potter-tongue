struct Restorer
{
    size_t curIndent;
    FILE*  file;
};  

bool restoreCode(Node* root, const char* filename)
{   
    assert(root     != nullptr);
    assert(filename != nullptr);

    FILE* file = fopen(filename, "w");
    if (file == nullptr) { return false; }

    Restorer restorer  = { 0, file };

    fprintf(file, "%s %s\n\n", KEYWORDS[PROG_START_KEYWORD].name, filename);

    Node* curDeclaration = root;
    while (curDeclaration != nullptr)
    {
        restoreFunction(&restorer, curDeclaration);
        curDeclaration = curDeclaration->left;
    }
}   

void restoreFunction(Restorer* restorer, Node* node)
{
    assert(restorer != nullptr);
    assert(node     != nullptr);


}