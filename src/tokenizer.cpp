#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "tokenizer.h"
#include "../libs/utilib.h"

#define ASSERT_TOKENIZER(tokenizer) assert((tokenizer)           != nullptr); \
                                    assert((tokenizer)->buffer   != nullptr); \
                                    assert((tokenizer)->position != nullptr); \
                                    assert((tokenizer)->tokens   != nullptr); 

const size_t MAX_TOKENS_COUNT = 8192;

bool   finished        (Tokenizer* tokenizer);
void   skipSpaces      (Tokenizer* tokenizer);
void   proceed         (Tokenizer* tokenizer, size_t step);
void   addToken        (Tokenizer* tokenizer, Token token);
bool   processKeyword  (Tokenizer* tokenizer);
bool   isKeywordNumber (Keyword keyword);
double keywordToNumber (Keyword keyword);
bool   processId       (Tokenizer* tokenizer);

void construct(Tokenizer* tokenizer, const char* buffer, size_t bufferSize)
{
    assert(tokenizer != nullptr);

    tokenizer->buffer      = buffer;
    tokenizer->bufferSize  = bufferSize;
    tokenizer->position    = buffer;

    tokenizer->tokens      = (Token*) calloc(MAX_TOKENS_COUNT, sizeof(Token));
    tokenizer->tokensCount = 0;
    tokenizer->currentLine = 0;

    assert(tokenizer->tokens != nullptr);
}

void destroy(Tokenizer* tokenizer)
{
    ASSERT_TOKENIZER(tokenizer);

    free(tokenizer->tokens);

    tokenizer->buffer      = nullptr;
    tokenizer->bufferSize  = 0;
    tokenizer->position    = nullptr;

    tokenizer->tokens      = nullptr;
    tokenizer->tokensCount = 0;
    tokenizer->currentLine = 0;
}

bool isNumberType(Token* token)
{
    assert(token != nullptr);

    return token->type == NUMBER_TOKEN_TYPE;
}

bool isIdType(Token* token)
{
    assert(token != nullptr);

    return token->type == ID_TOKEN_TYPE;
}

bool isKeywordType(Token* token)
{
    assert(token != nullptr);

    return token->type == KEYWORD_TOKEN_TYPE;
}

bool isNumber(Token* token, double number)
{
    assert(token != nullptr);

    return isNumberType(token) && dcompare(token->data.number, number) == 0;
}

bool isId(Token* token, const char* id)
{
    assert(token != nullptr);

    return isIdType(token) && strcmp(token->data.id, id) == 0;
}

bool isKeyword(Token* token, KeywordCode keywordCode)
{
    assert(token != nullptr);

    return isKeywordType(token) && token->data.keywordCode == keywordCode;
}

bool isComparand(Token* token)
{
    assert(token != nullptr);

    return isKeywordType(token) && 
           token->data.keywordCode >= EQUAL_KEYWORD && 
           token->data.keywordCode <= GREATER_EQUAL_KEYWORD;
}

bool isTerm(Token* token)
{
    assert(token != nullptr);

    return isKeywordType(token) && 
           token->data.keywordCode >= PLUS_KEYWORD && 
           token->data.keywordCode <= MINUS_KEYWORD;
}

bool isFactor(Token* token)
{
    assert(token != nullptr);

    return isKeywordType(token) && 
           token->data.keywordCode >= MUL_KEYWORD && 
           token->data.keywordCode <= DIV_KEYWORD;
}

void tokenizeBuffer(Tokenizer* tokenizer)
{
    ASSERT_TOKENIZER(tokenizer);

    skipSpaces(tokenizer);

    while (!finished(tokenizer))
    {
        if (!processKeyword(tokenizer) && !processId(tokenizer))
        {
            break;
        }
    }
}

bool finished(Tokenizer* tokenizer)
{
    ASSERT_TOKENIZER(tokenizer);

    return tokenizer->position - tokenizer->buffer > tokenizer->bufferSize;
}

void skipSpaces(Tokenizer* tokenizer)
{
    ASSERT_TOKENIZER(tokenizer);

    tokenizer->position += strspn(tokenizer->position, " \t");
}

void proceed(Tokenizer* tokenizer, size_t step)
{
    ASSERT_TOKENIZER(tokenizer);
    
    tokenizer->position += step;
    skipSpaces(tokenizer);
}

void addToken(Tokenizer* tokenizer, Token token)
{
    ASSERT_TOKENIZER(tokenizer);

    tokenizer->tokens[tokenizer->tokensCount++] = token;
}

bool processKeyword(Tokenizer* tokenizer)
{
    ASSERT_TOKENIZER(tokenizer);

    for (size_t i = 0; i < KEYWORDS_COUNT; i++)
    {
        Keyword keyword = KEYWORDS[i];

        if (tokenizer->position + keyword.length - tokenizer->buffer > tokenizer->bufferSize)
        {
            continue;
        }

        if (strncmp(tokenizer->position, keyword.name, keyword.length) == 0)
        {
            if (isKeywordNumber(keyword))
            {
                addToken(tokenizer, {NUMBER_TOKEN_TYPE, {.number = keywordToNumber(keyword)}, tokenizer->currentLine, tokenizer->position});
            }
            else
            {
                addToken(tokenizer, {KEYWORD_TOKEN_TYPE, {.keywordCode = keyword.code}, tokenizer->currentLine, tokenizer->position});
            }

            if (*(tokenizer->position) == '\n')
            {
                tokenizer->currentLine++;
            }

            proceed(tokenizer, keyword.length);

            return true;
        }
    }

    return false;
}

bool isKeywordNumber(Keyword keyword)
{
    return keyword.code >= ZERO_KEYWORD && keyword.code <= SIX_KEYWORD;  
}

double keywordToNumber(Keyword keyword)
{
    switch (keyword.code)
    {
        case ZERO_KEYWORD:  return 0;
        case TWO_KEYWORD:   return 2;
        case THREE_KEYWORD: return 3;
        case SIX_KEYWORD:   return 6;

        default:            return -1;
    }
}

bool processId(Tokenizer* tokenizer)
{
    ASSERT_TOKENIZER(tokenizer);

    size_t length = strspn(tokenizer->position, LETTERS);

    if (tokenizer->position + length - tokenizer->buffer > tokenizer->bufferSize || length == 0)
    {
        return false;
    }

    addToken(tokenizer, {ID_TOKEN_TYPE, {.id = copyString(tokenizer->position, length)}, tokenizer->currentLine, tokenizer->position});
    proceed(tokenizer, length);

    return true;
}

void dumpTokens(Token* tokens, size_t count)
{
    assert(tokens != nullptr);

    for (size_t i = 0; i < count; i++)
    {
        printf("Token %u:\n"
               "    type = %d\n"
               "    data = ", i, tokens[i].type);

        switch (tokens[i].type)
        {
            case NUMBER_TOKEN_TYPE:  { printf("(number) %lg\n",          tokens[i].data.number);                                                 break; }
            case ID_TOKEN_TYPE:      { printf("(id) '%s'\n",             tokens[i].data.id);                                                     break; }
            case KEYWORD_TOKEN_TYPE: { printf("(keywordCode) %d '%s'\n", tokens[i].data.keywordCode, KEYWORDS[tokens[i].data.keywordCode].name); break; }
        }

        printf("    line = %u\n", tokens[i].line);

        if (tokens[i].pos[0] == '\n')
        {
            printf("    pos  = '\\n'\n\n");
        }
        else
        {
            size_t length = strcspn(tokens[i].pos, "\n");
            printf("    pos  = '%.*s'\n\n", length, tokens[i].pos);
        }
    }
}