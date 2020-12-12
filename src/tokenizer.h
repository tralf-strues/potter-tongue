#pragma once
#include "syntax.h"

union TokenData
{
    double      number;
    char*       id;
    KeywordCode keywordCode;
};

enum TokenType
{
    NUMBER_TOKEN_TYPE,
    ID_TOKEN_TYPE,
    KEYWORD_TOKEN_TYPE
};

struct Token
{
    TokenType   type;
    TokenData   data;
    size_t      line;
    const char* pos;
};

struct Tokenizer
{
    const char* buffer;
    size_t      bufferSize;
    const char* position;

    Token*      tokens;
    size_t      tokensCount;
    size_t      currentLine;
};

void construct      (Tokenizer* tokenizer, const char* buffer, size_t bufferSize);
void destroy        (Tokenizer* tokenizer);

bool isNumberType   (Token* token);
bool isIdType       (Token* token);
bool isKeywordType  (Token* token);

bool isNumber       (Token* token, double number);
bool isId           (Token* token, const char* id);
bool isKeyword      (Token* token, KeywordCode keywordCode);

bool isComparand    (Token* token);
bool isTerm         (Token* token);
bool isFactor       (Token* token);
 
void tokenizeBuffer (Tokenizer* tokenizer);
void dumpTokens     (Token* tokens, size_t count);
