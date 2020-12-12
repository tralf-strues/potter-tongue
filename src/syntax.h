#pragma once

#include <stdlib.h>

enum MathOp
{
    ADD_OP,
    SUB_OP,
    MUL_OP,
    DIV_OP,

    EQUAL_OP,
    NOT_EQUAL_OP,
    LESS_OP,
    GREATER_OP,
    LESS_EQUAL_OP,
    GREATER_EQUAL_OP
};

enum KeywordCode
{
    PROG_START_KEYWORD,
    PROG_END_KEYWORD,
    NEW_LINE_KEYWORD,

    VDECL_KEYWORD,
    FDECL_KEYWORD,

    ZERO_KEYWORD,
    TWO_KEYWORD,
    THREE_KEYWORD,
    SIX_KEYWORD,

    CALL_KEYWORD,
    RETURN_KEYWORD,

    OPEN_BRACE_KEYWORD,
    CLOSE_BRACE_KEYWORD,

    BRACKET_KEYWORD,
    COMMA_KEYWORD,

    CMD_LINE_KEYWORD,

    IF_KEYWORD,
    ELSE_KEYWORD,
    LOOP_KEYWORD,

    ASSGN_KEYWORD,
    DEREF_KEYWORD,

    SCAN_KEYWORD,
    PRINT_KEYWORD,
    FLOOR_KEYWORD,

    PLUS_KEYWORD,
    MINUS_KEYWORD,
    MUL_KEYWORD,
    DIV_KEYWORD,

    EQUAL_KEYWORD,
    NOT_EQUAL_KEYWORD,
    LESS_KEYWORD,
    GREATER_KEYWORD,
    LESS_EQUAL_KEYWORD,
    GREATER_EQUAL_KEYWORD,

    KEYWORDS_COUNT
};

struct Keyword
{
    const char* name;
    size_t      length;
    KeywordCode code;
};

static const Keyword KEYWORDS[KEYWORDS_COUNT] = {
    { "Godric's-Hollow", 15, PROG_START_KEYWORD    },
    { "Privet-Drive",    12, PROG_END_KEYWORD      },
    { "\n",              1,  NEW_LINE_KEYWORD      },

    { "avenseguim",      10, VDECL_KEYWORD         },
    { "imperio",         7,  FDECL_KEYWORD         },

    { "horcrux",         7,  ZERO_KEYWORD          },
    { "duo",             3,  TWO_KEYWORD           },
    { "tria",            4,  THREE_KEYWORD         },
    { "maxima",          6,  SIX_KEYWORD           },

    { "depulso",         7,  CALL_KEYWORD          },
    { "reverte",         7,  RETURN_KEYWORD        },

    { "alohomora",       9,  OPEN_BRACE_KEYWORD    },
    { "colloportus",     11, CLOSE_BRACE_KEYWORD   },

    { "protego",         7,  BRACKET_KEYWORD       },
    { ",",               1,  COMMA_KEYWORD         },

    { "-",               1,  CMD_LINE_KEYWORD      },

    { "revelio",         7,  IF_KEYWORD            },
    { "otherwise",       9,  ELSE_KEYWORD          },
    { "while",           5,  LOOP_KEYWORD          },

    { "carpe-retractum", 15, ASSGN_KEYWORD         },
    { "legilimens",      10, DEREF_KEYWORD         },

    { "accio",           5,  SCAN_KEYWORD          },
    { "flagrate",        8,  PRINT_KEYWORD         },
    { "colloshoo",       9,  FLOOR_KEYWORD         },

    { "epoximise",       9,  PLUS_KEYWORD          },
    { "flipendo",        8,  MINUS_KEYWORD         },
    { "geminio",         7,  MUL_KEYWORD           },
    { "sectumsempra",    12, DIV_KEYWORD           },

    { "equal",           5,  EQUAL_KEYWORD         },
    { "not-equal",       9,  NOT_EQUAL_KEYWORD     },
    { "less",            4,  LESS_KEYWORD          },
    { "greater",         7,  GREATER_KEYWORD       },
    { "less-equal",      10, LESS_EQUAL_KEYWORD    },
    { "greater-equal",   13, GREATER_EQUAL_KEYWORD } 
};

const char* mathOpToString(MathOp operation);