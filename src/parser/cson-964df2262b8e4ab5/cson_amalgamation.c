/* auto-generated! Do not edit! */
#include "cson_amalgamation.h"
/* begin file cson_amalgamation_core.c */
/* auto-generated! Do not edit! */
/* begin file parser/JSON_parser.h */
/* See JSON_parser.c for copyright information and licensing. */

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

/* JSON_parser.h */


#include <stddef.h>

/* Windows DLL stuff */
#ifdef JSON_PARSER_DLL
#   ifdef _MSC_VER
#	    ifdef JSON_PARSER_DLL_EXPORTS
#		    define JSON_PARSER_DLL_API __declspec(dllexport)
#	    else
#		    define JSON_PARSER_DLL_API __declspec(dllimport)
#       endif
#   else
#	    define JSON_PARSER_DLL_API 
#   endif
#else
#	define JSON_PARSER_DLL_API 
#endif

/* Determine the integer type use to parse non-floating point numbers */
#ifdef _WIN32
typedef __int64 JSON_int_t;
#define JSON_PARSER_INTEGER_SSCANF_TOKEN "%I64d"
#define JSON_PARSER_INTEGER_SPRINTF_TOKEN "%I64d"
#elif (__STDC_VERSION__ >= 199901L) || (HAVE_LONG_LONG == 1)
typedef long long JSON_int_t;
#define JSON_PARSER_INTEGER_SSCANF_TOKEN "%lld"
#define JSON_PARSER_INTEGER_SPRINTF_TOKEN "%lld"
#else 
typedef long JSON_int_t;
#define JSON_PARSER_INTEGER_SSCANF_TOKEN "%ld"
#define JSON_PARSER_INTEGER_SPRINTF_TOKEN "%ld"
#endif


#ifdef __cplusplus
extern "C" {
#endif 

typedef enum 
{
    JSON_E_NONE = 0,
    JSON_E_INVALID_CHAR,
    JSON_E_INVALID_KEYWORD,
    JSON_E_INVALID_ESCAPE_SEQUENCE,
    JSON_E_INVALID_UNICODE_SEQUENCE,
    JSON_E_INVALID_NUMBER,
    JSON_E_NESTING_DEPTH_REACHED,
    JSON_E_UNBALANCED_COLLECTION,
    JSON_E_EXPECTED_KEY,
    JSON_E_EXPECTED_COLON,
    JSON_E_OUT_OF_MEMORY
} JSON_error;

typedef enum 
{
    JSON_T_NONE = 0,
    JSON_T_ARRAY_BEGIN,
    JSON_T_ARRAY_END,
    JSON_T_OBJECT_BEGIN,
    JSON_T_OBJECT_END,
    JSON_T_INTEGER,
    JSON_T_FLOAT,
    JSON_T_NULL,
    JSON_T_TRUE,
    JSON_T_FALSE,
    JSON_T_STRING,
    JSON_T_KEY,
    JSON_T_MAX
} JSON_type;

typedef struct JSON_value_struct {
    union {
        JSON_int_t integer_value;
        
        double float_value;
        
        struct {
            const char* value;
            size_t length;
        } str;
    } vu;
} JSON_value;

typedef struct JSON_parser_struct* JSON_parser;

/*! \brief JSON parser callback 

    \param ctx The pointer passed to new_JSON_parser.
    \param type An element of JSON_type but not JSON_T_NONE.    
    \param value A representation of the parsed value. This parameter is NULL for
        JSON_T_ARRAY_BEGIN, JSON_T_ARRAY_END, JSON_T_OBJECT_BEGIN, JSON_T_OBJECT_END,
        JSON_T_NULL, JSON_T_TRUE, and JSON_T_FALSE. String values are always returned
        as zero-terminated C strings.

    \return Non-zero if parsing should continue, else zero.
*/    
typedef int (*JSON_parser_callback)(void* ctx, int type, const JSON_value* value);


/**
   A typedef for allocator functions semantically compatible with malloc().
*/
typedef void* (*JSON_malloc_t)(size_t n);
/**
   A typedef for deallocator functions semantically compatible with free().
*/
typedef void (*JSON_free_t)(void* mem);

/*! \brief The structure used to configure a JSON parser object 
*/
typedef struct {
    /** Pointer to a callback, called when the parser has something to tell
        the user. This parameter may be NULL. In this case the input is
        merely checked for validity.
    */
    JSON_parser_callback    callback;
    /**
       Callback context - client-specified data to pass to the
       callback function. This parameter may be NULL.
    */
    void*                   callback_ctx;
    /** Specifies the levels of nested JSON to allow. Negative numbers yield unlimited nesting.
        If negative, the parser can parse arbitrary levels of JSON, otherwise
        the depth is the limit.
    */
    int                     depth;
    /**
       To allow C style comments in JSON, set to non-zero.
    */
    int                     allow_comments;
    /**
       To decode floating point numbers manually set this parameter to
       non-zero.
    */
    int                     handle_floats_manually;
    /**
       The memory allocation routine, which must be semantically
       compatible with malloc(3). If set to NULL, malloc(3) is used.

       If this is set to a non-NULL value then the 'free' member MUST be
       set to the proper deallocation counterpart for this function.
       Failure to do so results in undefined behaviour at deallocation
       time.
    */
    JSON_malloc_t       malloc;
    /**
       The memory deallocation routine, which must be semantically
       compatible with free(3). If set to NULL, free(3) is used.

       If this is set to a non-NULL value then the 'alloc' member MUST be
       set to the proper allocation counterpart for this function.
       Failure to do so results in undefined behaviour at deallocation
       time.
    */
    JSON_free_t         free;
} JSON_config;

/*! \brief Initializes the JSON parser configuration structure to default values.

    The default configuration is
    - 127 levels of nested JSON (depends on JSON_PARSER_STACK_SIZE, see json_parser.c)
    - no parsing, just checking for JSON syntax
    - no comments
    - Uses realloc() for memory de/allocation.

    \param config. Used to configure the parser.
*/
JSON_PARSER_DLL_API void init_JSON_config(JSON_config * config);

/*! \brief Create a JSON parser object 

    \param config. Used to configure the parser. Set to NULL to use
        the default configuration. See init_JSON_config.  Its contents are
        copied by this function, so it need not outlive the returned
        object.
    
    \return The parser object, which is owned by the caller and must eventually
    be freed by calling delete_JSON_parser().
*/
JSON_PARSER_DLL_API JSON_parser new_JSON_parser(JSON_config const* config);

/*! \brief Destroy a previously created JSON parser object. */
JSON_PARSER_DLL_API void delete_JSON_parser(JSON_parser jc);

/*! \brief Parse a character.

    \return Non-zero, if all characters passed to this function are part of are valid JSON.
*/
JSON_PARSER_DLL_API int JSON_parser_char(JSON_parser jc, int next_char);

/*! \brief Finalize parsing.

    Call this method once after all input characters have been consumed.
    
    \return Non-zero, if all parsed characters are valid JSON, zero otherwise.
*/
JSON_PARSER_DLL_API int JSON_parser_done(JSON_parser jc);

/*! \brief Determine if a given string is valid JSON white space 

    \return Non-zero if the string is valid, zero otherwise.
*/
JSON_PARSER_DLL_API int JSON_parser_is_legal_white_space_string(const char* s);

/*! \brief Gets the last error that occurred during the use of JSON_parser.

    \return A value from the JSON_error enum.
*/
JSON_PARSER_DLL_API int JSON_parser_get_last_error(JSON_parser jc);

/*! \brief Re-sets the parser to prepare it for another parse run.

    \return True (non-zero) on success, 0 on error (e.g. !jc).
*/
JSON_PARSER_DLL_API int JSON_parser_reset(JSON_parser jc);


#ifdef __cplusplus
}
#endif 
    

#endif /* JSON_PARSER_H */
/* end file parser/JSON_parser.h */
/* begin file parser/JSON_parser.c */
/*
Copyright (c) 2007-2013 Jean Gressmann (jean@0x42.de)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
    Changelog:
        2013-09-08
            Updated license to to be compatible with Debian license requirements.

        2012-06-06
            Fix for invalid UTF16 characters and some comment fixex (thomas.h.moog@intel.com).

        2010-11-25
            Support for custom memory allocation (sgbeal@googlemail.com).

        2010-05-07
            Added error handling for memory allocation failure (sgbeal@googlemail.com).
            Added diagnosis errors for invalid JSON.

        2010-03-25
            Fixed buffer overrun in grow_parse_buffer & cleaned up code.

        2009-10-19
            Replaced long double in JSON_value_struct with double after reports
            of strtold being broken on some platforms (charles@transmissionbt.com).

        2009-05-17
            Incorporated benrudiak@googlemail.com fix for UTF16 decoding.

        2009-05-14
            Fixed float parsing bug related to a locale being set that didn't
            use '.' as decimal point character (charles@transmissionbt.com).

        2008-10-14
            Renamed states.IN to states.IT to avoid name clash which IN macro
            defined in windef.h (alexey.pelykh@gmail.com)

        2008-07-19
            Removed some duplicate code & debugging variable (charles@transmissionbt.com)

        2008-05-28
            Made JSON_value structure ansi C compliant. This bug was report by
            trisk@acm.jhu.edu

        2008-05-20
            Fixed bug reported by charles@transmissionbt.com where the switching
            from static to dynamic parse buffer did not copy the static parse
            buffer's content.
*/



#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>


#ifdef _MSC_VER
#   if _MSC_VER >= 1400 /* Visual Studio 2005 and up */
#      pragma warning(disable:4996) /* unsecure sscanf */
#      pragma warning(disable:4127) /* conditional expression is constant */
#   endif
#endif


#define true  1
#define false 0
#define __   -1     /* the universal error code */

/* values chosen so that the object size is approx equal to one page (4K) */
#ifndef JSON_PARSER_STACK_SIZE
#   define JSON_PARSER_STACK_SIZE 128
#endif

#ifndef JSON_PARSER_PARSE_BUFFER_SIZE
#   define JSON_PARSER_PARSE_BUFFER_SIZE 3500
#endif

typedef void* (*JSON_debug_malloc_t)(size_t bytes, const char* reason);

#ifdef JSON_PARSER_DEBUG_MALLOC
#   define JSON_parser_malloc(func, bytes, reason) ((JSON_debug_malloc_t)func)(bytes, reason)
#else
#   define JSON_parser_malloc(func, bytes, reason) func(bytes)
#endif

typedef unsigned short UTF16;

struct JSON_parser_struct {
    JSON_parser_callback callback;
    void* ctx;
    signed char state, before_comment_state, type, escaped, comment, allow_comments, handle_floats_manually, error;
    char decimal_point;
    UTF16 utf16_high_surrogate;
    int current_char;
    int depth;
    int top;
    int stack_capacity;
    signed char* stack;
    char* parse_buffer;
    size_t parse_buffer_capacity;
    size_t parse_buffer_count;
    signed char static_stack[JSON_PARSER_STACK_SIZE];
    char static_parse_buffer[JSON_PARSER_PARSE_BUFFER_SIZE];
    JSON_malloc_t malloc;
    JSON_free_t free;
};

#define COUNTOF(x) (sizeof(x)/sizeof(x[0]))

/*
    Characters are mapped into these character classes. This allows for
    a significant reduction in the size of the state transition table.
*/



enum classes {
    C_SPACE,  /* space */
    C_WHITE,  /* other whitespace */
    C_LCURB,  /* {  */
    C_RCURB,  /* } */
    C_LSQRB,  /* [ */
    C_RSQRB,  /* ] */
    C_COLON,  /* : */
    C_COMMA,  /* , */
    C_QUOTE,  /* " */
    C_BACKS,  /* \ */
    C_SLASH,  /* / */
    C_PLUS,   /* + */
    C_MINUS,  /* - */
    C_POINT,  /* . */
    C_ZERO ,  /* 0 */
    C_DIGIT,  /* 123456789 */
    C_LOW_A,  /* a */
    C_LOW_B,  /* b */
    C_LOW_C,  /* c */
    C_LOW_D,  /* d */
    C_LOW_E,  /* e */
    C_LOW_F,  /* f */
    C_LOW_L,  /* l */
    C_LOW_N,  /* n */
    C_LOW_R,  /* r */
    C_LOW_S,  /* s */
    C_LOW_T,  /* t */
    C_LOW_U,  /* u */
    C_ABCDF,  /* ABCDF */
    C_E,      /* E */
    C_ETC,    /* everything else */
    C_STAR,   /* * */
    NR_CLASSES
};

static const signed char ascii_class[128] = {
/*
    This array maps the 128 ASCII characters into character classes.
    The remaining Unicode characters should be mapped to C_ETC.
    Non-whitespace control characters are errors.
*/
    __,      __,      __,      __,      __,      __,      __,      __,
    __,      C_WHITE, C_WHITE, __,      __,      C_WHITE, __,      __,
    __,      __,      __,      __,      __,      __,      __,      __,
    __,      __,      __,      __,      __,      __,      __,      __,

    C_SPACE, C_ETC,   C_QUOTE, C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,
    C_ETC,   C_ETC,   C_STAR,   C_PLUS, C_COMMA, C_MINUS, C_POINT, C_SLASH,
    C_ZERO,  C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT,
    C_DIGIT, C_DIGIT, C_COLON, C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,

    C_ETC,   C_ABCDF, C_ABCDF, C_ABCDF, C_ABCDF, C_E,     C_ABCDF, C_ETC,
    C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,
    C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,
    C_ETC,   C_ETC,   C_ETC,   C_LSQRB, C_BACKS, C_RSQRB, C_ETC,   C_ETC,

    C_ETC,   C_LOW_A, C_LOW_B, C_LOW_C, C_LOW_D, C_LOW_E, C_LOW_F, C_ETC,
    C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_LOW_L, C_ETC,   C_LOW_N, C_ETC,
    C_ETC,   C_ETC,   C_LOW_R, C_LOW_S, C_LOW_T, C_LOW_U, C_ETC,   C_ETC,
    C_ETC,   C_ETC,   C_ETC,   C_LCURB, C_ETC,   C_RCURB, C_ETC,   C_ETC
};


/*
    The state codes.
*/
enum states {
    GO,  /* start    */
    OK,  /* ok       */
    OB,  /* object   */
    KE,  /* key      */
    CO,  /* colon    */
    VA,  /* value    */
    AR,  /* array    */
    ST,  /* string   */
    ES,  /* escape   */
    U1,  /* u1       */
    U2,  /* u2       */
    U3,  /* u3       */
    U4,  /* u4       */
    MI,  /* minus    */
    ZE,  /* zero     */
    IT,  /* integer  */
    FR,  /* fraction */
    E1,  /* e        */
    E2,  /* ex       */
    E3,  /* exp      */
    T1,  /* tr       */
    T2,  /* tru      */
    T3,  /* true     */
    F1,  /* fa       */
    F2,  /* fal      */
    F3,  /* fals     */
    F4,  /* false    */
    N1,  /* nu       */
    N2,  /* nul      */
    N3,  /* null     */
    C1,  /* /        */
    C2,  /* / *     */
    C3,  /* *        */
    FX,  /* *.* *eE* */
    D1,  /* second UTF-16 character decoding started by \ */
    D2,  /* second UTF-16 character proceeded by u */
    NR_STATES
};

enum actions
{
    CB = -10, /* comment begin */
    CE = -11, /* comment end */
    FA = -12, /* false */
    TR = -13, /* false */
    NU = -14, /* null */
    DE = -15, /* double detected by exponent e E */
    DF = -16, /* double detected by fraction . */
    SB = -17, /* string begin */
    MX = -18, /* integer detected by minus */
    ZX = -19, /* integer detected by zero */
    IX = -20, /* integer detected by 1-9 */
    EX = -21, /* next char is escaped */
    UC = -22  /* Unicode character read */
};


static const signed char state_transition_table[NR_STATES][NR_CLASSES] = {
/*
    The state transition table takes the current state and the current symbol,
    and returns either a new state or an action. An action is represented as a
    negative number. A JSON text is accepted if at the end of the text the
    state is OK and if the mode is MODE_DONE.

                 white                                      1-9                                   ABCDF  etc
             space |  {  }  [  ]  :  ,  "  \  /  +  -  .  0  |  a  b  c  d  e  f  l  n  r  s  t  u  |  E  |  * */
/*start  GO*/ {GO,GO,-6,__,-5,__,__,__,__,__,CB,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*ok     OK*/ {OK,OK,__,-8,__,-7,__,-3,__,__,CB,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*object OB*/ {OB,OB,__,-9,__,__,__,__,SB,__,CB,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*key    KE*/ {KE,KE,__,__,__,__,__,__,SB,__,CB,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*colon  CO*/ {CO,CO,__,__,__,__,-2,__,__,__,CB,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*value  VA*/ {VA,VA,-6,__,-5,__,__,__,SB,__,CB,__,MX,__,ZX,IX,__,__,__,__,__,FA,__,NU,__,__,TR,__,__,__,__,__},
/*array  AR*/ {AR,AR,-6,__,-5,-7,__,__,SB,__,CB,__,MX,__,ZX,IX,__,__,__,__,__,FA,__,NU,__,__,TR,__,__,__,__,__},
/*string ST*/ {ST,__,ST,ST,ST,ST,ST,ST,-4,EX,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST},
/*escape ES*/ {__,__,__,__,__,__,__,__,ST,ST,ST,__,__,__,__,__,__,ST,__,__,__,ST,__,ST,ST,__,ST,U1,__,__,__,__},
/*u1     U1*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,U2,U2,U2,U2,U2,U2,U2,U2,__,__,__,__,__,__,U2,U2,__,__},
/*u2     U2*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,U3,U3,U3,U3,U3,U3,U3,U3,__,__,__,__,__,__,U3,U3,__,__},
/*u3     U3*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,U4,U4,U4,U4,U4,U4,U4,U4,__,__,__,__,__,__,U4,U4,__,__},
/*u4     U4*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,UC,UC,UC,UC,UC,UC,UC,UC,__,__,__,__,__,__,UC,UC,__,__},
/*minus  MI*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,ZE,IT,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*zero   ZE*/ {OK,OK,__,-8,__,-7,__,-3,__,__,CB,__,__,DF,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*int    IT*/ {OK,OK,__,-8,__,-7,__,-3,__,__,CB,__,__,DF,IT,IT,__,__,__,__,DE,__,__,__,__,__,__,__,__,DE,__,__},
/*frac   FR*/ {OK,OK,__,-8,__,-7,__,-3,__,__,CB,__,__,__,FR,FR,__,__,__,__,E1,__,__,__,__,__,__,__,__,E1,__,__},
/*e      E1*/ {__,__,__,__,__,__,__,__,__,__,__,E2,E2,__,E3,E3,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*ex     E2*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,E3,E3,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*exp    E3*/ {OK,OK,__,-8,__,-7,__,-3,__,__,__,__,__,__,E3,E3,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*tr     T1*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,T2,__,__,__,__,__,__,__},
/*tru    T2*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,T3,__,__,__,__},
/*true   T3*/ {__,__,__,__,__,__,__,__,__,__,CB,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__,__,__,__,__,__,__},
/*fa     F1*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,F2,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*fal    F2*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,F3,__,__,__,__,__,__,__,__,__},
/*fals   F3*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,F4,__,__,__,__,__,__},
/*false  F4*/ {__,__,__,__,__,__,__,__,__,__,CB,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__,__,__,__,__,__,__},
/*nu     N1*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,N2,__,__,__,__},
/*nul    N2*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,N3,__,__,__,__,__,__,__,__,__},
/*null   N3*/ {__,__,__,__,__,__,__,__,__,__,CB,__,__,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__,__,__,__,__},
/*/      C1*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,C2},
/*/star  C2*/ {C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C3},
/**      C3*/ {C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,CE,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C3},
/*_.     FX*/ {OK,OK,__,-8,__,-7,__,-3,__,__,__,__,__,__,FR,FR,__,__,__,__,E1,__,__,__,__,__,__,__,__,E1,__,__},
/*\      D1*/ {__,__,__,__,__,__,__,__,__,D2,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*\      D2*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,U1,__,__,__,__},
};


/*
    These modes can be pushed on the stack.
*/
enum modes {
    MODE_ARRAY = 1,
    MODE_DONE = 2,
    MODE_KEY = 3,
    MODE_OBJECT = 4
};

static void set_error(JSON_parser jc)
{
    switch (jc->state) {
        case GO:
            switch (jc->current_char) {
            case '{': case '}': case '[': case ']':
                jc->error = JSON_E_UNBALANCED_COLLECTION;
                break;
            default:
                jc->error = JSON_E_INVALID_CHAR;
                break;
            }
            break;
        case OB:
            jc->error = JSON_E_EXPECTED_KEY;
            break;
        case AR:
            jc->error = JSON_E_UNBALANCED_COLLECTION;
            break;
        case CO:
            jc->error = JSON_E_EXPECTED_COLON;
            break;
        case KE:
            jc->error = JSON_E_EXPECTED_KEY;
            break;
        /* \uXXXX\uYYYY */
        case U1: case U2: case U3: case U4: case D1: case D2:
            jc->error = JSON_E_INVALID_UNICODE_SEQUENCE;
            break;
        /* true, false, null */
        case T1: case T2: case T3: case F1: case F2: case F3: case F4: case N1: case N2: case N3:
            jc->error = JSON_E_INVALID_KEYWORD;
            break;
        /* minus, integer, fraction, exponent */
        case MI: case ZE: case IT: case FR: case E1: case E2: case E3:
            jc->error = JSON_E_INVALID_NUMBER;
            break;
        default:
            jc->error = JSON_E_INVALID_CHAR;
            break;
    }
}

static int
push(JSON_parser jc, int mode)
{
/*
    Push a mode onto the stack. Return false if there is overflow.
*/
    assert(jc->top <= jc->stack_capacity);

    if (jc->depth < 0) {
        if (jc->top == jc->stack_capacity) {
            const size_t bytes_to_copy = jc->stack_capacity * sizeof(jc->stack[0]);
            const size_t new_capacity = jc->stack_capacity * 2;
            const size_t bytes_to_allocate = new_capacity * sizeof(jc->stack[0]);
            void* mem = JSON_parser_malloc(jc->malloc, bytes_to_allocate, "stack");
            if (!mem) {
                jc->error = JSON_E_OUT_OF_MEMORY;
                return false;
            }
            jc->stack_capacity = (int)new_capacity;
            memcpy(mem, jc->stack, bytes_to_copy);
            if (jc->stack != &jc->static_stack[0]) {
                jc->free(jc->stack);
            }
            jc->stack = (signed char*)mem;
        }
    } else {
        if (jc->top == jc->depth) {
            jc->error = JSON_E_NESTING_DEPTH_REACHED;
            return false;
        }
    }
    jc->stack[++jc->top] = (signed char)mode;
    return true;
}


static int
pop(JSON_parser jc, int mode)
{
/*
    Pop the stack, assuring that the current mode matches the expectation.
    Return false if there is underflow or if the modes mismatch.
*/
    if (jc->top < 0 || jc->stack[jc->top] != mode) {
        return false;
    }
    jc->top -= 1;
    return true;
}


#define parse_buffer_clear(jc) \
    do {\
        jc->parse_buffer_count = 0;\
        jc->parse_buffer[0] = 0;\
    } while (0)

#define parse_buffer_pop_back_char(jc)\
    do {\
        assert(jc->parse_buffer_count >= 1);\
        --jc->parse_buffer_count;\
        jc->parse_buffer[jc->parse_buffer_count] = 0;\
    } while (0)



void delete_JSON_parser(JSON_parser jc)
{
    if (jc) {
        if (jc->stack != &jc->static_stack[0]) {
            jc->free((void*)jc->stack);
        }
        if (jc->parse_buffer != &jc->static_parse_buffer[0]) {
            jc->free((void*)jc->parse_buffer);
        }
        jc->free((void*)jc);
     }
}

int JSON_parser_reset(JSON_parser jc)
{
    if (NULL == jc) {
        return false;
    }

    jc->state = GO;
    jc->top = -1;

    /* parser has been used previously? */
    if (NULL == jc->parse_buffer) {

        /* Do we want non-bound stack? */
        if (jc->depth > 0) {
            jc->stack_capacity = jc->depth;
            if (jc->depth <= (int)COUNTOF(jc->static_stack)) {
                jc->stack = &jc->static_stack[0];
            } else {
                const size_t bytes_to_alloc = jc->stack_capacity * sizeof(jc->stack[0]);
                jc->stack = (signed char*)JSON_parser_malloc(jc->malloc, bytes_to_alloc, "stack");
                if (jc->stack == NULL) {
                    return false;
                }
            }
        } else {
            jc->stack_capacity = (int)COUNTOF(jc->static_stack);
            jc->depth = -1;
            jc->stack = &jc->static_stack[0];
        }

        /* set up the parse buffer */
        jc->parse_buffer = &jc->static_parse_buffer[0];
        jc->parse_buffer_capacity = COUNTOF(jc->static_parse_buffer);
    }

    /* set parser to start */
    push(jc, MODE_DONE);
    parse_buffer_clear(jc);

    return true;
}

JSON_parser
new_JSON_parser(JSON_config const * config)
{
/*
    new_JSON_parser starts the checking process by constructing a JSON_parser
    object. It takes a depth parameter that restricts the level of maximum
    nesting.

    To continue the process, call JSON_parser_char for each character in the
    JSON text, and then call JSON_parser_done to obtain the final result.
    These functions are fully reentrant.
*/

    int use_std_malloc = false;
    JSON_config default_config;
    JSON_parser jc;
    JSON_malloc_t alloc;

    /* set to default configuration if none was provided */
    if (NULL == config) {
        /* initialize configuration */
        init_JSON_config(&default_config);
        config = &default_config;
    }

    /* use std malloc if either the allocator or deallocator function isn't set */
    use_std_malloc = NULL == config->malloc || NULL == config->free;

    alloc = use_std_malloc ? malloc : config->malloc;

    jc = (JSON_parser)JSON_parser_malloc(alloc, sizeof(*jc), "parser");

    if (NULL == jc) {
        return NULL;
    }

    /* configure the parser */
    memset(jc, 0, sizeof(*jc));
    jc->malloc = alloc;
    jc->free = use_std_malloc ? free : config->free;
    jc->callback = config->callback;
    jc->ctx = config->callback_ctx;
    jc->allow_comments = (signed char)(config->allow_comments != 0);
    jc->handle_floats_manually = (signed char)(config->handle_floats_manually != 0);
    jc->decimal_point = *localeconv()->decimal_point;
    /* We need to be able to push at least one object */
    jc->depth = config->depth == 0 ? 1 : config->depth;

    /* reset the parser */
    if (!JSON_parser_reset(jc)) {
        jc->free(jc);
        return NULL;
    }

    return jc;
}

static int parse_buffer_grow(JSON_parser jc)
{
    const size_t bytes_to_copy = jc->parse_buffer_count * sizeof(jc->parse_buffer[0]);
    const size_t new_capacity = jc->parse_buffer_capacity * 2;
    const size_t bytes_to_allocate = new_capacity * sizeof(jc->parse_buffer[0]);
    void* mem = JSON_parser_malloc(jc->malloc, bytes_to_allocate, "parse buffer");

    if (mem == NULL) {
        jc->error = JSON_E_OUT_OF_MEMORY;
        return false;
    }

    assert(new_capacity > 0);
    memcpy(mem, jc->parse_buffer, bytes_to_copy);

    if (jc->parse_buffer != &jc->static_parse_buffer[0]) {
        jc->free(jc->parse_buffer);
    }

    jc->parse_buffer = (char*)mem;
    jc->parse_buffer_capacity = new_capacity;

    return true;
}

static int parse_buffer_reserve_for(JSON_parser jc, unsigned chars)
{
    while (jc->parse_buffer_count + chars + 1 > jc->parse_buffer_capacity) {
        if (!parse_buffer_grow(jc)) {
            assert(jc->error == JSON_E_OUT_OF_MEMORY);
            return false;
        }
    }

    return true;
}

#define parse_buffer_has_space_for(jc, count) \
    (jc->parse_buffer_count + (count) + 1 <= jc->parse_buffer_capacity)

#define parse_buffer_push_back_char(jc, c)\
    do {\
        assert(parse_buffer_has_space_for(jc, 1)); \
        jc->parse_buffer[jc->parse_buffer_count++] = c;\
        jc->parse_buffer[jc->parse_buffer_count]   = 0;\
    } while (0)

#define assert_is_non_container_type(jc) \
    assert( \
        jc->type == JSON_T_NULL || \
        jc->type == JSON_T_FALSE || \
        jc->type == JSON_T_TRUE || \
        jc->type == JSON_T_FLOAT || \
        jc->type == JSON_T_INTEGER || \
        jc->type == JSON_T_STRING)


static int parse_parse_buffer(JSON_parser jc)
{
    if (jc->callback) {
        JSON_value value, *arg = NULL;

        if (jc->type != JSON_T_NONE) {
            assert_is_non_container_type(jc);

            switch(jc->type) {
                case JSON_T_FLOAT:
                    arg = &value;
                    if (jc->handle_floats_manually) {
                        value.vu.str.value = jc->parse_buffer;
                        value.vu.str.length = jc->parse_buffer_count;
                    } else {
                        /* not checking with end pointer b/c there may be trailing ws */
                        value.vu.float_value = strtod(jc->parse_buffer, NULL);
                    }
                    break;
                case JSON_T_INTEGER:
                    arg = &value;
                    sscanf(jc->parse_buffer, JSON_PARSER_INTEGER_SSCANF_TOKEN, &value.vu.integer_value);
                    break;
                case JSON_T_STRING:
                    arg = &value;
                    value.vu.str.value = jc->parse_buffer;
                    value.vu.str.length = jc->parse_buffer_count;
                    break;
            }

            if (!(*jc->callback)(jc->ctx, jc->type, arg)) {
                return false;
            }
        }
    }

    parse_buffer_clear(jc);

    return true;
}

#define IS_HIGH_SURROGATE(uc) (((uc) & 0xFC00) == 0xD800)
#define IS_LOW_SURROGATE(uc)  (((uc) & 0xFC00) == 0xDC00)
#define DECODE_SURROGATE_PAIR(hi,lo) ((((hi) & 0x3FF) << 10) + ((lo) & 0x3FF) + 0x10000)
static const unsigned char utf8_lead_bits[4] = { 0x00, 0xC0, 0xE0, 0xF0 };

static int decode_unicode_char(JSON_parser jc)
{
    int i;
    unsigned uc = 0;
    char* p;
    int trail_bytes;

    assert(jc->parse_buffer_count >= 6);

    p = &jc->parse_buffer[jc->parse_buffer_count - 4];

    for (i = 12; i >= 0; i -= 4, ++p) {
        unsigned x = *p;

        if (x >= 'a') {
            x -= ('a' - 10);
        } else if (x >= 'A') {
            x -= ('A' - 10);
        } else {
            x &= ~0x30u;
        }

        assert(x < 16);

        uc |= x << i;
    }

    /* clear UTF-16 char from buffer */
    jc->parse_buffer_count -= 6;
    jc->parse_buffer[jc->parse_buffer_count] = 0;

    if (uc == 0xffff || uc == 0xfffe) {
        return false;
    }

    /* attempt decoding ... */
    if (jc->utf16_high_surrogate) {
        if (IS_LOW_SURROGATE(uc)) {
            uc = DECODE_SURROGATE_PAIR(jc->utf16_high_surrogate, uc);
            trail_bytes = 3;
            jc->utf16_high_surrogate = 0;
        } else {
            /* high surrogate without a following low surrogate */
            return false;
        }
    } else {
        if (uc < 0x80) {
            trail_bytes = 0;
        } else if (uc < 0x800) {
            trail_bytes = 1;
        } else if (IS_HIGH_SURROGATE(uc)) {
            /* save the high surrogate and wait for the low surrogate */
            jc->utf16_high_surrogate = (UTF16)uc;
            return true;
        } else if (IS_LOW_SURROGATE(uc)) {
            /* low surrogate without a preceding high surrogate */
            return false;
        } else {
            trail_bytes = 2;
        }
    }

    jc->parse_buffer[jc->parse_buffer_count++] = (char) ((uc >> (trail_bytes * 6)) | utf8_lead_bits[trail_bytes]);

    for (i = trail_bytes * 6 - 6; i >= 0; i -= 6) {
        jc->parse_buffer[jc->parse_buffer_count++] = (char) (((uc >> i) & 0x3F) | 0x80);
    }

    jc->parse_buffer[jc->parse_buffer_count] = 0;

    return true;
}

static int add_escaped_char_to_parse_buffer(JSON_parser jc, int next_char)
{
    assert(parse_buffer_has_space_for(jc, 1));

    jc->escaped = 0;
    /* remove the backslash */
    parse_buffer_pop_back_char(jc);
    switch(next_char) {
        case 'b':
            parse_buffer_push_back_char(jc, '\b');
            break;
        case 'f':
            parse_buffer_push_back_char(jc, '\f');
            break;
        case 'n':
            parse_buffer_push_back_char(jc, '\n');
            break;
        case 'r':
            parse_buffer_push_back_char(jc, '\r');
            break;
        case 't':
            parse_buffer_push_back_char(jc, '\t');
            break;
        case '"':
            parse_buffer_push_back_char(jc, '"');
            break;
        case '\\':
            parse_buffer_push_back_char(jc, '\\');
            break;
        case '/':
            parse_buffer_push_back_char(jc, '/');
            break;
        case 'u':
            parse_buffer_push_back_char(jc, '\\');
            parse_buffer_push_back_char(jc, 'u');
            break;
        default:
            return false;
    }

    return true;
}

static int add_char_to_parse_buffer(JSON_parser jc, int next_char, int next_class)
{
    if (!parse_buffer_reserve_for(jc, 1)) {
        assert(JSON_E_OUT_OF_MEMORY == jc->error);
        return false;
    }

    if (jc->escaped) {
        if (!add_escaped_char_to_parse_buffer(jc, next_char)) {
            jc->error = JSON_E_INVALID_ESCAPE_SEQUENCE;
            return false;
        }
    } else if (!jc->comment) {
        if ((jc->type != JSON_T_NONE) | !((next_class == C_SPACE) | (next_class == C_WHITE)) /* non-white-space */) {
            parse_buffer_push_back_char(jc, (char)next_char);
        }
    }

    return true;
}

#define assert_type_isnt_string_null_or_bool(jc) \
    assert(jc->type != JSON_T_FALSE); \
    assert(jc->type != JSON_T_TRUE); \
    assert(jc->type != JSON_T_NULL); \
    assert(jc->type != JSON_T_STRING)


int
JSON_parser_char(JSON_parser jc, int next_char)
{
/*
    After calling new_JSON_parser, call this function for each character (or
    partial character) in your JSON text. It can accept UTF-8, UTF-16, or
    UTF-32. It returns true if things are looking ok so far. If it rejects the
    text, it returns false.
*/
    int next_class, next_state;

/*
    Store the current char for error handling
*/
    jc->current_char = next_char;

/*
    Determine the character's class.
*/
    if (next_char < 0) {
        jc->error = JSON_E_INVALID_CHAR;
        return false;
    }
    if (next_char >= 128) {
        next_class = C_ETC;
    } else {
        next_class = ascii_class[next_char];
        if (next_class <= __) {
            set_error(jc);
            return false;
        }
    }

    if (!add_char_to_parse_buffer(jc, next_char, next_class)) {
        return false;
    }

/*
    Get the next state from the state transition table.
*/
    next_state = state_transition_table[jc->state][next_class];
    if (next_state >= 0) {
/*
    Change the state.
*/
        jc->state = (signed char)next_state;
    } else {
/*
    Or perform one of the actions.
*/
        switch (next_state) {
/* Unicode character */
        case UC:
            if(!decode_unicode_char(jc)) {
                jc->error = JSON_E_INVALID_UNICODE_SEQUENCE;
                return false;
            }
            /* check if we need to read a second UTF-16 char */
            if (jc->utf16_high_surrogate) {
                jc->state = D1;
            } else {
                jc->state = ST;
            }
            break;
/* escaped char */
        case EX:
            jc->escaped = 1;
            jc->state = ES;
            break;
/* integer detected by minus */
        case MX:
            jc->type = JSON_T_INTEGER;
            jc->state = MI;
            break;
/* integer detected by zero */
        case ZX:
            jc->type = JSON_T_INTEGER;
            jc->state = ZE;
            break;
/* integer detected by 1-9 */
        case IX:
            jc->type = JSON_T_INTEGER;
            jc->state = IT;
            break;

/* floating point number detected by exponent*/
        case DE:
            assert_type_isnt_string_null_or_bool(jc);
            jc->type = JSON_T_FLOAT;
            jc->state = E1;
            break;

/* floating point number detected by fraction */
        case DF:
            assert_type_isnt_string_null_or_bool(jc);
            if (!jc->handle_floats_manually) {
/*
    Some versions of strtod (which underlies sscanf) don't support converting
    C-locale formated floating point values.
*/
                assert(jc->parse_buffer[jc->parse_buffer_count-1] == '.');
                jc->parse_buffer[jc->parse_buffer_count-1] = jc->decimal_point;
            }
            jc->type = JSON_T_FLOAT;
            jc->state = FX;
            break;
/* string begin " */
        case SB:
            parse_buffer_clear(jc);
            assert(jc->type == JSON_T_NONE);
            jc->type = JSON_T_STRING;
            jc->state = ST;
            break;

/* n */
        case NU:
            assert(jc->type == JSON_T_NONE);
            jc->type = JSON_T_NULL;
            jc->state = N1;
            break;
/* f */
        case FA:
            assert(jc->type == JSON_T_NONE);
            jc->type = JSON_T_FALSE;
            jc->state = F1;
            break;
/* t */
        case TR:
            assert(jc->type == JSON_T_NONE);
            jc->type = JSON_T_TRUE;
            jc->state = T1;
            break;

/* closing comment */
        case CE:
            jc->comment = 0;
            assert(jc->parse_buffer_count == 0);
            assert(jc->type == JSON_T_NONE);
            jc->state = jc->before_comment_state;
            break;

/* opening comment  */
        case CB:
            if (!jc->allow_comments) {
                return false;
            }
            parse_buffer_pop_back_char(jc);
            if (!parse_parse_buffer(jc)) {
                return false;
            }
            assert(jc->parse_buffer_count == 0);
            assert(jc->type != JSON_T_STRING);
            switch (jc->stack[jc->top]) {
            case MODE_ARRAY:
            case MODE_OBJECT:
                switch(jc->state) {
                case VA:
                case AR:
                    jc->before_comment_state = jc->state;
                    break;
                default:
                    jc->before_comment_state = OK;
                    break;
                }
                break;
            default:
                jc->before_comment_state = jc->state;
                break;
            }
            jc->type = JSON_T_NONE;
            jc->state = C1;
            jc->comment = 1;
            break;
/* empty } */
        case -9:
            parse_buffer_clear(jc);
            if (jc->callback && !(*jc->callback)(jc->ctx, JSON_T_OBJECT_END, NULL)) {
                return false;
            }
            if (!pop(jc, MODE_KEY)) {
                return false;
            }
            jc->state = OK;
            break;

/* } */ case -8:
            parse_buffer_pop_back_char(jc);
            if (!parse_parse_buffer(jc)) {
                return false;
            }
            if (jc->callback && !(*jc->callback)(jc->ctx, JSON_T_OBJECT_END, NULL)) {
                return false;
            }
            if (!pop(jc, MODE_OBJECT)) {
                jc->error = JSON_E_UNBALANCED_COLLECTION;
                return false;
            }
            jc->type = JSON_T_NONE;
            jc->state = OK;
            break;

/* ] */ case -7:
            parse_buffer_pop_back_char(jc);
            if (!parse_parse_buffer(jc)) {
                return false;
            }
            if (jc->callback && !(*jc->callback)(jc->ctx, JSON_T_ARRAY_END, NULL)) {
                return false;
            }
            if (!pop(jc, MODE_ARRAY)) {
                jc->error = JSON_E_UNBALANCED_COLLECTION;
                return false;
            }

            jc->type = JSON_T_NONE;
            jc->state = OK;
            break;

/* { */ case -6:
            parse_buffer_pop_back_char(jc);
            if (jc->callback && !(*jc->callback)(jc->ctx, JSON_T_OBJECT_BEGIN, NULL)) {
                return false;
            }
            if (!push(jc, MODE_KEY)) {
                return false;
            }
            assert(jc->type == JSON_T_NONE);
            jc->state = OB;
            break;

/* [ */ case -5:
            parse_buffer_pop_back_char(jc);
            if (jc->callback && !(*jc->callback)(jc->ctx, JSON_T_ARRAY_BEGIN, NULL)) {
                return false;
            }
            if (!push(jc, MODE_ARRAY)) {
                return false;
            }
            assert(jc->type == JSON_T_NONE);
            jc->state = AR;
            break;

/* string end " */ case -4:
            parse_buffer_pop_back_char(jc);
            switch (jc->stack[jc->top]) {
            case MODE_KEY:
                assert(jc->type == JSON_T_STRING);
                jc->type = JSON_T_NONE;
                jc->state = CO;

                if (jc->callback) {
                    JSON_value value;
                    value.vu.str.value = jc->parse_buffer;
                    value.vu.str.length = jc->parse_buffer_count;
                    if (!(*jc->callback)(jc->ctx, JSON_T_KEY, &value)) {
                        return false;
                    }
                }
                parse_buffer_clear(jc);
                break;
            case MODE_ARRAY:
            case MODE_OBJECT:
                assert(jc->type == JSON_T_STRING);
                if (!parse_parse_buffer(jc)) {
                    return false;
                }
                jc->type = JSON_T_NONE;
                jc->state = OK;
                break;
            default:
                return false;
            }
            break;

/* , */ case -3:
            parse_buffer_pop_back_char(jc);
            if (!parse_parse_buffer(jc)) {
                return false;
            }
            switch (jc->stack[jc->top]) {
            case MODE_OBJECT:
/*
    A comma causes a flip from object mode to key mode.
*/
                if (!pop(jc, MODE_OBJECT) || !push(jc, MODE_KEY)) {
                    return false;
                }
                assert(jc->type != JSON_T_STRING);
                jc->type = JSON_T_NONE;
                jc->state = KE;
                break;
            case MODE_ARRAY:
                assert(jc->type != JSON_T_STRING);
                jc->type = JSON_T_NONE;
                jc->state = VA;
                break;
            default:
                return false;
            }
            break;

/* : */ case -2:
/*
    A colon causes a flip from key mode to object mode.
*/
            parse_buffer_pop_back_char(jc);
            if (!pop(jc, MODE_KEY) || !push(jc, MODE_OBJECT)) {
                return false;
            }
            assert(jc->type == JSON_T_NONE);
            jc->state = VA;
            break;
/*
    Bad action.
*/
        default:
            set_error(jc);
            return false;
        }
    }
    return true;
}

int
JSON_parser_done(JSON_parser jc)
{
    if ((jc->state == OK || jc->state == GO) && pop(jc, MODE_DONE))
    {
        return true;
    }

    jc->error = JSON_E_UNBALANCED_COLLECTION;
    return false;
}


int JSON_parser_is_legal_white_space_string(const char* s)
{
    int c, char_class;

    if (s == NULL) {
        return false;
    }

    for (; *s; ++s) {
        c = *s;

        if (c < 0 || c >= 128) {
            return false;
        }

        char_class = ascii_class[c];

        if (char_class != C_SPACE && char_class != C_WHITE) {
            return false;
        }
    }

    return true;
}

int JSON_parser_get_last_error(JSON_parser jc)
{
    return jc->error;
}


void init_JSON_config(JSON_config* config)
{
    if (config) {
        memset(config, 0, sizeof(*config));

        config->depth = JSON_PARSER_STACK_SIZE - 1;
        config->malloc = malloc;
        config->free = free;
    }
}

/* end file parser/JSON_parser.c */
/* begin file ./cson.c */
#include <assert.h>
#include <stdlib.h> /* malloc()/free() */
#include <string.h>
#include <errno.h>

#ifdef _MSC_VER
#   if _MSC_VER >= 1400 /* Visual Studio 2005 and up */
#     pragma warning( push )
#     pragma warning(disable:4996) /* unsecure sscanf (but snscanf() isn't in c89) */
#     pragma warning(disable:4244) /* complaining about data loss due
                                      to integer precision in the
                                      sqlite3 utf decoding routines */
#   endif
#endif

#if 1
#include <stdio.h>
#define MARKER if(1) printf("MARKER: %s:%d:%s():\t",__FILE__,__LINE__,__func__); if(1) printf
#else
static void noop_printf(char const * fmt, ...) {}
#define MARKER if(0) printf
#endif

#if defined(__cplusplus)
extern "C" {
#endif


    
/**
   This type holds the "vtbl" for type-specific operations when
   working with cson_value objects.

   All cson_values of a given logical type share a pointer to a single
   library-internal instance of this class.
*/
struct cson_value_api
{
    /**
       The logical JavaScript/JSON type associated with
       this object.
     */
    const cson_type_id typeID;
    /**
       Must free any memory associated with self,
       but not free self. If self is NULL then
       this function must do nothing.
    */
    void (*cleanup)( cson_value * self );
    /**
       POSSIBLE TODOs:

       // Deep copy.
       int (*clone)( cson_value const * self, cson_value ** tgt );

       // Using JS semantics for true/value
       char (*bool_value)( cson_value const * self );

       // memcmp() return value semantics
       int (*compare)( cson_value const * self, cson_value const * other );
     */
};

typedef struct cson_value_api cson_value_api;

/**
   Empty-initialized cson_value_api object.
*/
#define cson_value_api_empty_m {           \
        CSON_TYPE_UNDEF/*typeID*/,         \
        NULL/*cleanup*/\
      }
/**
   Empty-initialized cson_value_api object.
*/
static const cson_value_api cson_value_api_empty = cson_value_api_empty_m;


typedef unsigned int cson_counter_t;
struct cson_value
{
    /** The "vtbl" of type-specific operations. All instances
        of a given logical value type share a single api instance.

        Results are undefined if this value is NULL.
    */
    cson_value_api const * api;

    /** The raw value. Its interpretation depends on the value of the
        api member. Some value types require dynamically-allocated
        memory, so one must always call cson_value_free() to destroy a
        value when it is no longer needed. For stack-allocated values
        (which client could SHOULD NOT USE unless they are intimately
        familiar with the memory management rules and don't mind an
        occasional leak or crash), use cson_value_clean() instead of
        cson_value_free().
    */
    void * value;

    /**
       We use this to allow us to store cson_value instances in
       multiple containers or multiple times within a single container
       (provided no cycles are introduced).

       Notes about the rc implementation:

       - The refcount is for the cson_value instance itself, not its
       value pointer.

       - Instances start out with a refcount of 0 (not 1). Adding them
       to a container will increase the refcount. Cleaning up the container
       will decrement the count.

       - cson_value_free() decrements the refcount (if it is not already
       0) and cleans/frees the value only when the refcount is 0.

       - Some places in the internals add an "extra" reference to
       objects to avoid a premature deletion. Don't try this at home.
    */
    cson_counter_t refcount;
};


/**
   Empty-initialized cson_value object.
*/
#define cson_value_empty_m { &cson_value_api_empty/*api*/, NULL/*value*/, 0/*refcount*/ }
/**
   Empty-initialized cson_value object.
*/
static const cson_value cson_value_empty = cson_value_empty_m;
const cson_parse_opt cson_parse_opt_empty = cson_parse_opt_empty_m;
const cson_output_opt cson_output_opt_empty = cson_output_opt_empty_m;
const cson_object_iterator cson_object_iterator_empty = cson_object_iterator_empty_m;
const cson_buffer cson_buffer_empty = cson_buffer_empty_m;
const cson_parse_info cson_parse_info_empty = cson_parse_info_empty_m;

static void cson_value_destroy_zero_it( cson_value * self );
static void cson_value_destroy_object( cson_value * self );
/**
   If self is-a array then this function destroys its contents,
   else this function does nothing.
*/
static void cson_value_destroy_array( cson_value * self );

static const cson_value_api cson_value_api_null = { CSON_TYPE_NULL, cson_value_destroy_zero_it };
static const cson_value_api cson_value_api_undef = { CSON_TYPE_UNDEF, cson_value_destroy_zero_it };
static const cson_value_api cson_value_api_bool = { CSON_TYPE_BOOL, cson_value_destroy_zero_it };
static const cson_value_api cson_value_api_integer = { CSON_TYPE_INTEGER, cson_value_destroy_zero_it };
static const cson_value_api cson_value_api_double = { CSON_TYPE_DOUBLE, cson_value_destroy_zero_it };
static const cson_value_api cson_value_api_string = { CSON_TYPE_STRING, cson_value_destroy_zero_it };
static const cson_value_api cson_value_api_array = { CSON_TYPE_ARRAY, cson_value_destroy_array };
static const cson_value_api cson_value_api_object = { CSON_TYPE_OBJECT, cson_value_destroy_object };

static const cson_value cson_value_undef = { &cson_value_api_undef, NULL, 0 };
static const cson_value cson_value_null_empty = { &cson_value_api_null, NULL, 0 };
static const cson_value cson_value_bool_empty = { &cson_value_api_bool, NULL, 0 };
static const cson_value cson_value_integer_empty = { &cson_value_api_integer, NULL, 0 };
static const cson_value cson_value_double_empty = { &cson_value_api_double, NULL, 0 };
static const cson_value cson_value_string_empty = { &cson_value_api_string, NULL, 0 };
static const cson_value cson_value_array_empty = { &cson_value_api_array, NULL, 0 };
static const cson_value cson_value_object_empty = { &cson_value_api_object, NULL, 0 };

/**
   Strings are allocated as an instances of this class with N+1
   trailing bytes, where N is the length of the string being
   allocated. To convert a cson_string to c-string we simply increment
   the cson_string pointer. To do the opposite we use (cstr -
   sizeof(cson_string)). Zero-length strings are a special case
   handled by a couple of the cson_string functions.
*/
struct cson_string
{
    unsigned int length;
};
#define cson_string_empty_m {0/*length*/}
static const cson_string cson_string_empty = cson_string_empty_m;


/**
   Assumes V is a (cson_value*) ans V->value is a (T*). Returns
   V->value cast to a (T*).
*/
#define CSON_CAST(T,V) ((T*)((V)->value))
/**
   Assumes V is a pointer to memory which is allocated as part of a
   cson_value instance (the bytes immediately after that part).
   Returns a pointer a a cson_value by subtracting sizeof(cson_value)
   from that address and casting it to a (cson_value*)
*/
#define CSON_VCAST(V) ((cson_value *)(((unsigned char *)(V))-sizeof(cson_value)))

/**
   CSON_INT(V) assumes that V is a (cson_value*) of type
   CSON_TYPE_INTEGER. This macro returns a (cson_int_t*) representing
   its value (how that is stored depends on whether we are running in
   32- or 64-bit mode).
 */
#if CSON_VOID_PTR_IS_BIG
#  define CSON_INT(V) ((cson_int_t*)(&((V)->value)))
#else
#  define CSON_INT(V) ((cson_int_t*)(V)->value)
#endif

#define CSON_DBL(V) CSON_CAST(cson_double_t,(V))
#define CSON_STR(V) CSON_CAST(cson_string,(V))
#define CSON_OBJ(V) CSON_CAST(cson_object,(V))
#define CSON_ARRAY(V) CSON_CAST(cson_array,(V))

/**
 Holds special shared "constant" (though they are non-const)
 values. 
*/
static struct CSON_EMPTY_HOLDER_
{
    char trueValue;
    cson_string stringValue;
} CSON_EMPTY_HOLDER = {
    1/*trueValue*/,
    cson_string_empty_m
};

/**
    Indexes into the CSON_SPECIAL_VALUES array.
    
    If this enum changes in any way,
    makes damned sure that CSON_SPECIAL_VALUES is updated
    to match!!!
*/
enum CSON_INTERNAL_VALUES {
    
    CSON_VAL_UNDEF = 0,
    CSON_VAL_NULL = 1,
    CSON_VAL_TRUE = 2,
    CSON_VAL_FALSE = 3,
    CSON_VAL_INT_0 = 4,
    CSON_VAL_DBL_0 = 5,
    CSON_VAL_STR_EMPTY = 6,
    CSON_INTERNAL_VALUES_LENGTH
};

/**
  Some "special" shared cson_value instances.

  These values MUST be initialized in the order specified
  by the CSON_INTERNAL_VALUES enum.
   
  Note that they are not const because they are used as
  shared-allocation objects in non-const contexts. However, the
  public API provides no way to modifying them, and clients who
  modify values directly are subject to The Wrath of Undefined
  Behaviour.
*/
static cson_value CSON_SPECIAL_VALUES[] = {
{ &cson_value_api_undef, NULL, 0 }, /* UNDEF */
{ &cson_value_api_null, NULL, 0 }, /* NULL */
{ &cson_value_api_bool, &CSON_EMPTY_HOLDER.trueValue, 0 }, /* TRUE */
{ &cson_value_api_bool, NULL, 0 }, /* FALSE */
{ &cson_value_api_integer, NULL, 0 }, /* INT_0 */
{ &cson_value_api_double, NULL, 0 }, /* DBL_0 */
{ &cson_value_api_string, &CSON_EMPTY_HOLDER.stringValue, 0 }, /* STR_EMPTY */
{ NULL, NULL, 0 }
};


/**
    Returns non-0 (true) if m is one of our special
    "built-in" values, e.g. from CSON_SPECIAL_VALUES and some
    "empty" values.
     
    If this returns true, m MUST NOT be free()d!
 */
static char cson_value_is_builtin( void const * m )
{
    if((m >= (void const *)&CSON_EMPTY_HOLDER)
        && ( m < (void const *)(&CSON_EMPTY_HOLDER+1)))
        return 1;
    else return
        ((m >= (void const *)&CSON_SPECIAL_VALUES[0])
        && ( m < (void const *)&CSON_SPECIAL_VALUES[CSON_INTERNAL_VALUES_LENGTH]) )
        ? 1
        : 0;
}

char const * cson_rc_string(int rc)
{
    if(0 == rc) return "OK";
#define CHECK(N) else if(cson_rc.N == rc ) return #N
    CHECK(OK);
    CHECK(ArgError);
    CHECK(RangeError);
    CHECK(TypeError);
    CHECK(IOError);
    CHECK(AllocError);
    CHECK(NYIError);
    CHECK(InternalError);
    CHECK(UnsupportedError);
    CHECK(NotFoundError);
    CHECK(UnknownError);
    CHECK(Parse_INVALID_CHAR);
    CHECK(Parse_INVALID_KEYWORD);
    CHECK(Parse_INVALID_ESCAPE_SEQUENCE);
    CHECK(Parse_INVALID_UNICODE_SEQUENCE);
    CHECK(Parse_INVALID_NUMBER);
    CHECK(Parse_NESTING_DEPTH_REACHED);
    CHECK(Parse_UNBALANCED_COLLECTION);
    CHECK(Parse_EXPECTED_KEY);
    CHECK(Parse_EXPECTED_COLON);
    else return "UnknownError";
#undef CHECK
}

/**
   If CSON_LOG_ALLOC is true then the cson_malloc/realloc/free() routines
   will log a message to stderr.
*/
#define CSON_LOG_ALLOC 0


/**
   CSON_FOSSIL_MODE is only for use in the Fossil
   source tree, so that we can plug in to its allocators.
   We can't do this by, e.g., defining macros for the
   malloc/free funcs because fossil's lack of header files
   means we would have to #include "main.c" here to
   get the declarations.
 */
#if defined(CSON_FOSSIL_MODE)
extern void *fossil_malloc(size_t n);
extern void fossil_free(void *p);
extern void *fossil_realloc(void *p, size_t n);
#  define CSON_MALLOC_IMPL fossil_malloc
#  define CSON_FREE_IMPL fossil_free
#  define CSON_REALLOC_IMPL fossil_realloc
#endif

#if !defined CSON_MALLOC_IMPL
#  define CSON_MALLOC_IMPL malloc
#endif
#if !defined CSON_FREE_IMPL
#  define CSON_FREE_IMPL free
#endif
#if !defined CSON_REALLOC_IMPL
#  define CSON_REALLOC_IMPL realloc
#endif

/**
   A test/debug macro for simulating an OOM after the given number of
   bytes have been allocated.
*/
#define CSON_SIMULATE_OOM 0
#if CSON_SIMULATE_OOM
static unsigned int cson_totalAlloced = 0;
#endif

/** Simple proxy for malloc(). descr is a description of the allocation. */
static void * cson_malloc( size_t n, char const * descr )
{
#if CSON_LOG_ALLOC
    fprintf(stderr, "Allocating %u bytes [%s].\n", (unsigned int)n, descr);
#endif
#if CSON_SIMULATE_OOM
    cson_totalAlloced += n;
    if( cson_totalAlloced > CSON_SIMULATE_OOM )
    {
        return NULL;
    }
#endif
    return CSON_MALLOC_IMPL(n);
}

/** Simple proxy for free(). descr is a description of the memory being freed. */
static void cson_free( void * p, char const * descr )
{
#if CSON_LOG_ALLOC
    fprintf(stderr, "Freeing @%p [%s].\n", p, descr);
#endif
    if( !cson_value_is_builtin(p) )
    {
        CSON_FREE_IMPL( p );
    }
}
/** Simple proxy for realloc(). descr is a description of the (re)allocation. */
static void * cson_realloc( void * hint, size_t n, char const * descr )
{
#if CSON_LOG_ALLOC
    fprintf(stderr, "%sllocating %u bytes [%s].\n",
            hint ? "Rea" : "A",
            (unsigned int)n, descr);
#endif
#if CSON_SIMULATE_OOM
    cson_totalAlloced += n;
    if( cson_totalAlloced > CSON_SIMULATE_OOM )
    {
        return NULL;
    }
#endif
    if( 0==n )
    {
         cson_free(hint, descr);
         return NULL;
    }
    else
    {
        return CSON_REALLOC_IMPL( hint, n );
    }
}


#undef CSON_LOG_ALLOC
#undef CSON_SIMULATE_OOM



/**
   CLIENTS CODE SHOULD NEVER USE THIS because it opens up doors to
   memory leaks if it is not used in very controlled circumstances.
   Users must be very aware of how the underlying memory management
   works.

   Frees any resources owned by val, but does not free val itself
   (which may be stack-allocated). If !val or val->api or
   val->api->cleanup are NULL then this is a no-op.

   If v is a container type (object or array) its children are also
   cleaned up, recursively.

   After calling this, val will have the special "undefined" type.
*/
static void cson_value_clean( cson_value * val );

/**
   Increments cv's reference count by 1.  As a special case, values
   for which cson_value_is_builtin() returns true are not
   modified. assert()s if (NULL==cv).
*/
static void cson_refcount_incr( cson_value * cv )
{
    assert( NULL != cv );
    if( cson_value_is_builtin( cv ) )
    { /* do nothing: we do not want to modify the shared
         instances.
      */
        return;
    }
    else
    {
        ++cv->refcount;
    }
}

#if 0
int cson_value_refcount_set( cson_value * cv, unsigned short rc )
{
    if( NULL == cv ) return cson_rc.ArgError;
    else
    {
        cv->refcount = rc;
        return 0;
    }
}
#endif

int cson_value_add_reference( cson_value * cv )
{
    if( NULL == cv ) return cson_rc.ArgError;
    else if( (cv->refcount+1) < cv->refcount )
    {
        return cson_rc.RangeError;
    }
    else
    {
        cson_refcount_incr( cv );
        return 0;
    }
}

/**
   If cv is NULL or cson_value_is_builtin(cv) returns true then this
   function does nothing and returns 0, otherwise...  If
   cv->refcount is 0 or 1 then cson_value_clean(cv) is called, cv is
   freed, and 0 is returned. If cv->refcount is any other value then
   it is decremented and the new value is returned.
*/
static cson_counter_t cson_refcount_decr( cson_value * cv )
{
    if( (NULL == cv) || cson_value_is_builtin(cv) ) return 0;
    else if( (0 == cv->refcount) || (0 == --cv->refcount) )
    {
        cson_value_clean(cv);
        cson_free(cv,"cson_value::refcount=0");
        return 0;
    }
    else return cv->refcount;
}

unsigned int cson_string_length_bytes( cson_string const * str )
{
    return str ? str->length : 0;
}


/**
   Fetches v's string value as a non-const string.

   cson_strings are intended to be immutable, but this form provides
   access to the immutable bits, which are v->length bytes long. A
   length-0 string is returned as NULL from here, as opposed to
   "". (This is a side-effect of the string allocation mechanism.)
   Returns NULL if !v or if v is the internal empty-string singleton.
*/
static char * cson_string_str(cson_string *v)
{
    /*
      See http://groups.google.com/group/comp.lang.c.moderated/browse_thread/thread/2e0c0df5e8a0cd6a
    */
#if 1
    if( !v || (&CSON_EMPTY_HOLDER.stringValue == v) ) return NULL;
    else return (char *)((unsigned char *)( v+1 ));
#else
    static char empty[2] = {0,0};
    return ( NULL == v )
        ? NULL
        : (v->length
           ? (char *) (((unsigned char *)v) + sizeof(cson_string))
           : empty)
        ;
#endif
}

/**
   Fetches v's string value as a const string.
*/
char const * cson_string_cstr(cson_string const *v)
{
    /*
      See http://groups.google.com/group/comp.lang.c.moderated/browse_thread/thread/2e0c0df5e8a0cd6a
    */
#if 1
    if( ! v ) return NULL;
    else if( v == &CSON_EMPTY_HOLDER.stringValue ) return "";
    else {
        assert((0 < v->length) && "How do we have a non-singleton empty string?");
        return (char const *)((unsigned char const *)(v+1));
    }
#else
    return (NULL == v)
        ? NULL
        : (v->length
           ? (char const *) ((unsigned char const *)(v+1))
           : "");
#endif
}


#if 0
/**
   Just like strndup(3), in that neither are C89/C99-standard and both
   are documented in detail in strndup(3).
*/
static char * cson_strdup( char const * src, size_t n )
{
    char * rc = (char *)cson_malloc(n+1, "cson_strdup");
    if( ! rc ) return NULL;
    memset( rc, 0, n+1 );
    rc[n] = 0;
    return strncpy( rc, src, n );
}
#endif

int cson_string_cmp_cstr_n( cson_string const * str, char const * other, unsigned int otherLen )
{
    if( ! other && !str ) return 0;
    else if( other && !str ) return 1;
    else if( str && !other ) return -1;
    else if( !otherLen ) return  str->length ? 1 : 0;
    else if( !str->length ) return otherLen ? -1 : 0;
    else
    {
        unsigned const int max = (otherLen > str->length) ? otherLen : str->length;
        int const rc = strncmp( cson_string_cstr(str), other, max );
        return ( (0 == rc) && (otherLen != str->length) )
            ? (str->length < otherLen) ? -1 : 1
            : rc;
    }
}

int cson_string_cmp_cstr( cson_string const * lhs, char const * rhs )
{
    return cson_string_cmp_cstr_n( lhs, rhs, (rhs&&*rhs) ? strlen(rhs) : 0 );
}
int cson_string_cmp( cson_string const * lhs, cson_string const * rhs )
{
    return cson_string_cmp_cstr_n( lhs, cson_string_cstr(rhs), rhs ? rhs->length : 0 );
}


/**
   If self is not NULL, *self is overwritten to have the undefined
   type. self is not cleaned up or freed.
*/
void cson_value_destroy_zero_it( cson_value * self )
{
    if( self )
    {
        *self = cson_value_undef;
    }
}

/**
   A key/value pair collection.

   Each of these objects owns its key/value pointers, and they
   are cleaned up by cson_kvp_clean().
*/
struct cson_kvp
{
    cson_value * key;
    cson_value * value;
};
#define cson_kvp_empty_m {NULL,NULL}
static const cson_kvp cson_kvp_empty = cson_kvp_empty_m;

/** @def CSON_OBJECT_PROPS_SORT

    Don't use this - it has not been updated to account for internal
    changes in cson_object.

   If CSON_OBJECT_PROPS_SORT is set to a true value then
   qsort() and bsearch() are used to sort (upon insertion)
   and search cson_object::kvp property lists. This costs us
   a re-sort on each insertion but searching is O(log n)
   average/worst case (and O(1) best-case).

   i'm not yet convinced that the overhead of the qsort() justifies
   the potentially decreased search times - it has not been
   measured. Object property lists tend to be relatively short in
   JSON, and a linear search which uses the cson_string::length
   property as a quick check is quite fast when one compares it with
   the sort overhead required by the bsearch() approach.
*/
#define CSON_OBJECT_PROPS_SORT 0

/** @def CSON_OBJECT_PROPS_SORT_USE_LENGTH

    Don't use this - i'm not sure that it works how i'd like.

    If CSON_OBJECT_PROPS_SORT_USE_LENGTH is true then
    we use string lengths as quick checks when sorting
    property keys. This leads to a non-intuitive sorting
    order but "should" be faster.

    This is ignored if CSON_OBJECT_PROPS_SORT is false.

*/
#define CSON_OBJECT_PROPS_SORT_USE_LENGTH 0

#if CSON_OBJECT_PROPS_SORT

/**
   cson_kvp comparator for use with qsort(). ALMOST compares with
   strcmp() semantics, but it uses the strings' lengths as a quicker
   approach. This might give non-intuitive results, but it's faster.
 */
static int cson_kvp_cmp( void const * lhs, void const * rhs )
{
    cson_kvp const * lk = *((cson_kvp const * const*)lhs);
    cson_kvp const * rk = *((cson_kvp const * const*)rhs);
    cson_string const * l = cson_string_value(lk->key);
    cson_string const * r = cson_string_value(rk->key);
#if CSON_OBJECT_PROPS_SORT_USE_LENGTH
    if( l->length < r->length ) return -1;
    else if( l->length > r->length ) return 1;
    else return strcmp( cson_string_cstr( l ), cson_string_cstr( r ) );
#else
    return strcmp( cson_string_cstr( l ),
                   cson_string_cstr( r ) );
#endif /*CSON_OBJECT_PROPS_SORT_USE_LENGTH*/
}
#endif /*CSON_OBJECT_PROPS_SORT*/


#if CSON_OBJECT_PROPS_SORT
#error "Need to rework this for cson_string-to-cson_value refactoring"
/**
   A bsearch() comparison function which requires that lhs be a (char
   const *) and rhs be-a (cson_kvp const * const *). It compares lhs
   to rhs->key's value, using strcmp() semantics.
 */
static int cson_kvp_cmp_vs_cstr( void const * lhs, void const * rhs )
{
    char const * lk = (char const *)lhs;
    cson_kvp const * rk =
        *((cson_kvp const * const*)rhs)
        ;
#if CSON_OBJECT_PROPS_SORT_USE_LENGTH
    unsigned int llen = strlen(lk);
    if( llen < rk->key->length ) return -1;
    else if( llen > rk->key->length ) return 1;
    else return strcmp( lk, cson_string_cstr( rk->key ) );
#else
    return strcmp( lk, cson_string_cstr( rk->key ) );
#endif /*CSON_OBJECT_PROPS_SORT_USE_LENGTH*/
}
#endif /*CSON_OBJECT_PROPS_SORT*/


struct cson_kvp_list
{
    cson_kvp ** list;
    unsigned int count;
    unsigned int alloced;
};
typedef struct cson_kvp_list cson_kvp_list;
#define cson_kvp_list_empty_m {NULL/*list*/,0/*count*/,0/*alloced*/}
static const cson_kvp_list cson_kvp_list_empty = cson_kvp_list_empty_m;

struct cson_object
{
    cson_kvp_list kvp;
};
/*typedef struct cson_object cson_object;*/
#define cson_object_empty_m { cson_kvp_list_empty_m/*kvp*/ }
static const cson_object cson_object_empty = cson_object_empty_m;

struct cson_value_list
{
    cson_value ** list;
    unsigned int count;
    unsigned int alloced;
};
typedef struct cson_value_list cson_value_list;
#define cson_value_list_empty_m {NULL/*list*/,0/*count*/,0/*alloced*/}
static const cson_value_list cson_value_list_empty = cson_value_list_empty_m;

struct cson_array
{
    cson_value_list list;
};
/*typedef struct cson_array cson_array;*/
#define cson_array_empty_m { cson_value_list_empty_m/*list*/ }
static const cson_array cson_array_empty = cson_array_empty_m;


struct cson_parser
{
    JSON_parser p;
    cson_value * root;
    cson_value * node;
    cson_array stack;
    cson_string * ckey;
    int errNo;
    unsigned int totalKeyCount;
    unsigned int totalValueCount;
};
typedef struct cson_parser cson_parser;
static const cson_parser cson_parser_empty = {
NULL/*p*/,
NULL/*root*/,
NULL/*node*/,
cson_array_empty_m/*stack*/,
NULL/*ckey*/,
0/*errNo*/,
0/*totalKeyCount*/,
0/*totalValueCount*/
};

#if 1
/* The following funcs are declared in generated code (cson_lists.h),
   but we need early access to their decls for the Amalgamation build.
*/
static unsigned int cson_value_list_reserve( cson_value_list * self, unsigned int n );
static unsigned int cson_kvp_list_reserve( cson_kvp_list * self, unsigned int n );
static int cson_kvp_list_append( cson_kvp_list * self, cson_kvp * cp );
static void cson_kvp_list_clean( cson_kvp_list * self,
                                 void (*cleaner)(cson_kvp * obj) );
#if 0
static int cson_value_list_append( cson_value_list * self, cson_value * cp );
static void cson_value_list_clean( cson_value_list * self, void (*cleaner)(cson_value * obj));
static int cson_kvp_list_visit( cson_kvp_list * self,
                                int (*visitor)(cson_kvp * obj, void * visitorState ),
                                void * visitorState );
static int cson_value_list_visit( cson_value_list * self,
                                  int (*visitor)(cson_value * obj, void * visitorState ),
                                  void * visitorState );
#endif
#endif
    
#if 0
#  define LIST_T cson_value_list
#  define VALUE_T cson_value *
#  define VALUE_T_IS_PTR 1
#  define LIST_T cson_kvp_list
#  define VALUE_T cson_kvp *
#  define VALUE_T_IS_PTR 1
#else
#endif

/**
   Allocates a new value of the specified type. Ownership is
   transfered to the caller, who must eventually free it by passing it
   to cson_value_free() or transfering ownership to a container.

   extra is only valid for type CSON_TYPE_STRING, and must be the length
   of the string to allocate + 1 byte (for the NUL).

   The returned value->api member will be set appropriately and
   val->value will be set to point to the memory allocated to hold the
   native value type. Use the internal CSON_CAST() family of macros to
   convert the cson_values to their corresponding native
   representation.

   Returns NULL on allocation error.

   @see cson_value_new_array()
   @see cson_value_new_object()
   @see cson_value_new_string()
   @see cson_value_new_integer()
   @see cson_value_new_double()
   @see cson_value_new_bool()
   @see cson_value_free()
*/
static cson_value * cson_value_new(cson_type_id t, size_t extra)
{
    static const size_t vsz = sizeof(cson_value);
    const size_t sz = vsz + extra;
    size_t tx = 0;
    cson_value def = cson_value_undef;
    cson_value * v = NULL;
    char const * reason = "cson_value_new";
    switch(t)
    {
      case CSON_TYPE_ARRAY:
          assert( 0 == extra );
          def = cson_value_array_empty;
          tx = sizeof(cson_array);
          reason = "cson_value:array";
          break;
      case CSON_TYPE_DOUBLE:
          assert( 0 == extra );
          def = cson_value_double_empty;
          tx = sizeof(cson_double_t);
          reason = "cson_value:double";
          break;
      case CSON_TYPE_INTEGER:
          assert( 0 == extra );
          def = cson_value_integer_empty;
#if !CSON_VOID_PTR_IS_BIG
          tx = sizeof(cson_int_t);
#endif
          reason = "cson_value:int";
          break;
      case CSON_TYPE_STRING:
          assert( 0 != extra );
          def = cson_value_string_empty;
          tx = sizeof(cson_string);
          reason = "cson_value:string";
          break;
      case CSON_TYPE_OBJECT:
          assert( 0 == extra );
          def = cson_value_object_empty;
          tx = sizeof(cson_object);
          reason = "cson_value:object";
          break;
      default:
          assert(0 && "Unhandled type in cson_value_new()!");
          return NULL;
    }
    assert( def.api->typeID != CSON_TYPE_UNDEF );
    v = (cson_value *)cson_malloc(sz+tx, reason);
    if( v ) {
        *v = def;
        if(tx || extra){
            memset(v+1, 0, tx + extra);
            v->value = (void *)(v+1);
        }
    }
    return v;
}

void cson_value_free(cson_value *v)
{
    cson_refcount_decr( v );
}

#if 0 /* we might actually want this later on. */
/** Returns true if v is not NULL and has the given type ID. */
static char cson_value_is_a( cson_value const * v, cson_type_id is )
{
    return (v && v->api && (v->api->typeID == is)) ? 1 : 0;
}
#endif

cson_type_id cson_value_type_id( cson_value const * v )
{
    return (v && v->api) ? v->api->typeID : CSON_TYPE_UNDEF;
}

char cson_value_is_undef( cson_value const * v )
{
    return ( !v || !v->api || (v->api==&cson_value_api_undef))
        ? 1 : 0;
}
#define ISA(T,TID) char cson_value_is_##T( cson_value const * v ) {       \
        /*return (v && v->api) ? cson_value_is_a(v,CSON_TYPE_##TID) : 0;*/ \
        return (v && (v->api == &cson_value_api_##T)) ? 1 : 0; \
    } static const char bogusPlaceHolderForEmacsIndention##TID = CSON_TYPE_##TID
ISA(null,NULL);
ISA(bool,BOOL);
ISA(integer,INTEGER);
ISA(double,DOUBLE);
ISA(string,STRING);
ISA(array,ARRAY);
ISA(object,OBJECT);
#undef ISA
char cson_value_is_number( cson_value const * v )
{
    return cson_value_is_integer(v) || cson_value_is_double(v);
}


void cson_value_clean( cson_value * val )
{
    if( val && val->api && val->api->cleanup )
    {
        if( ! cson_value_is_builtin( val ) )
        {
            cson_counter_t const rc = val->refcount;
            val->api->cleanup(val);
            *val = cson_value_undef;
            val->refcount = rc;
        }
    }
}

static cson_value * cson_value_array_alloc()
{
    cson_value * v = cson_value_new(CSON_TYPE_ARRAY,0);
    if( NULL != v )
    {
        cson_array * ar = CSON_ARRAY(v);
        assert(NULL != ar);
        *ar = cson_array_empty;
    }
    return v;
}

static cson_value * cson_value_object_alloc()
{
    cson_value * v = cson_value_new(CSON_TYPE_OBJECT,0);
    if( NULL != v )
    {
        cson_object * obj = CSON_OBJ(v);
        assert(NULL != obj);
        *obj = cson_object_empty;
    }
    return v;
}

cson_value * cson_value_new_object()
{
    return cson_value_object_alloc();
}

cson_object * cson_new_object()
{
    
    return cson_value_get_object( cson_value_new_object() );
}

cson_value * cson_value_new_array()
{
    return cson_value_array_alloc();
}


cson_array * cson_new_array()
{
    return cson_value_get_array( cson_value_new_array() );
}

/**
   Frees kvp->key and kvp->value and sets them to NULL, but does not free
   kvp. If !kvp then this is a no-op.
*/
static void cson_kvp_clean( cson_kvp * kvp )
{
    if( kvp )
    {
        if(kvp->key)
        {
            cson_value_free(kvp->key);
            kvp->key = NULL;
        }
        if(kvp->value)
        {
            cson_value_free( kvp->value );
            kvp->value = NULL;
        }
    }
}

cson_string * cson_kvp_key( cson_kvp const * kvp )
{
    return kvp ? cson_value_get_string(kvp->key) : NULL;
}
cson_value * cson_kvp_value( cson_kvp const * kvp )
{
    return kvp ? kvp->value : NULL;
}


/**
   Calls cson_kvp_clean(kvp) and then frees kvp.
*/
static void cson_kvp_free( cson_kvp * kvp )
{
    if( kvp )
    {
        cson_kvp_clean(kvp);
        cson_free(kvp,"cson_kvp");
    }
}


/**
   cson_value_api::destroy_value() impl for Object
   values. Cleans up self-owned memory and overwrites
   self to have the undefined value, but does not
   free self.
*/
static void cson_value_destroy_object( cson_value * self )
{
    if(self && self->value) {
        cson_object * obj = (cson_object *)self->value;
        assert( self->value == obj );
        cson_kvp_list_clean( &obj->kvp, cson_kvp_free );
        *self = cson_value_undef;
    }
}

/**
   Cleans up the contents of ar->list, but does not free ar.

   After calling this, ar will have a length of 0.

   If properlyCleanValues is 1 then cson_value_free() is called on
   each non-NULL item, otherwise the outer list is destroyed but the
   individual items are assumed to be owned by someone else and are
   not freed.
*/
static void cson_array_clean( cson_array * ar, char properlyCleanValues )
{
    if( ar )
    {
        unsigned int i = 0;
        cson_value * val = NULL;
        for( ; i < ar->list.count; ++i )
        {
            val = ar->list.list[i];
            if(val)
            {
                ar->list.list[i] = NULL;
                if( properlyCleanValues )
                {
                    cson_value_free( val );
                }
            }
        }
        cson_value_list_reserve(&ar->list,0);
        ar->list = cson_value_list_empty
            /* Pedantic note: reserve(0) already clears the list-specific
               fields, but we do this just in case we ever add new fields
               to cson_value_list which are not used in the reserve() impl.
             */
            ;
    }
}

/**
   cson_value_api::destroy_value() impl for Array
   values. Cleans up self-owned memory and overwrites
   self to have the undefined value, but does not
   free self.
*/
static void cson_value_destroy_array( cson_value * self )
{
    cson_array * ar = cson_value_get_array(self);
    if(ar) {
        assert( self->value == ar );
        cson_array_clean( ar, 1 );
        *self = cson_value_undef;
    }
}

int cson_buffer_fill_from( cson_buffer * dest, cson_data_source_f src, void * state )
{
    int rc;
    enum { BufSize = 1024 * 4 };
    char rbuf[BufSize];
    size_t total = 0;
    unsigned int rlen = 0;
    if( ! dest || ! src ) return cson_rc.ArgError;
    dest->used = 0;
    while(1)
    {
        rlen = BufSize;
        rc = src( state, rbuf, &rlen );
        if( rc ) break;
        total += rlen;
        if( dest->capacity < (total+1) )
        {
            rc = cson_buffer_reserve( dest, total + 1);
            if( 0 != rc ) break;
        }
        memcpy( dest->mem + dest->used, rbuf, rlen );
        dest->used += rlen;
        if( rlen < BufSize ) break;
    }
    if( !rc && dest->used )
    {
        assert( dest->used < dest->capacity );
        dest->mem[dest->used] = 0;
    }
    return rc;
}

int cson_data_source_FILE( void * state, void * dest, unsigned int * n )
{
    FILE * f = (FILE*) state;
    if( ! state || ! n || !dest ) return cson_rc.ArgError;
    else if( !*n ) return cson_rc.RangeError;
    *n = (unsigned int)fread( dest, 1, *n, f );
    if( !*n )
    {
        return feof(f) ? 0 : cson_rc.IOError;
    }
    return 0;
}

int cson_parse_FILE( cson_value ** tgt, FILE * src,
                     cson_parse_opt const * opt, cson_parse_info * err )
{
    return cson_parse( tgt, cson_data_source_FILE, src, opt, err );
}


int cson_value_fetch_bool( cson_value const * val, char * v )
{
    /**
       FIXME: move the to-bool operation into cson_value_api, like we
       do in the C++ API.
     */
    if( ! val || !val->api ) return cson_rc.ArgError;
    else
    {
        int rc = 0;
        char b = 0;
        switch( val->api->typeID )
        {
          case CSON_TYPE_ARRAY:
          case CSON_TYPE_OBJECT:
              b = 1;
              break;
          case CSON_TYPE_STRING: {
              char const * str = cson_string_cstr(cson_value_get_string(val));
              b = (str && *str) ? 1 : 0;
              break;
          }
          case CSON_TYPE_UNDEF:
          case CSON_TYPE_NULL:
              break;
          case CSON_TYPE_BOOL:
              b = (NULL==val->value) ? 0 : 1;
              break;
          case CSON_TYPE_INTEGER: {
              cson_int_t i = 0;
              cson_value_fetch_integer( val, &i );
              b = i ? 1 : 0;
              break;
          }
          case CSON_TYPE_DOUBLE: {
              cson_double_t d = 0.0;
              cson_value_fetch_double( val, &d );
              b = (0.0==d) ? 0 : 1;
              break;
          }
          default:
              rc = cson_rc.TypeError;
              break;
        }
        if( v ) *v = b;
        return rc;
    }
}

char cson_value_get_bool( cson_value const * val )
{
    char i = 0;
    cson_value_fetch_bool( val, &i );
    return i;
}

int cson_value_fetch_integer( cson_value const * val, cson_int_t * v )
{
    if( ! val || !val->api ) return cson_rc.ArgError;
    else
    {
        cson_int_t i = 0;
        int rc = 0;
        switch(val->api->typeID)
        {
            case CSON_TYPE_UNDEF: 
            case CSON_TYPE_NULL:
              i = 0;
              break;
            case CSON_TYPE_BOOL: {
              char b = 0;
              cson_value_fetch_bool( val, &b );
              i = b;
              break;
            }
            case CSON_TYPE_INTEGER: {
                cson_int_t const * x = CSON_INT(val);
                if(!x)
                {
                    assert( val == &CSON_SPECIAL_VALUES[CSON_VAL_INT_0] );
                }
                i = x ? *x : 0;
                break;
            }
            case CSON_TYPE_DOUBLE: {
              cson_double_t d = 0.0;
              cson_value_fetch_double( val, &d );
              i = (cson_int_t)d;
              break;
            }
            case CSON_TYPE_STRING:
            case CSON_TYPE_ARRAY:
            case CSON_TYPE_OBJECT:
            default:
                rc = cson_rc.TypeError;
                break;
        }
        if(!rc && v) *v = i;
        return rc;
    }
}

cson_int_t cson_value_get_integer( cson_value const * val )
{
    cson_int_t i = 0;
    cson_value_fetch_integer( val, &i );
    return i;
}

int cson_value_fetch_double( cson_value const * val, cson_double_t * v )
{
    if( ! val || !val->api ) return cson_rc.ArgError;
    else
    {
        cson_double_t d = 0.0;
        int rc = 0;
        switch(val->api->typeID)
        {
          case CSON_TYPE_UNDEF: 
          case CSON_TYPE_NULL:
              d = 0;
              break;
          case CSON_TYPE_BOOL: {
              char b = 0;
              cson_value_fetch_bool( val, &b );
              d = b ? 1.0 : 0.0;
              break;
          }
          case CSON_TYPE_INTEGER: {
              cson_int_t i = 0;
              cson_value_fetch_integer( val, &i );
              d = i;
              break;
          }
          case CSON_TYPE_DOUBLE: {
              cson_double_t const* dv = CSON_DBL(val);
              d = dv ? *dv : 0.0;
              break;
          }
          default:
              rc = cson_rc.TypeError;
              break;
        }
        if(v) *v = d;
        return rc;
    }
}

cson_double_t cson_value_get_double( cson_value const * val )
{
    cson_double_t i = 0.0;
    cson_value_fetch_double( val, &i );
    return i;
}

int cson_value_fetch_string( cson_value const * val, cson_string ** dest )
{
    if( ! val || ! dest ) return cson_rc.ArgError;
    else if( ! cson_value_is_string(val) ) return cson_rc.TypeError;
    else
    {
        if( dest ) *dest = CSON_STR(val);
        return 0;
    }
}

cson_string * cson_value_get_string( cson_value const * val )
{
    cson_string * rc = NULL;
    cson_value_fetch_string( val, &rc );
    return rc;
}

char const * cson_value_get_cstr( cson_value const * val )
{
    return cson_string_cstr( cson_value_get_string(val) );
}

int cson_value_fetch_object( cson_value const * val, cson_object ** obj )
{
    if( ! val ) return cson_rc.ArgError;
    else if( ! cson_value_is_object(val) ) return cson_rc.TypeError;
    else
    {
        if(obj) *obj = CSON_OBJ(val);
        return 0;
    }
}
cson_object * cson_value_get_object( cson_value const * v )
{
    cson_object * obj = NULL;
    cson_value_fetch_object( v, &obj );
    return obj;
}

int cson_value_fetch_array( cson_value const * val, cson_array ** ar)
{
    if( ! val ) return cson_rc.ArgError;
    else if( !cson_value_is_array(val) ) return cson_rc.TypeError;
    else
    {
        if(ar) *ar = CSON_ARRAY(val);
        return 0;
    }
}

cson_array * cson_value_get_array( cson_value const * v )
{
    cson_array * ar = NULL;
    cson_value_fetch_array( v, &ar );
    return ar;
}

cson_kvp * cson_kvp_alloc()
{
    cson_kvp * kvp = (cson_kvp*)cson_malloc(sizeof(cson_kvp),"cson_kvp");
    if( kvp )
    {
        *kvp = cson_kvp_empty;
    }
    return kvp;
}



int cson_array_append( cson_array * ar, cson_value * v )
{
    if( !ar || !v ) return cson_rc.ArgError;
    else if( (ar->list.count+1) < ar->list.count ) return cson_rc.RangeError;
    else
    {
        if( !ar->list.alloced || (ar->list.count == ar->list.alloced-1))
        {
            unsigned int const n = ar->list.count ? (ar->list.count*2) : 7;
            if( n > cson_value_list_reserve( &ar->list, n ) )
            {
                return cson_rc.AllocError;
            }
        }
        return cson_array_set( ar, ar->list.count, v );
    }
}

#if 0
/**
   Removes and returns the last value from the given array,
   shrinking its size by 1. Returns NULL if ar is NULL,
   ar->list.count is 0, or the element at that index is NULL.
   

   If removeRef is true then cson_value_free() is called to remove
   ar's reference count for the value. In that case NULL is returned,
   even if the object still has live references. If removeRef is false
   then the caller takes over ownership of that reference count point.

   If removeRef is false then the caller takes over ownership
   of the return value, otherwise ownership is effectively
   determined by any remaining references for the returned
   value.
*/
static cson_value * cson_array_pop_back( cson_array * ar,
                                         char removeRef )
{
    if( !ar ) return NULL;
    else if( ! ar->list.count ) return NULL;
    else
    {
        unsigned int const ndx = --ar->list.count;
        cson_value * v = ar->list.list[ndx];
        ar->list.list[ndx] = NULL;
        if( removeRef )
        {
            cson_value_free( v );
            v = NULL;
        }
        return v;
    }
}
#endif

cson_value * cson_value_new_bool( char v )
{
    return v ? &CSON_SPECIAL_VALUES[CSON_VAL_TRUE] : &CSON_SPECIAL_VALUES[CSON_VAL_FALSE];
}

cson_value * cson_value_true()
{
    return &CSON_SPECIAL_VALUES[CSON_VAL_TRUE];
}
cson_value * cson_value_false()
{
    return &CSON_SPECIAL_VALUES[CSON_VAL_FALSE];
}

cson_value * cson_value_null()
{
    return &CSON_SPECIAL_VALUES[CSON_VAL_NULL];
}

cson_value * cson_new_int( cson_int_t v )
{
    return cson_value_new_integer(v);
}

cson_value * cson_value_new_integer( cson_int_t v )
{
    if( 0 == v ) return &CSON_SPECIAL_VALUES[CSON_VAL_INT_0];
    else
    {
        cson_value * c = cson_value_new(CSON_TYPE_INTEGER,0);
#if !defined(NDEBUG) && CSON_VOID_PTR_IS_BIG
        assert( sizeof(cson_int_t) <= sizeof(void *) );
#endif
        if( c )
        {
            *CSON_INT(c) = v;
        }
        return c;
    }
}

cson_value * cson_new_double( cson_double_t v )
{
    return cson_value_new_double(v);
}

cson_value * cson_value_new_double( cson_double_t v )
{
    if( 0.0 == v ) return &CSON_SPECIAL_VALUES[CSON_VAL_DBL_0];
    else
    {
        cson_value * c = cson_value_new(CSON_TYPE_DOUBLE,0);
        if( c )
        {
            *CSON_DBL(c) = v;
        }
        return c;
    }
}

cson_string * cson_new_string(char const * str, unsigned int len)
{
    if( !str || !*str || !len ) return &CSON_EMPTY_HOLDER.stringValue;
    else
    {
        cson_value * c = cson_value_new(CSON_TYPE_STRING, len + 1/*NUL byte*/);
        cson_string * s = NULL;
        if( c )
        {
            char * dest = NULL;
            s = CSON_STR(c);
            *s = cson_string_empty;
            assert( NULL != s );
            s->length = len;
            dest = cson_string_str(s);
            assert( NULL != dest );
            memcpy( dest, str, len );
            dest[len] = 0;
        }
        return s;
    }
}

cson_value * cson_value_new_string( char const * str, unsigned int len )
{
    return cson_string_value( cson_new_string(str, len) );
}

int cson_array_value_fetch( cson_array const * ar, unsigned int pos, cson_value ** v )
{
    if( !ar) return cson_rc.ArgError;
    if( pos >= ar->list.count ) return cson_rc.RangeError;
    else
    {
        if(v) *v = ar->list.list[pos];
        return 0;
    }
}

cson_value * cson_array_get( cson_array const * ar, unsigned int pos )
{
    cson_value *v = NULL;
    cson_array_value_fetch(ar, pos, &v);
    return v;
}

int cson_array_length_fetch( cson_array const * ar, unsigned int * v )
{
    if( ! ar || !v ) return cson_rc.ArgError;
    else
    {
        if(v) *v = ar->list.count;
        return 0;
    }
}

unsigned int cson_array_length_get( cson_array const * ar )
{
    unsigned int i = 0;
    cson_array_length_fetch(ar, &i);
    return i;
}

int cson_array_reserve( cson_array * ar, unsigned int size )
{
    if( ! ar ) return cson_rc.ArgError;
    else if( size <= ar->list.alloced )
    {
        /* We don't want to introduce a can of worms by trying to
           handle the cleanup from here.
        */
        return 0;
    }
    else
    {
        return (ar->list.alloced > cson_value_list_reserve( &ar->list, size ))
            ? cson_rc.AllocError
            : 0
            ;
    }
}

int cson_array_set( cson_array * ar, unsigned int ndx, cson_value * v )
{
    if( !ar || !v ) return cson_rc.ArgError;
    else if( (ndx+1) < ndx) /* overflow */return cson_rc.RangeError;
    else
    {
        unsigned const int len = cson_value_list_reserve( &ar->list, ndx+1 );
        if( len <= ndx ) return cson_rc.AllocError;
        else
        {
            cson_value * old = ar->list.list[ndx];
            if( old )
            {
                if(old == v) return 0;
                else cson_value_free(old);
            }
            cson_refcount_incr( v );
            ar->list.list[ndx] = v;
            if( ndx >= ar->list.count )
            {
                ar->list.count = ndx+1;
            }
            return 0;
        }
    }
}

/** @internal

   Searchs for the given key in the given object.

   Returns the found item on success, NULL on error.  If ndx is not
   NULL, it is set to the index (in obj->kvp.list) of the found
   item. *ndx is not modified if no entry is found.
*/
static cson_kvp * cson_object_search_impl( cson_object const * obj, char const * key, unsigned int * ndx )
{
    if( obj && key && *key && obj->kvp.count)
    {
#if CSON_OBJECT_PROPS_SORT
        cson_kvp ** s = (cson_kvp**)
            bsearch( key, obj->kvp.list,
                     obj->kvp.count, sizeof(cson_kvp*),
                     cson_kvp_cmp_vs_cstr );
        if( ndx && s )
        { /* index of found record is required by
             cson_object_unset(). Calculate the offset based on s...*/
#if 0
            *ndx = (((unsigned char const *)s - ((unsigned char const *)obj->kvp.list))
                   / sizeof(cson_kvp*));
#else
            *ndx = s - obj->kvp.list;
#endif
        }
        return s ? *s : NULL;
#else
        cson_kvp_list const * li = &obj->kvp;
        unsigned int i = 0;
        cson_kvp * kvp;
        const unsigned int klen = strlen(key);
        for( ; i < li->count; ++i )
        {
            cson_string const * sKey;
            kvp = li->list[i];
            assert( kvp && kvp->key );
            sKey = cson_value_get_string(kvp->key);
            assert(sKey);
            if( sKey->length != klen ) continue;
            else if(0==strcmp(key,cson_string_cstr(sKey)))
            {
                if(ndx) *ndx = i;
                return kvp;
            }
        }
#endif
    }
    return NULL;
}

cson_value * cson_object_get( cson_object const * obj, char const * key )
{
    cson_kvp * kvp = cson_object_search_impl( obj, key, NULL );
    return kvp ? kvp->value : NULL;
}

cson_value * cson_object_get_s( cson_object const * obj, cson_string const *key )
{
    cson_kvp * kvp = cson_object_search_impl( obj, cson_string_cstr(key), NULL );
    return kvp ? kvp->value : NULL;
}


#if CSON_OBJECT_PROPS_SORT
static void cson_object_sort_props( cson_object * obj )
{
    assert( NULL != obj );
    if( obj->kvp.count )
    {
        qsort( obj->kvp.list, obj->kvp.count, sizeof(cson_kvp*),
               cson_kvp_cmp );
    }

}
#endif    

int cson_object_unset( cson_object * obj, char const * key )
{
    if( ! obj || !key || !*key ) return cson_rc.ArgError;
    else
    {
        unsigned int ndx = 0;
        cson_kvp * kvp = cson_object_search_impl( obj, key, &ndx );
        if( ! kvp )
        {
            return cson_rc.NotFoundError;
        }
        assert( obj->kvp.count > 0 );
        assert( obj->kvp.list[ndx] == kvp );
        cson_kvp_free( kvp );
        obj->kvp.list[ndx] = NULL;
        { /* if my brain were bigger i'd use memmove(). */
            unsigned int i = ndx;
            for( ; i < obj->kvp.count; ++i )
            {
                obj->kvp.list[i] =
                    (i < (obj->kvp.alloced-1))
                    ? obj->kvp.list[i+1]
                    : NULL;
            }
        }
        obj->kvp.list[--obj->kvp.count] = NULL;
#if CSON_OBJECT_PROPS_SORT
        cson_object_sort_props( obj );
#endif
        return 0;
    }
}

int cson_object_set_s( cson_object * obj, cson_string * key, cson_value * v )
{
    if( !obj || !key ) return cson_rc.ArgError;
    else if( NULL == v ) return cson_object_unset( obj, cson_string_cstr(key) );
    else
    {
        char const * cKey;
        cson_value * vKey;
        cson_kvp * kvp;
        vKey = cson_string_value(key);
        assert(vKey && (key==CSON_STR(vKey)));
        if( vKey == CSON_VCAST(obj) ){
            return cson_rc.ArgError;
        }
        cKey =  cson_string_cstr(key);
        kvp = cson_object_search_impl( obj, cKey, NULL );
        if( kvp )
        { /* "I told 'em we've already got one!" */
            if( kvp->key != vKey ){
                cson_value_free( kvp->key );
                cson_refcount_incr(vKey);
                kvp->key = vKey;
            }
            if(kvp->value != v){
                cson_value_free( kvp->value );
                cson_refcount_incr( v );
                kvp->value = v;
            }
            return 0;
        }
        if( !obj->kvp.alloced || (obj->kvp.count == obj->kvp.alloced-1))
        { /* reserve space */
            unsigned int const n = obj->kvp.count ? (obj->kvp.count*2) : 6;
            if( n > cson_kvp_list_reserve( &obj->kvp, n ) )
            {
                return cson_rc.AllocError;
            }
        }
        { /* insert new item... */
            int rc = 0;
            kvp = cson_kvp_alloc();
            if( ! kvp )
            {
                return cson_rc.AllocError;
            }
            rc = cson_kvp_list_append( &obj->kvp, kvp );
            if( 0 != rc )
            {
                cson_kvp_free(kvp);
            }
            else
            {
                cson_refcount_incr(vKey);
                cson_refcount_incr(v);
                kvp->key = vKey;
                kvp->value = v;
#if CSON_OBJECT_PROPS_SORT
                cson_object_sort_props( obj );
#endif
            }
            return rc;
        }
    }

}
int cson_object_set( cson_object * obj, char const * key, cson_value * v )
{
    if( ! obj || !key || !*key ) return cson_rc.ArgError;
    else if( NULL == v )
    {
        return cson_object_unset( obj, key );
    }
    else
    {
        cson_string * cs = cson_new_string(key,strlen(key));
        if(!cs) return cson_rc.AllocError;
        else
        {
            int const rc = cson_object_set_s(obj, cs, v);
            if(rc) cson_value_free(cson_string_value(cs));
            return rc;
        }
    }
}

cson_value * cson_object_take( cson_object * obj, char const * key )
{
    if( ! obj || !key || !*key ) return NULL;
    else
    {
        /* FIXME: this is 90% identical to cson_object_unset(),
           only with different refcount handling.
           Consolidate them.
        */
        unsigned int ndx = 0;
        cson_kvp * kvp = cson_object_search_impl( obj, key, &ndx );
        cson_value * rc = NULL;
        if( ! kvp )
        {
            return NULL;
        }
        assert( obj->kvp.count > 0 );
        assert( obj->kvp.list[ndx] == kvp );
        rc = kvp->value;
        assert( rc );
        kvp->value = NULL;
        cson_kvp_free( kvp );
        assert( rc->refcount > 0 );
        --rc->refcount;
        obj->kvp.list[ndx] = NULL;
        { /* if my brain were bigger i'd use memmove(). */
            unsigned int i = ndx;
            for( ; i < obj->kvp.count; ++i )
            {
                obj->kvp.list[i] =
                    (i < (obj->kvp.alloced-1))
                    ? obj->kvp.list[i+1]
                    : NULL;
            }
        }
        obj->kvp.list[--obj->kvp.count] = NULL;
#if CSON_OBJECT_PROPS_SORT
        cson_object_sort_props( obj );
#endif
        return rc;
    }
}
/** @internal

   If p->node is-a Object then value is inserted into the object
   using p->key. In any other case cson_rc.InternalError is returned.

   Returns cson_rc.AllocError if an allocation fails.

   Returns 0 on success. On error, parsing must be ceased immediately.
   
   Ownership of val is ALWAYS TRANSFERED to this function. If this
   function fails, val will be cleaned up and destroyed. (This
   simplifies error handling in the core parser.)
*/
static int cson_parser_set_key( cson_parser * p, cson_value * val )
{
    assert( p && val );

    if( p->ckey && cson_value_is_object(p->node) )
    {
        int rc;
        cson_object * obj = cson_value_get_object(p->node);
        cson_kvp * kvp = NULL;
        assert( obj && (p->node->value == obj) );
        /**
           FIXME? Use cson_object_set() instead of our custom
           finagling with the object? We do it this way to avoid an
           extra alloc/strcpy of the key data.
        */
        if( !obj->kvp.alloced || (obj->kvp.count == obj->kvp.alloced-1))
        {
            if( obj->kvp.alloced > cson_kvp_list_reserve( &obj->kvp, obj->kvp.count ? (obj->kvp.count*2) : 5 ) )
            {
                cson_value_free(val);
                return cson_rc.AllocError;
            }
        }
        kvp = cson_kvp_alloc();
        if( ! kvp )
        {
            cson_value_free(val);
            return cson_rc.AllocError;
        }
        kvp->key = cson_string_value(p->ckey)/*transfer ownership*/;
        assert(0 == kvp->key->refcount);
        cson_refcount_incr(kvp->key);
        p->ckey = NULL;
        kvp->value = val;
        cson_refcount_incr( val );
        rc = cson_kvp_list_append( &obj->kvp, kvp );
        if( 0 != rc )
        {
            cson_kvp_free( kvp );
        }
        else
        {
            ++p->totalValueCount;
        }
        return rc;
    }
    else
    {
        if(val) cson_value_free(val);
        return p->errNo = cson_rc.InternalError;
    }

}

/** @internal

    Pushes val into the current object/array parent node, depending on the
    internal state of the parser.

    Ownership of val is always transfered to this function, regardless of
    success or failure.

    Returns 0 on success. On error, parsing must be ceased immediately.
*/
static int cson_parser_push_value( cson_parser * p, cson_value * val )
{
    if( p->ckey )
    { /* we're in Object mode */
        assert( cson_value_is_object( p->node ) );
        return cson_parser_set_key( p, val );
    }
    else if( cson_value_is_array( p->node ) )
    { /* we're in Array mode */
        cson_array * ar = cson_value_get_array( p->node );
        int rc;
        assert( ar && (ar == p->node->value) );
        rc = cson_array_append( ar, val );
        if( 0 != rc )
        {
            cson_value_free(val);
        }
        else
        {
            ++p->totalValueCount;
        }
        return rc;
    }
    else
    { /* WTF? */
        assert( 0 && "Internal error in cson_parser code" );
        return p->errNo = cson_rc.InternalError;
    }
}

/**
   Callback for JSON_parser API. Reminder: it returns 0 (meaning false)
   on error!
*/
static int cson_parse_callback( void * cx, int type, JSON_value const * value )
{
    cson_parser * p = (cson_parser *)cx;
    int rc = 0;
#define ALLOC_V(T,V) cson_value * v = cson_value_new_##T(V); if( ! v ) { rc = cson_rc.AllocError; break; }
    switch(type) {
      case JSON_T_ARRAY_BEGIN:
      case JSON_T_OBJECT_BEGIN: {
          cson_value * obja = (JSON_T_ARRAY_BEGIN == type)
              ? cson_value_new_array()
              : cson_value_new_object();
          if( ! obja )
          {
              p->errNo = cson_rc.AllocError;
              break;
          }
          if( 0 != rc ) break;
          if( ! p->root )
          {
              p->root = p->node = obja;
              rc = cson_array_append( &p->stack, obja );
              if( 0 != rc )
              { /* work around a (potential) corner case in the cleanup code. */
                  cson_value_free( p->root );
                  p->root = NULL;
              }
              else
              {
                  cson_refcount_incr( p->root )
                      /* simplifies cleanup later on. */
                      ;
                  ++p->totalValueCount;
              }
          }
          else
          {
              rc = cson_array_append( &p->stack, obja );
              if(rc) cson_value_free( obja );
              else
              {
                  rc = cson_parser_push_value( p, obja );
                  if( 0 == rc ) p->node = obja;
              }
          }
          break;
      }
      case JSON_T_ARRAY_END:
      case JSON_T_OBJECT_END: {
          if( 0 == p->stack.list.count )
          {
              rc = cson_rc.RangeError;
              break;
          }
#if CSON_OBJECT_PROPS_SORT
          if( cson_value_is_object(p->node) )
          {/* kludge: the parser uses custom cson_object property
              insertion as a malloc/strcpy-reduction optimization.
              Because of that, we have to sort the property list
              ourselves...
           */
              cson_object * obj = cson_value_get_object(p->node);
              assert( NULL != obj );
              cson_object_sort_props( obj );
          }
#endif

#if 1
          /* Reminder: do not use cson_array_pop_back( &p->stack )
             because that will clean up the object, and we don't want
             that.  We just want to forget this reference
             to it. The object is either the root or was pushed into
             an object/array in the parse tree (and is owned by that
             object/array).
          */
          --p->stack.list.count;
          assert( p->node == p->stack.list.list[p->stack.list.count] );
          cson_refcount_decr( p->node )
              /* p->node might be owned by an outer object but we
                 need to remove the list's reference. For the
                 root node we manually add a reference to
                 avoid a special case here. Thus when we close
                 the root node, its refcount is still 1.
              */;
          p->stack.list.list[p->stack.list.count] = NULL;
          if( p->stack.list.count )
          {
              p->node = p->stack.list.list[p->stack.list.count-1];
          }
          else
          {
              p->node = p->root;
          }
#else
          /*
             Causing a leak?
           */
          cson_array_pop_back( &p->stack, 1 );
          if( p->stack.list.count )
          {
              p->node = p->stack.list.list[p->stack.list.count-1];
          }
          else
          {
              p->node = p->root;
          }
          assert( p->node && (1==p->node->refcount) );
#endif
          break;
      }
      case JSON_T_INTEGER: {
          ALLOC_V(integer, value->vu.integer_value );
          rc = cson_parser_push_value( p, v );
          break;
      }
      case JSON_T_FLOAT: {
          ALLOC_V(double, value->vu.float_value );
          rc =  cson_parser_push_value( p, v );
          break;
      }
      case JSON_T_NULL: {
          rc = cson_parser_push_value( p, cson_value_null() );
          break;
      }
      case JSON_T_TRUE: {
          rc = cson_parser_push_value( p, cson_value_true() );
          break;
      }
      case JSON_T_FALSE: {
          rc = cson_parser_push_value( p, cson_value_false() );
          break;
      }
      case JSON_T_KEY: {
          assert(!p->ckey);
          p->ckey = cson_new_string( value->vu.str.value, value->vu.str.length );
          if( ! p->ckey )
          {
              rc = cson_rc.AllocError;
              break;
          }
          ++p->totalKeyCount;
          break;
      }
      case JSON_T_STRING: {
          cson_value * v = cson_value_new_string( value->vu.str.value, value->vu.str.length );
          rc = ( NULL == v ) 
            ? cson_rc.AllocError
            : cson_parser_push_value( p, v );
          break;
      }
      default:
          assert(0);
          rc = cson_rc.InternalError;
          break;
    }
#undef ALLOC_V
    return ((p->errNo = rc)) ? 0 : 1;
}


/**
   Converts a JSON_error code to one of the cson_rc values.
*/
static int cson_json_err_to_rc( JSON_error jrc )
{
    switch(jrc)
    {
      case JSON_E_NONE: return 0;
      case JSON_E_INVALID_CHAR: return cson_rc.Parse_INVALID_CHAR;
      case JSON_E_INVALID_KEYWORD: return cson_rc.Parse_INVALID_KEYWORD;
      case JSON_E_INVALID_ESCAPE_SEQUENCE: return cson_rc.Parse_INVALID_ESCAPE_SEQUENCE;
      case JSON_E_INVALID_UNICODE_SEQUENCE: return cson_rc.Parse_INVALID_UNICODE_SEQUENCE;
      case JSON_E_INVALID_NUMBER: return cson_rc.Parse_INVALID_NUMBER;
      case JSON_E_NESTING_DEPTH_REACHED: return cson_rc.Parse_NESTING_DEPTH_REACHED;
      case JSON_E_UNBALANCED_COLLECTION: return cson_rc.Parse_UNBALANCED_COLLECTION;
      case JSON_E_EXPECTED_KEY: return cson_rc.Parse_EXPECTED_KEY;
      case JSON_E_EXPECTED_COLON: return cson_rc.Parse_EXPECTED_COLON;
      case JSON_E_OUT_OF_MEMORY: return cson_rc.AllocError;
      default:
          return cson_rc.InternalError;
    }
}

/** @internal

   Cleans up all contents of p but does not free p.

   To properly take over ownership of the parser's root node on a
   successful parse:

   - Copy p->root's pointer and set p->root to NULL.
   - Eventually free up p->root with cson_value_free().
   
   If you do not set p->root to NULL, p->root will be freed along with
   any other items inserted into it (or under it) during the parsing
   process.
*/
static int cson_parser_clean( cson_parser * p )
{
    if( ! p ) return cson_rc.ArgError;
    else
    {
        if( p->p )
        {
            delete_JSON_parser(p->p);
            p->p = NULL;
        }
        if( p->ckey ){
            cson_value_free(cson_string_value(p->ckey));
        }
        cson_array_clean( &p->stack, 1 );
        if( p->root )
        {
            cson_value_free( p->root );
        }
        *p = cson_parser_empty;
        return 0;
    }
}


int cson_parse( cson_value ** tgt, cson_data_source_f src, void * state,
                cson_parse_opt const * opt_, cson_parse_info * info_ )
{
    unsigned char ch[2] = {0,0};
    cson_parse_opt const opt = opt_ ? *opt_ : cson_parse_opt_empty;
    int rc = 0;
    unsigned int len = 1;
    cson_parse_info info = info_ ? *info_ : cson_parse_info_empty;
    cson_parser p = cson_parser_empty;
    if( ! tgt || ! src ) return cson_rc.ArgError;
    
    {
        JSON_config jopt = {0};
        init_JSON_config( &jopt );
        jopt.allow_comments = opt.allowComments;
        jopt.depth = opt.maxDepth;
        jopt.callback_ctx = &p;
        jopt.handle_floats_manually = 0;
        jopt.callback = cson_parse_callback;
        p.p = new_JSON_parser(&jopt);
        if( ! p.p )
        {
            return cson_rc.AllocError;
        }
    }

    do
    { /* FIXME: buffer the input in multi-kb chunks. */
        len = 1;
        ch[0] = 0;
        rc = src( state, ch, &len );
        if( 0 != rc ) break;
        else if( !len /* EOF */ ) break;
        ++info.length;
        if('\n' == ch[0])
        {
            ++info.line;
            info.col = 0;
        }
        if( ! JSON_parser_char(p.p, ch[0]) )
        {
            rc = cson_json_err_to_rc( JSON_parser_get_last_error(p.p) );
            if(0==rc) rc = p.errNo;
            if(0==rc) rc = cson_rc.InternalError;
            info.errorCode = rc;
            break;
        }
        if( '\n' != ch[0]) ++info.col;
    } while(1);
    if( info_ )
    {
        info.totalKeyCount = p.totalKeyCount;
        info.totalValueCount = p.totalValueCount;
        *info_ = info;
    }
    if( 0 != rc )
    {
        cson_parser_clean(&p);
        return rc;
    }
    if( ! JSON_parser_done(p.p) )
    {
        rc = cson_json_err_to_rc( JSON_parser_get_last_error(p.p) );
        cson_parser_clean(&p);
        if(0==rc) rc = p.errNo;
        if(0==rc) rc = cson_rc.InternalError;
    }
    else
    {
        cson_value * root = p.root;
        p.root = NULL;
        cson_parser_clean(&p);
        if( root )
        {
            assert( (1 == root->refcount) && "Detected memory mismanagement in the parser." );
            root->refcount = 0
                /* HUGE KLUDGE! Avoids having one too many references
                   in some client code, leading to a leak. Here we're
                   accommodating a memory management workaround in the
                   parser code which manually adds a reference to the
                   root node to keep it from being cleaned up
                   prematurely.
                */;
            *tgt = root;
        }
        else
        { /* then can happen on empty input. */
            rc = cson_rc.UnknownError;
        }
    }
    return rc;
}

/**
   The UTF code was originally taken from sqlite3's public-domain
   source code (http://sqlite.org), modified only slightly for use
   here. This code generates some "possible data loss" warnings on
   MSVC, but if this code is good enough for sqlite3 then it's damned
   well good enough for me, so we disable that warning for Windows
   builds.
*/

/*
** This lookup table is used to help decode the first byte of
** a multi-byte UTF8 character.
*/
static const unsigned char cson_utfTrans1[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x00, 0x00
};


/*
** Translate a single UTF-8 character.  Return the unicode value.
**
** During translation, assume that the byte that zTerm points
** is a 0x00.
**
** Write a pointer to the next unread byte back into *pzNext.
**
** Notes On Invalid UTF-8:
**
**  *  This routine never allows a 7-bit character (0x00 through 0x7f) to
**     be encoded as a multi-byte character.  Any multi-byte character that
**     attempts to encode a value between 0x00 and 0x7f is rendered as 0xfffd.
**
**  *  This routine never allows a UTF16 surrogate value to be encoded.
**     If a multi-byte character attempts to encode a value between
**     0xd800 and 0xe000 then it is rendered as 0xfffd.
**
**  *  Bytes in the range of 0x80 through 0xbf which occur as the first
**     byte of a character are interpreted as single-byte characters
**     and rendered as themselves even though they are technically
**     invalid characters.
**
**  *  This routine accepts an infinite number of different UTF8 encodings
**     for unicode values 0x80 and greater.  It do not change over-length
**     encodings to 0xfffd as some systems recommend.
*/
#define READ_UTF8(zIn, zTerm, c)                           \
  c = *(zIn++);                                            \
  if( c>=0xc0 ){                                           \
    c = cson_utfTrans1[c-0xc0];                          \
    while( zIn!=zTerm && (*zIn & 0xc0)==0x80 ){            \
      c = (c<<6) + (0x3f & *(zIn++));                      \
    }                                                      \
    if( c<0x80                                             \
        || (c&0xFFFFF800)==0xD800                          \
        || (c&0xFFFFFFFE)==0xFFFE ){  c = 0xFFFD; }        \
  }
static int cson_utf8Read(
  const unsigned char *z,         /* First byte of UTF-8 character */
  const unsigned char *zTerm,     /* Pretend this byte is 0x00 */
  const unsigned char **pzNext    /* Write first byte past UTF-8 char here */
){
  int c;
  READ_UTF8(z, zTerm, c);
  *pzNext = z;
  return c;
}
#undef READ_UTF8

#ifdef _MSC_VER
#  if _MSC_VER >= 1400 /* Visual Studio 2005 and up */
#    pragma warning( pop )
#  endif
#endif

unsigned int cson_string_length_utf8( cson_string const * str )
{
    if( ! str ) return 0;
    else
    {
        char unsigned const * pos = (char unsigned const *)cson_string_cstr(str);
        char unsigned const * end = pos + str->length;
        unsigned int rc = 0;
        for( ; (pos < end) && cson_utf8Read(pos, end, &pos);
            ++rc )
        {
        };
        return rc;
    }
}

/**
   Escapes the first len bytes of the given string as JSON and sends
   it to the given output function (which will be called often - once
   for each logical character). The output is also surrounded by
   double-quotes.

   A NULL str will be escaped as an empty string, though we should
   arguably export it as "null" (without quotes). We do this because
   in JavaScript (typeof null === "object"), and by outputing null
   here we would effectively change the data type from string to
   object.
*/
static int cson_str_to_json( char const * str, unsigned int len,
                             char escapeFwdSlash,
                             cson_data_dest_f f, void * state )
{
    if( NULL == f ) return cson_rc.ArgError;
    else if( !str || !*str || (0 == len) )
    { /* special case for 0-length strings. */
        return f( state, "\"\"", 2 );
    }
    else
    {
        unsigned char const * pos = (unsigned char const *)str;
        unsigned char const * end = (unsigned char const *)(str ? (str + len) : NULL);
        unsigned char const * next = NULL;
        int ch;
        unsigned char clen = 0;
        char escChar[3] = {'\\',0,0};
        enum { UBLen = 8 };
        char ubuf[UBLen];
        int rc = 0;
        rc = f(state, "\"", 1 );
        for( ; (pos < end) && (0 == rc); pos += clen )
        {
            ch = cson_utf8Read(pos, end, &next);
            if( 0 == ch ) break;
            assert( next > pos );
            clen = next - pos;
            assert( clen );
            if( 1 == clen )
            { /* ASCII */
#if defined(CSON_FOSSIL_MODE)
                /* Workaround for fossil repo artifact
                   f460839cff85d4e4f1360b366bb2858cef1411ea,
                   which has what appears to be latin1-encoded
                   text. file(1) thinks it's a FORTRAN program.
                */
                if(0xfffd==ch){
                    assert(*pos != ch);
                    /* MARKER("ch=%04x, *pos=%04x\n", ch, *pos); */
                    ch = *pos
                        /* We should arguably translate to '?', and
                           will if this problem ever comes up with a
                           non-latin1 encoding. For latin1 this
                           workaround incidentally corrects the output
                           to proper UTF8-escaped characters, and only
                           for that reason is it being kept around.
                        */;
                    goto assume_latin1;
                }
#endif
                assert( (*pos == ch) && "Invalid UTF8" );
                escChar[1] = 0;
                switch(ch)
                {
                  case '\t': escChar[1] = 't'; break;
                  case '\r': escChar[1] = 'r'; break;
                  case '\n': escChar[1] = 'n'; break;
                  case '\f': escChar[1] = 'f'; break;
                  case '\b': escChar[1] = 'b'; break;
                  case '/':
      /*
        Regarding escaping of forward-slashes. See the main exchange below...

        --------------
        From: Douglas Crockford <douglas@crockford.com>
        To: Stephan Beal <sgbeal@googlemail.com>
        Subject: Re: Is escaping of forward slashes required?

        It is allowed, not required. It is allowed so that JSON can be safely
        embedded in HTML, which can freak out when seeing strings containing
        "</". JSON tolerates "<\/" for this reason.

        On 4/8/2011 2:09 PM, Stephan Beal wrote:
        > Hello, Jsonites,
        >
        > i'm a bit confused on a small grammatic detail of JSON:
        >
        > if i'm reading the grammar chart on http://www.json.org/ correctly,
        > forward slashes (/) are supposed to be escaped in JSON. However, the
        > JSON class provided with my browsers (Chrome and FF, both of which i
        > assume are fairly standards/RFC-compliant) do not escape such characters.
        >
        > Is backslash-escaping forward slashes required? If so, what is the
        > justification for it? (i ask because i find it unnecessary and hard to
        > look at.)
        --------------
      */
                      if( escapeFwdSlash ) escChar[1] = '/';
                      break;
                  case '\\': escChar[1] = '\\'; break;
                  case '"': escChar[1] = '"'; break;
                  default: break;
                }
                if( escChar[1])
                {
                    rc = f(state, escChar, 2);
                }
                else
                {
                    rc = f(state, (char const *)pos, clen);
                }
                continue;
            }
            else
            { /* UTF: transform it to \uXXXX */
#if defined(CSON_FOSSIL_MODE)
                assume_latin1:
#endif
                memset(ubuf,0,UBLen);
                rc = sprintf(ubuf, "\\u%04x",ch);
                if( rc != 6 )
                {
                    rc = cson_rc.RangeError;
                    break;
                }
                rc = f( state, ubuf, 6 );
                continue;
            }
        }
        if( 0 == rc )
        {
            rc = f(state, "\"", 1 );
        }
        return rc;
    }
}

int cson_object_iter_init( cson_object const * obj, cson_object_iterator * iter )
{
    if( ! obj || !iter ) return cson_rc.ArgError;
    else
    {
        iter->obj = obj;
        iter->pos = 0;
        return 0;
    }
}

cson_kvp * cson_object_iter_next( cson_object_iterator * iter )
{
    if( ! iter || !iter->obj ) return NULL;
    else if( iter->pos >= iter->obj->kvp.count ) return NULL;
    else
    {
        cson_kvp * rc = iter->obj->kvp.list[iter->pos++];
        while( (NULL==rc) && (iter->pos < iter->obj->kvp.count))
        {
            rc = iter->obj->kvp.list[iter->pos++];
        }
        return rc;
    }
}

static int cson_output_null( cson_data_dest_f f, void * state )
{
    if( !f ) return cson_rc.ArgError;
    else
    {
        return f(state, "null", 4);
    }
}

static int cson_output_bool( cson_value const * src, cson_data_dest_f f, void * state )
{
    if( !f ) return cson_rc.ArgError;
    else
    {
        char const v = cson_value_get_bool(src);
        return f(state, v ? "true" : "false", v ? 4 : 5);
    }
}

static int cson_output_integer( cson_value const * src, cson_data_dest_f f, void * state )
{
    if( !f ) return cson_rc.ArgError;
    else if( !cson_value_is_integer(src) ) return cson_rc.TypeError;
    else
    {
        enum { BufLen = 100 };
        char b[BufLen];
        int rc;
        memset( b, 0, BufLen );
        rc = sprintf( b, "%"CSON_INT_T_PFMT, cson_value_get_integer(src) )
            /* Reminder: snprintf() is C99 */
            ;
        return ( rc<=0 )
            ? cson_rc.RangeError
            : f( state, b, (unsigned int)rc )
            ;
    }
}

static int cson_output_double( cson_value const * src, cson_data_dest_f f, void * state )
{
    if( !f ) return cson_rc.ArgError;
    else if( !cson_value_is_double(src) ) return cson_rc.TypeError;
    else
    {
        enum { BufLen = 128 /* this must be relatively large or huge
                               doubles can cause us to overrun here,
                               resulting in stack-smashing errors.
                            */};
        char b[BufLen];
        int rc;
        memset( b, 0, BufLen );
        rc = sprintf( b, "%"CSON_DOUBLE_T_PFMT, cson_value_get_double(src) )
            /* Reminder: snprintf() is C99 */
            ;
        if( rc<=0 ) return cson_rc.RangeError;
        else if(1)
        { /* Strip trailing zeroes before passing it on... */
            unsigned int urc = (unsigned int)rc;
            char * pos = b + urc - 1;
            for( ; ('0' == *pos) && urc && (*(pos-1) != '.'); --pos, --urc )
            {
                *pos = 0;
            }
            assert(urc && *pos);
            return f( state, b, urc );
        }
        else
        {
            unsigned int urc = (unsigned int)rc;
            return f( state, b, urc );
        }
        return 0;
    }
}

static int cson_output_string( cson_value const * src, char escapeFwdSlash, cson_data_dest_f f, void * state )
{
    if( !f ) return cson_rc.ArgError;
    else if( ! cson_value_is_string(src) ) return cson_rc.TypeError;
    else
    {
        cson_string const * str = cson_value_get_string(src);
        assert( NULL != str );
        return cson_str_to_json(cson_string_cstr(str), str->length, escapeFwdSlash, f, state);
    }
}


/**
   Outputs indention spacing to f().

   blanks: (0)=no indentation, (1)=1 TAB per/level, (>1)=n spaces/level

   depth is the current depth of the output tree, and determines how much
   indentation to generate.

   If blanks is 0 this is a no-op. Returns non-0 on error, and the
   error code will always come from f().
*/
static int cson_output_indent( cson_data_dest_f f, void * state,
                               unsigned char blanks, unsigned int depth )
{
    if( 0 == blanks ) return 0;
    else
    {
#if 0
        /* FIXME: stuff the indention into the buffer and make a single
           call to f().
        */
        enum { BufLen = 200 };
        char buf[BufLen];
#endif
        unsigned int i;
        unsigned int x;
        char const ch = (1==blanks) ? '\t' : ' ';
        int rc = f(state, "\n", 1 );
        for( i = 0; (i < depth) && (0 == rc); ++i )
        {
            for( x = 0; (x < blanks) && (0 == rc); ++x )
            {
                rc = f(state, &ch, 1);
            }
        }
        return rc;
    }
}

static int cson_output_array( cson_value const * src, cson_data_dest_f f, void * state,
                              cson_output_opt const * fmt, unsigned int level );
static int cson_output_object( cson_value const * src, cson_data_dest_f f, void * state,
                               cson_output_opt const * fmt, unsigned int level );
/**
   Main cson_output() implementation. Dispatches to a different impl depending
   on src->api->typeID.

   Returns 0 on success.
*/
static int cson_output_impl( cson_value const * src, cson_data_dest_f f, void * state,
                             cson_output_opt const * fmt, unsigned int level )
{
    if( ! src || !f || !src->api ) return cson_rc.ArgError;
    else
    {
        int rc = 0;
        assert(fmt);
        switch( src->api->typeID )
        {
          case CSON_TYPE_UNDEF:
          case CSON_TYPE_NULL:
              rc = cson_output_null(f, state);
              break;
          case CSON_TYPE_BOOL:
              rc = cson_output_bool(src, f, state);
              break;
          case CSON_TYPE_INTEGER:
              rc = cson_output_integer(src, f, state);
              break;
          case CSON_TYPE_DOUBLE:
              rc = cson_output_double(src, f, state);
              break;
          case CSON_TYPE_STRING:
              rc = cson_output_string(src, fmt->escapeForwardSlashes, f, state);
              break;
          case CSON_TYPE_ARRAY:
              rc = cson_output_array( src, f, state, fmt, level );
              break;
          case CSON_TYPE_OBJECT:
              rc = cson_output_object( src, f, state, fmt, level );
              break;
          default:
              rc = cson_rc.TypeError;
              break;
        }
        return rc;
    }
}


static int cson_output_array( cson_value const * src, cson_data_dest_f f, void * state,
                              cson_output_opt const * fmt, unsigned int level )
{
    if( !src || !f || !fmt ) return cson_rc.ArgError;
    else if( ! cson_value_is_array(src) ) return cson_rc.TypeError;
    else if( level > fmt->maxDepth ) return cson_rc.RangeError;
    else
    {
        int rc;
        unsigned int i;
        cson_value const * v;
        char doIndent = fmt->indentation ? 1 : 0;
        cson_array const * ar = cson_value_get_array(src);
        assert( NULL != ar );
        if( 0 == ar->list.count )
        {
            return f(state, "[]", 2 );
        }
        else if( (1 == ar->list.count) && !fmt->indentSingleMemberValues ) doIndent = 0;
        rc = f(state, "[", 1);
        ++level;
        if( doIndent )
        {
            rc = cson_output_indent( f, state, fmt->indentation, level );
        }
        for( i = 0; (i < ar->list.count) && (0 == rc); ++i )
        {
            v = ar->list.list[i];
            if( v )
            {
                rc = cson_output_impl( v, f, state, fmt, level );
            }
            else
            {
                rc = cson_output_null( f, state );
            }
            if( 0 == rc )
            {
                if(i < (ar->list.count-1))
                {
                    rc = f(state, ",", 1);
                    if( 0 == rc )
                    {
                        rc = doIndent
                            ? cson_output_indent( f, state, fmt->indentation, level )
                            : 0 /*f( state, " ", 1 )*/;
                    }
                }
            }
        }
        --level;
        if( doIndent && (0 == rc) )
        {
            rc = cson_output_indent( f, state, fmt->indentation, level );
        }
        return (0 == rc)
            ? f(state, "]", 1)
            : rc;
    }
}

static int cson_output_object( cson_value const * src, cson_data_dest_f f, void * state,
                               cson_output_opt const * fmt, unsigned int level )
{
    if( !src || !f || !fmt ) return cson_rc.ArgError;
    else if( ! cson_value_is_object(src) ) return cson_rc.TypeError;
    else if( level > fmt->maxDepth ) return cson_rc.RangeError;
    else
    {
        int rc;
        unsigned int i;
        cson_kvp const * kvp;
        char doIndent = fmt->indentation ? 1 : 0;
        cson_object const * obj = cson_value_get_object(src);
        assert( (NULL != obj) && (NULL != fmt));
        if( 0 == obj->kvp.count )
        {
            return f(state, "{}", 2 );
        }
        else if( (1 == obj->kvp.count) && !fmt->indentSingleMemberValues ) doIndent = 0;
        rc = f(state, "{", 1);
        ++level;
        if( doIndent )
        {
            rc = cson_output_indent( f, state, fmt->indentation, level );
        }
        for( i = 0; (i < obj->kvp.count) && (0 == rc); ++i )
        {
            kvp = obj->kvp.list[i];
            if( kvp && kvp->key )
            {
                cson_string const * sKey = cson_value_get_string(kvp->key);
                char const * cKey = cson_string_cstr(sKey);
                rc = cson_str_to_json(cKey, sKey->length,
                                      fmt->escapeForwardSlashes, f, state);
                if( 0 == rc )
                {
                    rc = fmt->addSpaceAfterColon
                        ? f(state, ": ", 2 )
                        : f(state, ":", 1 )
                        ;
                }
                if( 0 == rc)
                {
                    rc = ( kvp->value )
                        ? cson_output_impl( kvp->value, f, state, fmt, level )
                        : cson_output_null( f, state );
                }
            }
            else
            {
                assert( 0 && "Possible internal error." );
                continue /* internal error? */;
            }
            if( 0 == rc )
            {
                if(i < (obj->kvp.count-1))
                {
                    rc = f(state, ",", 1);
                    if( 0 == rc )
                    {
                        rc = doIndent
                            ? cson_output_indent( f, state, fmt->indentation, level )
                            : 0 /*f( state, " ", 1 )*/;
                    }
                }
            }
        }
        --level;
        if( doIndent && (0 == rc) )
        {
            rc = cson_output_indent( f, state, fmt->indentation, level );
        }
        return (0 == rc)
            ? f(state, "}", 1)
            : rc;
    }
}

int cson_output( cson_value const * src, cson_data_dest_f f,
                 void * state, cson_output_opt const * fmt )
{
    int rc;
    if(! fmt ) fmt = &cson_output_opt_empty;
    rc = cson_output_impl(src, f, state, fmt, 0 );
    if( (0 == rc) && fmt->addNewline )
    {
        rc = f(state, "\n", 1);
    }
    return rc;
}

int cson_data_dest_FILE( void * state, void const * src, unsigned int n )
{
    if( ! state ) return cson_rc.ArgError;
    else if( !src || !n ) return 0;
    else
    {
        return ( 1 == fwrite( src, n, 1, (FILE*) state ) )
            ? 0
            : cson_rc.IOError;
    }
}

int cson_output_FILE( cson_value const * src, FILE * dest, cson_output_opt const * fmt )
{
    int rc = 0;
    if( fmt )
    {
        rc = cson_output( src, cson_data_dest_FILE, dest, fmt );
    }
    else
    {
        /* We normally want a newline on FILE output. */
        cson_output_opt opt = cson_output_opt_empty;
        opt.addNewline = 1;
        rc = cson_output( src, cson_data_dest_FILE, dest, &opt );
    }
    if( 0 == rc )
    {
        fflush( dest );
    }
    return rc;
}

int cson_output_filename( cson_value const * src, char const * dest, cson_output_opt const * fmt )
{
    if( !src || !dest ) return cson_rc.ArgError;
    else
    {
        FILE * f = fopen(dest,"wb");
        if( !f ) return cson_rc.IOError;
        else
        {
            int const rc = cson_output_FILE( src, f, fmt );
            fclose(f);
            return rc;
        }
    }
}

int cson_parse_filename( cson_value ** tgt, char const * src,
                         cson_parse_opt const * opt, cson_parse_info * err )
{
    if( !src || !tgt ) return cson_rc.ArgError;
    else
    {
        FILE * f = fopen(src, "r");
        if( !f ) return cson_rc.IOError;
        else
        {
            int const rc = cson_parse_FILE( tgt, f, opt, err );
            fclose(f);
            return rc;
        }
    }
}

/** Internal type to hold state for a JSON input string.
 */
typedef struct cson_data_source_StringSource_
{
    /** Start of input string. */
    char const * str;
    /** Current iteration position. Must initially be == str. */
    char const * pos;
    /** Logical EOF, one-past-the-end of str. */
    char const * end;
}  cson_data_source_StringSource_t;

/**
   A cson_data_source_f() implementation which requires the state argument
   to be a properly populated (cson_data_source_StringSource_t*).
*/
static int cson_data_source_StringSource( void * state, void * dest, unsigned int * n )
{
    if( !state || !n || !dest ) return cson_rc.ArgError;
    else if( !*n ) return 0 /* ignore this */;
    else
    {
        unsigned int i;
        cson_data_source_StringSource_t * ss = (cson_data_source_StringSource_t*) state;
        unsigned char * tgt = (unsigned char *)dest;
        for( i = 0; (i < *n) && (ss->pos < ss->end); ++i, ++ss->pos, ++tgt )
        {
            *tgt = *ss->pos;
        }
        *n = i;
        return 0;
    }
}

int cson_parse_string( cson_value ** tgt, char const * src, unsigned int len,
                       cson_parse_opt const * opt, cson_parse_info * err )
{
    if( ! tgt || !src ) return cson_rc.ArgError;
    else if( !*src || (len<2/*2==len of {} and []*/) ) return cson_rc.RangeError;
    else
    {
        cson_data_source_StringSource_t ss;
        ss.str = ss.pos = src;
        ss.end = src + len;
        return cson_parse( tgt, cson_data_source_StringSource, &ss, opt, err );
    }

}

int cson_parse_buffer( cson_value ** tgt,
                       cson_buffer const * buf,
                       cson_parse_opt const * opt,
                       cson_parse_info * err )
{
    return ( !tgt || !buf || !buf->mem || !buf->used )
        ? cson_rc.ArgError
        : cson_parse_string( tgt, (char const *)buf->mem,
                             buf->used, opt, err );
}

int cson_buffer_reserve( cson_buffer * buf, cson_size_t n )
{
    if( ! buf ) return cson_rc.ArgError;
    else if( 0 == n )
    {
        cson_free(buf->mem, "cson_buffer::mem");
        *buf = cson_buffer_empty;
        return 0;
    }
    else if( buf->capacity >= n )
    {
        return 0;
    }
    else
    {
        unsigned char * x = (unsigned char *)cson_realloc( buf->mem, n, "cson_buffer::mem" );
        if( ! x ) return cson_rc.AllocError;
        memset( x + buf->used, 0, n - buf->used );
        buf->mem = x;
        buf->capacity = n;
        ++buf->timesExpanded;
        return 0;
    }
}

cson_size_t cson_buffer_fill( cson_buffer * buf, char c )
{
    if( !buf || !buf->capacity || !buf->mem ) return 0;
    else
    {
        memset( buf->mem, c, buf->capacity );
        return buf->capacity;
    }
}

/**
   cson_data_dest_f() implementation, used by cson_output_buffer().

   arg MUST be a (cson_buffer*). This function appends n bytes at
   position arg->used, expanding the buffer as necessary.
*/
static int cson_data_dest_cson_buffer( void * arg, void const * data_, unsigned int n )
{
    if( !arg ) return cson_rc.ArgError;
    else if( ! n ) return 0;
    else
    {
        cson_buffer * sb = (cson_buffer*)arg;
        char const * data = (char const *)data_;
        cson_size_t npos = sb->used + n;
        unsigned int i;
        if( npos >= sb->capacity )
        {
            const cson_size_t oldCap = sb->capacity;
            const cson_size_t asz = npos * 2;
            if( asz < npos ) return cson_rc.ArgError; /* overflow */
            else if( 0 != cson_buffer_reserve( sb, asz ) ) return cson_rc.AllocError;
            assert( (sb->capacity > oldCap) && "Internal error in memory buffer management!" );
            /* make sure it gets NUL terminated. */
            memset( sb->mem + oldCap, 0, (sb->capacity - oldCap) );
        }
        for( i = 0; i < n; ++i, ++sb->used )
        {
            sb->mem[sb->used] = data[i];
        }
        return 0;
    }
}


int cson_output_buffer( cson_value const * v, cson_buffer * buf,
                        cson_output_opt const * opt )
{
    int rc = cson_output( v, cson_data_dest_cson_buffer, buf, opt );
    if( 0 == rc )
    { /* Ensure that the buffer is null-terminated. */
        rc = cson_buffer_reserve( buf, buf->used + 1 );
        if( 0 == rc )
        {
            buf->mem[buf->used] = 0;
        }
    }
    return rc;
}

/** @internal

Tokenizes an input string on a given separator. Inputs are:

- (inp) = is a pointer to the pointer to the start of the input.

- (separator) = the separator character

- (end) = a pointer to NULL. i.e. (*end == NULL)

This function scans *inp for the given separator char or a NUL char.
Successive separators at the start of *inp are skipped. The effect is
that, when this function is called in a loop, all neighboring
separators are ignored. e.g. the string "aa.bb...cc" will tokenize to
the list (aa,bb,cc) if the separator is '.' and to (aa.,...cc) if the
separator is 'b'.

Returns 0 (false) if it finds no token, else non-0 (true).

Output:

- (*inp) will be set to the first character of the next token.

- (*end) will point to the one-past-the-end point of the token.

If (*inp == *end) then the end of the string has been reached
without finding a token.

Post-conditions:

- (*end == *inp) if no token is found.

- (*end > *inp) if a token is found.

It is intolerant of NULL values for (inp, end), and will assert() in
debug builds if passed NULL as either parameter.
*/
static char cson_next_token( char const ** inp, char separator, char const ** end )
{
    char const * pos = NULL;
    assert( inp && end && *inp );
    if( *inp == *end ) return 0;
    pos = *inp;
    if( !*pos )
    {
        *end = pos;
        return 0;
    }
    for( ; *pos && (*pos == separator); ++pos) { /* skip preceeding splitters */ }
    *inp = pos;
    for( ; *pos && (*pos != separator); ++pos) { /* find next splitter */ }
    *end = pos;
    return (pos > *inp) ? 1 : 0;
}

int cson_object_fetch_sub2( cson_object const * obj, cson_value ** tgt, char const * path )
{
    if( ! obj || !path ) return cson_rc.ArgError;
    else if( !*path || !*(1+path) ) return cson_rc.RangeError;
    else return cson_object_fetch_sub(obj, tgt, path+1, *path);
}

int cson_object_fetch_sub( cson_object const * obj, cson_value ** tgt, char const * path, char sep )
{
    if( ! obj || !path ) return cson_rc.ArgError;
    else if( !*path || !sep ) return cson_rc.RangeError;
    else
    {
        char const * beg = path;
        char const * end = NULL;
        int rc;
        unsigned int i, len;
        unsigned int tokenCount = 0;
        cson_value * cv = NULL;
        cson_object const * curObj = obj;
        enum { BufSize = 128 };
        char buf[BufSize];
        memset( buf, 0, BufSize );

        while( cson_next_token( &beg, sep, &end ) )
        {
            if( beg == end ) break;
            else
            {
                ++tokenCount;
                beg = end;
                end = NULL;
            }
        }
        if( 0 == tokenCount ) return cson_rc.RangeError;
        beg = path;
        end = NULL;
        for( i = 0; i < tokenCount; ++i, beg=end, end=NULL )
        {
            rc = cson_next_token( &beg, sep, &end );
            assert( 1 == rc );
            assert( beg != end );
            assert( end > beg );
            len = end - beg;
            if( len > (BufSize-1) ) return cson_rc.RangeError;
            memset( buf, 0, len + 1 );
            memcpy( buf, beg, len );
            buf[len] = 0;
            cv = cson_object_get( curObj, buf );
            if( NULL == cv ) return cson_rc.NotFoundError;
            else if( i == (tokenCount-1) )
            {
                if(tgt) *tgt = cv;
                return 0;
            }
            else if( cson_value_is_object(cv) )
            {
                curObj = cson_value_get_object(cv);
                assert((NULL != curObj) && "Detected mis-management of internal memory!");
            }
            /* TODO: arrays. Requires numeric parsing for the index. */
            else
            {
                return cson_rc.NotFoundError;
            }
        }
        assert( i == tokenCount );
        return cson_rc.NotFoundError;
    }
}

cson_value * cson_object_get_sub( cson_object const * obj, char const * path, char sep )
{
    cson_value * v = NULL;
    cson_object_fetch_sub( obj, &v, path, sep );
    return v;
}

cson_value * cson_object_get_sub2( cson_object const * obj, char const * path )
{
    cson_value * v = NULL;
    cson_object_fetch_sub2( obj, &v, path );
    return v;
}


/**
   If v is-a Object or Array then this function returns a deep
   clone, otherwise it returns v. In either case, the refcount
   of the returned value is increased by 1 by this call.
*/
static cson_value * cson_value_clone_ref( cson_value * v )
{
    cson_value * rc = NULL;
#define TRY_SHARING 1
#if TRY_SHARING
    if(!v ) return rc;
    else if( cson_value_is_object(v)
             || cson_value_is_array(v))
    {
        rc = cson_value_clone( v );
    }
    else
    {
        rc = v;
    }
#else
    rc = cson_value_clone(v);
#endif
#undef TRY_SHARING
    cson_value_add_reference(rc);
    return rc;
}
    
static cson_value * cson_value_clone_array( cson_value const * orig )
{
    unsigned int i = 0;
    cson_array const * asrc = cson_value_get_array( orig );
    unsigned int alen = cson_array_length_get( asrc );
    cson_value * destV = NULL;
    cson_array * destA = NULL;
    assert( orig && asrc );
    destV = cson_value_new_array();
    if( NULL == destV ) return NULL;
    destA = cson_value_get_array( destV );
    assert( destA );
    if( 0 != cson_array_reserve( destA, alen ) )
    {
        cson_value_free( destV );
        return NULL;
    }
    for( ; i < alen; ++i )
    {
        cson_value * ch = cson_array_get( asrc, i );
        if( NULL != ch )
        {
            cson_value * cl = cson_value_clone_ref( ch );
            if( NULL == cl )
            {
                cson_value_free( destV );
                return NULL;
            }
            if( 0 != cson_array_set( destA, i, cl ) )
            {
                cson_value_free( cl );
                cson_value_free( destV );
                return NULL;
            }
            cson_value_free(cl)/*remove our artificial reference */;
        }
    }
    return destV;
}
    
static cson_value * cson_value_clone_object( cson_value const * orig )
{
    cson_object const * src = cson_value_get_object( orig );
    cson_value * destV = NULL;
    cson_object * dest = NULL;
    cson_kvp const * kvp = NULL;
    cson_object_iterator iter = cson_object_iterator_empty;
    assert( orig && src );
    if( 0 != cson_object_iter_init( src, &iter ) )
    {
        return NULL;
    }
    destV = cson_value_new_object();
    if( NULL == destV ) return NULL;
    dest = cson_value_get_object( destV );
    assert( dest );
    if( src->kvp.count > cson_kvp_list_reserve( &dest->kvp, src->kvp.count ) ){
        cson_value_free( destV );
        return NULL;
    }
    while( (kvp = cson_object_iter_next( &iter )) )
    {
        cson_value * key = NULL;
        cson_value * val = NULL;
        assert( kvp->key && (kvp->key->refcount>0) );
        key = cson_value_clone_ref(kvp->key);
        val = key ? cson_value_clone_ref(kvp->value) : NULL;
        if( ! key || !val ){
            goto error;
        }
        assert( CSON_STR(key) );
        if( 0 != cson_object_set_s( dest, CSON_STR(key), val ) )
        {
            goto error;
        }
        /* remove our references */
        cson_value_free(key);
        cson_value_free(val);
        continue;
        error:
        cson_value_free(key);
        cson_value_free(val);
        cson_value_free(destV);
        destV = NULL;
        break;
    }
    return destV;
}

cson_value * cson_value_clone( cson_value const * orig )
{
    if( NULL == orig ) return NULL;
    else
    {
        switch( orig->api->typeID )
        {
          case CSON_TYPE_UNDEF:
              assert(0 && "This should never happen.");
              return NULL;
          case CSON_TYPE_NULL:
              return cson_value_null();
          case CSON_TYPE_BOOL:
              return cson_value_new_bool( cson_value_get_bool( orig ) );
          case CSON_TYPE_INTEGER:
              return cson_value_new_integer( cson_value_get_integer( orig ) );
              break;
          case CSON_TYPE_DOUBLE:
              return cson_value_new_double( cson_value_get_double( orig ) );
              break;
          case CSON_TYPE_STRING: {
              cson_string const * str = cson_value_get_string( orig );
              return cson_value_new_string( cson_string_cstr( str ),
                                            cson_string_length_bytes( str ) );
          }
          case CSON_TYPE_ARRAY:
              return cson_value_clone_array( orig );
          case CSON_TYPE_OBJECT:
              return cson_value_clone_object( orig );
        }
        assert( 0 && "We can't get this far." );
        return NULL;
    }
}

cson_value * cson_string_value(cson_string const * s)
{
#define MT CSON_SPECIAL_VALUES[CSON_VAL_STR_EMPTY]
    return s
        ? ((s==MT.value) ? &MT : CSON_VCAST(s))
        : NULL;
#undef MT
}

cson_value * cson_object_value(cson_object const * s)
{
    return s
        ? CSON_VCAST(s)
        : NULL;
}


cson_value * cson_array_value(cson_array const * s)
{
    return s
        ? CSON_VCAST(s)
        : NULL;
}

void cson_free_object(cson_object *x)
{
    if(x) cson_value_free(cson_object_value(x));
}
void cson_free_array(cson_array *x)
{
    if(x) cson_value_free(cson_array_value(x));
}

void cson_free_string(cson_string *x)
{
    if(x) cson_value_free(cson_string_value(x));
}
void cson_free_value(cson_value *x)
{
    if(x) cson_value_free(x);
}


#if 0
/* i'm not happy with this... */
char * cson_pod_to_string( cson_value const * orig )
{
    if( ! orig ) return NULL;
    else
    {
        enum { BufSize = 64 };
        char * v = NULL;
        switch( orig->api->typeID )
        {
          case CSON_TYPE_BOOL: {
              char const bv = cson_value_get_bool(orig);
              v = cson_strdup( bv ? "true" : "false",
                               bv ? 4 : 5 );
              break;
          }
          case CSON_TYPE_UNDEF:
          case CSON_TYPE_NULL: {
              v = cson_strdup( "null", 4 );
              break;
          }
          case CSON_TYPE_STRING: {
              cson_string const * jstr = cson_value_get_string(orig);
              unsigned const int slen = cson_string_length_bytes( jstr );
              assert( NULL != jstr );
              v = cson_strdup( cson_string_cstr( jstr ), slen ); 
              break;
          }
          case CSON_TYPE_INTEGER: {
              char buf[BufSize] = {0};
              if( 0 < sprintf( v, "%"CSON_INT_T_PFMT, cson_value_get_integer(orig)) )
              {
                  v = cson_strdup( buf, strlen(buf) );
              }
              break;
          }
          case CSON_TYPE_DOUBLE: {
              char buf[BufSize] = {0};
              if( 0 < sprintf( v, "%"CSON_DOUBLE_T_PFMT, cson_value_get_double(orig)) )
              {
                  v = cson_strdup( buf, strlen(buf) );
              }
              break;
          }
          default:
              break;
        }
        return v;
    }
}
#endif

#if 0
/* i'm not happy with this... */
char * cson_pod_to_string( cson_value const * orig )
{
    if( ! orig ) return NULL;
    else
    {
        enum { BufSize = 64 };
        char * v = NULL;
        switch( orig->api->typeID )
        {
          case CSON_TYPE_BOOL: {
              char const bv = cson_value_get_bool(orig);
              v = cson_strdup( bv ? "true" : "false",
                               bv ? 4 : 5 );
              break;
          }
          case CSON_TYPE_UNDEF:
          case CSON_TYPE_NULL: {
              v = cson_strdup( "null", 4 );
              break;
          }
          case CSON_TYPE_STRING: {
              cson_string const * jstr = cson_value_get_string(orig);
              unsigned const int slen = cson_string_length_bytes( jstr );
              assert( NULL != jstr );
              v = cson_strdup( cson_string_cstr( jstr ), slen ); 
              break;
          }
          case CSON_TYPE_INTEGER: {
              char buf[BufSize] = {0};
              if( 0 < sprintf( v, "%"CSON_INT_T_PFMT, cson_value_get_integer(orig)) )
              {
                  v = cson_strdup( buf, strlen(buf) );
              }
              break;
          }
          case CSON_TYPE_DOUBLE: {
              char buf[BufSize] = {0};
              if( 0 < sprintf( v, "%"CSON_DOUBLE_T_PFMT, cson_value_get_double(orig)) )
              {
                  v = cson_strdup( buf, strlen(buf) );
              }
              break;
          }
          default:
              break;
        }
        return v;
    }
}
#endif

unsigned int cson_value_msize(cson_value const * v)
{
    if(!v) return 0;
    else if( cson_value_is_builtin(v) ) return 0;
    else {
        unsigned int rc = sizeof(cson_value);
        assert(NULL != v->api);
        switch(v->api->typeID){
          case CSON_TYPE_INTEGER:
              assert( v != &CSON_SPECIAL_VALUES[CSON_VAL_INT_0]);
              rc += sizeof(cson_int_t);
              break;
          case CSON_TYPE_DOUBLE:
              assert( v != &CSON_SPECIAL_VALUES[CSON_VAL_DBL_0]);
              rc += sizeof(cson_double_t);
              break;
          case CSON_TYPE_STRING:
              rc += sizeof(cson_string)
                  + CSON_STR(v)->length + 1/*NUL*/;
              break;
          case CSON_TYPE_ARRAY:{
              cson_array const * ar = CSON_ARRAY(v);
              cson_value_list const * li;
              unsigned int i = 0;
              assert( NULL != ar );
              li = &ar->list;
              rc += sizeof(cson_array)
                  + (li->alloced * sizeof(cson_value *));
              for( ; i < li->count; ++i ){
                  cson_value const * e = ar->list.list[i];
                  if( e ) rc += cson_value_msize( e );
              }
              break;
          }
          case CSON_TYPE_OBJECT:{
              cson_object const * obj = CSON_OBJ(v);
              unsigned int i = 0;
              cson_kvp_list const * kl;
              assert(NULL != obj);
              kl = &obj->kvp;
              rc += sizeof(cson_object)
                  + (kl->alloced * sizeof(cson_kvp*));
              for( ; i < kl->count; ++i ){
                  cson_kvp const * kvp = kl->list[i];
                  assert(NULL != kvp);
                  rc += cson_value_msize(kvp->key);
                  rc += cson_value_msize(kvp->value);
              }
              break;
          }
          case CSON_TYPE_UNDEF:
          case CSON_TYPE_NULL:
          case CSON_TYPE_BOOL:
              assert( 0 && "Should have been caught by is-builtin check!" );
              break;
          default:
              assert(0 && "Invalid typeID!");
              return 0;
#undef RCCHECK
        }
        return rc;
    }
}

int cson_object_merge( cson_object * dest, cson_object const * src, int flags ){
    cson_object_iterator iter = cson_object_iterator_empty;
    int rc;
    char const replace = (flags & CSON_MERGE_REPLACE);
    char const recurse = !(flags & CSON_MERGE_NO_RECURSE);
    cson_kvp const * kvp;
    if((!dest || !src) || (dest==src)) return cson_rc.ArgError;
    rc = cson_object_iter_init( src, &iter );
    if(rc) return rc;
    while( (kvp = cson_object_iter_next(&iter) ) )
    {
        cson_string * key = cson_kvp_key(kvp);
        cson_value * val = cson_kvp_value(kvp);
        cson_value * check = cson_object_get_s( dest, key );
        if(!check){
            cson_object_set_s( dest, key, val );
            continue;
        }
        else if(!replace && !recurse) continue;
        else if(replace && !recurse){
            cson_object_set_s( dest, key, val );
            continue;
        }
        else if( recurse ){
            if( cson_value_is_object(check) &&
                cson_value_is_object(val) ){
                rc = cson_object_merge( cson_value_get_object(check),
                                        cson_value_get_object(val),
                                        flags );
                if(rc) return rc;
                else continue;
            }
            else continue;
        }
        else continue;
    }
    return 0;
}

static cson_value * cson_guess_arg_type(char const *arg){
    char * end = NULL;
    if(!arg || !*arg) return cson_value_null();
    else if(('0'>*arg) || ('9'<*arg)){
        goto do_string;
    }
    else{ /* try numbers... */
        long const val = strtol(arg, &end, 10);
        if(!*end){
            return cson_value_new_integer( (cson_int_t)val);
        }
        else if( '.' != *end ) {
            goto do_string;
        }
        else {
            double const val = strtod(arg, &end);
            if(!*end){
                return cson_value_new_double(val);
            }
        }
    }
    do_string:
    return cson_value_new_string(arg, strlen(arg));
}


int cson_parse_argv_flags( int argc, char const * const * argv,
                           cson_object ** tgt, unsigned int * count ){
    cson_object * o = NULL;
    int rc = 0;
    int i = 0;
    if(argc<1 || !argc || !tgt) return cson_rc.ArgError;
    o = *tgt ? *tgt : cson_new_object();
    if(count) *count = 0;
    for( i = 0; i < argc; ++i ){
        char const * arg = argv[i];
        char const * key = arg;
        char const * pos;
        cson_string * k = NULL;
        cson_value * v = NULL;
        if('-' != *arg) continue;
        while('-'==*key) ++key;
        if(!*key) continue;
        pos = key;
        while( *pos && ('=' != *pos)) ++pos;
        k = cson_new_string(key, pos-key);
        if(!k){
            rc = cson_rc.AllocError;
            break;
        }
        if(!*pos){ /** --key */
            v = cson_value_true();
        }else{ /** --key=...*/
            assert('=' == *pos);
            ++pos /*skip '='*/;
            v = cson_guess_arg_type(pos);
        }
        if(0 != (rc=cson_object_set_s(o, k, v))){
            cson_free_string(k);
            cson_value_free(v);
            break;
        }
        else if(count) ++*count;
    }
    if(o != *tgt){
        if(rc) cson_free_object(o);
        else *tgt = o;
    }
    return rc;
}

#if defined(__cplusplus)
} /*extern "C"*/
#endif

#undef MARKER
#undef CSON_OBJECT_PROPS_SORT
#undef CSON_OBJECT_PROPS_SORT_USE_LENGTH
#undef CSON_CAST
#undef CSON_INT
#undef CSON_DBL
#undef CSON_STR
#undef CSON_OBJ
#undef CSON_ARRAY
#undef CSON_VCAST
#undef CSON_MALLOC_IMPL
#undef CSON_FREE_IMPL
#undef CSON_REALLOC_IMPL
/* end file ./cson.c */
/* begin file ./cson_lists.h */
/* Auto-generated from cson_list.h. Edit at your own risk! */
unsigned int cson_value_list_reserve( cson_value_list * self, unsigned int n )
{
    if( !self ) return 0;
    else if(0 == n)
    {
        if(0 == self->alloced) return 0;
        cson_free(self->list, "cson_value_list_reserve");
        self->list = NULL;
        self->alloced = self->count = 0;
        return 0;
    }
    else if( self->alloced >= n )
    {
        return self->alloced;
    }
    else
    {
        size_t const sz = sizeof(cson_value *) * n;
        cson_value * * m = (cson_value **)cson_realloc( self->list, sz, "cson_value_list_reserve" );
        if( ! m ) return self->alloced;

        memset( m + self->alloced, 0, (sizeof(cson_value *)*(n-self->alloced)));
        self->alloced = n;
        self->list = m;
        return n;
    }
}
int cson_value_list_append( cson_value_list * self, cson_value * cp )
{
    if( !self || !cp ) return cson_rc.ArgError;
    else if( self->alloced > cson_value_list_reserve(self, self->count+1) )
    {
        return cson_rc.AllocError;
    }
    else
    {
        self->list[self->count++] = cp;
        return 0;
    }
}
int cson_value_list_visit( cson_value_list * self,

                        int (*visitor)(cson_value * obj, void * visitorState ),



                        void * visitorState )
{
    int rc = cson_rc.ArgError;
    if( self && visitor )
    {
        unsigned int i = 0;
        for( rc = 0; (i < self->count) && (0 == rc); ++i )
        {

            cson_value * obj = self->list[i];



            if(obj) rc = visitor( obj, visitorState );
        }
    }
    return rc;
}
void cson_value_list_clean( cson_value_list * self,

                         void (*cleaner)(cson_value * obj)



                         )
{
    if( self && cleaner && self->count )
    {
        unsigned int i = 0;
        for( ; i < self->count; ++i )
        {

            cson_value * obj = self->list[i];



            if(obj) cleaner(obj);
        }
    }
    cson_value_list_reserve(self,0);
}
unsigned int cson_kvp_list_reserve( cson_kvp_list * self, unsigned int n )
{
    if( !self ) return 0;
    else if(0 == n)
    {
        if(0 == self->alloced) return 0;
        cson_free(self->list, "cson_kvp_list_reserve");
        self->list = NULL;
        self->alloced = self->count = 0;
        return 0;
    }
    else if( self->alloced >= n )
    {
        return self->alloced;
    }
    else
    {
        size_t const sz = sizeof(cson_kvp *) * n;
        cson_kvp * * m = (cson_kvp **)cson_realloc( self->list, sz, "cson_kvp_list_reserve" );
        if( ! m ) return self->alloced;

        memset( m + self->alloced, 0, (sizeof(cson_kvp *)*(n-self->alloced)));
        self->alloced = n;
        self->list = m;
        return n;
    }
}
int cson_kvp_list_append( cson_kvp_list * self, cson_kvp * cp )
{
    if( !self || !cp ) return cson_rc.ArgError;
    else if( self->alloced > cson_kvp_list_reserve(self, self->count+1) )
    {
        return cson_rc.AllocError;
    }
    else
    {
        self->list[self->count++] = cp;
        return 0;
    }
}
int cson_kvp_list_visit( cson_kvp_list * self,

                        int (*visitor)(cson_kvp * obj, void * visitorState ),



                        void * visitorState )
{
    int rc = cson_rc.ArgError;
    if( self && visitor )
    {
        unsigned int i = 0;
        for( rc = 0; (i < self->count) && (0 == rc); ++i )
        {

            cson_kvp * obj = self->list[i];



            if(obj) rc = visitor( obj, visitorState );
        }
    }
    return rc;
}
void cson_kvp_list_clean( cson_kvp_list * self,

                         void (*cleaner)(cson_kvp * obj)



                         )
{
    if( self && cleaner && self->count )
    {
        unsigned int i = 0;
        for( ; i < self->count; ++i )
        {

            cson_kvp * obj = self->list[i];



            if(obj) cleaner(obj);
        }
    }
    cson_kvp_list_reserve(self,0);
}
/* end file ./cson_lists.h */
/* end file cson_amalgamation_core.c */
/* begin file cson_amalgamation_session.c */
/* auto-generated! Do not edit! */
/* begin file ./cson_session.c */
#include <string.h> /* strlen() */
#include <stdlib.h> /* memcpy() */
#include <assert.h>


/**
   Maximum name length for cson_sessmgr_register(),
   including the trailing NUL.
*/
enum {
   CsonSessionNameLen = 32
};

typedef struct cson_sessmgr_reg cson_sessmgr_reg;
/**
   Holds name-to-factory mappings for cson_sessmgr implementations.
*/
struct cson_sessmgr_reg
{
    char name[CsonSessionNameLen];
    cson_sessmgr_factory_f factory;
};


#if !CSON_ENABLE_CPDO
int cson_sessmgr_cpdo( cson_sessmgr ** tgt, cson_object const * opt )
{
    return cson_rc.UnsupportedError;
}
#endif
#if !CSON_ENABLE_WHIO
int cson_sessmgr_whio_ht( cson_sessmgr ** tgt, cson_object const * opt )
{
    return cson_rc.UnsupportedError;
}
int cson_sessmgr_whio_epfs( cson_sessmgr ** tgt, cson_object const * opt )
{
    return cson_rc.UnsupportedError;
}
#endif

/**
   Holds the list of registered cson_sessmgr implementations. Used by
   cson_sessmgr_register(), cson_sessmgr_load(), and
   cson_sessmgr_names().

   Maintenance reminder: the API docs promise that at least 10 slots
   are initially available.
*/
static cson_sessmgr_reg CsonSessionReg[] = {
{{'f','i','l','e',0},cson_sessmgr_file},
#if CSON_ENABLE_CPDO
{{'c','p','d','o',0},cson_sessmgr_cpdo},
#endif
#if CSON_ENABLE_WHIO
{{'w','h','i','o','_','h','t',0},cson_sessmgr_whio_ht},
{{'w','h','i','o','_','e','p','f','s',0},cson_sessmgr_whio_epfs},
#endif
#define REG {{0},NULL}
REG,REG,REG,REG,REG,
REG,REG,REG,REG,REG
#undef REG
};
static const unsigned int CsonSessionRegLen = sizeof(CsonSessionReg)/sizeof(CsonSessionReg[0]);


int cson_sessmgr_register( char const * name, cson_sessmgr_factory_f f )
{
    if( ! name || !*name || !f ) return cson_rc.ArgError;
    else
    {
        cson_sessmgr_reg * r = CsonSessionReg;
        unsigned int nlen = strlen(name);
        unsigned int i = 0;
        if( nlen >= CsonSessionNameLen ) return cson_rc.RangeError;
        for( ; i < CsonSessionRegLen; ++i, ++r )
        {
            if( r->name[0] ) continue;
            memcpy( r->name, name, nlen );
            r->name[nlen] = 0;
            r->factory = f;
            return 0;
        }
        return cson_rc.RangeError;
    }
}


int cson_sessmgr_load( char const * name, cson_sessmgr ** tgt, cson_object const * opt )
{
    if( ! name || !*name || !tgt ) return cson_rc.ArgError;
    else
    {
        cson_sessmgr_reg const * r = CsonSessionReg;
        unsigned int i = 0;
        for( ; i < CsonSessionRegLen; ++i, ++r )
        {
            if( ! r->name[0] ) break /* end of list */;
            else if( 0 != strcmp( r->name, name ) ) continue;
            else
            {
                assert( NULL != r->factory );
                return r->factory( tgt, opt );
            }
            
        }
        return cson_rc.UnsupportedError;
    }
}

char const * const * cson_sessmgr_names()
{
    static char const * names[sizeof(CsonSessionReg)/sizeof(CsonSessionReg[0])+1];
    unsigned int i = 0;
    cson_sessmgr_reg const * r = CsonSessionReg;
    for( ; i < CsonSessionRegLen; ++i, ++r )
    {
        /*
          pedantic threading note: as long as this function is not
          used concurrently with cson_sessmgr_register(), the worst we
          will do here if this function is called, or its results
          used, concurrently is overwrite in-use values with the same
          values.
         */
        names[i] = r->name[0] ? r->name : NULL;
    }
    names[i] = NULL;
    return names;
}
/* end file ./cson_session.c */
/* begin file ./cson_session_file.c */
#if !defined(_WIN32) && !defined(_WIN64)
#  if !defined(_POSIX_VERSION)
#    define _POSIX_VERSION 200112L /* chmod(), unlink() */
#  endif
#  define ENABLE_POSIX_FILE_OPS 1
#else
#  define ENABLE_POSIX_FILE_OPS 0
#endif

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#if ENABLE_POSIX_FILE_OPS
#  define UNLINK_FILE unlink
#  include <unistd.h> /* unlink() */
#  include <sys/stat.h> /* chmod() */
#else
/* http://msdn.microsoft.com/en-us/library/1c3tczd6(v=vs.80).aspx
  #  define UNLINK_FILE _unlink
  #  include <io.h>
*/
#  define UNLINK_FILE remove
#  include <stdio.h> /* remove(), _unlink() */
#endif

static int cson_session_file_load( cson_sessmgr * self, cson_value ** tgt, char const * id );
static int cson_session_file_save( cson_sessmgr * self, cson_value const * root, char const * id );
static int cson_session_file_remove( cson_sessmgr * self, char const * id );
static void cson_session_file_finalize( cson_sessmgr * self );

static const cson_sessmgr_api cson_sessmgr_api_file =
{
    cson_session_file_load,
    cson_session_file_save,
    cson_session_file_remove,
    cson_session_file_finalize
};

typedef struct cson_sessmgr_file_impl cson_sessmgr_file_impl;
struct cson_sessmgr_file_impl
{
    char * dir;
    char * prefix;
    char * suffix;
};

static const cson_sessmgr cson_sessmgr_file_empty =
{
    &cson_sessmgr_api_file,
    NULL
};

static char * cson_session_file_strdup( char const * src )
{
    size_t const n = src ? strlen(src) : 0;
    char * rc = src ? (char *)calloc(1, n+1) : NULL;
    if( ! rc ) return NULL;
    memcpy( rc, src, n );
    return rc;
}

/* Helper macro for varios cson_sessmgr_api member implementations. */
#define IMPL_DECL(RC) \
    cson_sessmgr_file_impl * impl = (self && (self->api == &cson_sessmgr_api_file)) \
    ? (cson_sessmgr_file_impl*)self->impl \
    : NULL;                                   \
    if( NULL == impl ) return RC

static int cson_session_file_name( cson_sessmgr_file_impl * impl,
                                   char const * id,
                                   char * buf, unsigned int bufLen )
{
    char const * dir = impl->dir ? impl->dir : ".";
    char const * pre = impl->prefix ? impl->prefix : "";
    char const * suf = impl->suffix ? impl->suffix : "";
    char * pos = NULL /* current write possition. */;
    unsigned int flen = 0 /* length of the next token. */;
    unsigned int olen = 0 /* total number of bytes written so far. */;
    if( ! id || !*id ) return cson_rc.ArgError;

#define CHECKLEN if(olen >= bufLen) return cson_rc.RangeError; assert( pos < (buf+bufLen) )
    pos = buf;

#define PUSH(FIELD) \
    flen = strlen(FIELD); \
    olen += flen; \
    CHECKLEN; \
    strncpy( pos, FIELD, flen ); \
    pos += flen

    PUSH(dir);

    ++olen;
    CHECKLEN;
#if defined(_WIN32)
    *(pos++) = '\\';
#else
    *(pos++) = '/';
#endif

    PUSH(pre);
    PUSH(id);
    PUSH(suf);
    if( pos >= (buf + bufLen) ) return cson_rc.RangeError;
    *pos = 0;
    return 0;
#undef PUSH
#undef CHECKLEN
}

static int cson_session_file_load( cson_sessmgr * self, cson_value ** root, char const * id )
{
    enum { BufSize = 1024 };
    char fname[BufSize];
    FILE * fh = NULL;
    int rc;
    IMPL_DECL(cson_rc.ArgError);
    if( !root || !id || !*id ) return cson_rc.ArgError;
    memset( fname, 0, BufSize );
    rc = cson_session_file_name( impl, id, fname, BufSize );
    if( 0 != rc ) return rc;
    fh = fopen( fname, "r" );
    if( ! fh ) return cson_rc.IOError;
    rc = cson_parse_FILE( root, fh, NULL, NULL );
    fclose( fh );
    return rc;
}

static int cson_session_file_save( cson_sessmgr * self, cson_value const * root, char const * id )
{
    enum { BufSize = 1024 };
    char fname[BufSize];
    FILE * fh = NULL;
    int rc;
    IMPL_DECL(cson_rc.ArgError);
    if( !root || !id || !*id ) return cson_rc.ArgError;
    memset( fname, 0, BufSize );

    rc = cson_session_file_name( impl, id, fname, BufSize );
    if( 0 != rc ) return rc;
    fh = fopen( fname, "w" );
    if( ! fh ) return cson_rc.IOError;
#if ENABLE_POSIX_FILE_OPS
    chmod( fname, 0600 );
#endif
    rc = cson_output_FILE( root, fh, NULL );
    fclose( fh );
    if( rc )
    {
        UNLINK_FILE( fname );
    }
    return rc;
}

void cson_session_file_finalize( cson_sessmgr * self )
{
    if( self && (self->api == &cson_sessmgr_api_file) )
    {
        cson_sessmgr_file_impl * impl = (cson_sessmgr_file_impl *)self->impl;
        free( impl->dir );
        free( impl->prefix );
        free( impl->suffix );
        free( impl );
        *self = cson_sessmgr_file_empty;
        free( self );
    }
}

static int cson_session_file_remove( cson_sessmgr * self, char const * id )
{
    enum { BufSize = 1024 };
    char fname[BufSize];
    int rc;
    IMPL_DECL(cson_rc.ArgError);
    if( !id || !*id ) return cson_rc.ArgError;
    memset( fname, 0, BufSize );
    rc = cson_session_file_name( impl, id, fname, BufSize );
    if( 0 != rc ) return rc;
    rc = UNLINK_FILE( fname );
    return (0==rc) ? 0 : cson_rc.IOError;
}


int cson_sessmgr_file( cson_sessmgr ** tgt, cson_object const * opt )
{
    int rc;
    cson_sessmgr * m = tgt ? (cson_sessmgr *)malloc(sizeof(cson_sessmgr)) : NULL;
    cson_sessmgr_file_impl * impl = m
        ? (cson_sessmgr_file_impl *)malloc(sizeof(cson_sessmgr_file_impl))
        : NULL;
    if( ! m ) return tgt ? cson_rc.AllocError : cson_rc.ArgError;
    else if( ! impl )
    {
        free(m);
        return cson_rc.AllocError;
    }
    *m = cson_sessmgr_file_empty;
    m->impl = impl;
    if( opt )
    {
        cson_string const * jstr;
#define CP(KEY) \
        jstr = cson_value_get_string( cson_object_get( opt, # KEY ) ); \
        if( jstr ) { \
            impl->KEY = cson_session_file_strdup( cson_string_cstr( jstr ) ); \
            if( ! impl->KEY ) { \
                rc = cson_rc.AllocError;        \
                goto error_clean;               \
            } \
        } (void)0
        
        CP(dir);
        CP(prefix);
        CP(suffix);
#undef CP
    }
#define CP(KEY,VAL) if( ! impl->KEY ) { \
            impl->KEY = cson_session_file_strdup(VAL); \
            if( ! impl->KEY ) { \
                rc = cson_rc.AllocError;        \
                goto error_clean;               \
            } \
        } (void)0
#if ENABLE_POSIX_FILE_OPS
    CP(dir,"/tmp");
#else
    CP(dir,".");
#endif
    CP(prefix,"cson-session-");
    CP(suffix,".json");
#undef CP
    *tgt = m;
    return 0;
    error_clean:
    m->api->finalize( m );
    return rc;
}

#undef IMPL_DECL
#undef ENABLE_POSIX_FILE_OPS
#undef UNLINK_FILE
/* end file ./cson_session_file.c */
/* begin file ./cson_cpdo.c */
#if CSON_ENABLE_CPDO /* we do this here for the sake of the amalgamation build */
#include <string.h> /* strlen() */
#include <assert.h>

#if 0
#include <stdio.h>
#define MARKER if(1) printf("MARKER: %s:%d:%s():\t",__FILE__,__LINE__,__func__); if(1) printf
#else
#define MARKER if(0) printf
#endif

#if defined(__cplusplus)
extern "C" {
#endif

cson_value * cson_cpdo_stmt_to_value( cpdo_stmt * st, uint16_t ndx )
{
    if( ! st ) return NULL;
    else
    {
        cpdo_data_type vtype = CPDO_TYPE_ERROR;
        int rc = st->api->get.type( st, ndx, &vtype );
        MARKER("ndx=%u, vtype=%d, rc=%d\n",ndx, vtype,rc);
        if( rc ) return NULL;
        switch( vtype )
        {
          case CPDO_TYPE_NULL:
              return cson_value_null();
          case CPDO_TYPE_INT8:
              return cson_value_new_integer( (cson_int_t)cpdo_get_int8(st,ndx) );
          case CPDO_TYPE_INT16:
              return cson_value_new_integer( (cson_int_t)cpdo_get_int16(st,ndx) );
          case CPDO_TYPE_INT32:
              return cson_value_new_integer( (cson_int_t)cpdo_get_int32(st,ndx) );
          case CPDO_TYPE_INT64:
              return cson_value_new_integer( (cson_int_t)cpdo_get_int64(st,ndx) );
          case CPDO_TYPE_DOUBLE:
              return cson_value_new_double( cpdo_get_double(st, ndx) );
          case CPDO_TYPE_BLOB: {
              /* we'll just hope it's really a string. MySQL reports
                 TEXT fields as type BLOB.
              */
              uint32_t slen = 0;
              void const * bl = cpdo_get_blob( st, ndx, &slen );
              return cson_value_new_string(bl, bl ? slen : 0);
          }
          case CPDO_TYPE_CUSTOM /* we do this for MySQL time/date types */:
          case CPDO_TYPE_STRING: {
              uint32_t slen = 0;
              char const * str = cpdo_get_string( st, ndx, &slen );
              return cson_value_new_string(str, str ? slen : 0);
          }
          default:
              return NULL;
        }
    }
}

cson_value * cson_cpdo_row_to_object2( cpdo_stmt * st, cson_array * colNames )
{
    uint16_t colCount;
    cson_value * objV;
    cson_object * obj;
    cson_string * colName;
    uint16_t i;
    int rc;
    if( ! st ) return NULL;
    colCount = st->api->get.column_count( st );
    if( !colCount || (colCount>cson_array_length_get(colNames)) ) {
        return NULL;
    }
    objV = cson_value_new_object();
    if( ! objV ) return NULL;
    obj = cson_value_get_object( objV );
    for( i = 0; i < colCount; ++i )
    {
        cson_value * v = NULL;
        colName = cson_value_get_string( cson_array_get( colNames, i ) );
        if( ! colName ) goto error;
        v = cson_cpdo_stmt_to_value( st, i );
        if( ! v ) goto error;
        rc = cson_object_set_s( obj, colName, v );
        if( rc )
        {
            cson_value_free(v);
            goto error;
        }
    }
    return objV;
    error:
    cson_value_free( objV );
    return NULL;
}

cson_value * cson_cpdo_row_to_object( cpdo_stmt * st )
{
    uint16_t colCount;
    cson_value * objV;
    cson_object * obj;
    char const * colName;
    uint16_t i;
    int rc;
    if( ! st ) return NULL;
    colCount = st->api->get.column_count( st );
    if( ! colCount ) return NULL;
    objV = cson_value_new_object();
    if( ! objV ) return NULL;
    obj = cson_value_get_object( objV );
    for( i = 0; i < colCount; ++i )
    {
        cson_value * v = NULL;
        colName = st->api->get.column_name( st, i );
        if( ! colName ) goto error;
        v = cson_cpdo_stmt_to_value( st, i );
        if( ! v ) goto error;
        rc = cson_object_set( obj, colName, v );
        if( rc )
        {
            cson_value_free(v);
            goto error;
        }
    }
    return objV;
    error:
    cson_value_free( objV );
    return NULL;
}

cson_value * cson_cpdo_row_to_array( cpdo_stmt * st )
{
    uint16_t colCount;
    cson_value * arV;
    cson_array * ar;
    uint16_t i;
    int rc;
    if( ! st ) return NULL;
    colCount = st->api->get.column_count( st );
    if( ! colCount ) return NULL;
    arV = cson_value_new_array();
    if( ! arV ) return NULL;
    ar = cson_value_get_array( arV );
    for( i = 0; i < colCount; ++i )
    {
        cson_value * v = NULL;
        v = cson_cpdo_stmt_to_value( st, i );
        if( ! v ) goto error;
        rc = cson_array_append( ar, v );
        if( rc )
        {
            cson_value_free(v);
            goto error;
        }
    }
    return arV;
    error:
    cson_value_free( arV );
    return NULL;
}

cson_value * cson_cpdo_stmt_col_names( cpdo_stmt * st )
{
    cson_value * aryV = NULL;
    cson_array * ary = NULL;
    char const * colName = NULL;
    uint16_t i = 0;
    int rc = 0;
    uint16_t colCount = 0;
    cson_value * newVal = NULL;
    assert(st);
    colCount = st->api->get.column_count( st );
    if( ! colCount ) return NULL;
    aryV = cson_value_new_array();
    if( ! aryV ) return NULL;
    ary = cson_value_get_array(aryV);
    assert(ary);
    for( i = 0; (0==rc) && (i < colCount); ++i )
    {
        colName = st->api->get.column_name(st, i);
        if( ! colName ) rc = cson_rc.AllocError;
        else
        {
            newVal = cson_value_new_string(colName, strlen(colName));
            if( NULL == newVal )
            {
                rc = cson_rc.AllocError;
            }
            else
            {
                rc = cson_array_set( ary, i, newVal );
                if( rc ) cson_value_free( newVal );
            }
        }
    }
    if( 0 == rc ) return aryV;
    else
    {
        cson_value_free(aryV);
        return NULL;
    }
}

/**
    Internal impl of cson_sqlite3_stmt_to_json() when the 'fat'
    parameter is non-0.
*/
static int cson_cpdo_stmt_to_json_fat( cpdo_stmt * st, cson_value ** tgt )
{
#define RETURN(RC) { if(rootV) cson_value_free(rootV); return RC; } (void)0
    if( ! tgt || !st ) return cson_rc.ArgError;
    else
    {
        cson_value * rootV = NULL;
        cson_object * root = NULL;
        cson_value * colsV = NULL;
        cson_array * cols = NULL;
        cson_value * rowsV = NULL;
        cson_array * rows = NULL;
        cson_value * row = NULL;
        int rc = 0;
        uint16_t colCount = st->api->get.column_count(st);
        if( ! colCount ) return cson_rc.ArgError;
        rootV = cson_value_new_object();
        if( ! rootV ) return cson_rc.AllocError;
        colsV = cson_cpdo_stmt_col_names(st);
        if( ! colsV ) RETURN(cson_rc.AllocError);
        cols = cson_value_get_array(colsV);
        root = cson_value_get_object(rootV);
        rc = cson_object_set( root, "columns", colsV );
        if( rc )
        {
            cson_value_free(colsV);
            RETURN(rc);
        }
        colsV = NULL;
        
        rowsV = cson_value_new_array();
        if( ! rowsV ) RETURN(cson_rc.AllocError);
        rc = cson_object_set( root, "rows", rowsV );
        if( 0 != rc )
        {
            cson_value_free( rowsV );
            RETURN(rc);
        }
        rows = cson_value_get_array(rowsV);
        assert(rows);
        while( CPDO_STEP_OK == st->api->step(st) )
        {
            row = cson_cpdo_row_to_object2( st, cols );
            if( ! row ) RETURN(cson_rc.UnknownError);
            rc = cson_array_append( rows, row );
            if( 0 != rc )
            {
                cson_value_free( row );
                RETURN(rc);
            }
        }
        *tgt = rootV;
        return 0;
    }
#undef RETURN
}

/**
    Internal impl of cson_sqlite3_stmt_to_json() when the 'fat'
    parameter is 0.
*/
static int cson_cpdo_stmt_to_json_slim( cpdo_stmt * st, cson_value ** tgt )
{
#define RETURN(RC) { if(rootV) cson_value_free(rootV); return RC; }
    if( ! tgt || !st ) return cson_rc.ArgError;
    else
    {
        cson_value * rootV = NULL;
        cson_object * root = NULL;
        cson_value * aryV = NULL;
        cson_value * rowsV = NULL;
        cson_array * rows = NULL;
        int rc = 0;
        uint16_t const colCount = st->api->get.column_count(st);
        if( !colCount ) return cson_rc.ArgError;
        rootV = cson_value_new_object();
        if( ! rootV ) return cson_rc.AllocError;
        aryV = cson_cpdo_stmt_col_names(st);
        if( ! aryV ) RETURN(cson_rc.AllocError);
        root = cson_value_get_object(rootV);
        rc = cson_object_set( root, "columns", aryV );
        if( rc )
        {
            cson_value_free(aryV);
            RETURN(rc);
        }
        aryV = NULL;
        rowsV = cson_value_new_array();
        if( ! rowsV ) RETURN(cson_rc.AllocError);
        rc = cson_object_set( root, "rows", rowsV );
        if( 0 != rc )
        {
            cson_value_free( rowsV );
            RETURN(rc);
        }
        rows = cson_value_get_array(rowsV);
        assert(rows);
        while( CPDO_STEP_OK == st->api->step(st) )
        {
            aryV = cson_cpdo_row_to_array(st);
            if( ! aryV ) RETURN(cson_rc.UnknownError);
            rc = cson_array_append( rows, aryV );
            if( 0 != rc )
            {
                cson_value_free( aryV );
                RETURN(rc);
            }
        }
        *tgt = rootV;
        return 0;
    }
#undef RETURN
}

int cson_cpdo_stmt_to_json( cpdo_stmt * st, cson_value ** tgt, char fat )
{
    return fat
        ? cson_cpdo_stmt_to_json_fat(st,tgt)
        : cson_cpdo_stmt_to_json_slim(st,tgt)
        ;
}

int cson_cpdo_sql_to_json( cpdo_driver * db, cson_value ** tgt, char const * sql, char fat )
{
    if( !db || !tgt || !sql || !*sql ) return cson_rc.ArgError;
    else
    {
        cpdo_stmt * st = NULL;
        int rc = cpdo_prepare( db, &st, sql, strlen(sql) );
        if( 0 != rc ) return cson_rc.IOError /* FIXME: Better error code? */;
        rc = cson_cpdo_stmt_to_json( st, tgt, fat );
        st->api->finalize( st );
        return rc;
    }        
}

int cson_cpdo_parse_json( cpdo_stmt * st, uint16_t ndx, cson_value ** tgt )
{
    if( ! st || ! tgt) return cson_rc.ArgError;
    else
    {
        void const * blob = NULL;
        uint32_t slen = 0;
        int rc = st->api->get.blob( st, ndx, &blob, &slen );
        if( rc || ! slen)
        {
            char const * str = NULL;
            rc = st->api->get.string( st, ndx, &str, &slen );
            if( rc ) return cson_rc.TypeError;
            else if( !slen ) return 0;
            blob = str;
        }
        return cson_parse_string( tgt, (char const *)blob, slen, NULL, NULL );
    }
}

int cson_cpdo_bind_json( cpdo_stmt * st, uint16_t ndx, cson_value const * jv )
{
    if( ! st ) return cpdo_rc.ArgError;
    else if( ! jv || (!cson_value_is_array(jv) && !cson_value_is_object(jv)) )
    {
        return st->api->bind.null( st, ndx );
    }
    else
    {
        int rc = 0;
        cson_buffer buf = cson_buffer_empty;
        rc = cson_output_buffer( jv, &buf, NULL );
        if( rc ) return rc;
        rc = st->api->bind.blob( st, ndx, buf.mem, buf.used );
        if( rc )
        { /* try again as string ... */
             rc = st->api->bind.string( st, ndx, (char const *)buf.mem, buf.used );
        }
        cson_buffer_reserve( &buf, 0 );
        return rc;
    }
}    

int cson_cpdo_bind_value( cpdo_stmt * st, uint16_t ndx, cson_value const * v )
{
    if(!st || !ndx) return cpdo_rc.ArgError;
    else if( !v || cson_value_is_null(v) ){
        return st->api->bind.null(st, ndx);
    }
    else if( cson_value_is_integer(v) || cson_value_is_bool(v) ) {
        return st->api->bind.i64(st, ndx, cson_value_get_integer(v));
    }
    else if( cson_value_is_double(v) ) {
        return st->api->bind.dbl(st, ndx, cson_value_get_double(v));
    }
    else if( cson_value_is_string(v) ) {
        char const * str = cson_value_get_cstr(v);
        unsigned int slen = cson_string_length_bytes(cson_value_get_string(v));
        int rc = st->api->bind.string(st, ndx, str, slen);
        if(rc){ /* try again as blob... */
            rc = st->api->bind.blob(st, ndx, str, slen);
        }
        return rc;
    }
    else {
        return cpdo_rc.TypeError;
    }
}



#undef MARKER
#if defined(__cplusplus)
} /*extern "C"*/
#endif

#endif/*CSON_ENABLE_CPDO*/
/* end file ./cson_cpdo.c */
/* begin file ./cson_session_cpdo.c */
#if CSON_ENABLE_CPDO /* we do this only in the interest of the amalgamation build */
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if 0
#define MARKER if(1) printf("MARKER: %s:%d:%s():\t",__FILE__,__LINE__,__func__); if(1) printf
#else
#define MARKER if(0) printf
#endif


static int cson_session_cpdo_load( cson_sessmgr * self, cson_value ** tgt, char const * id );
static int cson_session_cpdo_save( cson_sessmgr * self, cson_value const * root, char const * id );
static int cson_session_cpdo_remove( cson_sessmgr * self, char const * id );
static void cson_session_cpdo_finalize( cson_sessmgr * self );

static const cson_sessmgr_api cson_sessmgr_api_cpdo =
{
    cson_session_cpdo_load,
    cson_session_cpdo_save,
    cson_session_cpdo_remove,
    cson_session_cpdo_finalize
};

typedef struct cson_sessmgr_cpdo_impl cson_sessmgr_cpdo_impl;
struct cson_sessmgr_cpdo_impl
{
    cpdo_driver * drv;
    char * table;
    char * fId;
    char * fSession;
    char * fTimestamp;
};

static cson_sessmgr_cpdo_impl cson_sessmgr_cpdo_impl_empty = {
    NULL /*drv*/,
    NULL /*table*/,
    NULL /*fId*/,
    NULL /*fSession*/,
    NULL /*fTimestamp*/
};

static const cson_sessmgr cson_sessmgr_cpdo_empty =
{
    &cson_sessmgr_api_cpdo,
    NULL
};

static char * cson_session_cpdo_strdup( char const * src )
{
    size_t const n = src ? strlen(src) : 0;
    char * rc = src ? (char *)calloc(1, n+1) : NULL;
    if( ! rc ) return NULL;
    memcpy( rc, src, n );
    return rc;
}

#define IMPL_DECL \
    cson_sessmgr_cpdo_impl * impl = (self && (self->api == &cson_sessmgr_api_cpdo)) \
    ? (cson_sessmgr_cpdo_impl*)self->impl \
    : NULL;                                   \
    if( NULL == impl ) return cson_rc.ArgError


static int cson_session_cpdo_load( cson_sessmgr * self, cson_value ** root, char const * id )
{
    char const * cstr = NULL;
    cpdo_stmt * st = NULL;
    int rc = 0;
    uint32_t len = 0;
    cpdo_step_code scode;
    void const * blob = NULL;
    IMPL_DECL;
    if( ! root || !id || !*id ) return cson_rc.ArgError;
    rc = cpdo_prepare_f( impl->drv, &st,
                         "SELECT %s FROM %s WHERE %s=?",
                         impl->fSession, impl->table,
                         impl->fId, id );
    if( rc )
    {
        MARKER("Error preparing SELECT code: cpdo error #%d (%s)\n", rc, cpdo_rc_string( rc ) );
        if( cpdo_rc.CheckDbError == rc )
        {
            char const * err = NULL;
            impl->drv->api->error_info( impl->drv, &err, NULL, NULL );
            MARKER("Db says: %s\n", err );
        }
        return cson_rc.IOError;
    }
    rc = cpdo_bind_string( st, 1, id, strlen(id) );
    if( rc )
    {
        rc = cpdo_rc.UnknownError;
        goto clean;
    }
    scode = st->api->step( st );
    if( CPDO_STEP_DONE == scode )
    { /* no data */
        rc = cson_rc.NotFoundError;
        goto clean;
    }
    else if( CPDO_STEP_ERROR == scode )
    {
        rc = cson_rc.IOError;
        goto clean;
    }
    rc = st->api->get.string( st, 0, &cstr, &len );
    if( rc )
    { /* try again as a blob */
        rc = st->api->get.blob( st, 0, &blob, &len );
        if( rc )
        {
            rc = cson_rc.IOError;
            goto clean;
        }
        cstr = (char const *)blob;
    }

    if( ! cstr )
    {
        rc = cson_rc.UnknownError;
        goto clean;
    }
    else
    {
        cson_value * jv = NULL;
        rc = cson_parse_string( &jv, cstr, len, NULL, NULL );
        if( rc )
        {
            rc = cson_rc.TypeError;
            goto clean;
        }
        *root = jv;
        rc = 0;
    }
    clean:
    st->api->finalize( st );
    return rc;
   
}

static int cson_session_cpdo_save( cson_sessmgr * self, cson_value const * root, char const * id )
{
    cpdo_stmt * st = NULL;
    int rc;
    cson_buffer jbuf = cson_buffer_empty;
    IMPL_DECL;
    rc = cson_output_buffer( root, &jbuf, NULL );
    if( rc ) return rc;
#define CHECKRC if( rc ) { rc = cpdo_rc.UnknownError; goto clean; } (void)0
#if 1
    /* Reminder:

       We use the non-standard REPLACE SQL construct here. It is
       supported by MySQL and sqlite3, and those are the only drivers
       currently supported by cpdo. This saves us having to either do
       delete/insert or update-or-insert (depending on whether it's
       already there or not). Interestingly sqlite3 requires the 'INTO'
       keyword where MySQL does not.

       If using non-standard SQL makes you queasy, or cpdo actually
       does get another driver some day, use the ifdef'd-out
       implementation instead of this code.
    */
    rc = cpdo_prepare_f( impl->drv, &st,
                         "REPLACE INTO %s (%s,%s,%s) VALUES(?,?,?)",
                         impl->table, impl->fId, impl->fTimestamp, impl->fSession );
#else
    if( ! root || !id || !*id ) return cson_rc.ArgError;
    rc = cson_session_cpdo_remove( self, id );
    if( rc ) return rc;
    rc = cpdo_prepare_f( impl->drv, &st,
                         "INSERT INTO %s (%s,%s,%s) VALUES(?,?,?)",
                         impl->table, impl->fId, impl->fTimestamp, impl->fSession );
#endif
    CHECKRC;
    rc = cpdo_bind_string( st, 1, id, strlen(id) );
    CHECKRC;
    rc = cpdo_bind_int32( st, 2, (int32_t)time(NULL) );
    CHECKRC;
    rc = cpdo_bind_string( st, 3, (char const *)jbuf.mem, jbuf.used );
    if( rc )
    { /* try again as blob */
        rc = cpdo_bind_blob( st, 3, jbuf.mem, jbuf.used );
    }
    CHECKRC;
#undef CHECKRC
    if( CPDO_STEP_ERROR == st->api->step(st) )
    {
        rc = cson_rc.IOError;
        goto clean;
    }
    clean:
    if( st ) st->api->finalize( st );
    cson_buffer_reserve( &jbuf, 0 );
    return rc;
}

static int cson_session_cpdo_remove( cson_sessmgr * self, char const * id )
{
    cpdo_stmt * st = NULL;
    int rc;
    IMPL_DECL;
    if( !id || !*id ) return cson_rc.ArgError;
    rc = cpdo_prepare_f( impl->drv, &st,
                         "DELETE FROM %s WHERE %s=?",
                         impl->table, impl->fId );
    if( rc )
    {
        rc = cson_rc.IOError;
        goto clean;
    }
    rc = cpdo_bind_string( st, 1, id, strlen(id) );
    if( rc ) { rc = cpdo_rc.UnknownError; goto clean; }
    if( CPDO_STEP_ERROR == st->api->step( st ) )
    {
        rc = cson_rc.IOError;
        goto clean;
    }

    clean:
    if( st )
    {
        st->api->finalize( st );
    }
    return rc;
}


void cson_session_cpdo_finalize( cson_sessmgr * self )
{
    if( self && (self->api == &cson_sessmgr_api_cpdo) )
    {
        cson_sessmgr_cpdo_impl * impl = (cson_sessmgr_cpdo_impl *)self->impl;
        free( impl->table );
        free( impl->fId );
        free( impl->fSession );
        free( impl->fTimestamp );
        if( impl->drv ) impl->drv->api->close( impl->drv );
        free( impl );
        *self = cson_sessmgr_cpdo_empty;
        free( self );
    }
}

int cson_sessmgr_cpdo( cson_sessmgr ** tgt, cson_object const * opt )
{
    if( !tgt || !opt ) return cson_rc.ArgError;
    else
    {
        cson_sessmgr * m = (cson_sessmgr *)malloc(sizeof(cson_sessmgr));
        cson_sessmgr_cpdo_impl * impl = m
            ? (cson_sessmgr_cpdo_impl *)malloc(sizeof(cson_sessmgr_cpdo_impl))
            : NULL;
        cpdo_driver * drv = NULL;
        char const * cstr;
        char const * user;
        char const * password;
        int rc;
        if( ! m ) return cson_rc.AllocError;
        else if( ! impl )
        {
            free(m);
            return cson_rc.AllocError;
        }
#if CPDO_ENABLE_SQLITE3
        cpdo_driver_sqlite3_register();
#endif
#if CPDO_ENABLE_MYSQL5
        cpdo_driver_mysql5_register();
#endif
        
        *impl = cson_sessmgr_cpdo_impl_empty;
        *m = cson_sessmgr_cpdo_empty;
        m->impl = impl;

#define GETSTR(KEY) cson_string_cstr( cson_value_get_string( cson_object_get( opt, KEY ) ) )
        cstr = GETSTR("dsn");
        if( ! cstr || !*cstr )
        {
            rc = cson_rc.ArgError;
            goto error_clean;
        }
        user = GETSTR("user");
        password = GETSTR("password");
        rc = cpdo_driver_new_connect( &drv, cstr, user, password );
        if( rc )
        {
            if( drv )
            {
                drv->api->close( drv );
            }
            /* There are overlaps/discrepancies in the cson_rc and cpdo_rc error
               values, so we're going to convert this to a cson_rc value...
             */
            rc = cson_rc.IOError;
            goto error_clean;
        }
        impl->drv = drv;
#define CP(KEY,KEYF)                                                \
        cstr = GETSTR(KEY);                                          \
        if( ! cstr ) {                                               \
            rc = cson_rc.ArgError;                                   \
            goto error_clean;                                        \
        } else {                                                     \
            impl->KEYF = cson_session_cpdo_strdup( cstr );           \
            if( ! impl->KEYF ) {                                     \
                rc = cson_rc.AllocError;                             \
                goto error_clean;                                    \
            }                                                        \
        } (void)0
        CP("table", table);
        CP("fieldId", fId);
        CP("fieldSession", fSession);
        CP("fieldTimestamp", fTimestamp);
#undef CP
#undef GETSTR
        *tgt = m;
        return 0;
        error_clean:
        m->api->finalize( m );
        return rc;
    }
}

#undef IMPL_DECL
#undef ENABLE_UNIX
#undef MARKER

#endif /* CSON_ENABLE_CPDO */
/* end file ./cson_session_cpdo.c */
/* begin file ./cson_sqlite3.c */
/** @file cson_sqlite3.c

This file contains the implementation code for the cson
sqlite3-to-JSON API.

License: the same as the cson core library.

Author: Stephan Beal (http://wanderinghorse.net/home/stephan)
*/
#if CSON_ENABLE_SQLITE3 /* we do this here for the sake of the amalgamation build */
#include <assert.h>
#include <string.h> /* strlen() */

#if 0
#include <stdio.h>
#define MARKER if(1) printf("MARKER: %s:%d:%s():\t",__FILE__,__LINE__,__func__); if(1) printf
#else
#define MARKER if(0) printf
#endif

#if defined(__cplusplus)
extern "C" {
#endif

cson_value * cson_sqlite3_column_to_value( sqlite3_stmt * st, int col )
{
    if( ! st ) return NULL;
    else
    {
#if 0
        sqlite3_value * val = sqlite3_column_type(st,col);
        int const vtype = val ? sqlite3_value_type(val) : -1;
        if( ! val ) return cson_value_null();
#else
        int const vtype = sqlite3_column_type(st,col);
#endif
        switch( vtype )
        {
          case SQLITE_NULL:
              return cson_value_null();
          case SQLITE_INTEGER:
              /* FIXME: for large integers fall back to Double instead. */
              return cson_value_new_integer( (cson_int_t) sqlite3_column_int64(st, col)  );
          case SQLITE_FLOAT:
              return cson_value_new_double( sqlite3_column_double(st, col) );
          case SQLITE_BLOB: /* arguably fall through... */
          case SQLITE_TEXT: {
              char const * str = (char const *)sqlite3_column_text(st,col);
              return cson_value_new_string(str, str ? strlen(str) : 0);
          }
          default:
              return NULL;
        }
    }
}

cson_value * cson_sqlite3_column_names( sqlite3_stmt * st )
{
    cson_value * aryV = NULL;
    cson_array * ary = NULL;
    char const * colName = NULL;
    int i = 0;
    int rc = 0;
    int colCount = 0;
    assert(st);
    colCount = sqlite3_column_count(st);
    if( colCount <= 0 ) return NULL;
    
    aryV = cson_value_new_array();
    if( ! aryV ) return NULL;
    ary = cson_value_get_array(aryV);
    assert(ary);
    for( i = 0; (0 ==rc) && (i < colCount); ++i )
    {
        colName = sqlite3_column_name( st, i );
        if( ! colName ) rc = cson_rc.AllocError;
        else
        {
            rc = cson_array_set( ary, (unsigned int)i,
                    cson_value_new_string(colName, strlen(colName)) );
        }
    }
    if( 0 == rc ) return aryV;
    else
    {
        cson_value_free(aryV);
        return NULL;
    }
}


cson_value * cson_sqlite3_row_to_object2( sqlite3_stmt * st,
                                          cson_array * colNames )
{
    cson_value * rootV = NULL;
    cson_object * root = NULL;
    cson_string * colName = NULL;
    int i = 0;
    int rc = 0;
    cson_value * currentValue = NULL;
    int const colCount = sqlite3_column_count(st);
    if( !colCount || (colCount>cson_array_length_get(colNames)) ) {
        return NULL;
    }
    rootV = cson_value_new_object();
    if(!rootV) return NULL;
    root = cson_value_get_object(rootV);
    for( i = 0; i < colCount; ++i )
    {
        colName = cson_value_get_string( cson_array_get( colNames, i ) );
        if( ! colName ) goto error;
        currentValue = cson_sqlite3_column_to_value(st,i);
        if( ! currentValue ) currentValue = cson_value_null();
        rc = cson_object_set_s( root, colName, currentValue );
        if( 0 != rc )
        {
            cson_value_free( currentValue );
            goto error;
        }
    }
    goto end;
    error:
    cson_value_free( rootV );
    rootV = NULL;
    end:
    return rootV;
}


cson_value * cson_sqlite3_row_to_object( sqlite3_stmt * st )
{
#if 0
    cson_value * arV = cson_sqlite3_column_names(st);
    cson_array * ar = NULL;
    cson_value * rc = NULL;
    if(!arV) return NULL;
    ar = cson_value_get_array(arV);
    assert( NULL != ar );
    rc = cson_sqlite3_row_to_object2(st, ar);
    cson_value_free(arV);
    return rc;
#else
    cson_value * rootV = NULL;
    cson_object * root = NULL;
    char const * colName = NULL;
    int i = 0;
    int rc = 0;
    cson_value * currentValue = NULL;
    int const colCount = sqlite3_column_count(st);
    if( !colCount ) return NULL;
    rootV = cson_value_new_object();
    if(!rootV) return NULL;
    root = cson_value_get_object(rootV);
    for( i = 0; i < colCount; ++i )
    {
        colName = sqlite3_column_name( st, i );
        if( ! colName ) goto error;
        currentValue = cson_sqlite3_column_to_value(st,i);
        if( ! currentValue ) currentValue = cson_value_null();
        rc = cson_object_set( root, colName, currentValue );
        if( 0 != rc )
        {
            cson_value_free( currentValue );
            goto error;
        }
    }
    goto end;
    error:
    cson_value_free( rootV );
    rootV = NULL;
    end:
    return rootV;
#endif
}

cson_value * cson_sqlite3_row_to_array( sqlite3_stmt * st )
{
    cson_value * aryV = NULL;
    cson_array * ary = NULL;
    int i = 0;
    int rc = 0;
    int const colCount = sqlite3_column_count(st);
    if( ! colCount ) return NULL;
    aryV = cson_value_new_array();
    if( ! aryV ) return NULL;
    ary = cson_value_get_array(aryV);
    rc = cson_array_reserve(ary, (unsigned int) colCount );
    if( 0 != rc ) goto error;

    for( i = 0; i < colCount; ++i ){
        cson_value * elem = cson_sqlite3_column_to_value(st,i);
        if( ! elem ) goto error;
        rc = cson_array_append(ary,elem);
        if(0!=rc)
        {
            cson_value_free( elem );
            goto end;
        }
    }
    goto end;
    error:
    cson_value_free(aryV);
    aryV = NULL;
    end:
    return aryV;
}

    
/**
    Internal impl of cson_sqlite3_stmt_to_json() when the 'fat'
    parameter is non-0.
*/
static int cson_sqlite3_stmt_to_json_fat( sqlite3_stmt * st, cson_value ** tgt )
{
#define RETURN(RC) { if(rootV) cson_value_free(rootV); return RC; }
    if( ! tgt || !st ) return cson_rc.ArgError;
    else
    {
        cson_value * rootV = NULL;
        cson_object * root = NULL;
        cson_value * colsV = NULL;
        cson_array * cols = NULL;
        cson_value * rowsV = NULL;
        cson_array * rows = NULL;
        cson_value * objV = NULL;
        int rc = 0;
        int const colCount = sqlite3_column_count(st);
        if( colCount <= 0 ) return cson_rc.ArgError;
        rootV = cson_value_new_object();
        if( ! rootV ) return cson_rc.AllocError;
        colsV = cson_sqlite3_column_names(st);
        if( ! colsV )
        {
            cson_value_free( rootV );
            RETURN(cson_rc.AllocError);
        }
        cols = cson_value_get_array(colsV);
        assert(NULL != cols);
        root = cson_value_get_object(rootV);
        rc = cson_object_set( root, "columns", colsV );
        if( rc )
        {
            cson_value_free( colsV );
            RETURN(rc);
        }
        rowsV = cson_value_new_array();
        if( ! rowsV ) RETURN(cson_rc.AllocError);
        rc = cson_object_set( root, "rows", rowsV );
        if( rc )
        {
            cson_value_free( rowsV );
            RETURN(rc);
        }
        rows = cson_value_get_array(rowsV);
        assert(rows);
        while( SQLITE_ROW == sqlite3_step(st) )
        {
            objV = cson_sqlite3_row_to_object2(st, cols);
            if( ! objV ) RETURN(cson_rc.UnknownError);
            rc = cson_array_append( rows, objV );
            if( rc )
            {
                cson_value_free( objV );
                RETURN(rc);
            }
        }
        *tgt = rootV;
        return 0;
    }
#undef RETURN
}

/**
    Internal impl of cson_sqlite3_stmt_to_json() when the 'fat'
    parameter is 0.
*/
static int cson_sqlite3_stmt_to_json_slim( sqlite3_stmt * st, cson_value ** tgt )
{
#define RETURN(RC) { if(rootV) cson_value_free(rootV); return RC; }
    if( ! tgt || !st ) return cson_rc.ArgError;
    else
    {
        cson_value * rootV = NULL;
        cson_object * root = NULL;
        cson_value * aryV = NULL;
        cson_value * rowsV = NULL;
        cson_array * rows = NULL;
        int rc = 0;
        int const colCount = sqlite3_column_count(st);
        if( colCount <= 0 ) return cson_rc.ArgError;
        rootV = cson_value_new_object();
        if( ! rootV ) return cson_rc.AllocError;
        aryV = cson_sqlite3_column_names(st);
        if( ! aryV )
        {
            cson_value_free( rootV );
            RETURN(cson_rc.AllocError);
        }
        root = cson_value_get_object(rootV);
        rc = cson_object_set( root, "columns", aryV );
        if( rc )
        {
            cson_value_free( aryV );
            RETURN(rc);
        }
        aryV = NULL;
        rowsV = cson_value_new_array();
        if( ! rowsV ) RETURN(cson_rc.AllocError);
        rc = cson_object_set( root, "rows", rowsV );
        if( 0 != rc )
        {
            cson_value_free( rowsV );
            RETURN(rc);
        }
        rows = cson_value_get_array(rowsV);
        assert(rows);
        while( SQLITE_ROW == sqlite3_step(st) )
        {
            aryV = cson_sqlite3_row_to_array(st);
            if( ! aryV ) RETURN(cson_rc.UnknownError);
            rc = cson_array_append( rows, aryV );
            if( 0 != rc )
            {
                cson_value_free( aryV );
                RETURN(rc);
            }
        }
        *tgt = rootV;
        return 0;
    }
#undef RETURN
}

int cson_sqlite3_stmt_to_json( sqlite3_stmt * st, cson_value ** tgt, char fat )
{
    return fat
        ? cson_sqlite3_stmt_to_json_fat(st,tgt)
        : cson_sqlite3_stmt_to_json_slim(st,tgt)
        ;
}

int cson_sqlite3_sql_to_json( sqlite3 * db, cson_value ** tgt, char const * sql, char fat )
{
    if( !db || !tgt || !sql || !*sql ) return cson_rc.ArgError;
    else
    {
        sqlite3_stmt * st = NULL;
        int rc = sqlite3_prepare_v2( db, sql, -1, &st, NULL );
        if( 0 != rc ) return cson_rc.IOError /* FIXME: Better error code? */;
        rc = cson_sqlite3_stmt_to_json( st, tgt, fat );
        sqlite3_finalize( st );
        return rc;
    }        
}

int cson_sqlite3_bind_value( sqlite3_stmt * st, int ndx, cson_value const * v )
{
    int rc = 0;
    char convertErr = 0;
    if(!st) return cson_rc.ArgError;
    else if( ndx < 1 ) {
        rc = cson_rc.RangeError;
    }
    else if( cson_value_is_array(v) ){
        cson_array * ar = cson_value_get_array(v);
        unsigned int len = cson_array_length_get(ar);
        unsigned int i;
        assert(NULL != ar);
        for( i = 0; !rc && (i < len); ++i ){
            rc = cson_sqlite3_bind_value( st, (int)i+ndx,
                                          cson_array_get(ar, i));
        }
    }
    else if(!v || cson_value_is_null(v)){
        rc = sqlite3_bind_null(st,ndx);
        convertErr = 1;
    }
    else if( cson_value_is_double(v) ){
        rc = sqlite3_bind_double( st, ndx, cson_value_get_double(v) );
        convertErr = 1;
    }
    else if( cson_value_is_bool(v) ){
        rc = sqlite3_bind_int( st, ndx, cson_value_get_bool(v) ? 1 : 0 );
        convertErr = 1;
    }
    else if( cson_value_is_integer(v) ){
        rc = sqlite3_bind_int64( st, ndx, cson_value_get_integer(v) );
        convertErr = 1;
    }
    else if( cson_value_is_string(v) ){
        cson_string const * s = cson_value_get_string(v);
        rc = sqlite3_bind_text( st, ndx,
                                cson_string_cstr(s),
                                cson_string_length_bytes(s),
                                SQLITE_TRANSIENT);
        convertErr = 1;
    }
    else {
        rc = cson_rc.TypeError;
    }
    if(convertErr && rc) switch(rc){
      case SQLITE_TOOBIG:
      case SQLITE_RANGE: rc = cson_rc.RangeError; break;
      case SQLITE_NOMEM: rc = cson_rc.AllocError; break;
      case SQLITE_IOERR: rc = cson_rc.IOError; break;
      default: rc = cson_rc.UnknownError; break;
    };
    return rc;
}


#if defined(__cplusplus)
} /*extern "C"*/
#endif
#undef MARKER
#endif /* CSON_ENABLE_SQLITE3 */
/* end file ./cson_sqlite3.c */
/* begin file cpdo_amalgamation.c */
#if !defined(__STDC_FORMAT_MACROS) /* required for PRIi32 and friends.*/
#  define __STDC_FORMAT_MACROS
#endif
/* start of file cpdo.c */
#include <assert.h>
#include <string.h> /* strcmp() */
#include <stdlib.h> /* atexit(), malloc() and friends. */

#include <stdio.h> /* only for debuggering */
#include <inttypes.h> /* only used for debuggering */
#define MARKER if(1) printf("MARKER: %s:%d:%s():\t",__FILE__,__LINE__,__func__); if(1) printf

#if defined(__cplusplus)
extern "C" {
#endif

const cpdo_stmt cpdo_stmt_empty = cpdo_stmt_empty_m;
const cpdo_connect_opt cpdo_connect_opt_empty = cpdo_connect_opt_empty_m;
const cpdo_driver_details cpdo_driver_details_empty = cpdo_driver_details_empty_m;
const cpdo_bind_val cpdo_bind_val_empty = cpdo_bind_val_empty_m;
const cpdo_buffer cpdo_buffer_empty = cpdo_buffer_empty_m;

char const * cpdo_rc_string(int rc)
{
    if(0 == rc) return "OK";
#define CHECK(N) else if(cpdo_rc.N == rc ) return #N
    CHECK(OK);
    CHECK(ArgError);
    CHECK(RangeError);
    CHECK(TypeError);
    CHECK(IOError);
    CHECK(AllocError);
    CHECK(NYIError);
    CHECK(InternalError);
    CHECK(UnsupportedError);
    CHECK(NotFoundError);
    CHECK(UnknownError);
    CHECK(CheckDbError);
    CHECK(ConnectionError);
    CHECK(AccessError);
    CHECK(UsageError);
    else return "UnknownError";
#undef CHECK
}


int cpdo_split_dsn( char const * dsn,
                    char * out, uint32_t outLen,
                    char const ** paramPos )
{
    size_t const slen = dsn ? strlen(dsn) : 0;
    if( !outLen || (slen<2) || (slen >= outLen) ) return cpdo_rc.RangeError;
    else
    {
        char * pos = out;
        uint32_t at = 0;
        memcpy(out, dsn, slen );
        memset(out+slen, 0, outLen-slen);
        for( ; *pos && (at<outLen) ; ++pos, ++at )
        {
            if( ':' == *pos )
            {
                *pos = 0;
                ++pos;
                break;
            }
        }
        if( !*pos ) return cpdo_rc.RangeError;
        if( paramPos ) *paramPos = pos;
        return (at == outLen) ? cpdo_rc.RangeError : 0;
    }
}


enum {
/**
    Max length, including NUL terminator, of a driver's  name.
    */
CPDO_DRIVER_MAX_NAME_LENGTH = 33,
/** Number of slots of driver registration.
    The API docs guaranty at least 20 slots.
*/
CPDO_DRIVER_LIST_LENGTH = 20
};
typedef struct cpdo_driver_reg cpdo_driver_reg;
struct cpdo_driver_reg
{
    char name[CPDO_DRIVER_MAX_NAME_LENGTH];
    int (*factory)( cpdo_driver ** );
};
#define cpdo_driver_reg_empty_m {{0},NULL}
/**
    The list used by cpdo_driver_register() and cpdo_driver_search().
*/
static cpdo_driver_reg CPDO_DRIVER_LIST[CPDO_DRIVER_LIST_LENGTH] =
{
#define X cpdo_driver_reg_empty_m
X,X,X,X,X,
X,X,X,X,X
#undef X
};
static char cpdo_bind_val_empty_blob = 0;

/**
   Searches for a registered driver with the given name. If one is
   found, 0 is returned and if slot is not NULL then *slot is assigned
   to the registration entry. Returns cpdo_rc.NotFoundError if not
   entry is found.
*/
static int cpdo_driver_search( char const * name, cpdo_driver_reg ** slot );


int cpdo_driver_new( cpdo_driver ** tgt,
                     char const * driver,
                     char const * opt )
{
    cpdo_driver_reg * reg = NULL;
    int rc;
    if( ! driver || !tgt ) return cpdo_rc.ArgError;
    /**
       FIXME: put these in a registration table.
    */
    rc = cpdo_driver_search( driver, &reg );
    if( rc ) return cpdo_rc.NotFoundError;
    assert( reg );
    return reg->factory(tgt);
}

int cpdo_driver_new_connect( cpdo_driver ** tgt, char const * dsn,
                             char const * user, char const * passwd )
{
    if( ! dsn || !tgt ) return cpdo_rc.ArgError;
    else
    {
        enum { BufSize = 256U };
        char buf[BufSize];
        cpdo_driver * drv = NULL;
        char const * params = NULL;
        int rc = cpdo_split_dsn( dsn, buf, BufSize, &params );
        if( rc ) return rc;
        rc = cpdo_driver_new( &drv, buf, params );
        if( rc ) return rc;
        else
        {
            cpdo_connect_opt copt = cpdo_connect_opt_empty;
            copt.dsn = dsn;
            copt.user_name = user;
            copt.password = passwd;
            rc = drv->api->connect( drv, &copt );
            *tgt = drv;
            return rc;
        }
    }
}

int cpdo_sql_escape( char const * sql, uint32_t * len, char ** dest,
                     char quoteChar, char escapeChar, char addQuotes )
{
    if( !dest || !len ) return cpdo_rc.ArgError;
    else if( !sql )
    {
        char * tmp = (char *)malloc(5);
        if( ! tmp ) return cpdo_rc.AllocError;
        strcpy( tmp, "NULL" );
        *dest = tmp;
        *len = 4;
        return 0;
    }
    else
    {
        uint32_t sz = (addQuotes ? 2 /* open/closing quotes */ : 0)
            + 1 /* NUL pad */
            ;
        char * esc = NULL;
        char * escStart = NULL;
        char const * pos = sql;
        uint32_t const slen = *len;
        /* count the number of single-quotes and base our malloc() off
           of that.
        */
        for( ; *pos && (pos<(sql+slen)); ++pos, ++sz )
        {
            if( quoteChar == *pos ) ++sz;
        }
        esc = (char *)calloc(1,sz);
        if( ! esc ) return cpdo_rc.AllocError;
        escStart = esc;
        pos = sql;
        if( addQuotes ) *(esc++) = quoteChar;
        for( ; *pos && (pos<(sql+slen)); ++pos )
        {
            if( quoteChar == *pos )
            {
                *(esc++) = escapeChar;
            }
            *(esc++) = *pos;
        }
        if( addQuotes ) *(esc++) = quoteChar;
        *len = esc - escStart;
        *dest = escStart;
        return 0;
    }
}

int cpdo_exec( cpdo_driver * drv, char const * sql, uint32_t len )
{
    if( !drv || !sql || !len || !*sql ) return cpdo_rc.ArgError;
    else if( !drv->api->is_connected(drv) ) return cpdo_rc.ConnectionError;
    else
    {
        cpdo_stmt * st = NULL;
        int rc = drv->api->prepare( drv, &st, sql, len );
        if( 0 == rc )
        {
            rc = st->api->step(st);
            st->api->finalize(st);
            if( rc != CPDO_STEP_ERROR ) rc = 0;
        }
        return rc;
    }
}

static int cpdo_driver_search( char const * name, cpdo_driver_reg ** slot )
{
    size_t nlen = 0;
    if( ! name ) return cpdo_rc.ArgError;
    else if( !*name ) return cpdo_rc.RangeError;
    else if( CPDO_DRIVER_MAX_NAME_LENGTH <= (nlen=strlen(name)) ) return cpdo_rc.RangeError;    
    else
    {
        cpdo_driver_reg * r = NULL;
        int i = 0;
        for( r = &CPDO_DRIVER_LIST[0];
             i < CPDO_DRIVER_LIST_LENGTH;
             ++i, r = &CPDO_DRIVER_LIST[i] )
        {
            if( ! *r->name )
            {
                continue;
            }
            else if( 0 == strcmp(r->name,name) )
            {
                if( slot ) *slot = r;
                return 0;
            }
        }
        return cpdo_rc.NotFoundError;
    }
}

int cpdo_driver_register( char const * name, cpdo_driver_factory_f factory )
{

    size_t nlen = 0;
    if( ! name || !factory ) return cpdo_rc.ArgError;
    else if( !*name ) return cpdo_rc.RangeError;
    else if( CPDO_DRIVER_MAX_NAME_LENGTH <= (nlen=strlen(name)) ) return cpdo_rc.RangeError;    
    else
    {
        cpdo_driver_reg * r = NULL;
        int i = 0;
        for( r = &CPDO_DRIVER_LIST[0];
             i < CPDO_DRIVER_LIST_LENGTH;
             ++i, r = &CPDO_DRIVER_LIST[i] )
        {
            if( ! *r->name )
            {
                break;
            }
            else if( 0 == strcmp(r->name, name) )
            {
                return cpdo_rc.AccessError;
            }
        }
        if( i == CPDO_DRIVER_LIST_LENGTH )
        {
            return cpdo_rc.AllocError;
        }
        assert( r && !*r->name );
        /*
            Reminder to self: set the factory before the name,
            to preempt a corner case in the un-thread-safety which
            could cause cpdo_driver_search() to match the name as its
            last byte is being written, but then step on a null factory
            handle.
        */        
        r->factory = factory;
        memcpy( r->name, name, nlen );
        return 0;
    }
}

char const * const * cpdo_available_drivers()
{

    static char const * names[CPDO_DRIVER_MAX_NAME_LENGTH+1];
    cpdo_driver_reg * r = NULL;
    int i = 0;
    int x = 0;
    names[CPDO_DRIVER_MAX_NAME_LENGTH] = NULL;
    for( r = &CPDO_DRIVER_LIST[0];
         i < CPDO_DRIVER_LIST_LENGTH;
         ++i, r = &CPDO_DRIVER_LIST[i] )
    {
        if( *r->name )
        {
            names[x++] = r->name;
        }
    }
    for( i = x; i < CPDO_DRIVER_LIST_LENGTH; ++i )
    {
        names[i] = NULL;
    }
    return names;
}


/** @internal

Tokenizes an input string on a given separator. Inputs are:

- (inp) = is a pointer to the pointer to the start of the input.

- (separator) = the separator character

- (end) = a pointer to NULL. i.e. (*end == NULL)

This function scans *inp for the given separator char or a NULL char.
Successive separators at the start of *inp are skipped. The effect is
that, when this function is called in a loop, all neighboring
separators are ignored. e.g. the string "aa.bb...cc" will tokenize to
the list (aa,bb,cc) if the separator is '.' and to (aa.,...cc) if the
separator is 'b'.

Returns 0 (false) if it finds no token, else non-0 (true).

Output:

- (*inp) will be set to the first character of the next token.

- (*end) will point to the one-past-the-end point of the token.

If (*inp == *end) then the end of the string has been reached
without finding a token.

Post-conditions:

- (*end == *inp) if no token is found.

- (*end > *inp) if a token is found.

It is intolerant of NULL values for (inp, end), and will assert() in
debug builds if passed NULL as either parameter.
*/
char cpdo_next_token( char const ** inp, char separator, char const ** end )
{
    char const * pos = NULL;
    assert( inp && end && *inp );
    if( ! inp || !end ) return 0;
    else if( *inp == *end ) return 0;
    pos = *inp;
    if( !*pos )
    {
        *end = pos;
        return 0;
    }
    for( ; *pos && (*pos == separator); ++pos) { /* skip preceeding splitters */ }
    *inp = pos;
    for( ; *pos && (*pos != separator); ++pos) { /* find next splitter */ }
    *end = pos;
    return (pos > *inp) ? 1 : 0;
}

char cpdo_token_bool_val(char const * str)
{
    return (str && *str && ((*str=='1')||(*str=='t')||(*str=='T')||(*str=='y')||(*str=='Y'))
            )
        ? 1
        : 0;
}

int cpdo_bind_val_clean( cpdo_bind_val * b )
{
    if( ! b ) return cpdo_rc.ArgError;
    else
    {
        switch(b->type)
        {
          case CPDO_TYPE_CUSTOM:
              if( b->valu.custom.dtor )
              {
                  b->valu.custom.dtor( b->valu.custom.mem );
              }
              break;
          case CPDO_TYPE_BLOB:
          case CPDO_TYPE_STRING:
#if 1
              if( &cpdo_bind_val_empty_blob != b->valu.blob.mem ) {
                  free( b->valu.blob.mem );
#if 0
                  assert(b->valu.blob.length && "WTF is this coming from?");
#endif
              }
#else
              /* this is leaking from somewhere! */
              if( b->valu.blob.length ) {
                  free( b->valu.blob.mem );
              }
#endif
              b->valu.blob.length = 0;
              b->valu.blob.mem = NULL;
              /* fall through */
          default:
              break;
        };
        *b = cpdo_bind_val_empty;
        return 0;
    }
}

int cpdo_bind_val_free( cpdo_bind_val * b )
{
    int const rc = cpdo_bind_val_clean(b);
    if( 0 == rc )
    {
        free(b);
    }
    return rc;
}

int cpdo_bind_val_custom( cpdo_bind_val * b, void * mem,
                          void (*dtor)(void *), int typeTag )
{
    if( ! b ) return cpdo_rc.ArgError;
    else
    {
        cpdo_bind_val_clean(b);
        b->type = CPDO_TYPE_CUSTOM;
        b->valu.custom.dtor = dtor;
        b->valu.custom.mem = mem;
        b->valu.custom.type_tag = typeTag;
        return 0;
    }
}

int cpdo_bind_val_null( cpdo_bind_val * b )
{
    if( ! b ) return cpdo_rc.ArgError;
    else
    {
        cpdo_bind_val_clean(b);
        b->type = CPDO_TYPE_NULL;
        return 0;
    }
}

int cpdo_bind_val_int8( cpdo_bind_val * b, int8_t v )
{
    if( ! b ) return cpdo_rc.ArgError;
    else
    {
        cpdo_bind_val_clean(b);
        b->type = CPDO_TYPE_INT8;
        b->valu.i8 = v;
        return 0;
    }
}
int cpdo_bind_val_int16( cpdo_bind_val * b, int16_t v )
{
    if( ! b ) return cpdo_rc.ArgError;
    else
    {
        cpdo_bind_val_clean(b);
        b->type = CPDO_TYPE_INT16;
        b->valu.i16 = v;
        return 0;
    }
}

int cpdo_bind_val_int32( cpdo_bind_val * b, int32_t v )
{
    if( ! b ) return cpdo_rc.ArgError;
    else
    {
        cpdo_bind_val_clean(b);
        b->type = CPDO_TYPE_INT32;
        b->valu.i32 = v;
        return 0;
    }
}
int cpdo_bind_val_int64( cpdo_bind_val * b, int64_t v )
{
    if( ! b ) return cpdo_rc.ArgError;
    else
    {
        cpdo_bind_val_clean(b);
        b->type = CPDO_TYPE_INT64;
        b->valu.i64 = v;
        return 0;
    }
}
    
int cpdo_bind_val_double( cpdo_bind_val * b, double v )
{
    if( ! b ) return cpdo_rc.ArgError;
    else
    {
        cpdo_bind_val_clean(b);
        b->type = CPDO_TYPE_DOUBLE;
        b->valu.dbl = v;
        return 0;
    }
}

int cpdo_bind_val_float( cpdo_bind_val * b, float v )
{
    if( ! b ) return cpdo_rc.ArgError;
    else
    {
        cpdo_bind_val_clean(b);
        b->type = CPDO_TYPE_FLOAT;
        b->valu.flt = v;
        return 0;
    }
}

/**
   Internal impl of cpdo_bind_val_string() and cpdo_bind_val_blob().
   If isString is true then the former, else the latter.
*/
static int cpdo_bind_val_strblob( cpdo_bind_val * b, char isString, void const * v, uint32_t len )
{
    if( ! b ) return cpdo_rc.ArgError;
    else
    {
        void * mem = NULL;
        if( (b->type == CPDO_TYPE_STRING) || (b->type == CPDO_TYPE_BLOB) )
        { /* see if we can re-use the buffer... */
            if( (b->valu.blob.length >= len) && b->valu.blob.mem )
            {
                if( v )
                { /* copy v */
                    memcpy(b->valu.blob.mem, v, len);
                    memset((char *)b->valu.blob.mem + len, 0,
                           b->valu.blob.length - len);
                }
                else
                {
                    memset(b->valu.blob.mem, 0, b->valu.blob.length);
                }
                b->type = isString ? CPDO_TYPE_STRING : CPDO_TYPE_BLOB
                    /* just in case we're switching from blob to
                       string or vice versa. */
                    ;
                return 0;
            }
        }
        cpdo_bind_val_clean(b);
        mem = len ? malloc(len+1) : &cpdo_bind_val_empty_blob;
        if( NULL == mem ) return cpdo_rc.AllocError;
        else
        {
            if(len){
                if(v) memcpy( mem, v, len );
                else memset( mem, 0, len );
                ((char *)mem)[len] = 0;
            }
            b->type = isString ? CPDO_TYPE_STRING : CPDO_TYPE_BLOB;
            b->valu.blob.mem = mem;
            b->valu.blob.length = len;
            return 0;
        }
    }
}

int cpdo_bind_val_string( cpdo_bind_val * b, char const * v, uint32_t len )
{
    return cpdo_bind_val_strblob( b, 1, v, len );
}

int cpdo_bind_val_blob( cpdo_bind_val * b, void const * v, uint32_t len )
{
    return cpdo_bind_val_strblob( b, 0, v, len );
}

cpdo_bind_val * cpdo_bind_val_list_new( uint16_t len )
{
    if( ! len ) return NULL;
    else
    {
        cpdo_bind_val * rc = (cpdo_bind_val *)malloc(len * sizeof(cpdo_bind_val));
        if( NULL != rc )
        {
            uint16_t i = 0;
            for( ; i < len; ++i )
            {
                rc[i] = cpdo_bind_val_empty;
            }
        }
        return rc;
    }
}

int cpdo_bind_val_list_free( cpdo_bind_val * list, uint16_t len )
{
    if( ! list || !len ) return cpdo_rc.ArgError;
    else
    {
        cpdo_bind_val * v = list;
        uint16_t i = 0;
        for( ; i < len; ++i )
        {
            cpdo_bind_val_clean( &v[i] );
        }
        free(list);
        return 0;
    }

}
cpdo_data_type cpdo_get_type( cpdo_stmt * st, uint16_t ndx )
{
    cpdo_data_type rc = CPDO_TYPE_ERROR;
    if( st ) st->api->get.type(st, ndx, &rc);
    return rc;
}
int8_t cpdo_get_int8( cpdo_stmt * st, uint16_t ndx )
{
    int8_t v = 0;
    if( st ) st->api->get.i8( st, ndx, &v);
    return v;
}
int16_t cpdo_get_int16( cpdo_stmt * st, uint16_t ndx )
{
    int16_t v = 0;
    if( st ) st->api->get.i16( st, ndx, &v);
    return v;
}
    
int32_t cpdo_get_int32( cpdo_stmt * st, uint16_t ndx )
{
    int32_t v = 0;
    if( st ) st->api->get.i32( st, ndx, &v);
    return v;
}

int64_t cpdo_get_int64( cpdo_stmt * st, uint16_t ndx )
{
    int64_t v = 0;
    if( st ) st->api->get.i64( st, ndx, &v);
    return v;
}

double cpdo_get_double( cpdo_stmt * st, uint16_t ndx )
{
    double v = 0.0;
    if( st ) st->api->get.dbl( st, ndx, &v);
    return v;
}

float cpdo_get_float( cpdo_stmt * st, uint16_t ndx )
{
    float v = 0.0;
    if( st ) st->api->get.flt( st, ndx, &v);
    return v;
}

char const * cpdo_get_string( cpdo_stmt * st, uint16_t ndx, uint32_t * len )
{
    char const * v = NULL;
    if( st ) st->api->get.string( st, ndx, &v, len);
    return v;
}

void const * cpdo_get_blob( cpdo_stmt * st, uint16_t ndx, uint32_t * len )
{
    void const * v = NULL;
    if( st ) st->api->get.blob( st, ndx, &v, len);
    return v;
}


int cpdo_bind_int8( cpdo_stmt * st, uint16_t ndx, int8_t v )
{
    return st
        ? st->api->bind.i8( st, ndx, v)
        : cpdo_rc.ArgError;
}

int cpdo_bind_int16( cpdo_stmt * st, uint16_t ndx, int16_t v )
{
    return st
        ? st->api->bind.i16( st, ndx, v)
        : cpdo_rc.ArgError;
}

int cpdo_bind_int32( cpdo_stmt * st, uint16_t ndx, int32_t v )
{
    return st
        ? st->api->bind.i32( st, ndx, v)
        : cpdo_rc.ArgError;
}

int cpdo_bind_int64( cpdo_stmt * st, uint16_t ndx, int64_t v )
{
    return st
        ? st->api->bind.i64( st, ndx, v)
        : cpdo_rc.ArgError;
}

int cpdo_bind_float( cpdo_stmt * st, uint16_t ndx, float v )
{
    return st
        ? st->api->bind.flt( st, ndx, v)
        : cpdo_rc.ArgError;
}

int cpdo_bind_double( cpdo_stmt * st, uint16_t ndx, double v )
{
    return st
        ? st->api->bind.dbl( st, ndx, v)
        : cpdo_rc.ArgError;
}

int cpdo_bind_string( cpdo_stmt * st, uint16_t ndx, char const * v, uint32_t length )
{
    return st
        ? st->api->bind.string( st, ndx, v, length)
        : cpdo_rc.ArgError;
}

int cpdo_bind_blob( cpdo_stmt * st, uint16_t ndx, void const * v, uint32_t length )
{
    return st
        ? st->api->bind.blob( st, ndx, v, length)
        : cpdo_rc.ArgError;
}

int cpdo_close( cpdo_driver * drv )
{
    return (drv&&drv->api)
        ? drv->api->close(drv)
        : cpdo_rc.ArgError;
}
int cpdo_driver_connect( cpdo_driver * drv, cpdo_connect_opt const * opt )
{
    return (drv&&drv->api)
        ? drv->api->connect(drv,opt)
        : cpdo_rc.ArgError;
}

int cpdo_stmt_finalize( cpdo_stmt * st )
{
    return (st&&st->api)
        ? st->api->finalize(st)
        : cpdo_rc.ArgError;
}


cpdo_step_code cpdo_step( cpdo_stmt * st )
{
    return (st&&st->api)
        ? st->api->step(st)
        : CPDO_STEP_ERROR;
}

char const * cpdo_driver_name( cpdo_driver const * drv )
{
    return drv ?
        drv->api->constants.driver_name
        : NULL;
}

int cpdo_prepare( cpdo_driver * drv, cpdo_stmt ** tgt, char const * sql, uint32_t length )
{
    return (drv && tgt && sql && *sql)
        ? drv->api->prepare(drv, tgt, sql, length)
        : cpdo_rc.ArgError;
}

uint64_t cpdo_last_insert_id( cpdo_driver * drv, char const * hint )
{
    uint64_t rv = 0;
    if( drv ) drv->api->last_insert_id(drv, &rv, hint);
    return rv;
}



/**
   Returns true (non-0) if ch is an SQL named parameter identifier
   character. If isFirst, then numberic characters will not
   be considered a match. Pass true only for the first character
   in the sequence you are testing.
*/
static char is_param_char(char ch, char isFirst)
{
    return ((ch == '_')
            || ((ch >= 'a') && (ch <= 'z'))
            || ((ch >= 'A') && (ch <= 'Z'))
            || (!isFirst
                && ((ch >= '0')
                    && (ch <= '9'))
                )
            )
        ;
}

char cpdo_find_next_named_param( char const * inp,
                                 uint32_t len,
                                 char paramChar,
                                 char alsoDoQMarks,
                                 char const **paramBegin,
                                 char const **paramEnd )
{
    char const * pos = inp;
    char const * markBegin = NULL;
    char const * inpEnd = inp + len;
    char ch;
    char isEsc = 0;
    char quoteChar = 0;
    if( !inp || !paramBegin || !paramEnd ) return 0;
    else if( ('"'==paramChar)
             || ('\''==paramChar)
             || ('\\'==paramChar)
             || ('?'==paramChar)
             )
    {
        /* for sanity's sake! */
        return 0;
    }
    for( ; (pos < inpEnd) && *pos; ++pos )
    {
        ch = *pos;
        if( isEsc )
        {
            isEsc = 0;
            continue;
        }
        else if( ('\'' == ch) || ('"'==ch) )
        {
            if( ch == quoteChar ) quoteChar = 0 /* end of string */;
            else if( ! quoteChar ) quoteChar = ch /* begining of string */;
            /* else we're in a string */
            continue;
        }
        else if( '\\' == ch )
        {
            isEsc = 1;
            continue;
        }
        else if( quoteChar /* we're in a string */) continue;
        else if( alsoDoQMarks && ('?' == ch) )
        { /* special case: treat '?' as an identifier */
            *paramBegin = pos++;
            *paramEnd = pos;
            return 1;
        }
        else if( '-' == ch  )
        { /* possible SQL comment ... */
            if( '-' == pos[1] )
            {
                for( ++pos ; (pos < inpEnd) && ('\n' != *pos); ++pos )
                {
                    /* read until EOL */
                }
            }
            continue;
        }
        else if( paramChar == ch )
        { /* parse out the parameter. We're assuming that it's well-formed! */
            markBegin = pos;
            for( ++pos ; (pos < inpEnd) && *pos; ++pos )
            {
                ch = *pos;
                if( is_param_char(ch, (markBegin==(pos+1))) ) {
                    continue;
                }
                else {
                    break;
                }
            }
            if( (pos-markBegin) > 1 )
            {
                *paramBegin = markBegin;
                *paramEnd = pos;
                return 1;
            }
        }
    }
    return 0;
}

int cpdo_named_params_to_qmarks( char const * inp,
                                 uint32_t len,
                                 char paramChar,
                                 char alsoDoQMarks,
                                 uint16_t * count,
                                 char ** out,
                                 uint32_t * outLen,
                                 char *** nameList )
{
    char * mem;
    char * mem2;
    char * outPos;
    char const * pos = inp;
    char const * inpEnd = inp + len;
    char const * begin = inp;
    char const * end = NULL;
    char ** list = NULL;
    uint16_t rc = 0;
    enum { BufSize = 100 };
    uint32_t osize = 0;
    uint32_t slen = 0;
    uint32_t i = 0;
    struct holder {
        char const * b;
        char const * e;
        uint16_t len;
    } h[BufSize];
    if( !count || !inp || !out ) return cpdo_rc.ArgError;
    mem = (char *)calloc( len + 1, 1 );
    if( ! mem ) return cpdo_rc.AllocError;
    memset( &h, 0, sizeof(h) );
    outPos = mem;
    while( cpdo_find_next_named_param( pos, (inpEnd-pos), paramChar, alsoDoQMarks, &begin, &end ) )
    {
        if( rc >= BufSize /* need last element to mark end of list */ )
        {
            free(mem);
            return cpdo_rc.RangeError;
        }
        for( ; pos < begin; ++pos )
        { /* write out bytes PRECEEDING named param. */
            *(outPos++) = *pos;
        }
        *(outPos++) = '?';
        h[rc].b = begin;
        h[rc].e = end;
        h[rc].len = end-begin;
        ++rc;
        pos = end;
        begin = end = NULL;
    }
    if( ! rc )
    {
        free(mem);
        *out = NULL;
        *count = 0;
        if( outLen ) *outLen = 0;
        return 0;
    }
    for( ; pos < inpEnd; ++pos )
    { /* write out trailing bytes. */
        *(outPos++) = *pos;
    }
    if( ! nameList )
    {
        *out = mem;
        *count = rc;
        if( outLen ) *outLen = (outPos-mem);
        return 0;
    }
    /* Now jump through some hoops to stuff the returned names into the
       same memory buffer as *out...
    */
    slen = strlen(mem);
    /* Figure out how much space we need... */
    osize = slen + 1/*NUL*/
        + (rc * sizeof(char *) /*nameList*/);
    for( i = 0; i < rc; ++i )
    {
        osize += h[i].len + 1/*NUL*/;
    }
    mem2 = (char *)realloc( mem, osize );
    if( ! mem2 )
    {
        free(mem);
        return cpdo_rc.AllocError;
    }
    mem = mem2;
    /* this code easily qualifies as trying to be far too clever... */
    memset( mem + slen, 0, osize - slen ) /* clear trailing bytes */;
    list = (char **)(mem + slen + 1);
    mem2 = mem + slen + 1 + (rc*sizeof(char*));
    *nameList = list;
    /** Now copy the stored parameter names into *nameList area... */
    for( i = 0; i < rc; ++i )
    {
        list[i] = mem2;
        memcpy( mem2, h[i].b, h[i].len );
        mem2[h[i].len] = 0;
        mem2 += h[i].len + 1;
    }
    *out = mem;
    *count = rc;
    if( outLen ) *outLen = slen;
    return 0;
}

#if CPDO_ENABLE_64_BIT
typedef uint64_t cpdo_hash_t;
#else
typedef uint32_t cpdo_hash_t;
#endif
int cpdo_bind_val_tag_type_hash( cpdo_driver_api const * key, int v )
{
    return ((int)((cpdo_hash_t) key) & 0xffffff00) | v;
}


char cpdo_bind_val_tag_type_check_origin( cpdo_driver_api const * key, int const v )
{
    return ( (((cpdo_hash_t) key) & 0xffffff00) & v) ? 1 : 0;
}


char * cpdo_mprintf_v( char const * fmt, va_list vargs )
{
    return cpdo_printfv_str( fmt, vargs );
}

char * cpdo_mprintf( char const * fmt, ... )
{
    char * ret = NULL;
    va_list vargs;
    va_start( vargs, fmt );
    ret = cpdo_printfv_str( fmt, vargs );
    va_end( vargs );
    return ret;
}

int cpdo_prepare_f_v( cpdo_driver * drv, cpdo_stmt ** tgt, char const * fmt, va_list vargs)
{
    char * sql;
    int rc;
    if( ! drv || !tgt || !fmt || !*fmt ) return cpdo_rc.ArgError;
    sql = cpdo_mprintf_v( fmt, vargs );
    if( ! sql ) return cpdo_rc.AllocError;
    rc = cpdo_prepare( drv, tgt, sql, strlen(sql) );
    free(sql);
    return rc;
}

int cpdo_prepare_f( cpdo_driver * drv, cpdo_stmt ** tgt, char const * fmt, ... )
{
    int ret;
    va_list vargs;
    va_start( vargs, fmt );
    ret = cpdo_prepare_f_v( drv, tgt, fmt, vargs );
    va_end( vargs );
    return ret;
}

int cpdo_exec_f_v(  cpdo_driver * drv, char const * fmt, va_list vargs)
{
    char * sql;
    int rc;
    sql = cpdo_mprintf_v( fmt, vargs );
    if( ! sql ) return cpdo_rc.AllocError;
    rc = cpdo_exec( drv, sql, strlen(sql) );
    free(sql);
    return rc;
}

int cpdo_exec_f(  cpdo_driver * drv, char const * fmt, ... )
{
    int ret;
    va_list vargs;
    if( ! drv || !fmt || !*fmt ) return cpdo_rc.ArgError;
    va_start( vargs, fmt );
    ret = cpdo_exec_f_v( drv, fmt, vargs );
    va_end( vargs );
    return ret;
}

int cpdo_bind_string_f_v( cpdo_stmt * st, uint16_t ndx, char const * fmt, va_list vargs )
{
    char * sql;
    int rc;
    if( !st || !fmt || !*fmt ) return cpdo_rc.ArgError;
    sql = cpdo_mprintf_v( fmt, vargs );
    if( ! sql ) return cpdo_rc.AllocError;
    rc = cpdo_bind_string( st, ndx, sql, strlen(sql) );
    free(sql);
    return rc;
}

int cpdo_bind_string_f( cpdo_stmt * st, uint16_t ndx, char const *fmt, ... )
{
    int ret;
    va_list vargs;
    va_start( vargs, fmt );
    ret = cpdo_bind_string_f_v( st, ndx, fmt, vargs );
    va_end( vargs );
    return ret;
}

int cpdo_driver_opt_set( cpdo_driver * self, char const * key, ... )
{
    int ret;
    va_list vargs;
    if( !self || !key || !*key ) return cpdo_rc.ArgError;
    va_start( vargs, key );
    ret = self->api->opt.set( self, key, vargs );
    va_end( vargs );
    return ret;
}

int cpdo_driver_opt_get( cpdo_driver * self, char const * key, ... )
{
    int ret;
    va_list vargs;
    if( !self || !key || !*key ) return cpdo_rc.ArgError;
    va_start( vargs, key );
    ret = self->api->opt.get( self, key, vargs );
    va_end( vargs );
    return ret;
}

uint16_t cpdo_param_index( cpdo_stmt * st, char const * name )
{
    return (!st || !name || !*name || ('?'==*name))
        ? cpdo_rc.ArgError
        : st->api->bind.param_index( st, name );
}

char const * cpdo_param_name( cpdo_stmt * st, uint16_t ndx )
{
    return st
        ? st->api->bind.param_name( st, ndx )
        : NULL;
}

int cpdo_version_number()
{
    return CPDO_VERSION_NUMBER;
}

char * cpdo_qualify_identifier( cpdo_driver * drv, char const * iden )
{
    if(!drv || !iden || !*iden) return NULL;
    else
    {
        char * rc = NULL;
        drv->api->qualify(drv, iden, NULL, &rc);
        return rc;
    }
}

int cpdo_free_string( cpdo_driver * drv, char * str )
{
    if(!drv || !str) return cpdo_rc.ArgError;
    else return drv->api->free_string(drv, str);
}

int cpdo_step_each( cpdo_stmt * stmt, cpdo_step_each_f f, void * state )
{
    if(!stmt || !f) return cpdo_rc.ArgError;
    else {
        int rc = 0;
        cpdo_step_code scode;
        while( CPDO_STEP_OK==(scode=stmt->api->step(stmt))){
            rc = f( stmt, state );
            if(rc) break;
        }
        if(!rc && (CPDO_STEP_DONE!=scode) ){
            rc = cpdo_rc.CheckDbError;
        }
        return rc;
    }
}

int cpdo_exec_each( cpdo_driver * db, cpdo_step_each_f f, void * state, char const * sql, unsigned int sqlLength )
{
    int rc = 0;
    cpdo_stmt * st = NULL;
    if(!db || !f || !sql || !sqlLength) return cpdo_rc.ArgError;
    rc = cpdo_prepare( db, &st, sql, sqlLength );
    if(rc) return rc;
    rc = cpdo_step_each( st, f, state );
    st->api->finalize(st);
    return rc;
}

int cpdo_exec_each_f_v( cpdo_driver * db, cpdo_step_each_f f, void * state, char const * fmt, va_list vargs )
{
    int rc = 0;
    char * sql = NULL;
    if(!db || !f || !fmt) return cpdo_rc.ArgError;
    sql = cpdo_mprintf_v( fmt, vargs );
    if( ! sql ) return cpdo_rc.AllocError;
    rc = cpdo_exec_each( db, f, state, sql, strlen(sql) );
    free(sql);
    return rc;

}

int cpdo_exec_each_f( cpdo_driver * db, cpdo_step_each_f f, void * state, char const * fmt, ... )
{
    int rc;
    va_list vargs;
    va_start( vargs, fmt );
    rc = cpdo_exec_each_f_v( db, f, state, fmt, vargs );
    va_end( vargs );
    return rc;
}

int cpdo_buffer_reserve( cpdo_buffer * buf, unsigned int n )
{
    if( ! buf ) return cpdo_rc.ArgError;
    else if( 0 == n )
    {
        free(buf->mem);
        *buf = cpdo_buffer_empty;
        return 0;
    }
    else if( buf->capacity >= n )
    {
        return 0;
    }
    else
    {
        unsigned char * x = (unsigned char *)realloc( buf->mem, n );
        if( ! x ) return cpdo_rc.AllocError;
        memset( x + buf->used, 0, n - buf->used );
        buf->mem = x;
        buf->capacity = n;
        return 0;
    }
}

unsigned int cpdo_buffer_fill( cpdo_buffer * buf, char c )
{
    if( !buf || !buf->capacity || !buf->mem ) return 0;
    else
    {
        memset( buf->mem, c, buf->capacity );
        return buf->capacity;
    }
}

int cpdo_buffer_fill_from( cpdo_buffer * dest, cpdo_data_source_f src, void * state )
{
    int rc;
    enum { BufSize = 1024 * 4 };
    char rbuf[BufSize];
    size_t total = 0;
    unsigned int rlen = 0;
    if( ! dest || ! src ) return cpdo_rc.ArgError;
    dest->used = 0;
    while(1)
    {
        rlen = BufSize;
        rc = src( state, rbuf, &rlen );
        if( rc ) break;
        total += rlen;
        if( dest->capacity < (total+1) )
        {
            rc = cpdo_buffer_reserve( dest, total + 1 );
            if( 0 != rc ) break;
        }
        memcpy( dest->mem + dest->used, rbuf, rlen );
        dest->used += rlen;
        if( rlen < BufSize ) break;
    }
    if( !rc && dest->used )
    {
        assert( dest->used < dest->capacity );
        dest->mem[dest->used] = 0;
    }
    return rc;
}

    
int cpdo_data_source_FILE( void * state, void * dest, unsigned int * n )
{
    FILE * f = (FILE*) state;
    if( ! state || ! n || !dest ) return cpdo_rc.ArgError;
    else if( !*n ) return cpdo_rc.RangeError;
    *n = (unsigned int)fread( dest, 1, *n, f );
    if( !*n )
    {
        return feof(f) ? 0 : cpdo_rc.IOError;
    }
    return 0;
}


#undef MARKER
#if defined(__cplusplus)
} /*extern "C"*/
#endif
/* end of file cpdo.c */
/* start of file cpdo_skeleton.c */
/** @file cpdo_skeleton.c

    Dummy/empty cpdo_driver impl to provide a starting point for
    new drivers. The goal here is to create a skeleton which will
    compile, but won't actually work because it has no db-specific
    implementation code.
    
    To create a driver, first replace "_skel_" with "_driverName_"
    globally throughout this file. Then go through the whole file,
    from start to finish, and look for places to add the concrete
    impl code. There are many such places. Start with:
    
    - cpdo_skel_driver_alloc()
    - cpdo_skel_driver_free()
    - cpdo_skel_connect()
    - cpdo_skel_close()
    - cpdo_skel_stmt_alloc()
    - cpdo_skel_stmt_free()
    
    With those you can at least get connected and will have the basics
    (including the most important memory management) out of the way.
    
    After that comes the more intimate bits (_how_ intimate depends
    on the underlying driver). See the cpdo_sqlite3 and cpdo_mysql5
    implementations for two very different implementations (sqlite3
    makes it easy for us, MySQL not so much).

    DSN driver parameters:

   - abc=TYPE

   TODO:

    - There's always a list of TODOs.

   LICENSE:

    Add your license here.
*/
#include <assert.h>
#include <stdlib.h> /* malloc()/free() */
#include <string.h> /* strlen() */

#include <stdio.h> /* only for debuggering */
#include <inttypes.h> /* only(?) for debuggering */
#define MARKER if(1) printf("MARKER: %s:%d:%s():\t",__FILE__,__LINE__,__func__); if(1) printf

#if defined(__cplusplus)
extern "C" {
#endif

#define CPDO_DRIVER_NAME "FIXME_SET_THIS_TO_THE_DRIVERS_DSN_NAME"
/************************************************************************
 cpdo_driver_api members:
************************************************************************/
int cpdo_skel_connect( cpdo_driver * self, cpdo_connect_opt const * opt );
static int cpdo_skel_sql_quote( cpdo_driver * self, char const * src, uint32_t * len, char ** dest );
static int cpdo_skel_sql_qualify( cpdo_driver * self, char const * src, uint32_t * len, char ** dest );
static int cpdo_skel_free_string( cpdo_driver * self, char * str);
static int cpdo_skel_prepare( cpdo_driver * self, cpdo_stmt ** tgt, const char * sql, uint32_t len );
static int cpdo_skel_error_info( cpdo_driver * self, char const ** dest, uint32_t * len, int * errorCode );
static char cpdo_skel_is_connected( cpdo_driver * self );
static int cpdo_skel_close( cpdo_driver * self );
static int cpdo_skel_last_insert_id( cpdo_driver * self, uint64_t * v, char const * hint );
static int cpdo_skel_capabilities( const cpdo_driver * self );
static cpdo_driver_details const * cpdo_skel_driver_details();
static int cpdo_skel_driver_begin_transaction( cpdo_driver * self );
static int cpdo_skel_driver_commit( cpdo_driver * self );
static int cpdo_skel_driver_rollback( cpdo_driver * self );
static char cpdo_skel_driver_in_trans( cpdo_driver * self );
static int cpdo_skel_driver_opt_set( cpdo_driver * self, char const * key, va_list vargs );
static int cpdo_skel_driver_opt_get( cpdo_driver * self, char const * key, va_list vargs );
const cpdo_driver_api cpdo_skel_driver_api =
{
    cpdo_skel_driver_details,
    cpdo_skel_connect,
    cpdo_skel_sql_quote,
    cpdo_skel_sql_qualify,
    cpdo_skel_free_string,
    cpdo_skel_prepare,
    cpdo_skel_error_info,
    cpdo_skel_is_connected,
    cpdo_skel_close,
    cpdo_skel_last_insert_id,
    cpdo_skel_capabilities,
    {/*transaction*/
         cpdo_skel_driver_begin_transaction,
         cpdo_skel_driver_commit,
         cpdo_skel_driver_rollback,
         cpdo_skel_driver_in_trans
    },
    {/*opt*/
        cpdo_skel_driver_opt_set,
        cpdo_skel_driver_opt_get
    },
    {/*constants*/
        CPDO_DRIVER_NAME /*driver_name*/
    }
};

/************************************************************************
 cpdo_stmt_api members...
************************************************************************/
static cpdo_step_code cpdo_skel_stmt_step( cpdo_stmt * self );
static int cpdo_skel_stmt_error_info( cpdo_stmt * self, char const ** dest, uint32_t * len, int * errorCode );
static uint16_t cpdo_skel_stmt_column_count( cpdo_stmt * self );
static char const * cpdo_skel_stmt_column_name( cpdo_stmt * self, uint16_t ndx );
static int cpdo_skel_stmt_reset( cpdo_stmt * self );
static uint16_t cpdo_skel_stmt_bind_count( cpdo_stmt * self );
static uint16_t cpdo_skel_stmt_param_index( cpdo_stmt * self, char const * name );
static char const * cpdo_skel_stmt_param_name( cpdo_stmt * self, uint16_t ndx );
static int cpdo_skel_stmt_bind_null( cpdo_stmt * self, uint16_t ndx );
static int cpdo_skel_stmt_bind_int8( cpdo_stmt * self, uint16_t ndx, int8_t v );
static int cpdo_skel_stmt_bind_int16( cpdo_stmt * self, uint16_t ndx, int16_t v );
static int cpdo_skel_stmt_bind_int32( cpdo_stmt * self, uint16_t ndx, int32_t v );
static int cpdo_skel_stmt_bind_int64( cpdo_stmt * self, uint16_t ndx, int64_t v );
static int cpdo_skel_stmt_bind_float( cpdo_stmt * self, uint16_t ndx, float v );
static int cpdo_skel_stmt_bind_double( cpdo_stmt * self, uint16_t ndx, double v );
static int cpdo_skel_stmt_bind_string( cpdo_stmt * self, uint16_t ndx, char const * v, uint32_t len );
static int cpdo_skel_stmt_bind_blob( cpdo_stmt * self, uint16_t ndx, void const * v, uint32_t len );
static int cpdo_skel_stmt_get_type_ndx( cpdo_stmt * self, uint16_t ndx, cpdo_data_type * val );
static int cpdo_skel_stmt_get_int8_ndx( cpdo_stmt * self, uint16_t ndx, int8_t * val );
static int cpdo_skel_stmt_get_int16_ndx( cpdo_stmt * self, uint16_t ndx, int16_t * val );
static int cpdo_skel_stmt_get_int32_ndx( cpdo_stmt * self, uint16_t ndx, int32_t * val );
static int cpdo_skel_stmt_get_int64_ndx( cpdo_stmt * self, uint16_t ndx, int64_t * val );
static int cpdo_skel_stmt_get_float_ndx( cpdo_stmt * self, uint16_t ndx, float * val );
static int cpdo_skel_stmt_get_double_ndx( cpdo_stmt * self, uint16_t ndx, double * val );
static int cpdo_skel_stmt_get_string_ndx( cpdo_stmt * self, uint16_t ndx, char const ** val, uint32_t * len );
static int cpdo_skel_stmt_get_blob_ndx( cpdo_stmt * self, uint16_t ndx, void const ** v, uint32_t * len );
static int cpdo_skel_stmt_finalize( cpdo_stmt * self );
const cpdo_stmt_api cpdo_skel_stmt_api = {
    cpdo_skel_stmt_step,
    cpdo_skel_stmt_error_info,
    cpdo_skel_stmt_finalize,
    {/*bind*/
        cpdo_skel_stmt_reset,
        cpdo_skel_stmt_bind_count,
        cpdo_skel_stmt_param_index,
        cpdo_skel_stmt_param_name,
        cpdo_skel_stmt_bind_null,
        cpdo_skel_stmt_bind_int8,
        cpdo_skel_stmt_bind_int16,
        cpdo_skel_stmt_bind_int32,
        cpdo_skel_stmt_bind_int64,
        cpdo_skel_stmt_bind_float,
        cpdo_skel_stmt_bind_double,
        cpdo_skel_stmt_bind_string,
        cpdo_skel_stmt_bind_blob
    },
    {/*get*/
        cpdo_skel_stmt_column_count,
        cpdo_skel_stmt_column_name,
        cpdo_skel_stmt_get_type_ndx,
        cpdo_skel_stmt_get_int8_ndx,
        cpdo_skel_stmt_get_int16_ndx,
        cpdo_skel_stmt_get_int32_ndx,
        cpdo_skel_stmt_get_int64_ndx,
        cpdo_skel_stmt_get_float_ndx,
        cpdo_skel_stmt_get_double_ndx,
        cpdo_skel_stmt_get_string_ndx,
        cpdo_skel_stmt_get_blob_ndx
    }
};


/** Internal data types */
typedef struct cpdo_skel_stmt cpdo_skel_stmt;
static int cpdo_skel_stmt_free(cpdo_skel_stmt *s);
static cpdo_skel_stmt * cpdo_skel_stmt_alloc();

typedef struct cpdo_skel_driver cpdo_skel_driver;
static int cpdo_skel_driver_free(cpdo_skel_driver *d);
static cpdo_skel_driver * cpdo_skel_driver_alloc();


typedef void * FIXME_DB_TYPE;
struct cpdo_skel_driver
{
    FIXME_DB_TYPE conn;
    char isConnected;
    char inTransaction;
    cpdo_driver self;
};


const cpdo_skel_driver cpdo_skel_driver_empty = {
    NULL/*conn*/,
    0/*isConnected*/,
    0/*inTransaction*/,
    {/*self*/
        &cpdo_skel_driver_api /*api*/,
        NULL /*impl*/
    }
};
typedef void * FIXME_STMT_TYPE;
struct cpdo_skel_stmt
{
    FIXME_STMT_TYPE stmt;
    cpdo_skel_driver * driver;
    uint16_t colCount;
    uint16_t paramCount;
    cpdo_stmt self;
};

const cpdo_skel_stmt cpdo_skel_stmt_empty = {
    NULL /*stmt*/,
    NULL /*driver*/,
    0 /* colCount */,
    0 /* paramCount */,
    {/*self*/
        &cpdo_skel_stmt_api /*api*/,
        NULL /*impl*/
    }
};

static cpdo_skel_driver * cpdo_skel_driver_alloc()
{
    cpdo_skel_driver * s = (cpdo_skel_driver*)malloc(sizeof(cpdo_skel_driver));
    if( s )
    {
        /* FIXME: instantiate the connection here if needed. */
        FIXME_DB_TYPE conn = NULL;
        if( ! conn )
        {
            free(s);
            return NULL;
        }
        else
        {
            *s = cpdo_skel_driver_empty;
            s->conn = conn;
            s->self.impl = s;
        }
    }
    return s;
}

/**
   Closes d->conn and frees all memory associated with d.  d does not
   track statements it opens, and all statements must be closed before
   closing the db, else Undefined Behaviour.
*/
static int cpdo_skel_driver_free(cpdo_skel_driver *d)
{
    int rc = cpdo_rc.ArgError;
    if( d )
    {
        rc = 0;
        if( d->conn )
        {
            /*FIXME: call FIXME_DB_TYPE's destructor here:  mysql_close(d->conn); */
        }
        *d = cpdo_skel_driver_empty;
        free(d);
    }
    return rc;
}


/**
   Allocates a new cpdo_skel_stmt and initializes
   its self.impl member to point to the returned
   object.
*/
static cpdo_skel_stmt * cpdo_skel_stmt_alloc()
{
    cpdo_skel_stmt * s = (cpdo_skel_stmt*)malloc(sizeof(cpdo_skel_stmt));
    if( s )
    {
        *s = cpdo_skel_stmt_empty;
        s->self.impl = s;
    }
    return s;
}

/**
   Frees all resources belonging to this statement.  It can return
   non-0, but there is no generic recovery strategy for this, and s is
   freed regardless of whether or not sqlite3_finalize() succeeds.
*/
static int cpdo_skel_stmt_free(cpdo_skel_stmt *st)
{
    int rc = cpdo_rc.ArgError;
    if( st )
    {
        rc = 0;
        if( st->stmt )
        {
            /* FIXME: call FIXME_STMT_TYPE destructor here!
                mysql_stmt_close(st->stmt);
            */
        }
        *st = cpdo_skel_stmt_empty;
        free( st );
    }
    return rc;
}


/**
   cpdo_driver_factory_f() impl._ Allocates a new cpdo_skel_driver.
*/
int cpdo_skel_driver_new( cpdo_driver ** tgt )
{
    if( ! tgt ) return cpdo_rc.ArgError;
    else
    {
        cpdo_skel_driver * d = cpdo_skel_driver_alloc();
        if( d )
        {
            *tgt = &d->self;
            return 0;
        }
        else return cpdo_rc.AllocError;
    }
}

/******************************************************
Convenience macros, called often later on...
*****************************************************/
#define DRV_DECL(RC) cpdo_skel_driver * drv = (self && self->impl && (self->api==&cpdo_skel_driver_api)) \
        ? (cpdo_skel_driver *)self->impl : NULL; \
    if( ! drv ) return RC

#define STMT_DECL(RC) cpdo_skel_stmt * stmt = (self && self->impl && (self->api==&cpdo_skel_stmt_api)) \
        ? (cpdo_skel_stmt *)self->impl : NULL; \
    if( ! stmt ) return RC

#define GETNDX_DECL(NDX) STMT_DECL(cpdo_rc.ArgError); if((NDX) >= stmt->colCount) return cpdo_rc.RangeError;
#define BINDNDX_DECL(NDX) STMT_DECL(cpdo_rc.ArgError); if( !(NDX) || ((NDX) > stmt->paramCount)) return cpdo_rc.RangeError;

static int cpdo_skel_last_insert_id( cpdo_driver * self, uint64_t * v, char const * hint )
{
    DRV_DECL(cpdo_rc.ArgError);
    if( ! v ) return cpdo_rc.ArgError;
    else
    {
        return cpdo_rc.NYIError;
    }
}
static int cpdo_skel_capabilities( const cpdo_driver * self )
{
    return CPDO_CAPABILITY_NONE;
}
    
static int cpdo_skel_close( cpdo_driver * self )
{
    DRV_DECL(cpdo_rc.ArgError);
    cpdo_skel_driver_free(drv);
    return 0;
}

static char cpdo_skel_is_connected( cpdo_driver * self )
{
    DRV_DECL(0);
    return drv->isConnected;
}

static int cpdo_skel_error_info( cpdo_driver * self, char const ** dest, uint32_t * len, int * errorCode )
{
    DRV_DECL(cpdo_rc.ArgError);
    if( ! drv->conn ) return cpdo_rc.ConnectionError;
    else
    {
        return cpdo_rc.NYIError;
#if 0 /* it should look something like this: */
        if( errorCode ) *errorCode = mysql_errno(drv->conn);
        if( dest )
        {
            *dest = mysql_error(drv->conn);
            if( len )
            {
                *len = *dest ? strlen(*dest) : 0;
            }
        }
        return 0;
#endif
    }
}

static int cpdo_skel_sql_quote( cpdo_driver * self, char const * str, uint32_t * len, char ** dest )
{
    if( ! len || !dest ) return cpdo_rc.ArgError;
    else if( NULL == str )
    {
        char * tmp = (char *)malloc(5);
        if( ! tmp ) return cpdo_rc.AllocError;
        strcpy( tmp, "NULL" );
        *dest = tmp;
        *len = 4;
        return 0;
    }
    else
    {
#if 0 /* something like: */
        char * to = NULL;
        unsigned long aLen;
        DRV_DECL(cpdo_rc.ArgError);
        if( ! len ) return cpdo_rc.ArgError;
        aLen = *len * 2 + 1;
        to = calloc(aLen,1);
        if( ! to ) return cpdo_rc.AllocError;
        *len = mysql_real_escape_string( drv->conn, to, str, *len );
        *dest = to;
        return 0;
#else
        return cpdo_rc.NYIError;
#endif
    }
}

static int cpdo_skel_sql_qualify( cpdo_driver * self, char const * str, uint32_t * len, char ** dest )
{
    if(!str || !dest || !*str) return cpdo_rc.ArgError;
    else
    {
        char * rc = cpdo_mprintf("%s", str);
        if(!rc) return cpdo_rc.AllocError;
        else
        {
            *dest = rc;
            if(len) *len = strlen(rc);
            return 0;
        }
    }
}
    
static int cpdo_skel_free_string( cpdo_driver * self, char * str)
{
    return str ? (free(str),0) : cpdo_rc.ArgError;
}



static int cpdo_skel_prepare( cpdo_driver * self, cpdo_stmt ** tgt, char const * sql, uint32_t len  )
{
    cpdo_skel_stmt * st = NULL;
    cpdo_stmt * cst = NULL;
    DRV_DECL(cpdo_rc.ArgError);
    st = cpdo_skel_stmt_alloc();
    cst = &st->self;
    /* ... add impl here ... on error be sure to clean up st */
    
    cst->api->finalize(cst) /* is part of st, and will destroy it all */;
    return cpdo_rc.NYIError;
}

int cpdo_skel_connect( cpdo_driver * self, cpdo_connect_opt const * opt )
{
    DRV_DECL(cpdo_rc.ArgError);
    if( ! opt || !opt->dsn ) return cpdo_rc.ArgError;
    else
    {
#if 0
        enum { BufSize = 128U };
        char const * tokBegin = opt->dsn;
        char const * tokEnd = NULL;
        char kbuf[BufSize] = {0,0};
        char pDbName[BufSize] = {0,0};
        char pHost[BufSize] = {0,0};
        int port = 0;
        int rc = 0;
        if( drv->isConnected ) return cpdo_rc.ConnectionError;
        for( ; *tokBegin && (*tokBegin != ':'); ++tokBegin ) {
            /* skip driver name part of dsn. */
        }
        if( ':' != *tokBegin ) return cpdo_rc.RangeError;
        ++tokBegin /* skip ':'*/;
        port = rc = 0;
         /* Parse DSN options... */
        while( cpdo_next_token( &tokBegin, ';', &tokEnd) )
        { /* TODO: wrap most of this into a helper function
             which does the key/value splitting. We'll need
             this in other drivers.
          */
            if( tokBegin == tokEnd ) break;
            else
            {
                char const * key = tokBegin;
                char const * value = NULL;
                char * at = kbuf;
                if( (tokEnd - tokBegin) >= BufSize ) return cpdo_rc.RangeError;
                memset( kbuf, 0, BufSize );
                /* Write the key part to the buffer... */
                for( ; (key<tokEnd) && *key && ('='!=*key); ++key ) {
                    *(at++) = *key;
                }
                *(at++) = 0;
                value = at;
                if( '=' == *key ) {
                    ++key;
                }
                /* now write the value part to the buffer... */
                for( ; (key<tokEnd) && *key; ++key ) {
                    *(at++) = *key;
                }
                key = kbuf;
                /*MARKER("key=[%s] value=[%s]\n", key, value);*/

                /* Done parsing. Now see if we understand how to use
                   this option... */
                if( 0 == strcmp("port",key) )
                { /* remember that mysql ignores the port number when
                     connecting to localhost via a UNIX socket.
                  */
                    port = *value ? atoi(value) : 0;
                    if( port < 0 ) port = 0;
                }
                else if( 0 == strcmp("dbname",key) )
                {
                    size_t const slen = strlen(value);
                    if( slen >= BufSize ) return cpdo_rc.RangeError;
                    memcpy( pDbName, value, slen );
                    pDbName[slen] = 0;
                }
                else if( 0 == strcmp("host",key) )
                {
                    size_t const slen = strlen(value);
                    if( slen >= BufSize ) return cpdo_rc.RangeError;
                    memcpy( pHost, value, slen );
                    pDbName[slen] = 0;
                }
                else
                {
                    /* ignore unknown keys: this is optional in the CPDO
                       interface.
                    */
                }
                /* set up for the next token... */
                tokBegin = tokEnd;
                tokEnd = NULL;
            }
        } /* options parsing */
        
        /**
            TODO: establish the connection to drv->conn.
            On success set (drv->isConnected = 1).
        */
#endif        
        return cpdo_rc.NYIError;
    }
}


static int cpdo_skel_driver_begin_transaction( cpdo_driver * self )
{
    int rc;
    DRV_DECL(cpdo_rc.ArgError);
    if( drv->inTransaction ) return cpdo_rc.UnsupportedError;
    rc = cpdo_exec( self, "BEGIN", 5 );
    if( 0 == rc ) drv->inTransaction = 1;
    return rc;
}

static int cpdo_skel_driver_commit( cpdo_driver * self )
{
    int rc;
    DRV_DECL(cpdo_rc.ArgError);
    rc = cpdo_exec( self, "COMMIT", 6 );
    drv->inTransaction = 0;
    return (0==rc)
        ? 0
        : cpdo_rc.CheckDbError;
}

static int cpdo_skel_driver_rollback( cpdo_driver * self )
{
    int rc;
    DRV_DECL(cpdo_rc.ArgError);
    rc = cpdo_exec( self, "ROLLBACK", 8 );
    drv->inTransaction = 0;
    return (0==rc)
        ? 0
        : cpdo_rc.CheckDbError;
}

static char cpdo_skel_driver_in_trans( cpdo_driver * self )
{
    DRV_DECL(0);
    return drv->inTransaction;
}

static int cpdo_skel_driver_opt_set( cpdo_driver * self, char const * key, va_list vargs )
{
    return cpdo_rc.NYIError;
}
static int cpdo_skel_driver_opt_get( cpdo_driver * self, char const * key, va_list vargs )
{
    return cpdo_rc.NYIError;
}

static cpdo_step_code cpdo_skel_stmt_step( cpdo_stmt * self )
{
    STMT_DECL(CPDO_STEP_ERROR);
    if( ! stmt->stmt ) return CPDO_STEP_ERROR;
    return CPDO_STEP_ERROR;
}

static int cpdo_skel_stmt_reset( cpdo_stmt * self )
{
    STMT_DECL(cpdo_rc.ArgError);
    return cpdo_rc.NYIError;
}

static uint16_t cpdo_skel_stmt_column_count( cpdo_stmt * self )
{
    STMT_DECL(cpdo_rc.ArgError);
    return 0;
}

static char const * cpdo_skel_stmt_column_name( cpdo_stmt * self, uint16_t ndx )
{
    STMT_DECL(NULL);
    return NULL;
}

static uint16_t cpdo_skel_stmt_bind_count( cpdo_stmt * self )
{
    STMT_DECL(0);
    return 0;
}

static uint16_t cpdo_skel_stmt_param_index( cpdo_stmt * self, char const * name )
{
    STMT_DECL(0);
    return 0;
}

static char const * cpdo_skel_stmt_param_name( cpdo_stmt * self, uint16_t ndx )
{
    STMT_DECL(NULL);
    return NULL;
}

static int cpdo_skel_stmt_bind_null( cpdo_stmt * self, uint16_t ndx )
{
    STMT_DECL(cpdo_rc.ArgError);
    return cpdo_rc.NYIError;
}


static int cpdo_skel_stmt_bind_int8( cpdo_stmt * self, uint16_t ndx, int8_t v )
{
    BINDNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_bind_int16( cpdo_stmt * self, uint16_t ndx, int16_t v )
{
    BINDNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_bind_int32( cpdo_stmt * self, uint16_t ndx, int32_t v )
{
    BINDNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_bind_int64( cpdo_stmt * self, uint16_t ndx, int64_t v )
{
    BINDNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_bind_float( cpdo_stmt * self, uint16_t ndx, float v )
{
    BINDNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_bind_double( cpdo_stmt * self, uint16_t ndx, double v )
{
    BINDNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_bind_string( cpdo_stmt * self, uint16_t ndx, char const * v, uint32_t len )
{
    BINDNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_bind_blob( cpdo_stmt * self, uint16_t ndx, void const * v, uint32_t len )
{
    BINDNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_get_type_ndx( cpdo_stmt * self, uint16_t ndx, cpdo_data_type * val )
{
    GETNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_get_int8_ndx( cpdo_stmt * self, uint16_t ndx, int8_t * val )
{
    GETNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_get_int16_ndx( cpdo_stmt * self, uint16_t ndx, int16_t * val )
{
    GETNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_get_int32_ndx( cpdo_stmt * self, uint16_t ndx, int32_t * val )
{
    GETNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_get_int64_ndx( cpdo_stmt * self, uint16_t ndx, int64_t * val )
{
    GETNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_get_double_ndx( cpdo_stmt * self, uint16_t ndx, double * val )
{
    GETNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_get_float_ndx( cpdo_stmt * self, uint16_t ndx, float * val )
{
    GETNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_get_string_ndx( cpdo_stmt * self, uint16_t ndx, char const ** val, uint32_t * len )
{
    GETNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_get_blob_ndx( cpdo_stmt * self, uint16_t ndx, void const ** val, uint32_t * len )
{
    GETNDX_DECL(ndx);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_error_info( cpdo_stmt * self, char const ** dest, uint32_t * len, int * errorCode )
{
    STMT_DECL(cpdo_rc.ArgError);
    return cpdo_rc.NYIError;
}

static int cpdo_skel_stmt_finalize( cpdo_stmt * self )
{
    STMT_DECL(cpdo_rc.ArgError);
    return cpdo_skel_stmt_free(stmt);
}

static cpdo_driver_details const * cpdo_skel_driver_details()
{
    static const cpdo_driver_details bob = {
    CPDO_DRIVER_NAME/*driver_name*/,
    "FIXME:VERSION"/*driver_version*/,
    "FIXME:LICENSE"/*license*/,
    "http://fossil.wanderinghorse.net/repos/cpdo/" /*url*/,
    "FIXME: NAME (email or URL)" /*authors*/
    };
    return &bob;
}

int cpdo_driver_skel_register()
{
    return cpdo_driver_register( CPDO_DRIVER_NAME, cpdo_skel_driver_new );
}


#if defined(__cplusplus)
} /*extern "C"*/
#endif

#undef DRV_DECL
#undef STMT_DECL
#undef GETNDX_DECL
#undef BINDNDX_DECL
#undef MARKER
#undef CPDO_DRIVER_NAME
/* end of file cpdo_skeleton.c */
/* start of file cpdo_printf.c */
/************************************************************************
The printf-like implementation in this file is based on the one found
in the sqlite3 distribution is in the Public Domain.

This copy was forked for use with the clob API in Feb 2008 by Stephan
Beal (http://wanderinghorse.net/home/stephan/) and modified to send
its output to arbitrary targets via a callback mechanism. Also
refactored the %X specifier handlers a bit to make adding/removing
specific handlers easier.

All code in this file is released into the Public Domain.

The printf implementation (cpdo_printfv()) is pretty easy to extend
(e.g. adding or removing %-specifiers for cpdo_printfv()) if you're
willing to poke around a bit and see how the specifiers are declared
and dispatched. For an example, grep for 'etSTRING' and follow it
through the process of declaration to implementation.

See below for several CPDO_PRINTF_OMIT_xxx macros which can be set to
remove certain features/extensions.
************************************************************************/

#include <stdio.h> /* FILE */
#include <string.h> /* strlen() */
#include <stdlib.h> /* free/malloc() */
#include <ctype.h>
#include <stdint.h>
#if defined(__cplusplus)
extern "C" {
#endif

typedef long double LONGDOUBLE_TYPE;

/*
   If CPDO_PRINTF_OMIT_FLOATING_POINT is defined to a true value, then
   floating point conversions are disabled.
*/
#ifndef CPDO_PRINTF_OMIT_FLOATING_POINT
#  define CPDO_PRINTF_OMIT_FLOATING_POINT 0
#endif

/*
   If CPDO_PRINTF_OMIT_SIZE is defined to a true value, then
   the %n specifier is disabled.
*/
#ifndef CPDO_PRINTF_OMIT_SIZE
#  define CPDO_PRINTF_OMIT_SIZE 0
#endif

/*
   If CPDO_PRINTF_OMIT_SQL is defined to a true value, then
   the %q and %Q specifiers are disabled.
*/
#ifndef CPDO_PRINTF_OMIT_SQL
#  define CPDO_PRINTF_OMIT_SQL 0
#endif

/*
   If CPDO_PRINTF_OMIT_HTML is defined to a true value then the %h (HTML
   escape), %t (URL escape), and %T (URL unescape) specifiers are
   disabled.
*/
#ifndef CPDO_PRINTF_OMIT_HTML
#  define CPDO_PRINTF_OMIT_HTML 0
#endif

/*
Most C compilers handle variable-sized arrays, so we enable
that by default. Some (e.g. tcc) do not, so we provide a way
to disable it: set CPDO_PRINTF_HAVE_VARARRAY to 0

One approach would be to look at:

  defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)

but some compilers support variable-sized arrays even when not
explicitly running in c99 mode.
*/
#if !defined(CPDO_PRINTF_HAVE_VARARRAY)
#  if defined(__TINYC__)
#    define CPDO_PRINTF_HAVE_VARARRAY 0
#  elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#    define CPDO_PRINTF_HAVE_VARARRAY 1
#  else
#    define CPDO_PRINTF_HAVE_VARARRAY 0
#  endif
#endif

/**
CPDO_PRINTF_CHARARRAY is a helper to allocate variable-sized arrays.
This exists mainly so this code can compile with the tcc compiler.
*/
#if CPDO_PRINTF_HAVE_VARARRAY
#  define CPDO_PRINTF_CHARARRAY(V,N) char V[N+1]; memset(V,0,N+1);
#  define CPDO_PRINTF_CHARARRAY_FREE(V)
#else
#  define CPDO_PRINTF_CHARARRAY(V,N) char * V = (char *)malloc(N+1); memset(V,0,N+1);
#  define CPDO_PRINTF_CHARARRAY_FREE(V) free(V)
#endif

/*
   Conversion types fall into various categories as defined by the
   following enumeration.
*/
enum PrintfCategory {etRADIX = 1, /* Integer types.  %d, %x, %o, and so forth */
		     etFLOAT = 2, /* Floating point.  %f */
		     etEXP = 3, /* Exponentional notation. %e and %E */
		     etGENERIC = 4, /* Floating or exponential, depending on exponent. %g */
		     etSIZE = 5, /* Return number of characters processed so far. %n */
		     etSTRING = 6, /* Strings. %s */
		     etDYNSTRING = 7, /* Dynamically allocated strings. %z */
		     etPERCENT = 8, /* Percent symbol. %% */
		     etCHARX = 9, /* Characters. %c */
/* The rest are extensions, not normally found in printf() */
		     etCHARLIT = 10, /* Literal characters.  %' */
#if !CPDO_PRINTF_OMIT_SQL
		     etSQLESCAPE = 11, /* Strings with '\'' doubled.  %q */
		     etSQLESCAPE2 = 12, /* Strings with '\'' doubled and enclosed in '',
                          NULL pointers replaced by SQL NULL.  %Q */
		     etSQLESCAPE3 = 16, /* %w -> Strings with '\"' doubled */
#endif /* !CPDO_PRINTF_OMIT_SQL */
		     etPOINTER = 15, /* The %p conversion */
		     etORDINAL = 17, /* %r -> 1st, 2nd, 3rd, 4th, etc.  English only */
#if ! CPDO_PRINTF_OMIT_HTML
                     etHTML = 18, /* %h -> basic HTML escaping. */
                     etURLENCODE = 19, /* %t -> URL encoding. */
                     etURLDECODE = 20, /* %T -> URL decoding. */
#endif
		     etPLACEHOLDER = 100
};

/*
   An "etByte" is an 8-bit unsigned value.
*/
typedef unsigned char etByte;

/*
   Each builtin conversion character (ex: the 'd' in "%d") is described
   by an instance of the following structure
*/
typedef struct et_info {   /* Information about each format field */
  char fmttype;            /* The format field code letter */
  etByte base;             /* The base for radix conversion */
  etByte flags;            /* One or more of FLAG_ constants below */
  etByte type;             /* Conversion paradigm */
  etByte charset;          /* Offset into aDigits[] of the digits string */
  etByte prefix;           /* Offset into aPrefix[] of the prefix string */
} et_info;

/*
   Allowed values for et_info.flags
*/
enum et_info_flags { FLAG_SIGNED = 1,    /* True if the value to convert is signed */
		     FLAG_EXTENDED = 2,  /* True if for internal/extended use only. */
		     FLAG_STRING = 4     /* Allow infinity precision */
};

/*
  Historically, the following table was searched linearly, so the most
  common conversions were kept at the front.

  Change 2008 Oct 31 by Stephan Beal: we reserve an array or ordered
  entries for all chars in the range [32..126]. Format character
  checks can now be done in constant time by addressing that array
  directly.  This takes more static memory, but reduces the time and
  per-call overhead costs of cpdo_printfv().
*/
static const char aDigits[] = "0123456789ABCDEF0123456789abcdef";
static const char aPrefix[] = "-x0\000X0";
static const et_info fmtinfo[] = {
/**
   If CPDO_PRINTF_FMTINFO_FIXED is 1 then we use the original
   implementation: a linear list of entries. Search time is linear. If
   CPDO_PRINTF_FMTINFO_FIXED is 0 then we use a fixed-size array which
   we index directly using the format char as the key.
*/
#define CPDO_PRINTF_FMTINFO_FIXED 0
#if CPDO_PRINTF_FMTINFO_FIXED
  {  'd', 10, FLAG_SIGNED, etRADIX,      0,  0 },
  {  's',  0, FLAG_STRING, etSTRING,     0,  0 },
  {  'g',  0, FLAG_SIGNED, etGENERIC,    30, 0 },
  {  'z',  0, FLAG_STRING, etDYNSTRING,  0,  0 },
  {  'c',  0, 0, etCHARX,      0,  0 },
  {  'o',  8, 0, etRADIX,      0,  2 },
  {  'u', 10, 0, etRADIX,      0,  0 },
  {  'x', 16, 0, etRADIX,      16, 1 },
  {  'X', 16, 0, etRADIX,      0,  4 },
  {  'i', 10, FLAG_SIGNED, etRADIX,      0,  0 },
#if !CPDO_PRINTF_OMIT_FLOATING_POINT
  {  'f',  0, FLAG_SIGNED, etFLOAT,      0,  0 },
  {  'e',  0, FLAG_SIGNED, etEXP,        30, 0 },
  {  'E',  0, FLAG_SIGNED, etEXP,        14, 0 },
  {  'G',  0, FLAG_SIGNED, etGENERIC,    14, 0 },
#endif /* !CPDO_PRINTF_OMIT_FLOATING_POINT */
  {  '%',  0, 0, etPERCENT,    0,  0 },
  {  'p', 16, 0, etPOINTER,    0,  1 },
  {  'r', 10, (FLAG_EXTENDED|FLAG_SIGNED), etORDINAL,    0,  0 },
#if ! CPDO_PRINTF_OMIT_SQL
  {  'q',  0, FLAG_STRING, etSQLESCAPE,  0,  0 },
  {  'Q',  0, FLAG_STRING, etSQLESCAPE2, 0,  0 },
  {  'w',  0, FLAG_STRING, etSQLESCAPE3, 0,  0 },
#endif /* !CPDO_PRINTF_OMIT_SQL */
#if ! CPDO_PRINTF_OMIT_HTML
  {  'h',  0, FLAG_STRING, etHTML, 0, 0 },
  {  't',  0, FLAG_STRING, etURLENCODE, 0, 0 },
  {  'T',  0, FLAG_STRING, etURLDECODE, 0, 0 },
#endif /* !CPDO_PRINTF_OMIT_HTML */
#if !CPDO_PRINTF_OMIT_SIZE
  {  'n',  0, 0, etSIZE,       0,  0 },
#endif
#else /* CPDO_PRINTF_FMTINFO_FIXED */
  /*
    These entries MUST stay in ASCII order, sorted
    on their fmttype member!
  */
  {' '/*32*/, 0, 0, 0, 0, 0 },
  {'!'/*33*/, 0, 0, 0, 0, 0 },
  {'"'/*34*/, 0, 0, 0, 0, 0 },
  {'#'/*35*/, 0, 0, 0, 0, 0 },
  {'$'/*36*/, 0, 0, 0, 0, 0 },
  {'%'/*37*/, 0, 0, etPERCENT, 0, 0 },
  {'&'/*38*/, 0, 0, 0, 0, 0 },
  {'\''/*39*/, 0, 0, 0, 0, 0 },
  {'('/*40*/, 0, 0, 0, 0, 0 },
  {')'/*41*/, 0, 0, 0, 0, 0 },
  {'*'/*42*/, 0, 0, 0, 0, 0 },
  {'+'/*43*/, 0, 0, 0, 0, 0 },
  {','/*44*/, 0, 0, 0, 0, 0 },
  {'-'/*45*/, 0, 0, 0, 0, 0 },
  {'.'/*46*/, 0, 0, 0, 0, 0 },
  {'/'/*47*/, 0, 0, 0, 0, 0 },
  {'0'/*48*/, 0, 0, 0, 0, 0 },
  {'1'/*49*/, 0, 0, 0, 0, 0 },
  {'2'/*50*/, 0, 0, 0, 0, 0 },
  {'3'/*51*/, 0, 0, 0, 0, 0 },
  {'4'/*52*/, 0, 0, 0, 0, 0 },
  {'5'/*53*/, 0, 0, 0, 0, 0 },
  {'6'/*54*/, 0, 0, 0, 0, 0 },
  {'7'/*55*/, 0, 0, 0, 0, 0 },
  {'8'/*56*/, 0, 0, 0, 0, 0 },
  {'9'/*57*/, 0, 0, 0, 0, 0 },
  {':'/*58*/, 0, 0, 0, 0, 0 },
  {';'/*59*/, 0, 0, 0, 0, 0 },
  {'<'/*60*/, 0, 0, 0, 0, 0 },
  {'='/*61*/, 0, 0, 0, 0, 0 },
  {'>'/*62*/, 0, 0, 0, 0, 0 },
  {'?'/*63*/, 0, 0, 0, 0, 0 },
  {'@'/*64*/, 0, 0, 0, 0, 0 },
  {'A'/*65*/, 0, 0, 0, 0, 0 },
  {'B'/*66*/, 0, 0, 0, 0, 0 },
  {'C'/*67*/, 0, 0, 0, 0, 0 },
  {'D'/*68*/, 0, 0, 0, 0, 0 },
  {'E'/*69*/, 0, FLAG_SIGNED, etEXP, 14, 0 },
  {'F'/*70*/, 0, 0, 0, 0, 0 },
  {'G'/*71*/, 0, FLAG_SIGNED, etGENERIC, 14, 0 },
  {'H'/*72*/, 0, 0, 0, 0, 0 },
  {'I'/*73*/, 0, 0, 0, 0, 0 },
  {'J'/*74*/, 0, 0, 0, 0, 0 },
  {'K'/*75*/, 0, 0, 0, 0, 0 },
  {'L'/*76*/, 0, 0, 0, 0, 0 },
  {'M'/*77*/, 0, 0, 0, 0, 0 },
  {'N'/*78*/, 0, 0, 0, 0, 0 },
  {'O'/*79*/, 0, 0, 0, 0, 0 },
  {'P'/*80*/, 0, 0, 0, 0, 0 },
  {'Q'/*81*/, 0, FLAG_STRING, etSQLESCAPE2, 0, 0 },
  {'R'/*82*/, 0, 0, 0, 0, 0 },
  {'S'/*83*/, 0, 0, 0, 0, 0 },
  {'T'/*84*/,  0, FLAG_STRING, etURLDECODE, 0, 0 },
  {'U'/*85*/, 0, 0, 0, 0, 0 },
  {'V'/*86*/, 0, 0, 0, 0, 0 },
  {'W'/*87*/, 0, 0, 0, 0, 0 },
  {'X'/*88*/, 16, 0, etRADIX,      0,  4 },
  {'Y'/*89*/, 0, 0, 0, 0, 0 },
  {'Z'/*90*/, 0, 0, 0, 0, 0 },
  {'['/*91*/, 0, 0, 0, 0, 0 },
  {'\\'/*92*/, 0, 0, 0, 0, 0 },
  {']'/*93*/, 0, 0, 0, 0, 0 },
  {'^'/*94*/, 0, 0, 0, 0, 0 },
  {'_'/*95*/, 0, 0, 0, 0, 0 },
  {'`'/*96*/, 0, 0, 0, 0, 0 },
  {'a'/*97*/, 0, 0, 0, 0, 0 },
  {'b'/*98*/, 0, 0, 0, 0, 0 },
  {'c'/*99*/, 0, 0, etCHARX,      0,  0 },
  {'d'/*100*/, 10, FLAG_SIGNED, etRADIX,      0,  0 },
  {'e'/*101*/, 0, FLAG_SIGNED, etEXP,        30, 0 },
  {'f'/*102*/, 0, FLAG_SIGNED, etFLOAT,      0,  0},
  {'g'/*103*/, 0, FLAG_SIGNED, etGENERIC,    30, 0 },
  {'h'/*104*/, 0, FLAG_STRING, etHTML, 0, 0 },
  {'i'/*105*/, 10, FLAG_SIGNED, etRADIX,      0,  0},
  {'j'/*106*/, 0, 0, 0, 0, 0 },
  {'k'/*107*/, 0, 0, 0, 0, 0 },
  {'l'/*108*/, 0, 0, 0, 0, 0 },
  {'m'/*109*/, 0, 0, 0, 0, 0 },
  {'n'/*110*/, 0, 0, etSIZE, 0, 0 },
  {'o'/*111*/, 8, 0, etRADIX,      0,  2 },
  {'p'/*112*/, 16, 0, etPOINTER, 0, 1 },
  {'q'/*113*/, 0, FLAG_STRING, etSQLESCAPE,  0, 0 },
  {'r'/*114*/, 10, (FLAG_EXTENDED|FLAG_SIGNED), etORDINAL,    0,  0},
  {'s'/*115*/, 0, FLAG_STRING, etSTRING,     0,  0 },
  {'t'/*116*/,  0, FLAG_STRING, etURLENCODE, 0, 0 },
  {'u'/*117*/, 10, 0, etRADIX,      0,  0 },
  {'v'/*118*/, 0, 0, 0, 0, 0 },
  {'w'/*119*/, 0, FLAG_STRING, etSQLESCAPE3, 0, 0 },
  {'x'/*120*/, 16, 0, etRADIX,      16, 1  },
  {'y'/*121*/, 0, 0, 0, 0, 0 },
  {'z'/*122*/, 0, FLAG_STRING, etDYNSTRING,  0,  0},
  {'{'/*123*/, 0, 0, 0, 0, 0 },
  {'|'/*124*/, 0, 0, 0, 0, 0 },
  {'}'/*125*/, 0, 0, 0, 0, 0 },
  {'~'/*126*/, 0, 0, 0, 0, 0 },
#endif /* CPDO_PRINTF_FMTINFO_FIXED */
};
#define etNINFO  (sizeof(fmtinfo)/sizeof(fmtinfo[0]))

#if ! CPDO_PRINTF_OMIT_FLOATING_POINT
/*
   "*val" is a double such that 0.1 <= *val < 10.0
   Return the ascii code for the leading digit of *val, then
   multiply "*val" by 10.0 to renormalize.
**
   Example:
       input:     *val = 3.14159
       output:    *val = 1.4159    function return = '3'
**
   The counter *cnt is incremented each time.  After counter exceeds
   16 (the number of significant digits in a 64-bit float) '0' is
   always returned.
*/
static int et_getdigit(LONGDOUBLE_TYPE *val, int *cnt){
  int digit;
  LONGDOUBLE_TYPE d;
  if( (*cnt)++ >= 16 ) return '0';
  digit = (int)*val;
  d = digit;
  digit += '0';
  *val = (*val - d)*10.0;
  return digit;
}
#endif /* !CPDO_PRINTF_OMIT_FLOATING_POINT */

/*
   On machines with a small(?) stack size, you can redefine the
   CPDO_PRINTF_BUF_SIZE to be less than 350.  But beware - for smaller
   values some %f conversions may go into an infinite loop.
*/
#ifndef CPDO_PRINTF_BUF_SIZE
#  define CPDO_PRINTF_BUF_SIZE 350  /* Size of the output buffer for numeric conversions */
#endif

#if ! defined(__STDC__) && !defined(__TINYC__)
#ifdef CPDO_PRINTF_INT64_TYPE
  typedef CPDO_PRINTF_INT64_TYPE int64_t;
  typedef unsigned CPDO_PRINTF_INT64_TYPE uint64_t;
#elif defined(_MSC_VER) || defined(__BORLANDC__)
  typedef __int64 int64_t;
  typedef unsigned __int64 uint64_t;
#else
  typedef long long int int64_t;
  typedef unsigned long long int uint64_t;
#endif
#endif

#if 0
/   Not yet used. */
enum PrintfArgTypes {
TypeInt = 0,
TypeIntP = 1,
TypeFloat = 2,
TypeFloatP = 3,
TypeCString = 4
};
#endif


#if 0
/   Not yet used. */
typedef struct cpdo_printf_spec_handler_def
{
	char letter; /   e.g. %s */
	int xtype; /* reference to the etXXXX values, or fmtinfo[*].type. */
	int ntype; /* reference to PrintfArgTypes enum. */
} spec_handler;
#endif

/**
   cpdo_printf_spec_handler is an almost-generic interface for farming
   work out of cpdo_printfv()'s code into external functions.  It doesn't
   actually save much (if any) overall code, but it makes the cpdo_printfv()
   code more manageable.


   REQUIREMENTS of implementations:

   - Expects an implementation-specific vargp pointer.
   cpdo_printfv() passes a pointer to the converted value of
   an entry from the format va_list. If it passes a type
   other than the expected one, undefined results.

   - If it calls pf then it must return the return value
   from that function.

   - If it calls pf it must do: pf( pfArg, D, N ), where D is
   the data to export and N is the number of bytes to export.
   It may call pf() an arbitrary number of times

   - If pf() successfully is called, the return value must be the
   accumulated totals of its return value(s), plus (possibly, but
   unlikely) an imnplementation-specific amount.

   - If it does not call pf() then it must return 0 (success)
   or a negative number (an error) or do all of the export
   processing itself and return the number of bytes exported.


   SIGNIFICANT LIMITATIONS:

   - Has no way of iterating over the format string,
   so handling precisions and such here can't work too
   well.
*/
typedef long (*cpdo_printf_spec_handler)( cpdo_printf_appender pf,
				       void * pfArg,
				       void * vargp );


/**
  cpdo_printf_spec_handler for etSTRING types. It assumes that varg is a
  null-terminated (char [const] *)
*/
static long spech_string( cpdo_printf_appender pf,
			  void * pfArg,
			  void * varg )
{
	char const * ch = (char const *) varg;
	return ch ? pf( pfArg, ch, strlen(ch) ) : 0;
}

/**
  cpdo_printf_spec_handler for etDYNSTRING types.  It assumes that varg
  is a non-const (char *). It behaves identically to spec_string() and
  then calls free() on that (char *).
*/
static long spech_dynstring( cpdo_printf_appender pf,
			     void * pfArg,
			     void * varg )
{
  long ret = spech_string( pf, pfArg, varg );
  free( (char *) varg );
  return ret;
}

#if !CPDO_PRINTF_OMIT_HTML
static long spech_string_to_html( cpdo_printf_appender pf,
                                  void * pfArg,
                                  void * varg )
{
    char const * ch = (char const *) varg;
    long ret = 0;
    if( ! ch ) return 0;
    ret = 0;
    for( ; *ch; ++ch )
    {
        switch( *ch )
        {
          case '<': ret += pf( pfArg, "&lt;", 4 );
              break;
          case '&': ret += pf( pfArg, "&amp;", 5 );
              break;
          default:
              ret += pf( pfArg, ch, 1 );
              break;
        };
    }
    return ret;
}

static int httpurl_needs_escape( int c )
{
    /*
      Definition of "safe" and "unsafe" chars
      was taken from:

      http://www.codeguru.com/cpp/cpp/cpp_mfc/article.php/c4029/
    */
    return ( (c >= 32 && c <=47)
             || ( c>=58 && c<=64)
             || ( c>=91 && c<=96)
             || ( c>=123 && c<=126)
             || ( c<32 || c>=127)
             );
}

/**
   The handler for the etURLENCODE specifier.

   It expects varg to be a string value, which it will preceed to
   encode using an URL encoding algothrim (certain characters are
   converted to %XX, where XX is their hex value) and passes the
   encoded string to pf(). It returns the total length of the output
   string.
 */
static long spech_urlencode( cpdo_printf_appender pf,
                             void * pfArg,
                             void * varg )
{
    char const * str = (char const *) varg;
    long ret = 0;
    char ch = 0;
    char const * hex = "0123456789ABCDEF";
#define xbufsz 10
    char xbuf[xbufsz];
    int slen = 0;
    if( ! str ) return 0;
    memset( xbuf, 0, xbufsz );
    ch = *str;
#define xbufsz 10
    for( ; ch; ch = *(++str) )
    {
        if( ! httpurl_needs_escape( ch ) )
        {
            ret += pf( pfArg, str, 1 );
            continue;
        }
        else {
            slen = sprintf( xbuf, "%%%c%c",
                            hex[((ch>>4)&0xf)],
                            hex[(ch&0xf)]);
            ret += pf( pfArg, xbuf, slen );
        }
    }
#undef xbufsz
    return ret;
}

/* 
   hexchar_to_int():

   For 'a'-'f', 'A'-'F' and '0'-'9', returns the appropriate decimal
   number.  For any other character it returns -1.
    */
static int hexchar_to_int( int ch )
{
    if( (ch>='a' && ch<='f') ) return ch-'a'+10;
    else if( (ch>='A' && ch<='F') ) return ch-'A'+10;
    else if( (ch>='0' && ch<='9') ) return ch-'0';
    return -1;
}

/**
   The handler for the etURLDECODE specifier.

   It expects varg to be a ([const] char *), possibly encoded
   with URL encoding. It decodes the string using a URL decode
   algorithm and passes the decoded string to
   pf(). It returns the total length of the output string.
   If the input string contains malformed %XX codes then this
   function will return prematurely.
 */
static long spech_urldecode( cpdo_printf_appender pf,
                             void * pfArg,
                             void * varg )
{
    char const * str = (char const *) varg;
    long ret = 0;
    char ch = 0;
    char ch2 = 0;
    char xbuf[4];
    int decoded;
    ch = *str;
    if( ! str ) return 0;
    while( ch )
    {
        if( ch == '%' )
        {
            ch = *(++str);
            ch2 = *(++str);
            if( isxdigit(ch) &&
                isxdigit(ch2) )
            {
                decoded = (hexchar_to_int( ch ) * 16)
                    + hexchar_to_int( ch2 );
                xbuf[0] = (char)decoded;
                xbuf[1] = 0;
                ret += pf( pfArg, xbuf, 1 );
                ch = *(++str);
                continue;
            }
            else
            {
                xbuf[0] = '%';
                xbuf[1] = ch;
                xbuf[2] = ch2;
                xbuf[3] = 0;
                ret += pf( pfArg, xbuf, 3 );
                ch = *(++str);
                continue;
            }
        }
        else if( ch == '+' )
        {
            xbuf[0] = ' ';
            xbuf[1] = 0;
            ret += pf( pfArg, xbuf, 1 );
            ch = *(++str);
            continue;
        }
        xbuf[0] = ch;
        xbuf[1] = 0;
        ret += pf( pfArg, xbuf, 1 );
        ch = *(++str);
    }
    return ret;
}

#endif /* !CPDO_PRINTF_OMIT_HTML */


#if !CPDO_PRINTF_OMIT_SQL
/**
   Quotes the (char *) varg as an SQL string 'should'
   be quoted. The exact type of the conversion
   is specified by xtype, which must be one of
   etSQLESCAPE, etSQLESCAPE2, or etSQLESCAPE3.

   Search this file for those constants to find
   the associated documentation.
*/
static long spech_sqlstring_main( int xtype,
				  cpdo_printf_appender pf,
				  void * pfArg,
				  void * varg )
{
    int i, j, n, ch, isnull;
    int needQuote;
    char q = ((xtype==etSQLESCAPE3)?'"':'\'');   /* Quote character */
    char const * escarg = (char const *) varg;
    long ret;
	char * bufpt = 0;
        isnull = escarg==0;
        if( isnull ) escarg = (xtype==etSQLESCAPE2 ? "NULL" : "(NULL)");
        for(i=n=0; (ch=escarg[i])!=0; i++){
          if( ch==q )  n++;
        }
        needQuote = !isnull && xtype==etSQLESCAPE2;
        n += i + 1 + needQuote*2;
	/* FIXME: use a static buffer here instead of malloc()! Shame on you!*/
	bufpt = (char *)malloc( n );
	if( ! bufpt ) return -1;
        j = 0;
        if( needQuote ) bufpt[j++] = q;
        for(i=0; (ch=escarg[i])!=0; i++){
          bufpt[j++] = ch;
          if( ch==q ) bufpt[j++] = ch;
        }
        if( needQuote ) bufpt[j++] = q;
        bufpt[j] = 0;
	ret = pf( pfArg, bufpt, j );
	free( bufpt );
	return ret;
}

static long spech_sqlstring1( cpdo_printf_appender pf,
			      void * pfArg,
			      void * varg )
{
	return spech_sqlstring_main( etSQLESCAPE, pf, pfArg, varg );
}

static long spech_sqlstring2( cpdo_printf_appender pf,
			      void * pfArg,
			      void * varg )
{
	return spech_sqlstring_main( etSQLESCAPE2, pf, pfArg, varg );
}

static long spech_sqlstring3( cpdo_printf_appender pf,
			      void * pfArg,
			      void * varg )
{
	return spech_sqlstring_main( etSQLESCAPE3, pf, pfArg, varg );
}

#endif /* !CPDO_PRINTF_OMIT_SQL */

				      

/*
   The root printf program.  All variations call this core.  It
   implements most of the common printf behaviours plus (optionally)
   some extended ones.

   INPUTS:

     pfAppend : The is a cpdo_printf_appender function which is responsible
     for accumulating the output. If pfAppend returns a negative integer
     then processing stops immediately.

     pfAppendArg : is ignored by this function but passed as the first
     argument to pfAppend. pfAppend will presumably use it as a data
     store for accumulating its string.

     fmt : This is the format string, as in the usual printf().

     ap : This is a pointer to a list of arguments.  Same as in
     vprintf() and friends.

   OUTPUTS:

   The return value is the total number of characters sent to the
   function "func".  Returns -1 on a error.

   Note that the order in which automatic variables are declared below
   seems to make a big difference in determining how fast this beast
   will run.

   Much of this code dates back to the early 1980's, supposedly.

   Known change history (most historic info has been lost):

   10 Feb 2008 by Stephan Beal: refactored to remove the 'useExtended'
   flag (which is now always on). Added the cpdo_printf_appender typedef to
   make this function generic enough to drop into other source trees
   without much work.

   31 Oct 2008 by Stephan Beal: refactored the et_info lookup to be
   constant-time instead of linear.
*/
long cpdo_printfv(
  cpdo_printf_appender pfAppend,          /* Accumulate results here */
  void * pfAppendArg,                /* Passed as first arg to pfAppend. */
  const char *fmt,                   /* Format string */
  va_list ap                         /* arguments */
){
    /**
       HISTORIC NOTE (author and year unknown):

       Note that the order in which automatic variables are declared below
       seems to make a big difference in determining how fast this beast
       will run.
    */

#if CPDO_PRINTF_FMTINFO_FIXED
  const int useExtended = 1; /* Allow extended %-conversions */
#endif
  long outCount = 0;          /* accumulated output count */
  int pfrc = 0;              /* result from calling pfAppend */
  int c;                     /* Next character in the format string */
  char *bufpt = 0;           /* Pointer to the conversion buffer */
  int precision;             /* Precision of the current field */
  int length;                /* Length of the field */
  int idx;                   /* A general purpose loop counter */
  int width;                 /* Width of the current field */
  etByte flag_leftjustify;   /* True if "-" flag is present */
  etByte flag_plussign;      /* True if "+" flag is present */
  etByte flag_blanksign;     /* True if " " flag is present */
  etByte flag_alternateform; /* True if "#" flag is present */
  etByte flag_altform2;      /* True if "!" flag is present */
  etByte flag_zeropad;       /* True if field width constant starts with zero */
  etByte flag_long;          /* True if "l" flag is present */
  etByte flag_longlong;      /* True if the "ll" flag is present */
  etByte done;               /* Loop termination flag */
  uint64_t longvalue;   /* Value for integer types */
  LONGDOUBLE_TYPE realvalue; /* Value for real types */
  const et_info *infop = 0;      /* Pointer to the appropriate info structure */
  char buf[CPDO_PRINTF_BUF_SIZE];       /* Conversion buffer */
  char prefix;               /* Prefix character.  "+" or "-" or " " or '\0'. */
  etByte xtype = 0;              /* Conversion paradigm */
  char * zExtra = 0;              /* Extra memory used for etTCLESCAPE conversions */
#if ! CPDO_PRINTF_OMIT_FLOATING_POINT
  int  exp, e2;              /* exponent of real numbers */
  double rounder;            /* Used for rounding floating point values */
  etByte flag_dp;            /* True if decimal point should be shown */
  etByte flag_rtz;           /* True if trailing zeros should be removed */
  etByte flag_exp;           /* True to force display of the exponent */
  int nsd;                   /* Number of significant digits returned */
#endif
    cpdo_printf_spec_handler spf;


  /* CPDO_PRINTF_RETURN, CPDO_PRINTF_CHECKERR, and CPDO_PRINTF_SPACES
     are internal helpers.
  */
#define CPDO_PRINTF_RETURN if( zExtra ) free(zExtra); return outCount;
#define CPDO_PRINTF_CHECKERR(FREEME) if( pfrc<0 ) { CPDO_PRINTF_CHARARRAY_FREE(FREEME); CPDO_PRINTF_RETURN; } else outCount += pfrc;
#define CPDO_PRINTF_SPACES(N) \
if(1){				       \
    CPDO_PRINTF_CHARARRAY(zSpaces,N);		      \
    memset( zSpaces,' ',N);			      \
    pfrc = pfAppend(pfAppendArg, zSpaces, N);	      \
    CPDO_PRINTF_CHECKERR(zSpaces);			      \
    CPDO_PRINTF_CHARARRAY_FREE(zSpaces);		      \
}

  length = 0;
  bufpt = 0;
  for(; (c=(*fmt))!=0; ++fmt){
    if( c!='%' ){
      int amt;
      bufpt = (char *)fmt;
      amt = 1;
      while( (c=(*++fmt))!='%' && c!=0 ) amt++;
      pfrc = pfAppend( pfAppendArg, bufpt, amt);
      CPDO_PRINTF_CHECKERR(0);
      if( c==0 ) break;
    }
    if( (c=(*++fmt))==0 ){
      pfrc = pfAppend( pfAppendArg, "%", 1);
      CPDO_PRINTF_CHECKERR(0);
      break;
    }
    /* Find out what flags are present */
    flag_leftjustify = flag_plussign = flag_blanksign = 
     flag_alternateform = flag_altform2 = flag_zeropad = 0;
    done = 0;
    do{
      switch( c ){
        case '-':   flag_leftjustify = 1;     break;
        case '+':   flag_plussign = 1;        break;
        case ' ':   flag_blanksign = 1;       break;
        case '#':   flag_alternateform = 1;   break;
        case '!':   flag_altform2 = 1;        break;
        case '0':   flag_zeropad = 1;         break;
        default:    done = 1;                 break;
      }
    }while( !done && (c=(*++fmt))!=0 );
    /* Get the field width */
    width = 0;
    if( c=='*' ){
      width = va_arg(ap,int);
      if( width<0 ){
        flag_leftjustify = 1;
        width = -width;
      }
      c = *++fmt;
    }else{
      while( c>='0' && c<='9' ){
        width = width*10 + c - '0';
        c = *++fmt;
      }
    }
    if( width > CPDO_PRINTF_BUF_SIZE-10 ){
      width = CPDO_PRINTF_BUF_SIZE-10;
    }
    /* Get the precision */
    if( c=='.' ){
      precision = 0;
      c = *++fmt;
      if( c=='*' ){
        precision = va_arg(ap,int);
        if( precision<0 ) precision = -precision;
        c = *++fmt;
      }else{
        while( c>='0' && c<='9' ){
          precision = precision*10 + c - '0';
          c = *++fmt;
        }
      }
    }else{
      precision = -1;
    }
    /* Get the conversion type modifier */
    if( c=='l' ){
      flag_long = 1;
      c = *++fmt;
      if( c=='l' ){
        flag_longlong = 1;
        c = *++fmt;
      }else{
        flag_longlong = 0;
      }
    }else{
      flag_long = flag_longlong = 0;
    }
    /* Fetch the info entry for the field */
    infop = 0;
#if CPDO_PRINTF_FMTINFO_FIXED
    for(idx=0; idx<etNINFO; idx++){
      if( c==fmtinfo[idx].fmttype ){
        infop = &fmtinfo[idx];
        if( useExtended || (infop->flags & FLAG_EXTENDED)==0 ){
          xtype = infop->type;
        }else{
	    CPDO_PRINTF_RETURN;
        }
        break;
      }
    }
#else
#define FMTNDX(N) (N - fmtinfo[0].fmttype)
#define FMTINFO(N) (fmtinfo[ FMTNDX(N) ])
    infop = ((c>=(fmtinfo[0].fmttype)) && (c<fmtinfo[etNINFO-1].fmttype))
	? &FMTINFO(c)
	: 0;
    if( infop ) xtype = infop->type;
#undef FMTINFO
#undef FMTNDX
#endif /* CPDO_PRINTF_FMTINFO_FIXED */
    zExtra = 0;
    if( (!infop) || (!infop->type) ){
	CPDO_PRINTF_RETURN;
    }


    /* Limit the precision to prevent overflowing buf[] during conversion */
    if( precision>CPDO_PRINTF_BUF_SIZE-40 && (infop->flags & FLAG_STRING)==0 ){
      precision = CPDO_PRINTF_BUF_SIZE-40;
    }

    /*
       At this point, variables are initialized as follows:
    **
         flag_alternateform          TRUE if a '#' is present.
         flag_altform2               TRUE if a '!' is present.
         flag_plussign               TRUE if a '+' is present.
         flag_leftjustify            TRUE if a '-' is present or if the
                                     field width was negative.
         flag_zeropad                TRUE if the width began with 0.
         flag_long                   TRUE if the letter 'l' (ell) prefixed
                                     the conversion character.
         flag_longlong               TRUE if the letter 'll' (ell ell) prefixed
                                     the conversion character.
         flag_blanksign              TRUE if a ' ' is present.
         width                       The specified field width.  This is
                                     always non-negative.  Zero is the default.
         precision                   The specified precision.  The default
                                     is -1.
         xtype                       The class of the conversion.
         infop                       Pointer to the appropriate info struct.
    */
    switch( xtype ){
      case etPOINTER:
        flag_longlong = sizeof(char*)==sizeof(int64_t);
        flag_long = sizeof(char*)==sizeof(long int);
        /* Fall through into the next case */
      case etORDINAL:
      case etRADIX:
        if( infop->flags & FLAG_SIGNED ){
          int64_t v;
          if( flag_longlong )   v = va_arg(ap,int64_t);
          else if( flag_long )  v = va_arg(ap,long int);
          else                  v = va_arg(ap,int);
          if( v<0 ){
            longvalue = -v;
            prefix = '-';
          }else{
            longvalue = v;
            if( flag_plussign )        prefix = '+';
            else if( flag_blanksign )  prefix = ' ';
            else                       prefix = 0;
          }
        }else{
          if( flag_longlong )   longvalue = va_arg(ap,uint64_t);
          else if( flag_long )  longvalue = va_arg(ap,unsigned long int);
          else                  longvalue = va_arg(ap,unsigned int);
          prefix = 0;
        }
        if( longvalue==0 ) flag_alternateform = 0;
        if( flag_zeropad && precision<width-(prefix!=0) ){
          precision = width-(prefix!=0);
        }
        bufpt = &buf[CPDO_PRINTF_BUF_SIZE-1];
        if( xtype==etORDINAL ){
	    /** i sure would like to shake the hand of whoever figured this out: */
          static const char zOrd[] = "thstndrd";
          int x = longvalue % 10;
          if( x>=4 || (longvalue/10)%10==1 ){
            x = 0;
          }
          buf[CPDO_PRINTF_BUF_SIZE-3] = zOrd[x*2];
          buf[CPDO_PRINTF_BUF_SIZE-2] = zOrd[x*2+1];
          bufpt -= 2;
        }
        {
          const char *cset;
          int base;
          cset = &aDigits[infop->charset];
          base = infop->base;
          do{                                           /* Convert to ascii */
            *(--bufpt) = cset[longvalue%base];
            longvalue = longvalue/base;
          }while( longvalue>0 );
        }
        length = &buf[CPDO_PRINTF_BUF_SIZE-1]-bufpt;
        for(idx=precision-length; idx>0; idx--){
          *(--bufpt) = '0';                             /* Zero pad */
        }
        if( prefix ) *(--bufpt) = prefix;               /* Add sign */
        if( flag_alternateform && infop->prefix ){      /* Add "0" or "0x" */
          const char *pre;
          char x;
          pre = &aPrefix[infop->prefix];
          if( *bufpt!=pre[0] ){
            for(; (x=(*pre))!=0; pre++) *(--bufpt) = x;
          }
        }
        length = &buf[CPDO_PRINTF_BUF_SIZE-1]-bufpt;
        break;
      case etFLOAT:
      case etEXP:
      case etGENERIC:
        realvalue = va_arg(ap,double);
#if ! CPDO_PRINTF_OMIT_FLOATING_POINT
        if( precision<0 ) precision = 6;         /* Set default precision */
        if( precision>CPDO_PRINTF_BUF_SIZE/2-10 ) precision = CPDO_PRINTF_BUF_SIZE/2-10;
        if( realvalue<0.0 ){
          realvalue = -realvalue;
          prefix = '-';
        }else{
          if( flag_plussign )          prefix = '+';
          else if( flag_blanksign )    prefix = ' ';
          else                         prefix = 0;
        }
        if( xtype==etGENERIC && precision>0 ) precision--;
#if 0
        /* Rounding works like BSD when the constant 0.4999 is used.  Wierd! */
        for(idx=precision, rounder=0.4999; idx>0; idx--, rounder*=0.1);
#else
        /* It makes more sense to use 0.5 */
        for(idx=precision, rounder=0.5; idx>0; idx--, rounder*=0.1){}
#endif
        if( xtype==etFLOAT ) realvalue += rounder;
        /* Normalize realvalue to within 10.0 > realvalue >= 1.0 */
        exp = 0;
#if 1
	if( (realvalue)!=(realvalue) ){
	    /* from sqlite3: #define sqlite3_isnan(X)  ((X)!=(X)) */
	    /* This weird array thing is to avoid constness violations
	       when assinging, e.g. "NaN" to bufpt.
	    */
	    static char NaN[4] = {'N','a','N','\0'};
	    bufpt = NaN;
          length = 3;
          break;
        }
#endif
        if( realvalue>0.0 ){
          while( realvalue>=1e32 && exp<=350 ){ realvalue *= 1e-32; exp+=32; }
          while( realvalue>=1e8 && exp<=350 ){ realvalue *= 1e-8; exp+=8; }
          while( realvalue>=10.0 && exp<=350 ){ realvalue *= 0.1; exp++; }
          while( realvalue<1e-8 && exp>=-350 ){ realvalue *= 1e8; exp-=8; }
          while( realvalue<1.0 && exp>=-350 ){ realvalue *= 10.0; exp--; }
          if( exp>350 || exp<-350 ){
            if( prefix=='-' ){
		static char Inf[5] = {'-','I','n','f','\0'};
		bufpt = Inf;
            }else if( prefix=='+' ){
		static char Inf[5] = {'+','I','n','f','\0'};
		bufpt = Inf;
            }else{
		static char Inf[4] = {'I','n','f','\0'};
		bufpt = Inf;
            }
            length = strlen(bufpt);
            break;
          }
        }
        bufpt = buf;
        /*
           If the field type is etGENERIC, then convert to either etEXP
           or etFLOAT, as appropriate.
        */
        flag_exp = xtype==etEXP;
        if( xtype!=etFLOAT ){
          realvalue += rounder;
          if( realvalue>=10.0 ){ realvalue *= 0.1; exp++; }
        }
        if( xtype==etGENERIC ){
          flag_rtz = !flag_alternateform;
          if( exp<-4 || exp>precision ){
            xtype = etEXP;
          }else{
            precision = precision - exp;
            xtype = etFLOAT;
          }
        }else{
          flag_rtz = 0;
        }
        if( xtype==etEXP ){
          e2 = 0;
        }else{
          e2 = exp;
        }
        nsd = 0;
        flag_dp = (precision>0) | flag_alternateform | flag_altform2;
        /* The sign in front of the number */
        if( prefix ){
          *(bufpt++) = prefix;
        }
        /* Digits prior to the decimal point */
        if( e2<0 ){
          *(bufpt++) = '0';
        }else{
          for(; e2>=0; e2--){
            *(bufpt++) = et_getdigit(&realvalue,&nsd);
          }
        }
        /* The decimal point */
        if( flag_dp ){
          *(bufpt++) = '.';
        }
        /* "0" digits after the decimal point but before the first
           significant digit of the number */
        for(e2++; e2<0 && precision>0; precision--, e2++){
          *(bufpt++) = '0';
        }
        /* Significant digits after the decimal point */
        while( (precision--)>0 ){
          *(bufpt++) = et_getdigit(&realvalue,&nsd);
        }
        /* Remove trailing zeros and the "." if no digits follow the "." */
        if( flag_rtz && flag_dp ){
          while( bufpt[-1]=='0' ) *(--bufpt) = 0;
          /* assert( bufpt>buf ); */
          if( bufpt[-1]=='.' ){
            if( flag_altform2 ){
              *(bufpt++) = '0';
            }else{
              *(--bufpt) = 0;
            }
          }
        }
        /* Add the "eNNN" suffix */
        if( flag_exp || (xtype==etEXP && exp) ){
          *(bufpt++) = aDigits[infop->charset];
          if( exp<0 ){
            *(bufpt++) = '-'; exp = -exp;
          }else{
            *(bufpt++) = '+';
          }
          if( exp>=100 ){
            *(bufpt++) = (exp/100)+'0';                /* 100's digit */
            exp %= 100;
          }
          *(bufpt++) = exp/10+'0';                     /* 10's digit */
          *(bufpt++) = exp%10+'0';                     /* 1's digit */
        }
        *bufpt = 0;

        /* The converted number is in buf[] and zero terminated. Output it.
           Note that the number is in the usual order, not reversed as with
           integer conversions. */
        length = bufpt-buf;
        bufpt = buf;

        /* Special case:  Add leading zeros if the flag_zeropad flag is
           set and we are not left justified */
        if( flag_zeropad && !flag_leftjustify && length < width){
          int i;
          int nPad = width - length;
          for(i=width; i>=nPad; i--){
            bufpt[i] = bufpt[i-nPad];
          }
          i = prefix!=0;
          while( nPad-- ) bufpt[i++] = '0';
          length = width;
        }
#endif /* !CPDO_PRINTF_OMIT_FLOATING_POINT */
        break;
#if !CPDO_PRINTF_OMIT_SIZE
      case etSIZE:
        *(va_arg(ap,int*)) = outCount;
        length = width = 0;
        break;
#endif
      case etPERCENT:
        buf[0] = '%';
        bufpt = buf;
        length = 1;
        break;
      case etCHARLIT:
      case etCHARX:
        c = buf[0] = (xtype==etCHARX ? va_arg(ap,int) : *++fmt);
        if( precision>=0 ){
          for(idx=1; idx<precision; idx++) buf[idx] = c;
          length = precision;
        }else{
          length =1;
        }
        bufpt = buf;
        break;
      case etSTRING:
      case etDYNSTRING: {
	  bufpt = va_arg(ap,char*);
	  spf = (xtype==etSTRING)
              ? spech_string : spech_dynstring;
	  pfrc = spf( pfAppend, pfAppendArg, bufpt );
	  CPDO_PRINTF_CHECKERR(0);
	  length = 0;
	  if( precision>=0 && precision<length ) length = precision;
	}
        break;
#if ! CPDO_PRINTF_OMIT_HTML
      case etHTML:
	  bufpt = va_arg(ap,char*);
	  pfrc = spech_string_to_html( pfAppend, pfAppendArg, bufpt );
	  CPDO_PRINTF_CHECKERR(0);
	  length = 0;
        break;
      case etURLENCODE:
	  bufpt = va_arg(ap,char*);
	  pfrc = spech_urlencode( pfAppend, pfAppendArg, bufpt );
	  CPDO_PRINTF_CHECKERR(0);
	  length = 0;
        break;
      case etURLDECODE:
          bufpt = va_arg(ap,char *);
	  pfrc = spech_urldecode( pfAppend, pfAppendArg, bufpt );
	  CPDO_PRINTF_CHECKERR(0);
          length = 0;
          break;
#endif /* CPDO_PRINTF_OMIT_HTML */
#if ! CPDO_PRINTF_OMIT_SQL
      case etSQLESCAPE:
      case etSQLESCAPE2:
      case etSQLESCAPE3: {
	      cpdo_printf_spec_handler spf =
		      (xtype==etSQLESCAPE)
		      ? spech_sqlstring1
		      : ((xtype==etSQLESCAPE2)
			 ? spech_sqlstring2
			 : spech_sqlstring3
			 );
	      bufpt = va_arg(ap,char*);
	      pfrc = spf( pfAppend, pfAppendArg, bufpt );
	      CPDO_PRINTF_CHECKERR(0);
	      length = 0;
	      if( precision>=0 && precision<length ) length = precision;
      }
#endif /* !CPDO_PRINTF_OMIT_SQL */
    }/* End switch over the format type */
    /*
       The text of the conversion is pointed to by "bufpt" and is
       "length" characters long.  The field width is "width".  Do
       the output.
    */
    if( !flag_leftjustify ){
      int nspace;
      nspace = width-length;
      if( nspace>0 ){
	      CPDO_PRINTF_SPACES(nspace);
      }
    }
    if( length>0 ){
      pfrc = pfAppend( pfAppendArg, bufpt, length);
      CPDO_PRINTF_CHECKERR(0);
    }
    if( flag_leftjustify ){
      int nspace;
      nspace = width-length;
      if( nspace>0 ){
	      CPDO_PRINTF_SPACES(nspace);
      }
    }
    if( zExtra ){
      free(zExtra);
      zExtra = 0;
    }
  }/* End for loop over the format string */
  CPDO_PRINTF_RETURN;
} /* End of function */


#undef CPDO_PRINTF_SPACES
#undef CPDO_PRINTF_CHECKERR
#undef CPDO_PRINTF_RETURN
#undef CPDO_PRINTF_OMIT_FLOATING_POINT
#undef CPDO_PRINTF_OMIT_SIZE
#undef CPDO_PRINTF_OMIT_SQL
#undef CPDO_PRINTF_BUF_SIZE
#undef CPDO_PRINTF_OMIT_HTML

long cpdo_printf(cpdo_printf_appender pfAppend,          /* Accumulate results here */
	    void * pfAppendArg,                /* Passed as first arg to pfAppend. */
	    const char *fmt,                   /* Format string */
	    ... )
{
	va_list vargs;
    long ret;
	va_start( vargs, fmt );
	ret = cpdo_printfv( pfAppend, pfAppendArg, fmt, vargs );
	va_end(vargs);
	return ret;
}


long cpdo_printf_FILE_appender( void * a, char const * s, long n )
{
	FILE * fp = (FILE *)a;
    long ret;
	if( ! fp ) return -1;
	ret = fwrite( s, sizeof(char), n, fp );
	return (ret >= 0) ? ret : -2;
}


long cpdo_printf_file( FILE * fp, char const * fmt, ... )
{
	va_list vargs;
    int ret;
	va_start( vargs, fmt );
	ret = cpdo_printfv( cpdo_printf_FILE_appender, fp, fmt, vargs );
	va_end(vargs);
	return ret;
}

/**
   Internal implementation details for cpdo_printfv_appender_stringbuf.
*/
typedef struct cpdo_printfv_stringbuf
{
    /** dynamically allocated buffer */
    char * buffer;
    /** bytes allocated to buffer */
    size_t alloced;
    /** Current position within buffer. */
    size_t pos;
} cpdo_printfv_stringbuf;
static const cpdo_printfv_stringbuf cpdo_printfv_stringbuf_init = { 0, 0, 0 };

/**
   A cpdo_printfv_appender implementation which requires arg to be a
   (cpdo_printfv_stringbuf*). It appends n bytes of data to the
   cpdo_printfv_stringbuf object's buffer, reallocating it as
   needed. Returns less than 0 on error, else the number of bytes
   appended to the buffer. The buffer will always be null terminated.
*/
static long cpdo_printfv_appender_stringbuf( void * arg, char const * data, long n )
{
    cpdo_printfv_stringbuf * sb = (cpdo_printfv_stringbuf*)arg;
    size_t npos;
    size_t asz;
    char * buf;
    long rc;
    if( ! sb || (n<0) ) return -1;
    if( ! n ) return 0;
    npos = sb->pos + n;
    if( npos >= sb->alloced )
    {
	asz = (npos * 1.5) + 1;
	if( asz < npos ) return -1; /* overflow */
	buf = (char *)realloc( sb->buffer, asz );
	if( ! buf ) return -1;
	memset( buf + sb->pos, 0, asz - sb->pos );
	sb->buffer = buf;
	sb->alloced = asz;
    }
    rc = 0;
    for( ; rc < n; ++rc, ++sb->pos )
    {
        sb->buffer[sb->pos] = data[rc];
    }
    return rc;
}


char * cpdo_printfv_str( char const * fmt, va_list vargs )
{
    cpdo_printfv_stringbuf sb;
    long rc;
    if( ! fmt ) return 0;
    sb = cpdo_printfv_stringbuf_init;
    rc = cpdo_printfv( cpdo_printfv_appender_stringbuf, &sb, fmt, vargs );
    if( rc <= 0 )
    {
	free( sb.buffer );
	sb.buffer = 0;
    }
    return sb.buffer;
}

char * cpdo_printf_str( char const * fmt, ... )
{
    char * ret;
    va_list vargs;
    va_start( vargs, fmt );
    ret = cpdo_printfv_str( fmt, vargs );
    va_end( vargs );
    return ret;
}
#undef etNINFO
#undef CPDO_PRINTF_FMTINFO_FIXED
#undef CPDO_PRINTF_RETURN
#undef CPDO_PRINTF_CHECKERR
#undef CPDO_PRINTF_SPACES

#if defined(__cplusplus)
} /* extern "C" */
#endif
/* end of file cpdo_printf.c */
#if CPDO_ENABLE_SQLITE3
/** @file cpdo_sqlite3.c
    
   sqlite3 driver implementation for cpdo_driver interface.

   Peculiarities vis-a-vis the interface specification:

   - This is the reference driver implementation, and has no known
   incompatibilities with the interface's required features.


   Using this driver:

   The simplest approach is to link it to you app and do:

   @code
   extern int cpdo_driver_sqlite3_register();
   ...
   cpdo_driver_sqlite3_register();
   @endcode

   If you are using C++, or can use C++ for one file of your
   project, you can have that code automatically run by assigning
   a dummy static variable like this:

   @code
   namespace { static int reg_sqlite3 = cpdo_driver_sqlite3_register(); }
   @endcode

   
*/
#include <assert.h>
#include <sqlite3.h>
#include <stdlib.h> /* malloc()/free() */
#include <string.h> /* strlen() */

#if defined(__cplusplus)
extern "C" {
#endif

/************************************************************************
 cpdo_driver_api members:
************************************************************************/
#define CPDO_DRIVER_NAME "sqlite3"
int cpdo_sq3_connect( cpdo_driver * self, cpdo_connect_opt const * opt );
static int cpdo_sq3_sql_quote( cpdo_driver * self, char const * src, uint32_t * len, char ** dest );
static int cpdo_sq3_sql_qualify( cpdo_driver * self, char const * src, uint32_t * len, char ** dest );
static int cpdo_sq3_free_string( cpdo_driver * self, char * str);
static int cpdo_sq3_prepare( cpdo_driver * self, cpdo_stmt ** tgt, char const * sql, uint32_t len );
static int cpdo_sq3_error_info( cpdo_driver * self, char const ** dest, uint32_t * len, int * errorCode );
static char cpdo_sq3_is_connected( cpdo_driver * self );
static int cpdo_sq3_close( cpdo_driver * self );
static int cpdo_sq3_last_insert_id( cpdo_driver * self, uint64_t * v, char const * hint );
static int cpdo_sq3_capabilities( const cpdo_driver * self );
static cpdo_driver_details const * cpdo_sq3_driver_details();

static int cpdo_sq3_driver_begin_transaction( cpdo_driver * self );
static int cpdo_sq3_driver_commit( cpdo_driver * self );
static int cpdo_sq3_driver_rollback( cpdo_driver * self );
static char cpdo_sq3_driver_in_trans( cpdo_driver * self );

static int cpdo_sq3_driver_opt_set( cpdo_driver * self, char const * key, va_list vargs );
static int cpdo_sq3_driver_opt_get( cpdo_driver * self, char const * key, va_list vargs );

const cpdo_driver_api cpdo_sq3_driver_api =
{
    cpdo_sq3_driver_details,
    cpdo_sq3_connect,
    cpdo_sq3_sql_quote,
    cpdo_sq3_sql_qualify,
    cpdo_sq3_free_string,
    cpdo_sq3_prepare,
    cpdo_sq3_error_info,
    cpdo_sq3_is_connected,
    cpdo_sq3_close,
    cpdo_sq3_last_insert_id,
    cpdo_sq3_capabilities,
    {/*transaction*/
         cpdo_sq3_driver_begin_transaction,
         cpdo_sq3_driver_commit,
         cpdo_sq3_driver_rollback,
         cpdo_sq3_driver_in_trans
    },
    {/*opt*/
        cpdo_sq3_driver_opt_set,
        cpdo_sq3_driver_opt_get
    },
    {/*constants*/
        CPDO_DRIVER_NAME /*driver_name*/
    }
};

/************************************************************************
cpdo_stmt_api members...
************************************************************************/
static cpdo_step_code cpdo_sq3_stmt_step( cpdo_stmt * self );
static int cpdo_sq3_stmt_error_info( cpdo_stmt * self, char const ** dest, uint32_t * len, int * errorCode );
static uint16_t cpdo_sq3_stmt_column_count( cpdo_stmt * self );
static char const * cpdo_sq3_stmt_column_name( cpdo_stmt * self, uint16_t ndx );
static int cpdo_sq3_stmt_reset( cpdo_stmt * self );
static uint16_t cpdo_sq3_stmt_bind_count( cpdo_stmt * self );
static uint16_t cpdo_sq3_stmt_param_index( cpdo_stmt * self, char const * name );
static char const * cpdo_sq3_stmt_param_name( cpdo_stmt * self, uint16_t ndx );
static int cpdo_sq3_stmt_bind_null( cpdo_stmt * self, uint16_t ndx );
static int cpdo_sq3_stmt_bind_int8( cpdo_stmt * self, uint16_t ndx, int8_t v );
static int cpdo_sq3_stmt_bind_int16( cpdo_stmt * self, uint16_t ndx, int16_t v );
static int cpdo_sq3_stmt_bind_int32( cpdo_stmt * self, uint16_t ndx, int32_t v );
static int cpdo_sq3_stmt_bind_int64( cpdo_stmt * self, uint16_t ndx, int64_t v );
static int cpdo_sq3_stmt_bind_float( cpdo_stmt * self, uint16_t ndx, float v );
static int cpdo_sq3_stmt_bind_double( cpdo_stmt * self, uint16_t ndx, double v );
static int cpdo_sq3_stmt_bind_string( cpdo_stmt * self, uint16_t ndx, char const * v, uint32_t len );
static int cpdo_sq3_stmt_bind_blob( cpdo_stmt * self, uint16_t ndx, void const * v, uint32_t len );
static int cpdo_sq3_stmt_get_type_ndx( cpdo_stmt * self, uint16_t ndx, cpdo_data_type * val );
static int cpdo_sq3_stmt_get_int8_ndx( cpdo_stmt * self, uint16_t ndx, int8_t * val );
static int cpdo_sq3_stmt_get_int16_ndx( cpdo_stmt * self, uint16_t ndx, int16_t * val );
static int cpdo_sq3_stmt_get_int32_ndx( cpdo_stmt * self, uint16_t ndx, int32_t * val );
static int cpdo_sq3_stmt_get_int64_ndx( cpdo_stmt * self, uint16_t ndx, int64_t * val );
static int cpdo_sq3_stmt_get_float_ndx( cpdo_stmt * self, uint16_t ndx, float * val );
static int cpdo_sq3_stmt_get_double_ndx( cpdo_stmt * self, uint16_t ndx, double * val );
static int cpdo_sq3_stmt_get_string_ndx( cpdo_stmt * self, uint16_t ndx, char const ** val, uint32_t * len );
static int cpdo_sq3_stmt_get_blob_ndx( cpdo_stmt * self, uint16_t ndx, void const ** v, uint32_t * len );
static int cpdo_sq3_stmt_finalize( cpdo_stmt * self );
const cpdo_stmt_api cpdo_sq3_stmt_api = {
    cpdo_sq3_stmt_step,
    cpdo_sq3_stmt_error_info,
    cpdo_sq3_stmt_finalize,
    {/*bind*/
        cpdo_sq3_stmt_reset,
        cpdo_sq3_stmt_bind_count,
        cpdo_sq3_stmt_param_index,
        cpdo_sq3_stmt_param_name,
        cpdo_sq3_stmt_bind_null,
        cpdo_sq3_stmt_bind_int8,
        cpdo_sq3_stmt_bind_int16,
        cpdo_sq3_stmt_bind_int32,
        cpdo_sq3_stmt_bind_int64,
        cpdo_sq3_stmt_bind_float,
        cpdo_sq3_stmt_bind_double,
        cpdo_sq3_stmt_bind_string,
        cpdo_sq3_stmt_bind_blob
    },
    {/*get*/
        cpdo_sq3_stmt_column_count,
        cpdo_sq3_stmt_column_name,
        cpdo_sq3_stmt_get_type_ndx,
        cpdo_sq3_stmt_get_int8_ndx,
        cpdo_sq3_stmt_get_int16_ndx,
        cpdo_sq3_stmt_get_int32_ndx,
        cpdo_sq3_stmt_get_int64_ndx,
        cpdo_sq3_stmt_get_float_ndx,
        cpdo_sq3_stmt_get_double_ndx,
        cpdo_sq3_stmt_get_string_ndx,
        cpdo_sq3_stmt_get_blob_ndx
    }
};



typedef struct cpdo_sq3_stmt cpdo_sq3_stmt;
static int cpdo_sq3_stmt_free(cpdo_sq3_stmt *s);
static cpdo_sq3_stmt * cpdo_sq3_stmt_alloc();

typedef struct cpdo_sq3_driver cpdo_sq3_driver;
static int cpdo_sq3_driver_free(cpdo_sq3_driver *d);
static cpdo_sq3_driver * cpdo_sq3_driver_alloc();


struct cpdo_sq3_driver
{
    sqlite3 * db;
    char inTransaction;
    cpdo_driver self;
};


struct cpdo_sq3_stmt
{
    sqlite3_stmt * stmt;
    cpdo_sq3_driver * driver;
    cpdo_stmt self;
};

const cpdo_sq3_driver cpdo_sq3_driver_empty = {
    NULL /*db*/,
    0/*inTransaction*/,
    {/*self*/
        &cpdo_sq3_driver_api /*api*/,
        NULL /*impl*/
    }
};

const cpdo_sq3_stmt cpdo_sq3_stmt_empty = {
    NULL /*stmt*/,
    NULL /*driver*/,
    {/*self*/
        &cpdo_sq3_stmt_api /*api*/,
        NULL /*impl*/
    }
};

static cpdo_sq3_driver * cpdo_sq3_driver_alloc()
{
    cpdo_sq3_driver * s = (cpdo_sq3_driver*)malloc(sizeof(cpdo_sq3_driver));
    if( s )
    {
        *s = cpdo_sq3_driver_empty;
        s->self.impl = s;
    }
    return s;
}

static int cpdo_sq3_driver_free(cpdo_sq3_driver *d)
{
    int rc = cpdo_rc.ArgError;
    if( d )
    {
        rc = 0;
        if( d->db )
        {
            rc = sqlite3_close(d->db);
            if(rc) rc = cpdo_rc.UnknownError
                /* we can't use CheckDbError
                   here because we're destroying the
                   db the client would be checking.
                */
                ;
        }
        *d = cpdo_sq3_driver_empty;
        free(d);
    }
    return rc;
}


/**
   Allocates a new cpdo_sq3_stmt and initializes
   its self.impl member to point to the returned
   object.
*/
static cpdo_sq3_stmt * cpdo_sq3_stmt_alloc()
{
    cpdo_sq3_stmt * s = (cpdo_sq3_stmt*)malloc(sizeof(cpdo_sq3_stmt));
    if( s )
    {
        *s = cpdo_sq3_stmt_empty;
        s->self.impl = s;
    }
    return s;
}

/**
   Frees all resources belonging to this statement.  It can return
   non-0, but there is no generic recovery strategy for this, and s is
   freed regardless of whether or not sqlite3_finalize() succeeds.
*/
static int cpdo_sq3_stmt_free(cpdo_sq3_stmt *s)
{
    int rc = cpdo_rc.ArgError;
    if( s )
    {
        rc = 0;
        if( s->stmt )
        {
            rc = sqlite3_finalize(s->stmt);
            if(0 != rc ) rc = cpdo_rc.CheckDbError;
        }
        *s = cpdo_sq3_stmt_empty;
        free(s);
    }
    return rc;
}


int cpdo_sq3_driver_new( cpdo_driver ** tgt )
{
    if( ! tgt ) return cpdo_rc.ArgError;
    else
    {
        cpdo_sq3_driver * d = cpdo_sq3_driver_alloc();
        if( d )
        {
            *tgt = &d->self;
            return 0;
        }
        else return cpdo_rc.AllocError;
    }
}

#define DRV_DECL(RC) cpdo_sq3_driver * drv = (self && self->impl && (self->api==&cpdo_sq3_driver_api)) \
        ? (cpdo_sq3_driver *)self->impl : NULL; \
    if( ! drv ) return RC
#define STMT_DECL(RC) cpdo_sq3_stmt * stmt = (self && self->impl && (self->api==&cpdo_sq3_stmt_api)) \
        ? (cpdo_sq3_stmt *)self->impl : NULL; \
    if( ! stmt ) return RC

static int cpdo_sq3_last_insert_id( cpdo_driver * self, uint64_t * v, char const * hint )
{
    DRV_DECL(cpdo_rc.ArgError);
    if( ! v ) return cpdo_rc.ArgError;
#if 0 /* enabling this adds LOTS of mallocs()! */
    else if(hint && *hint) { /* check sqlite_sequence table... */
        char const * sql = "SELECT seq FROM sqlite_sequence WHERE name=?";
        sqlite3_stmt * st3 = NULL;
        int rc = sqlite3_prepare( drv->db, sql, (int)strlen(sql), &st3, NULL );
        if(rc){
            return cpdo_rc.CheckDbError;
        }
        rc = sqlite3_bind_text( st3, 1, hint, (int)strlen(hint), SQLITE_STATIC );
        if(rc) goto end;
        rc = sqlite3_step( st3 );
        if( SQLITE_ROW != rc ) {
            *v = sqlite3_last_insert_rowid(drv->db);
            rc = 0;
            goto end;
        }
        rc = 0;
        *v = (uint64_t) sqlite3_column_int64( st3, 0 );
        end:
        sqlite3_finalize(st3);
        return rc ? cpdo_rc.CheckDbError : 0;;
    }
#endif
    else
    {
        *v = sqlite3_last_insert_rowid(drv->db);
        return 0;
    }
}

static int cpdo_sq3_capabilities( const cpdo_driver * self )
{
    return
        CPDO_CAPABILITY_AUTOINCREMENT
        | CPDO_CAPABILITY_LAST_INSERT_ID
        | CPDO_CAPABILITY_NUMBER_TO_STRING
        | CPDO_CAPABILITY_STRING_TO_NUMBER
        | CPDO_CAPABILITY_NAMED_PARAMETERS
        | CPDO_CAPABILITY_BLOBS
        | CPDO_CAPABILITY_MULTIPLE_STATEMENTS
        ;
}

    
static int cpdo_sq3_close( cpdo_driver * self )
{
    DRV_DECL(cpdo_rc.ArgError);
    return cpdo_sq3_driver_free(drv);
}

static char cpdo_sq3_is_connected( cpdo_driver * self )
{
    DRV_DECL(0);
    return drv->db ? 1 : 0;
}

static int cpdo_sq3_error_info( cpdo_driver * self, char const ** dest, uint32_t * len, int * errorCode )
{
    DRV_DECL(cpdo_rc.ArgError);
    if( ! drv->db ) return cpdo_rc.ConnectionError;
    else
    {
        if( errorCode ) *errorCode =
#if (SQLITE_VERSION_NUMBER >= 3003009 /* FIXME: which version number is correct???*/)
			    sqlite3_extended_errcode
#else
			    sqlite3_errcode
#endif
			    (drv->db);
        if( dest )
        {
            *dest = sqlite3_errmsg(drv->db);
            if( len )
            {
                *len = *dest ? strlen(*dest) : 0;
            }
        }
        return 0;
    }
}

#define TRY_SHARED_STRINGS 1
#if TRY_SHARED_STRINGS
static struct {
    char sql_null[5];
    char quoted_empty[3];
} sq3_shared_strings = {
{'N','U','L','L',0},
{'\'','\'',0}
};
#endif
static int cpdo_sq3_sql_quote( cpdo_driver * self, char const * str, uint32_t * len, char ** dest )
{
    DRV_DECL(cpdo_rc.ArgError);
    if( ! len || !dest ) return cpdo_rc.ArgError;
    else if( NULL == str )
    {
#if TRY_SHARED_STRINGS
        *dest = sq3_shared_strings.sql_null;
        *len = 4;
        return 0;
#else
        char * tmp = (char *)malloc(5);
        if( ! tmp ) return cpdo_rc.AllocError;
        strcpy( tmp, "NULL" );
        *dest = tmp;
        *len = 4;
        return 0;
#endif
    }
    else if(!*str || !*len){
        *dest = sq3_shared_strings.quoted_empty;
        *len = 2;
        return 0;
    }
    else
    {
        return cpdo_sql_escape( str, len, dest,
                                '\'',
                                '\'',
                                1 );
    }
}

static int cpdo_sq3_sql_qualify( cpdo_driver * self, char const * str, uint32_t * len, char ** dest )
{
    if(!str || !dest || !*str) return cpdo_rc.ArgError;
    else if( (NULL != strstr(str, ";"))
             ||  (NULL != strstr(str, "'"))
             ||  (NULL != strstr(str, "["))
             )
    {
        return cpdo_rc.RangeError;
    }
    else
    {
        int sz = 0;
        char * rc = cpdo_mprintf("[%s]%n", str, &sz);
        if(!rc) return cpdo_rc.AllocError;
        else
        {
            assert( sz > 0 );
            *dest = rc;
            if(len) *len = (uint32_t) sz; /*strlen(rc);*/
            return 0;
        }
    }
}
    
static int cpdo_sq3_free_string( cpdo_driver * self, char * str)
{
    if(!self || !str) return cpdo_rc.ArgError;
#if TRY_SHARED_STRINGS
    else if( ((void const *)str >= (void const *)&sq3_shared_strings)
        && ((void const *)str < (void const *)((unsigned char *)&sq3_shared_strings + sizeof(sq3_shared_strings)))){
        return 0;
    }
    else
#endif
    {
        free(str);
        return 0;
    }
}

static int cpdo_sq3_prepare( cpdo_driver * self, cpdo_stmt ** tgt, char const * sql, uint32_t len )
{
    int rc;
    sqlite3_stmt * st3 = NULL;
    cpdo_sq3_stmt * stmt = NULL;
    DRV_DECL(cpdo_rc.ArgError);
    if(!drv->db) return cpdo_rc.ConnectionError;
    else if( ! tgt ) return cpdo_rc.ArgError;
    rc =
#if (SQLITE_VERSION_NUMBER >= 3003009)
	sqlite3_prepare_v2
#else
	sqlite3_prepare
#endif
	( drv->db, sql, (int)len, &st3, NULL );
    if( 0 != rc ) return cpdo_rc.CheckDbError;
    stmt = cpdo_sq3_stmt_alloc();
    if( ! stmt )
    {
        sqlite3_finalize(st3);
        return cpdo_rc.AllocError;
    }
    stmt->stmt = st3;
    stmt->driver = drv;
    *tgt = &stmt->self;
    return 0;
}

static int cpdo_sq3_mode_to_flags( char const * m ){

    if(!m || !*m) return 0;
    else if(0 == strcmp("rwc",m)) return SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    else if(0 == strcmp("rw",m)) return SQLITE_OPEN_READWRITE;
    else if(0 == strcmp("ro",m)) return SQLITE_OPEN_READONLY;
    else return -1;
}

int cpdo_sq3_connect( cpdo_driver * self, cpdo_connect_opt const * opt )
{
    int rc;
    DRV_DECL(cpdo_rc.ArgError);
    if( ! opt ) return cpdo_rc.ArgError;
    else if( drv->db ) return cpdo_rc.ConnectionError;
    {
        enum { BufSize = 256U };
        char buf[BufSize];
        char const * tokBegin = NULL;
        char const * tokEnd = NULL;
        char kbuf[BufSize] = {0,0};
        char nameBuf[BufSize] = {0,0};
        char * pos;
        char const * key = NULL;
#if (SQLITE_VERSION_NUMBER >= 3005001)
        int flags; flags = 0; /* not yet used */
#endif
        rc = cpdo_split_dsn( opt->dsn, buf, BufSize, &tokBegin );
        if( rc ) return rc;
        assert( NULL != tokBegin );
        pos = nameBuf;
        while( *tokBegin && (';'!=*tokBegin) ){ /* skip filename part */
            if((pos-nameBuf) >= (BufSize-1)){
                return cpdo_rc.RangeError;
            }
            *(pos++) = *(tokBegin++);
        }
        *pos = 0; /* NUL-terminate file name */
        if(';'==*tokBegin) ++tokBegin;
        while( cpdo_next_token( &tokBegin, ';', &tokEnd ) ){
            if(tokBegin==tokEnd) break;
            else {
                char const * value = NULL;
                char * at = kbuf;
                if( (tokEnd - tokBegin) >= (BufSize-1) ) return cpdo_rc.RangeError;
                memset( kbuf, 0, BufSize );
                key = tokBegin;
                /* Write the key part to the buffer... */
                for( ; (key<tokEnd) && *key && ('='!=*key); ++key ) {
                    *(at++) = *key;
                }
                *(at++) = 0;
                value = at;
                if( '=' == *key ) {
                    ++key;
                }
                /* Write the value part to the buffer... */
                for( ; (key<tokEnd) && *key; ++key ) {
                    *(at++) = *key;
                }
                key = kbuf;
                /* Done parsing. Now see if we understand how to use
                   this option... */
                if( 0 == strcmp("openmode",key) )
                {
#if (SQLITE_VERSION_NUMBER >= 3005001)
                    flags = cpdo_sq3_mode_to_flags( value );
                    if(flags<0){
                        /* FIXME: add error string to db class */
                        return cpdo_rc.RangeError;
                    }
#else
                    /* TODO: emit a warning here. */
#endif
                }
                else
                {
                    /* ignore unknown keys: this is optional in the CPDO
                       interface. If we add warning support, i'll add the
                       warning here. Or if i'm feeling pedantic later i'll
                       throw the error here.
                    */
                }
                /* set up for the next token... */
                tokBegin = tokEnd;
                tokEnd = NULL;
            }
        }
        /*
          FIXME: strip any parameters after the first ';' separator
          (just replace the first ';' with a NUL).
         */
#if (SQLITE_VERSION_NUMBER >= 3005001)
        rc = flags
            ? sqlite3_open_v2( nameBuf, &drv->db, flags, NULL )
            : sqlite3_open( nameBuf, &drv->db );
#else
        rc = sqlite3_open( params, &drv->db );
#endif
        /* reminder: don't close so the caller can get error info. */
        return rc ? cpdo_rc.CheckDbError : 0;
    }
}


static int cpdo_sq3_driver_begin_transaction( cpdo_driver * self )
{
    char const * sql = NULL;
    int rc;
    DRV_DECL(cpdo_rc.ArgError);
    if( drv->inTransaction ) return cpdo_rc.UnsupportedError;
    sql = "BEGIN TRANSACTION";
    rc = cpdo_exec( self, sql, strlen(sql) );
    if( 0 == rc ) drv->inTransaction = 1;
    return rc;
}

static int cpdo_sq3_driver_commit( cpdo_driver * self )
{
    char const * sql = NULL;
    int rc;
    DRV_DECL(cpdo_rc.ArgError);
    sql = "COMMIT";
    rc = cpdo_exec( self, sql, strlen(sql) );
    drv->inTransaction = 0;
    return rc;
}

static int cpdo_sq3_driver_rollback( cpdo_driver * self )
{
    char const * sql = NULL;
    int rc;
    DRV_DECL(cpdo_rc.ArgError);
    sql = "ROLLBACK";
    rc = cpdo_exec( self, sql, strlen(sql) );
    drv->inTransaction = 0;
    return rc;
}

static char cpdo_sq3_driver_in_trans( cpdo_driver * self )
{
    DRV_DECL(0);
    return drv->inTransaction;
}

static int cpdo_sq3_driver_opt_set( cpdo_driver * self, char const * key, va_list vargs )
{
    return cpdo_rc.NYIError;
}
static int cpdo_sq3_driver_opt_get( cpdo_driver * self, char const * key, va_list vargs )
{
    return cpdo_rc.NYIError;
}

static cpdo_step_code cpdo_sq3_stmt_step( cpdo_stmt * self )
{
    STMT_DECL(CPDO_STEP_ERROR);
    switch( sqlite3_step( stmt->stmt ) )
    {
      case SQLITE_ROW:
          return CPDO_STEP_OK;
      case SQLITE_DONE:
          return CPDO_STEP_DONE;
      default:
          return CPDO_STEP_ERROR;
    }
}

static int cpdo_sq3_stmt_reset( cpdo_stmt * self )
{
    STMT_DECL(cpdo_rc.ArgError);
    return sqlite3_reset( stmt->stmt ) ? cpdo_rc.CheckDbError : 0;
}

static uint16_t cpdo_sq3_stmt_column_count( cpdo_stmt * self )
{
    int rc;
    STMT_DECL(0);
    rc = sqlite3_column_count( stmt->stmt );
    return (rc>0) ? (uint32_t)rc : 0;
}

static char const * cpdo_sq3_stmt_column_name( cpdo_stmt * self, uint16_t ndx )
{
    STMT_DECL(NULL);
    return sqlite3_column_name( stmt->stmt, (int)ndx );
}

static uint16_t cpdo_sq3_stmt_bind_count( cpdo_stmt * self )
{
    int rc;
    STMT_DECL(0);
    rc = sqlite3_bind_parameter_count( stmt->stmt );
    return (rc<=0) ? 0 : (uint16_t)rc;
}


static uint16_t cpdo_sq3_stmt_param_index( cpdo_stmt * self, char const * name )
{
    int rc;
    STMT_DECL(0);
    if( ! name ) return 0;
    else
    {
        rc = sqlite3_bind_parameter_index(stmt->stmt, name);
        return (rc<=0) ? 0U : (uint16_t)rc;
    }
}

static char const * cpdo_sq3_stmt_param_name( cpdo_stmt * self, uint16_t ndx )
{
    STMT_DECL(NULL);
    return sqlite3_bind_parameter_name( stmt->stmt, (int)ndx );
}

/** Converts sqlite3_bind_xxx() return value to cpdo_rc. */
#define SQ3_TO_CPDO_BIND_RC(RC) \
    switch( RC ){ \
      case 0: return 0; \
      case SQLITE_NOMEM: return cpdo_rc.AllocError; \
      case SQLITE_RANGE: return cpdo_rc.RangeError; \
      default: return cpdo_rc.CheckDbError; \
    }
    
static int cpdo_sq3_stmt_bind_null( cpdo_stmt * self, uint16_t ndx )
{
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    rc = sqlite3_bind_null( stmt->stmt, (int)ndx );
    SQ3_TO_CPDO_BIND_RC(rc);
}

static int cpdo_sq3_stmt_bind_int8( cpdo_stmt * self, uint16_t ndx, int8_t v )
{
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    rc = sqlite3_bind_int( stmt->stmt, (int)ndx, (int)v );
    SQ3_TO_CPDO_BIND_RC(rc);
}

static int cpdo_sq3_stmt_bind_int16( cpdo_stmt * self, uint16_t ndx, int16_t v )
{
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    rc = sqlite3_bind_int( stmt->stmt, (int)ndx, (int)v );
    SQ3_TO_CPDO_BIND_RC(rc);
}
    
static int cpdo_sq3_stmt_bind_int32( cpdo_stmt * self, uint16_t ndx, int32_t v )
{
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    rc = sqlite3_bind_int( stmt->stmt, (int)ndx, (int)v );
    SQ3_TO_CPDO_BIND_RC(rc);
}

static int cpdo_sq3_stmt_bind_int64( cpdo_stmt * self, uint16_t ndx, int64_t v )
{
    int rc;
    typedef
#if SQLITE_VERSION_NUMBER <= 3003006
	/*FIXME: in which version did they rename sqlite_int64 to
	  sqlite3_int64?*/
	sqlite_int64
#else
	sqlite3_int64
#endif
	sq3_int64_t;
    STMT_DECL(cpdo_rc.ArgError);
    rc = sqlite3_bind_int64( stmt->stmt, (int)ndx, (sq3_int64_t)v );
    SQ3_TO_CPDO_BIND_RC(rc);
}

static int cpdo_sq3_stmt_bind_float( cpdo_stmt * self, uint16_t ndx, float v )
{
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    rc = sqlite3_bind_double( stmt->stmt, (int)ndx, v );
    SQ3_TO_CPDO_BIND_RC(rc);
}

static int cpdo_sq3_stmt_bind_double( cpdo_stmt * self, uint16_t ndx, double v )
{
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    rc = sqlite3_bind_double( stmt->stmt, (int)ndx, v );
    SQ3_TO_CPDO_BIND_RC(rc);
}

static int cpdo_sq3_stmt_bind_string( cpdo_stmt * self, uint16_t ndx, char const * v, uint32_t len )
{
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    if(!v){
        rc = sqlite3_bind_null( stmt->stmt, (int)ndx );
    }
    else {
        rc = sqlite3_bind_text( stmt->stmt, (int)ndx, v, len, SQLITE_TRANSIENT );
    }
    SQ3_TO_CPDO_BIND_RC(rc);
}

static int cpdo_sq3_stmt_bind_blob( cpdo_stmt * self, uint16_t ndx, void const * v, uint32_t len )
{
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    if(!v){
        rc = sqlite3_bind_null( stmt->stmt, (int)ndx );
    }
    else {
        if( len != (uint32_t) ((int)len) ) return cpdo_rc.RangeError;
        rc = sqlite3_bind_blob( stmt->stmt, (int)ndx, v, (int)len, SQLITE_TRANSIENT );
    }
    SQ3_TO_CPDO_BIND_RC(rc);
}
#undef SQ3_TO_CPDO_BIND_RC

static int cpdo_sq3_stmt_get_type_ndx( cpdo_stmt * self, uint16_t ndx, cpdo_data_type * val )
{
    int rc = 0;
    STMT_DECL(cpdo_rc.ArgError);
    if( ! val ) return cpdo_rc.ArgError;
    switch( sqlite3_column_type(stmt->stmt, (int)ndx ) )
    {
      case SQLITE_INTEGER:
          *val = CPDO_TYPE_INT64;
          break;
      case SQLITE_FLOAT:
          *val = CPDO_TYPE_DOUBLE;
          break;
      case SQLITE_TEXT: /* my sqlite3.h defines
                           both SQLITE_TEXT and SQLITE3_TEXT
                           to the same value.
                        */
          *val = CPDO_TYPE_STRING;
          break;
      case SQLITE_BLOB:
          *val = CPDO_TYPE_BLOB;
          break;
      case SQLITE_NULL:
          *val = CPDO_TYPE_NULL;
          break;
      default:
          rc = cpdo_rc.TypeError;
          break;
    }
    return rc;
}

/** Returns cpdo_rc.RangeError if 0-based ndx is out of bounds. */
#define STMT_CHECK_GET_NDX if( ndx >= sqlite3_column_count(stmt->stmt) ) return cpdo_rc.RangeError

static int cpdo_sq3_stmt_get_int8_ndx( cpdo_stmt * self, uint16_t ndx, int8_t * val )
{
    STMT_DECL(cpdo_rc.ArgError);
    STMT_CHECK_GET_NDX;
    else if( val ) *val = (int8_t) sqlite3_column_int( stmt->stmt, (int)ndx );
    return 0;
}
static int cpdo_sq3_stmt_get_int16_ndx( cpdo_stmt * self, uint16_t ndx, int16_t * val )
{
    STMT_DECL(cpdo_rc.ArgError);
    STMT_CHECK_GET_NDX;
    else if( val ) *val = (int16_t) sqlite3_column_int( stmt->stmt, (int)ndx );
    return 0;
}

static int cpdo_sq3_stmt_get_int32_ndx( cpdo_stmt * self, uint16_t ndx, int32_t * val )
{
    STMT_DECL(cpdo_rc.ArgError);
    STMT_CHECK_GET_NDX;
    else if( val ) *val = (int32_t) sqlite3_column_int( stmt->stmt, (int)ndx );
    return 0;
}

static int cpdo_sq3_stmt_get_int64_ndx( cpdo_stmt * self, uint16_t ndx, int64_t * val )
{
    STMT_DECL(cpdo_rc.ArgError);
    STMT_CHECK_GET_NDX;
    else if( val ) *val = (int64_t) sqlite3_column_int64( stmt->stmt, (int)ndx );
    return 0;
}

static int cpdo_sq3_stmt_get_float_ndx( cpdo_stmt * self, uint16_t ndx, float * val )
{
    STMT_DECL(cpdo_rc.ArgError);
    STMT_CHECK_GET_NDX;
    else if( val ) *val = (float)sqlite3_column_double( stmt->stmt, (int)ndx );
    return 0;
}
    
static int cpdo_sq3_stmt_get_double_ndx( cpdo_stmt * self, uint16_t ndx, double * val )
{
    STMT_DECL(cpdo_rc.ArgError);
    STMT_CHECK_GET_NDX;
    else if( val ) *val = sqlite3_column_double( stmt->stmt, (int)ndx );
    return 0;
}

static int cpdo_sq3_stmt_get_string_ndx( cpdo_stmt * self, uint16_t ndx, char const ** val, uint32_t * len )
{
    STMT_DECL(cpdo_rc.ArgError);
    if( ! val ) return cpdo_rc.ArgError;
    else STMT_CHECK_GET_NDX;
    else
    {
        sqlite3_value * sv = sqlite3_column_value(stmt->stmt, (int)ndx);
        if( ! sv ) return cpdo_rc.RangeError;
        *val = (SQLITE_NULL == sqlite3_value_type(sv))
            ? NULL
            : (char const *)sqlite3_column_text( stmt->stmt, (int)ndx );
        if( len )
        {
            *len = (val && *val) ? strlen(*val) : 0;
        }
        return 0;
    }
}

static int cpdo_sq3_stmt_get_blob_ndx( cpdo_stmt * self, uint16_t ndx, void const ** val, uint32_t * len )
{
    STMT_DECL(cpdo_rc.ArgError);
    if( ! val ) return cpdo_rc.ArgError;
    else STMT_CHECK_GET_NDX;
    else
    {
        static char zeroLenBlob = {0};
        cpdo_data_type ty = CPDO_TYPE_ERROR;
        int const rc = cpdo_sq3_stmt_get_type_ndx( self, ndx, &ty );
        assert(0 == rc);
        assert( CPDO_TYPE_ERROR != ty );
        switch( ty ){
          case CPDO_TYPE_NULL:
              *val = NULL;
              if(len) *len = 0;
              break;
          default:
              *val = sqlite3_column_blob( stmt->stmt, (int)ndx )
                  /* reminder: sqlite3_column_blob() returns NULL for
                     length-zero blobs. */
                  ;
              if(!*val){
                  if(len) *len = 0;
                  *val = &zeroLenBlob;
              }
              else if( len ){
                  *len = (uint32_t)sqlite3_column_bytes( stmt->stmt, (int)ndx )
                      /*
                        reminder: sqlite3_column_bytes()'s return
                        value can potentially be way off if the value
                        is UTF16:
                        
                        http://www.sqlite.org/c3ref/column_blob.html
                      */
                      ;
              }
        }
        return 0;
    }
}
#undef STMT_CHECK_GET_NDX

int cpdo_sq3_stmt_error_info( cpdo_stmt * self, char const ** dest, uint32_t * len, int * errorCode )
{
    STMT_DECL(cpdo_rc.ArgError);
    return cpdo_sq3_error_info( &stmt->driver->self, dest, len, errorCode );
}

    
static int cpdo_sq3_stmt_finalize( cpdo_stmt * self )
{
    STMT_DECL(cpdo_rc.ArgError);
    return cpdo_sq3_stmt_free(stmt);
}



static cpdo_driver_details const * cpdo_sq3_driver_details()
{
    static const cpdo_driver_details bob = {
    CPDO_DRIVER_NAME/*driver_name*/,
    "20120413"/*driver_version*/,
    "Dual Public Domain/MIT"/*license*/,
    "http://fossil.wanderinghorse.net/repos/cpdo/",
    "Stephan Beal (http://wanderinghorse.net)"
    };
    return &bob;
}

int cpdo_driver_sqlite3_register()
{
    return cpdo_driver_register( CPDO_DRIVER_NAME, cpdo_sq3_driver_new );
}




#if defined(__cplusplus)
} /*extern "C"*/
#endif

#undef DRV_DECL
#undef STMT_DECL
#undef CPDO_DRIVER_NAME
#undef TRY_SHARED_STRINGS
#endif
/*CPDO_ENABLE_SQLITE3*/
#if CPDO_ENABLE_SQLITE4
/** @file cpdo_sqlite4.c
    
   sqlite4 driver implementation for cpdo_driver interface.

   Peculiarities vis-a-vis the interface specification:

   - This is the reference driver implementation, and has no known
   incompatibilities with the interface's required features.


   Using this driver:

   The simplest approach is to link it to you app and do:

   @code
   extern int cpdo_driver_sqlite4_register();
   ...
   cpdo_driver_sqlite4_register();
   @endcode

   If you are using C++, or can use C++ for one file of your
   project, you can have that code automatically run by assigning
   a dummy static variable like this:

   @code
   namespace { static int reg_sqlite4 = cpdo_driver_sqlite4_register(); }
   @endcode

   
*/
#include <assert.h>
#include <sqlite4.h>
#include <stdlib.h> /* malloc()/free() */
#include <string.h> /* strlen() */

#if defined(__cplusplus)
extern "C" {
#endif

/************************************************************************
 cpdo_driver_api members:
************************************************************************/
#define CPDO_DRIVER_NAME "sqlite4"
int cpdo_sq4_connect( cpdo_driver * self, cpdo_connect_opt const * opt );
static int cpdo_sq4_sql_quote( cpdo_driver * self, char const * src, uint32_t * len, char ** dest );
static int cpdo_sq4_sql_qualify( cpdo_driver * self, char const * src, uint32_t * len, char ** dest );
static int cpdo_sq4_free_string( cpdo_driver * self, char * str);
static int cpdo_sq4_prepare( cpdo_driver * self, cpdo_stmt ** tgt, char const * sql, uint32_t len );
static int cpdo_sq4_error_info( cpdo_driver * self, char const ** dest, uint32_t * len, int * errorCode );
static char cpdo_sq4_is_connected( cpdo_driver * self );
static int cpdo_sq4_close( cpdo_driver * self );
static int cpdo_sq4_last_insert_id( cpdo_driver * self, uint64_t * v, char const * hint );
static int cpdo_sq4_capabilities( const cpdo_driver * self );
static cpdo_driver_details const * cpdo_sq4_driver_details();

static int cpdo_sq4_driver_begin_transaction( cpdo_driver * self );
static int cpdo_sq4_driver_commit( cpdo_driver * self );
static int cpdo_sq4_driver_rollback( cpdo_driver * self );
static char cpdo_sq4_driver_in_trans( cpdo_driver * self );

static int cpdo_sq4_driver_opt_set( cpdo_driver * self, char const * key, va_list vargs );
static int cpdo_sq4_driver_opt_get( cpdo_driver * self, char const * key, va_list vargs );

const cpdo_driver_api cpdo_sq4_driver_api =
{
    cpdo_sq4_driver_details,
    cpdo_sq4_connect,
    cpdo_sq4_sql_quote,
    cpdo_sq4_sql_qualify,
    cpdo_sq4_free_string,
    cpdo_sq4_prepare,
    cpdo_sq4_error_info,
    cpdo_sq4_is_connected,
    cpdo_sq4_close,
    cpdo_sq4_last_insert_id,
    cpdo_sq4_capabilities,
    {/*transaction*/
         cpdo_sq4_driver_begin_transaction,
         cpdo_sq4_driver_commit,
         cpdo_sq4_driver_rollback,
         cpdo_sq4_driver_in_trans
    },
    {/*opt*/
        cpdo_sq4_driver_opt_set,
        cpdo_sq4_driver_opt_get
    },
    {/*constants*/
        CPDO_DRIVER_NAME /*driver_name*/
    }
};

/************************************************************************
cpdo_stmt_api members...
************************************************************************/
static cpdo_step_code cpdo_sq4_stmt_step( cpdo_stmt * self );
static int cpdo_sq4_stmt_error_info( cpdo_stmt * self, char const ** dest, uint32_t * len, int * errorCode );
static uint16_t cpdo_sq4_stmt_column_count( cpdo_stmt * self );
static char const * cpdo_sq4_stmt_column_name( cpdo_stmt * self, uint16_t ndx );
static int cpdo_sq4_stmt_reset( cpdo_stmt * self );
static uint16_t cpdo_sq4_stmt_bind_count( cpdo_stmt * self );
static uint16_t cpdo_sq4_stmt_param_index( cpdo_stmt * self, char const * name );
static char const * cpdo_sq4_stmt_param_name( cpdo_stmt * self, uint16_t ndx );
static int cpdo_sq4_stmt_bind_null( cpdo_stmt * self, uint16_t ndx );
static int cpdo_sq4_stmt_bind_int8( cpdo_stmt * self, uint16_t ndx, int8_t v );
static int cpdo_sq4_stmt_bind_int16( cpdo_stmt * self, uint16_t ndx, int16_t v );
static int cpdo_sq4_stmt_bind_int32( cpdo_stmt * self, uint16_t ndx, int32_t v );
static int cpdo_sq4_stmt_bind_int64( cpdo_stmt * self, uint16_t ndx, int64_t v );
static int cpdo_sq4_stmt_bind_float( cpdo_stmt * self, uint16_t ndx, float v );
static int cpdo_sq4_stmt_bind_double( cpdo_stmt * self, uint16_t ndx, double v );
static int cpdo_sq4_stmt_bind_string( cpdo_stmt * self, uint16_t ndx, char const * v, uint32_t len );
static int cpdo_sq4_stmt_bind_blob( cpdo_stmt * self, uint16_t ndx, void const * v, uint32_t len );
static int cpdo_sq4_stmt_get_type_ndx( cpdo_stmt * self, uint16_t ndx, cpdo_data_type * val );
static int cpdo_sq4_stmt_get_int8_ndx( cpdo_stmt * self, uint16_t ndx, int8_t * val );
static int cpdo_sq4_stmt_get_int16_ndx( cpdo_stmt * self, uint16_t ndx, int16_t * val );
static int cpdo_sq4_stmt_get_int32_ndx( cpdo_stmt * self, uint16_t ndx, int32_t * val );
static int cpdo_sq4_stmt_get_int64_ndx( cpdo_stmt * self, uint16_t ndx, int64_t * val );
static int cpdo_sq4_stmt_get_float_ndx( cpdo_stmt * self, uint16_t ndx, float * val );
static int cpdo_sq4_stmt_get_double_ndx( cpdo_stmt * self, uint16_t ndx, double * val );
static int cpdo_sq4_stmt_get_string_ndx( cpdo_stmt * self, uint16_t ndx, char const ** val, uint32_t * len );
static int cpdo_sq4_stmt_get_blob_ndx( cpdo_stmt * self, uint16_t ndx, void const ** v, uint32_t * len );
static int cpdo_sq4_stmt_finalize( cpdo_stmt * self );
const cpdo_stmt_api cpdo_sq4_stmt_api = {
    cpdo_sq4_stmt_step,
    cpdo_sq4_stmt_error_info,
    cpdo_sq4_stmt_finalize,
    {/*bind*/
        cpdo_sq4_stmt_reset,
        cpdo_sq4_stmt_bind_count,
        cpdo_sq4_stmt_param_index,
        cpdo_sq4_stmt_param_name,
        cpdo_sq4_stmt_bind_null,
        cpdo_sq4_stmt_bind_int8,
        cpdo_sq4_stmt_bind_int16,
        cpdo_sq4_stmt_bind_int32,
        cpdo_sq4_stmt_bind_int64,
        cpdo_sq4_stmt_bind_float,
        cpdo_sq4_stmt_bind_double,
        cpdo_sq4_stmt_bind_string,
        cpdo_sq4_stmt_bind_blob
    },
    {/*get*/
        cpdo_sq4_stmt_column_count,
        cpdo_sq4_stmt_column_name,
        cpdo_sq4_stmt_get_type_ndx,
        cpdo_sq4_stmt_get_int8_ndx,
        cpdo_sq4_stmt_get_int16_ndx,
        cpdo_sq4_stmt_get_int32_ndx,
        cpdo_sq4_stmt_get_int64_ndx,
        cpdo_sq4_stmt_get_float_ndx,
        cpdo_sq4_stmt_get_double_ndx,
        cpdo_sq4_stmt_get_string_ndx,
        cpdo_sq4_stmt_get_blob_ndx
    }
};



typedef struct cpdo_sq4_stmt cpdo_sq4_stmt;
static int cpdo_sq4_stmt_free(cpdo_sq4_stmt *s);
static cpdo_sq4_stmt * cpdo_sq4_stmt_alloc();

typedef struct cpdo_sq4_driver cpdo_sq4_driver;
static int cpdo_sq4_driver_free(cpdo_sq4_driver *d);
static cpdo_sq4_driver * cpdo_sq4_driver_alloc();

static struct {
    int envInitCount;
} Sq4State = {
0 /* envInitCount */
};

struct cpdo_sq4_driver
{
    sqlite4 * db;
    char inTransaction;
    cpdo_driver self;
};


struct cpdo_sq4_stmt
{
    sqlite4_stmt * stmt;
    cpdo_sq4_driver * driver;
    cpdo_stmt self;
};

const cpdo_sq4_driver cpdo_sq4_driver_empty = {
    NULL /*db*/,
    0/*inTransaction*/,
    {/*self*/
        &cpdo_sq4_driver_api /*api*/,
        NULL /*impl*/
    }
};

const cpdo_sq4_stmt cpdo_sq4_stmt_empty = {
    NULL /*stmt*/,
    NULL /*driver*/,
    {/*self*/
        &cpdo_sq4_stmt_api /*api*/,
        NULL /*impl*/
    }
};

static cpdo_sq4_driver * cpdo_sq4_driver_alloc()
{
    cpdo_sq4_driver * s = (cpdo_sq4_driver*)malloc(sizeof(cpdo_sq4_driver));
    if( s )
    {
        *s = cpdo_sq4_driver_empty;
        s->self.impl = s;
    }
    return s;
}

static int cpdo_sq4_driver_free(cpdo_sq4_driver *d)
{
    int rc = cpdo_rc.ArgError;
    if( d )
    {
        rc = 0;
        if( d->db )
        {
            rc = sqlite4_close(d->db);
            if(rc) rc = cpdo_rc.UnknownError
                /* we can't use CheckDbError
                   here because we're destroying the
                   db the client would be checking.
                */
                ;
        }
        *d = cpdo_sq4_driver_empty;
        free(d);
    }
    if(0==--Sq4State.envInitCount){
        sqlite4_shutdown(NULL);
    }
    return rc;
}


/**
   Allocates a new cpdo_sq4_stmt and initializes
   its self.impl member to point to the returned
   object.
*/
static cpdo_sq4_stmt * cpdo_sq4_stmt_alloc()
{
    cpdo_sq4_stmt * s = (cpdo_sq4_stmt*)malloc(sizeof(cpdo_sq4_stmt));
    if( s )
    {
        *s = cpdo_sq4_stmt_empty;
        s->self.impl = s;
    }
    return s;
}

/**
   Frees all resources belonging to this statement.  It can return
   non-0, but there is no generic recovery strategy for this, and s is
   freed regardless of whether or not sqlite4_finalize() succeeds.
*/
static int cpdo_sq4_stmt_free(cpdo_sq4_stmt *s)
{
    int rc = cpdo_rc.ArgError;
    if( s )
    {
        rc = 0;
        if( s->stmt )
        {
            rc = sqlite4_finalize(s->stmt);
            if(0 != rc ) rc = cpdo_rc.CheckDbError;
        }
        *s = cpdo_sq4_stmt_empty;
        free(s);
    }
    return rc;
}


int cpdo_sq4_driver_new( cpdo_driver ** tgt )
{
    if( ! tgt ) return cpdo_rc.ArgError;
    else
    {
        cpdo_sq4_driver * d = cpdo_sq4_driver_alloc();
        if( d )
        {
            *tgt = &d->self;
            return 0;
        }
        else return cpdo_rc.AllocError;
    }
}

#define DRV_DECL(RC) cpdo_sq4_driver * drv = (self && self->impl && (self->api==&cpdo_sq4_driver_api)) \
        ? (cpdo_sq4_driver *)self->impl : NULL; \
    if( ! drv ) return RC
#define STMT_DECL(RC) cpdo_sq4_stmt * stmt = (self && self->impl && (self->api==&cpdo_sq4_stmt_api)) \
        ? (cpdo_sq4_stmt *)self->impl : NULL; \
    if( ! stmt ) return RC

static int cpdo_sq4_last_insert_id( cpdo_driver * self, uint64_t * v, char const * hint )
{
#if 1
    return cpdo_rc.UnsupportedError;
#else
    DRV_DECL(cpdo_rc.ArgError);
    if( ! v ) return cpdo_rc.ArgError;
#if 0 /* enabling this adds LOTS of mallocs()! */
    else if(hint && *hint) { /* check sqlite_sequence table... */
        char const * sql = "SELECT seq FROM sqlite_sequence WHERE name=?";
        sqlite4_stmt * st3 = NULL;
        int rc = sqlite4_prepare( drv->db, sql, (int)strlen(sql), &st3, NULL );
        if(rc){
            return cpdo_rc.CheckDbError;
        }
        rc = sqlite4_bind_text( st3, 1, hint, (int)strlen(hint), SQLITE4_STATIC );
        if(rc) goto end;
        rc = sqlite4_step( st3 );
        if( SQLITE4_ROW != rc ) {
            *v = sqlite4_last_insert_rowid(drv->db);
            rc = 0;
            goto end;
        }
        rc = 0;
        *v = (uint64_t) sqlite4_column_int64( st3, 0 );
        end:
        sqlite4_finalize(st3);
        return rc ? cpdo_rc.CheckDbError : 0;;
    }
#endif
    else
    {
        *v = sqlite4_last_insert_rowid(drv->db);
        return 0;
    }
#endif
}

static int cpdo_sq4_capabilities( const cpdo_driver * self )
{
    return CPDO_CAPABILITY_NUMBER_TO_STRING
        | CPDO_CAPABILITY_STRING_TO_NUMBER
        | CPDO_CAPABILITY_NAMED_PARAMETERS
        | CPDO_CAPABILITY_BLOBS
        | CPDO_CAPABILITY_MULTIPLE_STATEMENTS
        ;
}

static int cpdo_sq4_close( cpdo_driver * self )
{
    DRV_DECL(cpdo_rc.ArgError);
    return cpdo_sq4_driver_free(drv);
}

static char cpdo_sq4_is_connected( cpdo_driver * self )
{
    DRV_DECL(0);
    return drv->db ? 1 : 0;
}

static int cpdo_sq4_error_info( cpdo_driver * self, char const ** dest, uint32_t * len, int * errorCode )
{
    DRV_DECL(cpdo_rc.ArgError);
    if( ! drv->db ) return cpdo_rc.ConnectionError;
    else
    {
        if( errorCode ) *errorCode = sqlite4_errcode(drv->db);
        if( dest )
        {
            *dest = sqlite4_errmsg(drv->db);
            if( len )
            {
                *len = *dest ? strlen(*dest) : 0;
            }
        }
        return 0;
    }
}

    
#define TRY_SHARED_STRINGS 1
#if TRY_SHARED_STRINGS
static struct {
    char sql_null[5];
    char quoted_empty[3];
} sq4_shared_strings = {
{'N','U','L','L',0},
{'\'','\'',0}
};
#endif
static int cpdo_sq4_sql_quote( cpdo_driver * self, char const * str, uint32_t * len, char ** dest )
{
    DRV_DECL(cpdo_rc.ArgError);
    if( ! len || !dest ) return cpdo_rc.ArgError;
    else if( NULL == str )
    {
#if TRY_SHARED_STRINGS
        *dest = sq4_shared_strings.sql_null;
        *len = 4;
        return 0;
#else
        char * tmp = (char *)malloc(5);
        if( ! tmp ) return cpdo_rc.AllocError;
        strcpy( tmp, "NULL" );
        *dest = tmp;
        *len = 4;
        return 0;
#endif
    }
    else if(!*str || !*len){
        *dest = sq4_shared_strings.quoted_empty;
        *len = 2;
        return 0;
    }
    else
    {
        return cpdo_sql_escape( str, len, dest,
                                '\'',
                                '\'',
                                1 );
    }
}

static int cpdo_sq4_sql_qualify( cpdo_driver * self, char const * str, uint32_t * len, char ** dest )
{
    if(!str || !dest || !*str) return cpdo_rc.ArgError;
    else if( (NULL != strstr(str, ";"))
             ||  (NULL != strstr(str, "'"))
             ||  (NULL != strstr(str, "["))
             )
    {
        return cpdo_rc.RangeError;
    }
    else
    {
        int sz = 0;
        char * rc = cpdo_mprintf("[%s]%n", str, &sz);
        if(!rc) return cpdo_rc.AllocError;
        else
        {
            assert( sz > 0 );
            *dest = rc;
            if(len) *len = (uint32_t) sz; /*strlen(rc);*/
            return 0;
        }
    }
}
    
static int cpdo_sq4_free_string( cpdo_driver * self, char * str)
{
    if(!self || !str) return cpdo_rc.ArgError;
#if TRY_SHARED_STRINGS
    else if( ((void const *)str >= (void const *)&sq4_shared_strings)
        && ((void const *)str < (void const *)((unsigned char *)&sq4_shared_strings + sizeof(sq4_shared_strings)))){
        return 0;
    }
    else
#endif
    {
        free(str);
        return 0;
    }
}

static int cpdo_sq4_prepare( cpdo_driver * self, cpdo_stmt ** tgt, char const * sql, uint32_t len )
{
    int rc;
    sqlite4_stmt * st3 = NULL;
    cpdo_sq4_stmt * stmt = NULL;
    DRV_DECL(cpdo_rc.ArgError);
    if(!drv->db) return cpdo_rc.ConnectionError;
    else if( ! tgt ) return cpdo_rc.ArgError;
    rc =
	sqlite4_prepare( drv->db, sql, (int)len, &st3, NULL );
    if( 0 != rc ) return cpdo_rc.CheckDbError;
    stmt = cpdo_sq4_stmt_alloc();
    if( ! stmt )
    {
        sqlite4_finalize(st3);
        return cpdo_rc.AllocError;
    }
    stmt->stmt = st3;
    stmt->driver = drv;
    *tgt = &stmt->self;
    return 0;
}

static int cpdo_sq4_mode_to_flags( char const * m ){

    if(!m || !*m) return 0;
    else if(0 == strcmp("rwc",m)) return SQLITE4_OPEN_READWRITE | SQLITE4_OPEN_CREATE;
    else if(0 == strcmp("rw",m)) return SQLITE4_OPEN_READWRITE;
    else if(0 == strcmp("ro",m)) return SQLITE4_OPEN_READONLY;
    else return -1;
}

int cpdo_sq4_connect( cpdo_driver * self, cpdo_connect_opt const * opt )
{
    int rc;
    DRV_DECL(cpdo_rc.ArgError);
    if( ! opt ) return cpdo_rc.ArgError;
    else if( drv->db ) return cpdo_rc.ConnectionError;
    {
        enum { BufSize = 256U };
        char buf[BufSize];
        char const * tokBegin = NULL;
        char const * tokEnd = NULL;
        char kbuf[BufSize] = {0,0};
        char nameBuf[BufSize] = {0,0};
        char * pos;
        char const * key = NULL;
#if (SQLITE4_VERSION_NUMBER >= 3005001)
        int flags; flags = 0;
#endif
        rc = cpdo_split_dsn( opt->dsn, buf, BufSize, &tokBegin );
        if( rc ) return rc;
        assert( NULL != tokBegin );
        pos = nameBuf;
        while( *tokBegin && (';'!=*tokBegin) ){ /* skip filename part */
            if((pos-nameBuf) >= (BufSize-1)){
                return cpdo_rc.RangeError;
            }
            *(pos++) = *(tokBegin++);
        }
        *pos = 0; /* NUL-terminate file name */
        if(';'==*tokBegin) ++tokBegin;
        while( cpdo_next_token( &tokBegin, ';', &tokEnd ) ){
            if(tokBegin==tokEnd) break;
            else {
                char const * value = NULL;
                char * at = kbuf;
                if( (tokEnd - tokBegin) >= (BufSize-1) ) return cpdo_rc.RangeError;
                memset( kbuf, 0, BufSize );
                key = tokBegin;
                /* Write the key part to the buffer... */
                for( ; (key<tokEnd) && *key && ('='!=*key); ++key ) {
                    *(at++) = *key;
                }
                *(at++) = 0;
                value = at;
                if( '=' == *key ) {
                    ++key;
                }
                /* Write the value part to the buffer... */
                for( ; (key<tokEnd) && *key; ++key ) {
                    *(at++) = *key;
                }
                key = kbuf;
                /* Done parsing. Now see if we understand how to use
                   this option... */
                if( 0 == strcmp("openmode",key) )
                {
#if (SQLITE4_VERSION_NUMBER >= 3005001)
                    flags = cpdo_sq4_mode_to_flags( value );
                    if(flags<0){
                        /* FIXME: add error string to db class */
                        return cpdo_rc.RangeError;
                    }
#else
                    /* TODO: emit a warning here. */
#endif
                }
                else
                {
                    /* ignore unknown keys: this is optional in the CPDO
                       interface. If we add warning support, i'll add the
                       warning here. Or if i'm feeling pedantic later i'll
                       throw the error here.
                    */
                }
                /* set up for the next token... */
                tokBegin = tokEnd;
                tokEnd = NULL;
            }
        }
        if(1==++Sq4State.envInitCount){
            sqlite4_initialize(NULL);
        }

        /*
          FIXME: strip any parameters after the first ';' separator
          (just replace the first ';' with a NUL).
         */
        rc = /*flags
            ? sqlite4_open_v2( NULL, nameBuf, &drv->db, flags, NULL )
            : */sqlite4_open( NULL, nameBuf, &drv->db, flags, 0 )
            /* reminder: don't close so the caller can get error info. */;
        return rc ? cpdo_rc.CheckDbError : 0;
    }
}


static int cpdo_sq4_driver_begin_transaction( cpdo_driver * self )
{
    char const * sql = NULL;
    int rc;
    DRV_DECL(cpdo_rc.ArgError);
    if( drv->inTransaction ) return cpdo_rc.UnsupportedError;
    sql = "BEGIN TRANSACTION";
    rc = cpdo_exec( self, sql, strlen(sql) );
    if( 0 == rc ) drv->inTransaction = 1;
    return rc;
}

static int cpdo_sq4_driver_commit( cpdo_driver * self )
{
    char const * sql = NULL;
    int rc;
    DRV_DECL(cpdo_rc.ArgError);
    sql = "COMMIT";
    rc = cpdo_exec( self, sql, strlen(sql) );
    drv->inTransaction = 0;
    return rc;
}

static int cpdo_sq4_driver_rollback( cpdo_driver * self )
{
    char const * sql = NULL;
    int rc;
    DRV_DECL(cpdo_rc.ArgError);
    sql = "ROLLBACK";
    rc = cpdo_exec( self, sql, strlen(sql) );
    drv->inTransaction = 0;
    return rc;
}

static char cpdo_sq4_driver_in_trans( cpdo_driver * self )
{
    DRV_DECL(0);
    return drv->inTransaction;
}

static int cpdo_sq4_driver_opt_set( cpdo_driver * self, char const * key, va_list vargs )
{
    return cpdo_rc.NYIError;
}
static int cpdo_sq4_driver_opt_get( cpdo_driver * self, char const * key, va_list vargs )
{
    return cpdo_rc.NYIError;
}

static cpdo_step_code cpdo_sq4_stmt_step( cpdo_stmt * self )
{
    STMT_DECL(CPDO_STEP_ERROR);
    switch( sqlite4_step( stmt->stmt ) )
    {
      case SQLITE4_ROW:
          return CPDO_STEP_OK;
      case SQLITE4_DONE:
          return CPDO_STEP_DONE;
      default:
          return CPDO_STEP_ERROR;
    }
}

static int cpdo_sq4_stmt_reset( cpdo_stmt * self )
{
    STMT_DECL(cpdo_rc.ArgError);
    return sqlite4_reset( stmt->stmt ) ? cpdo_rc.CheckDbError : 0;
}

static uint16_t cpdo_sq4_stmt_column_count( cpdo_stmt * self )
{
    int rc;
    STMT_DECL(0);
    rc = sqlite4_column_count( stmt->stmt );
    return (rc>0) ? (uint32_t)rc : 0;
}

static char const * cpdo_sq4_stmt_column_name( cpdo_stmt * self, uint16_t ndx )
{
    STMT_DECL(NULL);
    return sqlite4_column_name( stmt->stmt, (int)ndx );
}

static uint16_t cpdo_sq4_stmt_bind_count( cpdo_stmt * self )
{
    int rc;
    STMT_DECL(0);
    rc = sqlite4_bind_parameter_count( stmt->stmt );
    return (rc<=0) ? 0 : (uint16_t)rc;
}


static uint16_t cpdo_sq4_stmt_param_index( cpdo_stmt * self, char const * name )
{
    int rc;
    STMT_DECL(0);
    if( ! name ) return 0;
    else
    {
        rc = sqlite4_bind_parameter_index(stmt->stmt, name);
        return (rc<=0) ? 0U : (uint16_t)rc;
    }
}

static char const * cpdo_sq4_stmt_param_name( cpdo_stmt * self, uint16_t ndx )
{
    STMT_DECL(NULL);
    return sqlite4_bind_parameter_name( stmt->stmt, (int)ndx );
}

/** Converts sqlite4_bind_xxx() return value to cpdo_rc. */
#define SQ4_TO_CPDO_BIND_RC(RC) \
    if( 0 == (RC) ) return (RC); \
    if( SQLITE4_NOMEM == (RC) ) return cpdo_rc.AllocError; \
    else if( SQLITE4_RANGE == (RC) ) return cpdo_rc.RangeError; \
    else return cpdo_rc.CheckDbError
    
static int cpdo_sq4_stmt_bind_null( cpdo_stmt * self, uint16_t ndx )
{
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    rc = sqlite4_bind_null( stmt->stmt, (int)ndx );
    SQ4_TO_CPDO_BIND_RC(rc);
}

static int cpdo_sq4_stmt_bind_int8( cpdo_stmt * self, uint16_t ndx, int8_t v )
{
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    rc = sqlite4_bind_int( stmt->stmt, (int)ndx, (int)v );
    SQ4_TO_CPDO_BIND_RC(rc);
}

static int cpdo_sq4_stmt_bind_int16( cpdo_stmt * self, uint16_t ndx, int16_t v )
{
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    rc = sqlite4_bind_int( stmt->stmt, (int)ndx, (int)v );
    SQ4_TO_CPDO_BIND_RC(rc);
}
    
static int cpdo_sq4_stmt_bind_int32( cpdo_stmt * self, uint16_t ndx, int32_t v )
{
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    rc = sqlite4_bind_int( stmt->stmt, (int)ndx, (int)v );
    SQ4_TO_CPDO_BIND_RC(rc);
}

static int cpdo_sq4_stmt_bind_int64( cpdo_stmt * self, uint16_t ndx, int64_t v )
{
    int rc;
    typedef
#if SQLITE4_VERSION_NUMBER <= 3003006
	/*FIXME: in which version did they rename sqlite_int64 to
	  sqlite4_int64?*/
	sqlite_int64
#else
	sqlite4_int64
#endif
	sq4_int64_t;
    STMT_DECL(cpdo_rc.ArgError);
    rc = sqlite4_bind_int64( stmt->stmt, (int)ndx, (sq4_int64_t)v );
    SQ4_TO_CPDO_BIND_RC(rc);
}

static int cpdo_sq4_stmt_bind_float( cpdo_stmt * self, uint16_t ndx, float v )
{
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    rc = sqlite4_bind_double( stmt->stmt, (int)ndx, v );
    SQ4_TO_CPDO_BIND_RC(rc);
}

static int cpdo_sq4_stmt_bind_double( cpdo_stmt * self, uint16_t ndx, double v )
{
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    rc = sqlite4_bind_double( stmt->stmt, (int)ndx, v );
    SQ4_TO_CPDO_BIND_RC(rc);
}

static int cpdo_sq4_stmt_bind_string( cpdo_stmt * self, uint16_t ndx, char const * v, uint32_t len )
{
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    if( v )
    {
        rc = sqlite4_bind_text( stmt->stmt, (int)ndx, (char const *)v, (int)len, SQLITE4_TRANSIENT );
    }
    else
    {
        rc = sqlite4_bind_null( stmt->stmt, (int)ndx );
    }
    SQ4_TO_CPDO_BIND_RC(rc);
}

static int cpdo_sq4_stmt_bind_blob( cpdo_stmt * self, uint16_t ndx, void const * v, uint32_t len )
{
    
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    if(!v){
        rc = sqlite4_bind_null( stmt->stmt, (int)ndx );
    }
    else {
        if( len != (uint32_t) ((int)len) ) return cpdo_rc.RangeError;
        rc = sqlite4_bind_blob( stmt->stmt, (int)ndx, v, (int)len, SQLITE4_TRANSIENT );
    }
    SQ4_TO_CPDO_BIND_RC(rc);
}
#undef SQ4_TO_CPDO_BIND_RC

static int cpdo_sq4_stmt_get_type_ndx( cpdo_stmt * self, uint16_t ndx, cpdo_data_type * val )
{
    int rc = 0;
    STMT_DECL(cpdo_rc.ArgError);
    if( ! val ) return cpdo_rc.ArgError;
    switch( sqlite4_column_type(stmt->stmt, (int)ndx ) )
    {
      case SQLITE4_INTEGER:
          *val = CPDO_TYPE_INT64;
          break;
      case SQLITE4_FLOAT:
          *val = CPDO_TYPE_DOUBLE;
          break;
      case SQLITE4_TEXT: /* my sqlite4.h defines
                           both SQLITE4_TEXT and SQLITE4_TEXT
                           to the same value.
                        */
          *val = CPDO_TYPE_STRING;
          break;
      case SQLITE4_BLOB:
          *val = CPDO_TYPE_BLOB;
          break;
      case SQLITE4_NULL:
          *val = CPDO_TYPE_NULL;
          break;
      default:
          rc = cpdo_rc.TypeError;
          break;
    }
    return rc;
}

/** Returns cpdo_rc.RangeError if 0-based ndx is out of bounds. */
#define STMT_CHECK_GET_NDX if( ndx >= sqlite4_column_count(stmt->stmt) ) return cpdo_rc.RangeError

static int cpdo_sq4_stmt_get_int8_ndx( cpdo_stmt * self, uint16_t ndx, int8_t * val )
{
    STMT_DECL(cpdo_rc.ArgError);
    STMT_CHECK_GET_NDX;
    else if( val ) *val = (int8_t) sqlite4_column_int( stmt->stmt, (int)ndx );
    return 0;
}
static int cpdo_sq4_stmt_get_int16_ndx( cpdo_stmt * self, uint16_t ndx, int16_t * val )
{
    STMT_DECL(cpdo_rc.ArgError);
    STMT_CHECK_GET_NDX;
    else if( val ) *val = (int16_t) sqlite4_column_int( stmt->stmt, (int)ndx );
    return 0;
}

static int cpdo_sq4_stmt_get_int32_ndx( cpdo_stmt * self, uint16_t ndx, int32_t * val )
{
    STMT_DECL(cpdo_rc.ArgError);
    STMT_CHECK_GET_NDX;
    else if( val ) *val = (int32_t) sqlite4_column_int( stmt->stmt, (int)ndx );
    return 0;
}

static int cpdo_sq4_stmt_get_int64_ndx( cpdo_stmt * self, uint16_t ndx, int64_t * val )
{
    STMT_DECL(cpdo_rc.ArgError);
    STMT_CHECK_GET_NDX;
    else if( val ) *val = (int64_t) sqlite4_column_int64( stmt->stmt, (int)ndx );
    return 0;
}

static int cpdo_sq4_stmt_get_float_ndx( cpdo_stmt * self, uint16_t ndx, float * val )
{
    STMT_DECL(cpdo_rc.ArgError);
    STMT_CHECK_GET_NDX;
    else if( val ) *val = (float)sqlite4_column_double( stmt->stmt, (int)ndx );
    return 0;
}
    
static int cpdo_sq4_stmt_get_double_ndx( cpdo_stmt * self, uint16_t ndx, double * val )
{
    STMT_DECL(cpdo_rc.ArgError);
    STMT_CHECK_GET_NDX;
    else if( val ) *val = sqlite4_column_double( stmt->stmt, (int)ndx );
    return 0;
}

static int cpdo_sq4_stmt_get_string_ndx( cpdo_stmt * self, uint16_t ndx, char const ** val, uint32_t * len )
{
    STMT_DECL(cpdo_rc.ArgError);
    if( ! val ) return cpdo_rc.ArgError;
    else STMT_CHECK_GET_NDX;
    else
    {
        sqlite4_value * sv = sqlite4_column_value(stmt->stmt, (int)ndx);
        if( ! sv ) return cpdo_rc.RangeError;
        *val = (SQLITE4_NULL == sqlite4_value_type(sv))
            ? NULL
            : (char const *)sqlite4_column_text( stmt->stmt, (int)ndx );
        if( len )
        {
            *len = (val && *val) ? strlen(*val) : 0;
        }
        return 0;
    }
}

static int cpdo_sq4_stmt_get_blob_ndx( cpdo_stmt * self, uint16_t ndx, void const ** val, uint32_t * len )
{
    STMT_DECL(cpdo_rc.ArgError);
    if( ! val ) return cpdo_rc.ArgError;
    else STMT_CHECK_GET_NDX;
    else
    {
        static char zeroLenBlob = {0};
        cpdo_data_type ty = CPDO_TYPE_ERROR;
        int const rc = cpdo_sq4_stmt_get_type_ndx( self, ndx, &ty );
        assert(0 == rc);
        assert( CPDO_TYPE_ERROR != ty );
        switch( ty ){
          case CPDO_TYPE_NULL:
              *val = NULL;
              if(len) *len = 0;
              break;
          default:
              *val = sqlite4_column_blob( stmt->stmt, (int)ndx )
                  /* reminder: sqlite4_column_blob() returns NULL for
                     length-zero blobs. */
                  ;
              if(!*val){
                  if(len) *len = 0;
                  *val = &zeroLenBlob;
              }
              else if( len ){
                  *len = (uint32_t)sqlite4_column_bytes( stmt->stmt, (int)ndx )
                      /*
                        reminder: sqlite4_column_bytes()'s return
                        value can potentially be way off if the value
                        is UTF16:
                        
                        http://www.sqlite.org/c3ref/column_blob.html
                      */
                      ;
              }
        }
        return 0;
    }
}
#undef STMT_CHECK_GET_NDX

int cpdo_sq4_stmt_error_info( cpdo_stmt * self, char const ** dest, uint32_t * len, int * errorCode )
{
    STMT_DECL(cpdo_rc.ArgError);
    return cpdo_sq4_error_info( &stmt->driver->self, dest, len, errorCode );
}

    
static int cpdo_sq4_stmt_finalize( cpdo_stmt * self )
{
    STMT_DECL(cpdo_rc.ArgError);
    return cpdo_sq4_stmt_free(stmt);
}



static cpdo_driver_details const * cpdo_sq4_driver_details()
{
    static const cpdo_driver_details bob = {
    CPDO_DRIVER_NAME/*driver_name*/,
    "20120628"/*driver_version*/,
    "Dual Public Domain/MIT"/*license*/,
    "http://fossil.wanderinghorse.net/repos/cpdo/",
    "Stephan Beal (http://wanderinghorse.net)"
    };
    return &bob;
}

int cpdo_driver_sqlite4_register()
{
    return cpdo_driver_register( CPDO_DRIVER_NAME, cpdo_sq4_driver_new );
}




#if defined(__cplusplus)
} /*extern "C"*/
#endif

#undef DRV_DECL
#undef STMT_DECL
#undef CPDO_DRIVER_NAME
#undef TRY_SHARED_STRINGS
#endif
/*CPDO_ENABLE_SQLITE4*/
#if CPDO_ENABLE_MYSQL5
/** @file cpdo_mysql5.c

  MySQL v5 driver implementation for cpdo_driver interface.

   DSN driver parameters:

   - port=NUM

   - autocommit=BOOL

   - host=STRING

   - dbname=STRING

   - enableblobtostring=BOOL (default=false). If true, the
   get_string() operation is allowed to return BLOB-declared values
   (as-is), as opposed signaling an error. Added 20120413.
   
   - DEPRECATED (not needed): fieldbuffersize=INTEGER. When fetching
   string/blob data, if the db cannot tell us the maximum length for
   the field then this value is used as a fallback. The driver ignores
   "very small" values, but how small "very small" is is not
   specified.


   TODO:

   - DSN option: debug=BOOL [default=false]
   
   - DSN option: logfile=STRING [default=NULL]

   Peculiarities vis-a-vis, and optional features supported from, the
   cpdo interface specification:
   
   - There are no known incompatibilities with the CPDO interface's
   required features.
   
   - When the autocommit DSN parameter is used and
   cpdo_stmt_api::transaction::begin() is called, auto-commit mode is
   disabled until commit() or rollback() are called. It is then
   re-enabled. If the autocommit parameter is not explicitly provided
   then no finagling of the autocommit mode is done.

   - When fetching string/blob data, if the driver cannot figure out
   the declared maximum size then it uses a fixed-size buffer, which
   won't work for large data. Use the fieldbuffersize=N DSN parameter
   to specify your own size, but beware that this will be used for all
   string fields when we cannot determine the size automatically.

   - The cpdo_stmt_api::get::string() impl can convert numeric and
   TIME/DATE/DATETIME/TIMESTAMP data to a string. It will treat BLOB
   fields as raw string data only if enableblobtostring=true, otherwise
   it will signal an error for BLOB fields.

   - MySQL leaks some memory when connecting and calling
   mysql_library_end() at app shutdown does not always free it.
   i can do nothing about this.

   - MySQL does not natively support binding parameters by name, that
   feature is implemented using a custom parser. It is possible that
   that parser might misbehave. This feature defaults to enabled but
   can be disabled with the DSN option "enablenamedparams=false".

   - The statement's param_count() and bind_xxx() operations operates
   "like sqlite3" when faced with named parameters: each unique name
   is counted only once, regardless of how many times it is used in
   the SQL. e.g. the list (:p1, :p2, :p1, :p2) has a param_count() of
   2, not 4. Driver versions prior to 20130507 reported 4 for that
   case and did not support binding multiple uses of the same named
   parameter. As of 20120507, when a _named_ parameter is bound (by
   its index as returned by param_index()) then any further uses of
   that same parameter name in the parameters list to the right of
   that one are also bound to that value. Thus it is possible to
   re-use a named parameter in a given SQL string as of that version.

   Using this driver:

   The simplest approach is to link it to you app and do:

   @code
   extern int cpdo_driver_mysql5_register();
   ...
   cpdo_driver_mysql5_register();
   @endcode

   If you are using C++, or can use C++ for one file of your
   project, you can have that code automatically run by assigning
   a dummy static variable like this:

   @code
   extern int cpdo_driver_mysql5_register();
   namespace {
       static int reg_mysql5 = cpdo_driver_mysql5_register();
   }
   @endcode

   You also must link to mysql if the app/DLL containing _this_ code
   is not linked against it. The 'mysql_config' script (installed as
   part of the mysql packages) can give you the proper values for
   compiling/linking.
   
   LICENSE:

   Copyright 2011 Stephan Beal (http://wanderinghorse.net/home/stephan)
   
   This code may be used under the terms of the license(s) the client
   has accepted for the MySQL library which is used when building this
   software. Thus if the client has accepted the GNU GPL for his MySQL
   installation, then this software may be used under the terms of the
   GPL. If he has accepted a commercial license (e.g. from Oracle)
   then he may use those licensing terms.

   In other words, if you have accepted a MySQL license (which you
   presumably have, or you would not be using this driver) then you
   have already accepted the license for this code.
*/
/* From my inttypes.h, in regards to PRIi32 and friends:

   "The ISO C99 standard specifies that these macros must only be
   defined if explicitly requested."
*/
#if defined(__cplusplus) && !defined(__STDC_FORMAT_MACROS)
#  define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h> /* PRIi32 and friends */

#include <assert.h>
#include <mysql/mysql.h>
#include <stdlib.h> /* malloc()/free() */
#include <string.h> /* strlen() */

#include <stdio.h> /* only for debuggering */

#define MEGADEBUG 0

#define MARKER if(1) printf("MARKER: %s:%d:%s():\t",__FILE__,__LINE__,__func__); if(1) printf

#if !defined(CPDO_MY5_HAS_PRINT64)
/** CPDO_MY5_HAS_PRINT64 is used to determine whether or not to install
    our own int64-to-string logic. C89 does not specify the equivalent
    of C99's PRIi64 format specifier.
    
    If CPDO_MY5_HAS_PRINT64 is true then we use PRIi64 and sprintf(),
    otherwise we use cpdo_printf() to do the conversion.
*/
#define CPDO_MY5_HAS_PRINT64 (CPDO_ENABLE_64_BIT || (__STDC_VERSION__ >= 199901L) || (HAVE_LONG_LONG == 1))
#endif

#if !CPDO_MY5_HAS_PRINT64
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/************************************************************************
 cpdo_driver_api members:
************************************************************************/
#define CPDO_DRIVER_NAME "mysql5"
int cpdo_my5_connect( cpdo_driver * self, cpdo_connect_opt const * opt );
static int cpdo_my5_sql_quote( cpdo_driver * self, char const * src, uint32_t * len, char ** dest );
static int cpdo_my5_sql_qualify( cpdo_driver * self, char const * src, uint32_t * len, char ** dest );
static int cpdo_my5_free_string( cpdo_driver * self, char * str);
static int cpdo_my5_prepare( cpdo_driver * self, cpdo_stmt ** tgt, const char * sql, uint32_t len );
static int cpdo_my5_error_info( cpdo_driver * self, char const ** dest, uint32_t * len, int * errorCode );
static char cpdo_my5_is_connected( cpdo_driver * self );
static int cpdo_my5_close( cpdo_driver * self );
static int cpdo_my5_last_insert_id( cpdo_driver * self, uint64_t * v, char const * hint );
static int cpdo_my5_capabilities( const cpdo_driver * self );
static cpdo_driver_details const * cpdo_my5_driver_details();

static int cpdo_my5_driver_begin_transaction( cpdo_driver * self );
static int cpdo_my5_driver_commit( cpdo_driver * self );
static int cpdo_my5_driver_rollback( cpdo_driver * self );
static char cpdo_my5_driver_in_trans( cpdo_driver * self );

static int cpdo_my5_driver_opt_set( cpdo_driver * self, char const * key, va_list vargs );
static int cpdo_my5_driver_opt_get( cpdo_driver * self, char const * key, va_list vargs );

const cpdo_driver_api cpdo_my5_driver_api =
{
    cpdo_my5_driver_details,
    cpdo_my5_connect,
    cpdo_my5_sql_quote,
    cpdo_my5_sql_qualify,
    cpdo_my5_free_string,
    cpdo_my5_prepare,
    cpdo_my5_error_info,
    cpdo_my5_is_connected,
    cpdo_my5_close,
    cpdo_my5_last_insert_id,
    cpdo_my5_capabilities,
    {/*transaction*/
         cpdo_my5_driver_begin_transaction,
         cpdo_my5_driver_commit,
         cpdo_my5_driver_rollback,
         cpdo_my5_driver_in_trans
    },
    {/*opt*/
        cpdo_my5_driver_opt_set,
        cpdo_my5_driver_opt_get
    },
    {/*constants*/
        CPDO_DRIVER_NAME /*driver_name*/
    }
};

/************************************************************************
 cpdo_stmt_api members...
************************************************************************/
static cpdo_step_code cpdo_my5_stmt_step( cpdo_stmt * self );
static int cpdo_my5_stmt_error_info( cpdo_stmt * self, char const ** dest, uint32_t * len, int * errorCode );
static uint16_t cpdo_my5_stmt_column_count( cpdo_stmt * self );
static char const * cpdo_my5_stmt_column_name( cpdo_stmt * self, uint16_t ndx );
static int cpdo_my5_stmt_reset( cpdo_stmt * self );
static uint16_t cpdo_my5_stmt_bind_count( cpdo_stmt * self );
static uint16_t cpdo_my5_stmt_param_index( cpdo_stmt * self, char const * name );
static char const * cpdo_my5_stmt_param_name( cpdo_stmt * self, uint16_t ndx );
static int cpdo_my5_stmt_bind_null( cpdo_stmt * self, uint16_t ndx );
static int cpdo_my5_stmt_bind_int8( cpdo_stmt * self, uint16_t ndx, int8_t v );
static int cpdo_my5_stmt_bind_int16( cpdo_stmt * self, uint16_t ndx, int16_t v );
static int cpdo_my5_stmt_bind_int32( cpdo_stmt * self, uint16_t ndx, int32_t v );
static int cpdo_my5_stmt_bind_int64( cpdo_stmt * self, uint16_t ndx, int64_t v );
static int cpdo_my5_stmt_bind_float( cpdo_stmt * self, uint16_t ndx, float v );
static int cpdo_my5_stmt_bind_double( cpdo_stmt * self, uint16_t ndx, double v );
static int cpdo_my5_stmt_bind_string( cpdo_stmt * self, uint16_t ndx, char const * v, uint32_t len );
static int cpdo_my5_stmt_bind_blob( cpdo_stmt * self, uint16_t ndx, void const * v, uint32_t len );
static int cpdo_my5_stmt_get_type_ndx( cpdo_stmt * self, uint16_t ndx, cpdo_data_type * val );
static int cpdo_my5_stmt_get_int8_ndx( cpdo_stmt * self, uint16_t ndx, int8_t * val );
static int cpdo_my5_stmt_get_int16_ndx( cpdo_stmt * self, uint16_t ndx, int16_t * val );
static int cpdo_my5_stmt_get_int32_ndx( cpdo_stmt * self, uint16_t ndx, int32_t * val );
static int cpdo_my5_stmt_get_int64_ndx( cpdo_stmt * self, uint16_t ndx, int64_t * val );
static int cpdo_my5_stmt_get_float_ndx( cpdo_stmt * self, uint16_t ndx, float * val );
static int cpdo_my5_stmt_get_double_ndx( cpdo_stmt * self, uint16_t ndx, double * val );
static int cpdo_my5_stmt_get_string_ndx( cpdo_stmt * self, uint16_t ndx, char const ** val, uint32_t * len );
static int cpdo_my5_stmt_get_blob_ndx( cpdo_stmt * self, uint16_t ndx, void const ** v, uint32_t * len );
static int cpdo_my5_stmt_finalize( cpdo_stmt * self );
const cpdo_stmt_api cpdo_my5_stmt_api = {
    cpdo_my5_stmt_step,
    cpdo_my5_stmt_error_info,
    cpdo_my5_stmt_finalize,
    {/*bind*/
        cpdo_my5_stmt_reset,
        cpdo_my5_stmt_bind_count,
        cpdo_my5_stmt_param_index,
        cpdo_my5_stmt_param_name,
        cpdo_my5_stmt_bind_null,
        cpdo_my5_stmt_bind_int8,
        cpdo_my5_stmt_bind_int16,
        cpdo_my5_stmt_bind_int32,
        cpdo_my5_stmt_bind_int64,
        cpdo_my5_stmt_bind_float,
        cpdo_my5_stmt_bind_double,
        cpdo_my5_stmt_bind_string,
        cpdo_my5_stmt_bind_blob
    },
    {/*get*/
        cpdo_my5_stmt_column_count,
        cpdo_my5_stmt_column_name,
        cpdo_my5_stmt_get_type_ndx,
        cpdo_my5_stmt_get_int8_ndx,
        cpdo_my5_stmt_get_int16_ndx,
        cpdo_my5_stmt_get_int32_ndx,
        cpdo_my5_stmt_get_int64_ndx,
        cpdo_my5_stmt_get_float_ndx,
        cpdo_my5_stmt_get_double_ndx,
        cpdo_my5_stmt_get_string_ndx,
        cpdo_my5_stmt_get_blob_ndx
    }
};



typedef struct cpdo_my5_stmt cpdo_my5_stmt;
static int cpdo_my5_stmt_free(cpdo_my5_stmt *s);
static cpdo_my5_stmt * cpdo_my5_stmt_alloc();

typedef struct cpdo_my5_driver cpdo_my5_driver;
static int cpdo_my5_driver_free(cpdo_my5_driver *d);
static cpdo_my5_driver * cpdo_my5_driver_alloc();

enum cpdo_my_constants {
    CPDO_MY5_MAX_NAMED_PARAMS = 100
};


/**
    Internal impl of cpdo_driver.
*/
struct cpdo_my5_driver
{
    MYSQL * conn;
    uint32_t fieldBufferSize;
    int lastErrNo;
    char isConnected;
    char inTransaction;
    char explicitAutoCommit;
    char blobsAllowGetString;
    char enableNamedParams;
    char * lastErrMsg;
    cpdo_my5_stmt * currentStmt;
    /** The "this" object of the instance. */
    cpdo_driver self;
};

/**
    Empty-initialized cpdo_my5_driver object.
*/
const cpdo_my5_driver cpdo_my5_driver_empty = {
    NULL/*conn*/,
    1024 * 4 /*fieldBufferSize*/,
    0 /* lastErrNo */,
    0/*isConnected*/,
    0/*inTransaction*/,
    -1/*explicitAutoCommit. -1 is a magic value later on, so don't change it!.*/,
    0 /*blobsAllowGetString*/,
    1 /*enablenamedparams*/,
    NULL /* lastErrMsg */,
    NULL /*currentStmt*/,
    {/*self*/
        &cpdo_my5_driver_api /*api*/,
        NULL /*impl*/
    }
};

/**
    Internan cpdo_stmt impl.
*/
struct cpdo_my5_stmt
{
    MYSQL_STMT * stmt;
    MYSQL_RES * colMeta;
    MYSQL_ROW * row;
    cpdo_my5_driver * driver;
    char needsExec;
    /**
        Used when driver->enableNamedParams is true to store
        the translated SQL.
    */
    char * sql;
    /**
       Buffers for holding bound parameter data.
     */
    struct
    {
        /** Number of bound parameters. */
        uint16_t count;
        /** Number of unique bound parameter names. */
        uint16_t uniqueCount;
        /** MySQL's interface into the bound parameters.
            This is an array count items long.
        */
        MYSQL_BIND * myBinders;
        /** Our buffers where myBinders point to.
            This is an array count items long.
        */
        cpdo_bind_val * cBinders;
        /**
            If driver->enableNamedParams is true then this
            is an array, count items long, holding the names
            of the bound parameters. The name is "?" if
            the bound parameter is a question-mark parameter
            placeholder.
            
            As odd as this seems, this memory is owned by the sql
            pointer, and freeing that memory frees this as well.
            See cpdo_named_params_to_qmarks() for why.
        */
        char ** names;
    } pbind;
    /**
       Buffers for holding result set data.
     */
    struct
    {
        /** The number of columns in the result set.
        */
        uint16_t count;
        /** MySQL's interface into the result data colums.
            This is an array count items long.
        */
        MYSQL_BIND * myBinders;
        /** Our buffers where myBinders point to.
            This is an array count items long.
        */
        cpdo_bind_val * cBinders;
        /**
            Buffers for holding T-to-String conversions.  This is an
            array count items long. We use this in the get_string() op
            to cache the result of the string conversions so that the
            lifetime of the returned bytes can be extended to a usable
            duration.
        */
        cpdo_bind_val * convBuf;
    } rbind;
    /** The "this" object of the instance. */
    cpdo_stmt self;
};

/**
    Empty-initialized cpdo_my5_stmt object.
*/
const cpdo_my5_stmt cpdo_my5_stmt_empty = {
    NULL /*stmt*/,
    NULL /*colMeta*/,
    NULL /*row*/,
    NULL /*driver*/,
    1 /*needsExec*/,
    NULL /*sql*/,
    {/*pbind*/
        0/*count*/,
        0/*uniqueCount*/,
        NULL /*myBinders*/,
        NULL /*cBinders*/,
        NULL /*names*/
    },
    {/*rbind*/
        0/*count*/,
        NULL /*myBinders*/,
        NULL /*cBinders*/,
        NULL /*convBuf*/
    },
    {/*self*/
        &cpdo_my5_stmt_api /*api*/,
        NULL /*impl*/
    }
};

/**
    Allocates a new cpdo_my5_driver and its conn member, and
    points theObject->self.impl at theObject for later reference
    and proper cleanup.

    Returns NULL on error. On success the caller owns the
    object, of course.
*/
static cpdo_my5_driver * cpdo_my5_driver_alloc()
{
    cpdo_my5_driver * s = (cpdo_my5_driver*)malloc(sizeof(cpdo_my5_driver));
    if( s )
    {
        MYSQL * con = mysql_init(NULL);
        if( ! con )
        {
            free(s);
            return NULL;
        }
        else
        {
            *s = cpdo_my5_driver_empty;
            s->conn = con;
            s->self.impl = s;
        }
    }
    return s;
}

/**
   Closes d->conn and frees all memory associated with d.  d does not
   track statements it opens, and all statements must be closed before
   closing the db, else Undefined Behaviour.
*/
static int cpdo_my5_driver_free(cpdo_my5_driver *d)
{
    int rc = cpdo_rc.ArgError;
    if( d )
    {
        rc = 0;
        free( d->lastErrMsg );
        if( d->conn )
        {
            mysql_close(d->conn);
        }
        *d = cpdo_my5_driver_empty;
        free(d);
    }
    return rc;
}


/**
   Allocates a new cpdo_my5_stmt and initializes
   its self.impl member to point to the returned
   object.
   
   Returns NULL on error. On success the caller owns the
   object, of course.
*/
static cpdo_my5_stmt * cpdo_my5_stmt_alloc()
{
    cpdo_my5_stmt * s = (cpdo_my5_stmt*)malloc(sizeof(cpdo_my5_stmt));
    if( s )
    {
        *s = cpdo_my5_stmt_empty;
        s->self.impl = s;
    }
    return s;
}

/**
   Frees all resources belonging to this statement.  It can return
   non-0, but there is no generic recovery strategy for this, and s is
   freed regardless of whether or not sqlite3_finalize() succeeds.
*/
static int cpdo_my5_stmt_free(cpdo_my5_stmt *st)
{
    int rc = cpdo_rc.ArgError;
    if( st )
    {
        rc = 0;
        if( st->sql )
        {
            assert( 0 != st->pbind.count );
            free( st->sql );
            st->pbind.names = NULL /* memory is owned by sql string!*/;
        }
        if( st->stmt )
        {
            mysql_stmt_close(st->stmt);
        }
        if( st->colMeta )
        {
            mysql_free_result(st->colMeta);
        }
        if( st->pbind.myBinders )
        {
            assert( 0 != st->pbind.count );
            free(st->pbind.myBinders);
        }
        if( st->pbind.cBinders )
        {
            assert( 0 != st->pbind.count );
            cpdo_bind_val_list_free( st->pbind.cBinders, st->pbind.count );
        }
        if( st->rbind.convBuf )
        {
            assert( 0 != st->rbind.count );
            cpdo_bind_val_list_free( st->rbind.convBuf, st->rbind.count );
        }
        if( st->rbind.myBinders )
        {
            assert( 0 != st->rbind.count );
            free(st->rbind.myBinders);
        }
        if( st->rbind.cBinders )
        {
            assert( 0 != st->rbind.count );
            cpdo_bind_val_list_free( st->rbind.cBinders, st->rbind.count );
        }
        *st = cpdo_my5_stmt_empty;
        free( st );
    }
    return rc;
}


/**
   cpdo_driver_factory_f() impl._ Allocates a new cpdo_my5_driver.
*/
int cpdo_my5_driver_new( cpdo_driver ** tgt )
{
    if( ! tgt ) return cpdo_rc.ArgError;
    else
    {
        cpdo_my5_driver * d = cpdo_my5_driver_alloc();
        if( d )
        {
            static char inited = 0;
            if( ! inited && (inited=1) )
            {
                atexit( mysql_library_end );
            }
            *tgt = &d->self;
            return 0;
        }
        else return cpdo_rc.AllocError;
    }
}

#define DRV_DECL(RC) cpdo_my5_driver * drv = (self && self->impl && (self->api==&cpdo_my5_driver_api)) \
        ? (cpdo_my5_driver *)self->impl : NULL; \
    if( ! drv ) return RC

#define STMT_DECL(RC) cpdo_my5_stmt * stmt = (self && self->impl && (self->api==&cpdo_my5_stmt_api)) \
        ? (cpdo_my5_stmt *)self->impl : NULL; \
    if( ! stmt ) return RC
#define PBIND_DECL(NDX) MYSQL_BIND * mybin; cpdo_bind_val * cbin;     \
        STMT_DECL(cpdo_rc.ArgError);                                    \
        if(!(NDX) || ((NDX)>stmt->pbind.count)) return cpdo_rc.RangeError;  \
        mybin = &stmt->pbind.myBinders[(NDX)-1]; cbin = &stmt->pbind.cBinders[(NDX)-1]; \
        if( (void*)cbin == (void*)mybin ) { /* only to avoid assinged-but-not-used warning */}



static int cpdo_my5_drv_err2( cpdo_my5_driver * self, int n, char const * msg )
{
    size_t const slen = msg ? strlen(msg) : 0;
    char * x;
    free( self->lastErrMsg );
    self->lastErrMsg = NULL;
    self->lastErrNo = n;
    if( ! slen ) return 0;
    x = (char *)malloc( slen + 1 );
    if( !x ) return cpdo_rc.AllocError;
    else
    {
        memcpy( x, msg, slen );
        x[slen] = 0;
        self->lastErrMsg = x;
        return 0;
    }
}

static int cpdo_my5_drv_err( cpdo_my5_driver * drv )
{
    return cpdo_my5_drv_err2( drv, mysql_errno(drv->conn), mysql_error(drv->conn) );
}
static int cpdo_my5_stmt_err( cpdo_my5_stmt * self )
{
    return cpdo_my5_drv_err2( self->driver,
                              mysql_stmt_errno(self->stmt),
                              mysql_stmt_error(self->stmt) );
}

static int cpdo_my5_last_insert_id( cpdo_driver * self, uint64_t * v, char const * hint )
{
    DRV_DECL(cpdo_rc.ArgError);
    if( ! v ) return cpdo_rc.ArgError;
    else
    {
        *v = mysql_insert_id(drv->conn);
        return 0;
    }
}

static int cpdo_my5_capabilities( const cpdo_driver * self )
{
    DRV_DECL(CPDO_CAPABILITY_ERROR);
    return (drv->enableNamedParams ? CPDO_CAPABILITY_NAMED_PARAMETERS : 0)
        | CPDO_CAPABILITY_AUTOINCREMENT
        | CPDO_CAPABILITY_LAST_INSERT_ID
        | CPDO_CAPABILITY_NUMBER_TO_STRING
        | CPDO_CAPABILITY_STRING_TO_NUMBER
        | CPDO_CAPABILITY_BLOBS
        ;
}
    
#if 0
static void cpdo_my5_clear_err( cpdo_my5_driver * drv )
{
    cpdo_my5_drv_err2( drv, 0, NULL );
}
#endif

static int cpdo_my5_close( cpdo_driver * self )
{
    DRV_DECL(cpdo_rc.ArgError);
    cpdo_my5_driver_free(drv);
    return 0;
}

static char cpdo_my5_is_connected( cpdo_driver * self )
{
    DRV_DECL(0);
    return drv->isConnected;
}

static int cpdo_my5_error_info( cpdo_driver * self, char const ** dest, uint32_t * len, int * errorCode )
{
    DRV_DECL(cpdo_rc.ArgError);
    if( ! drv->conn ) return cpdo_rc.ConnectionError;
    else
    {
        if( errorCode )
        {
            *errorCode = drv->lastErrMsg ? drv->lastErrNo : mysql_errno(drv->conn);
        }
        if( dest )
        {
            *dest = drv->lastErrMsg ? drv->lastErrMsg : mysql_error(drv->conn);
            if( len )
            {
                *len = *dest ? strlen(*dest) : 0;
            }
        }
        return 0;
    }
}

/**
   RESET_DRV_ERR should be called in each function which might return
   cpdo_rc.CheckDbError. (Because those are the functions which
   update drv->lastErrMsg.)

   We need this to avoid a use-case (corner case?) in error handling
   where drv->lastErrMsg would shadow the value from mysql_error().

   That said, there probably are some odd corner-cases this laying
   around from this hack (which was necessary for us to be able to
   report statement-preparation-failed error info).
 */
#define RESET_DRV_ERR(DRIVER) cpdo_my5_drv_err2( (DRIVER), 0, NULL )

#define TRY_SHARED_STRINGS 1
#if TRY_SHARED_STRINGS
static struct {
    char sql_null[5];
    char quoted_empty[3];
} my5_shared_strings = {
{'N','U','L','L',0},
{'\'','\'',0}
};
#endif
static int cpdo_my5_sql_quote( cpdo_driver * self, char const * str, uint32_t * len, char ** dest )
{
    DRV_DECL(cpdo_rc.ArgError);
    RESET_DRV_ERR(drv);
    if( ! len || !dest ) return cpdo_rc.ArgError;
    else if( NULL == str )
    {
#if TRY_SHARED_STRINGS
        *dest = my5_shared_strings.sql_null;
        *len = 4;
        return 0;
#else
        char * tmp = (char *)malloc(5);
        if( ! tmp ) return cpdo_rc.AllocError;
        strcpy( tmp, "NULL" );
        *dest = tmp;
        *len = 4;
        return 0;
#endif
    }
    else if(!*str || !*len){
        *dest = my5_shared_strings.quoted_empty;
        *len = 2;
        return 0;
    }
    else
    {
        char * to = NULL;
        unsigned long aLen;
        cpdo_my5_drv_err2( drv, 0, NULL );
        aLen = *len * 2 + 1;
        to = (char *)calloc(aLen,1);
        if( ! to ) return cpdo_rc.AllocError;
        *len = mysql_real_escape_string( drv->conn, to, str, *len );
        *dest = to;
        return 0;
    }
}

static int cpdo_my5_sql_qualify( cpdo_driver * self, char const * str, uint32_t * len, char ** dest )
{
    if(!str || !dest || !*str) return cpdo_rc.ArgError;
    else if( (NULL != strstr(str, ";"))
             ||  (NULL != strstr(str, "'"))
             ||  (NULL != strstr(str, "`"))
             )
    {
        return cpdo_rc.RangeError;
    }
    else
    {
        int sz = 0;
        char * rc = cpdo_mprintf("`%s`%n", str, &sz);
        if(!rc) return cpdo_rc.AllocError;
        else
        {
            assert( sz > 0 );
            *dest = rc;
            if(len) *len = (uint32_t)sz; /*strlen(rc);*/
            return 0;
        }
    }
}

static int cpdo_my5_free_string( cpdo_driver * self, char * str)
{
    if(!self || !str) return cpdo_rc.ArgError;
#if TRY_SHARED_STRINGS
    else if( ((void const *)str >= (void const *)&my5_shared_strings)
        && ((void const *)str < (void const *)((unsigned char *)&my5_shared_strings + sizeof(my5_shared_strings)))){
        return 0;
    }
    else
#endif
    {
        free(str);
        return 0;
    }
}

/**
   A horribly slow/poor algorithm which runs through stmt->pbind.names
   and checks for duplicate named parameters. Each unique parameter is
   counted only once. Returns the number of unique parameter names,
   where each '?' placeholder is (non-intuitively) counted as unique.

   e.g. the list (:p1, :p2, ?, :p1, ?) should calculate to the param
   count of 3 (because :p1 is duplicated and each '?' counts once).
*/
static uint16_t cpdo_my_calc_real_param_count( cpdo_my5_stmt * stmt ){
    uint16_t i, x, n = 0;
    char * names[CPDO_MY5_MAX_NAMED_PARAMS];
    if(!stmt->pbind.count) return 0;
    memset(names,0, sizeof(names)/sizeof(names[0]));
    for( i = 0; i < stmt->pbind.count; ++i ){
        names[i] = stmt->pbind.names[i];
    }
    for( i = 0; i < stmt->pbind.count; ++i )
    {
        char const * name = names[i];
        if(!name) continue;
        names[i] = 0;
        if('?' == *name) {
            ++n;
            continue;
        }
        else if(':'==*name){
            ++n;
            for( x = i+1; x < stmt->pbind.count; ++x ){
                char const * key = names[x];
                if(!key) continue;
                if( key && (0==strcmp(key,name)) ){
                    names[x] = 0;
                }
            }
        }
    }
    return n;
}


/**
   Allocates enough space for paramCount bound parameters and
   columnCount result columns, not including the memory we will need
   to point MYSQ_BIND to for result data.

   Returns 0 on success.
*/
static int cpdo_my5_setup_bind_buffers( cpdo_my5_stmt * stmt, uint16_t paramCount,
                                        uint16_t columnCount  )
{
    if( paramCount > 0 )
    { /* bound parameters ... */
        stmt->pbind.myBinders = (MYSQL_BIND*) calloc(sizeof(MYSQL_BIND),paramCount);
        if( ! stmt->pbind.myBinders ) return cpdo_rc.AllocError;
        stmt->pbind.count = (uint16_t)paramCount
            /* reminder: we need this value set first in case the next
               ops fail, so that we can clean up downstream.
            */
            ;
        stmt->pbind.uniqueCount = stmt->driver->enableNamedParams
            ? cpdo_my_calc_real_param_count( stmt )
            : stmt->pbind.count;
        /*MARKER("param counts: %u, %u\n", stmt->pbind.count, stmt->pbind.uniqueCount);*/
        
        stmt->pbind.cBinders = cpdo_bind_val_list_new(stmt->pbind.count);
        if( ! stmt->pbind.cBinders ) return cpdo_rc.AllocError;
    }
    if( columnCount > 0 )
    { /* fetchable fields... */
        stmt->rbind.myBinders = (MYSQL_BIND*) calloc(sizeof(MYSQL_BIND), columnCount);
        if( ! stmt->rbind.myBinders ) return cpdo_rc.AllocError;
        stmt->rbind.count = (uint16_t)columnCount
            /* reminder we need this value set here in case the next ops fail. */
            ;
        stmt->rbind.cBinders = cpdo_bind_val_list_new(stmt->rbind.count);
        if( ! stmt->rbind.cBinders ) return cpdo_rc.AllocError;

        /* stmt->rbind.convBuf initialization is delayed until get_string(). */
    }
    if( stmt->colMeta && !stmt->rbind.count ) return cpdo_rc.InternalError;
    return 0;
}

/**
   Frees v, which is expected to be a MYSQL_TIME value
   stored by cpdo_my5_setup_result_bindings().

   Used by the MYSQL_TIME/DATE/DATETIME/TIMESTAMP-binding code.
*/
static void cpdo_my5_MYSQL_TIME_free( void * v )
{
    free(v);
}
/**
   Sets up stmt->rbind depending on the column state of stmt->stmt. It
   sets up the buffers needed by MySQL for binding fetchable results.

   Returns 0 on success. On error stmt initialization must
   be considered to have failed. The only realistic errors here
   are:

   a) Bugs in this code. i'm not aware of any, but we normally aren't.

   b) An allocation error. This can certainly happen if someone
   fetches huge string/blob fields.
*/
static int cpdo_my5_setup_result_bindings( cpdo_my5_stmt * stmt )
{
    if( ! stmt->rbind.count ) return 0;
    else if( ! stmt->colMeta ) return cpdo_rc.InternalError;
    else if( ! stmt->rbind.count ) return 0;
    else
    {
        uint16_t i;
        MYSQL_FIELD * fld = NULL;
        MYSQL_BIND * bin = NULL;
        cpdo_bind_val * bv = NULL;
        int rc;
        for( i = 0; i < stmt->rbind.count; ++i )
        {
            fld = &stmt->stmt->fields[i];
            bin = &stmt->rbind.myBinders[i];
            bv = &stmt->rbind.cBinders[i];
            bin->is_null = &bv->is_null;
            bin->error = &bv->has_error;
            switch( fld->type )
            {
              case MYSQL_TYPE_TINY:
                  cpdo_bind_val_int8( bv, 0 );
                  bin->buffer = (char *)&bv->valu.i8;
                  break;
              case MYSQL_TYPE_SHORT:
                  cpdo_bind_val_int16( bv, 0 );
                  bin->buffer = (char *)&bv->valu.i16;
                  break;
              case MYSQL_TYPE_LONG:
                  cpdo_bind_val_int32( bv, 0 );
                  bin->buffer = (char *)&bv->valu.i32;
                  break;
              case MYSQL_TYPE_LONGLONG:
                  cpdo_bind_val_int64( bv, 0 );
                  bin->buffer = (char *)&bv->valu.i64;
                  break;
              case MYSQL_TYPE_FLOAT:
                  cpdo_bind_val_float( bv, 0.0 );
                  bin->buffer = (char *)&bv->valu.flt;
                  break;
              case MYSQL_TYPE_DOUBLE:
                  cpdo_bind_val_double( bv, 0.0 );
                  bin->buffer = (char *)&bv->valu.dbl;
                  break;
              case MYSQL_TYPE_VAR_STRING:
              case MYSQL_TYPE_STRING: {
                  unsigned int const allocLen = fld->length
                      ? fld->length
                      : stmt->driver->fieldBufferSize
                      ;
                  rc = cpdo_bind_val_string( bv, NULL, allocLen );
                  if( rc ) return rc;
                  bin->buffer = (char *)bv->valu.blob.mem;
                  bin->buffer_length = bv->valu.blob.length;
                  bin->length = &bv->valu.blob.length;
                  break;
              }
              case MYSQL_TYPE_BLOB: {
                  unsigned int allocLen = fld->length
                      ? fld->length
                      : stmt->driver->fieldBufferSize
                      ;
                  assert(0 != allocLen);
                  rc = cpdo_bind_val_blob( bv, NULL, allocLen );
                  if( rc ) return rc;
                  bin->buffer = (char *)bv->valu.blob.mem;
                  bin->buffer_length = bv->valu.blob.length;
                  bin->length = &bv->valu.blob.length;
                  break;
              }
              case MYSQL_TYPE_DATE:
              case MYSQL_TYPE_TIME:
              case MYSQL_TYPE_DATETIME:
              case MYSQL_TYPE_TIMESTAMP: {
                  MYSQL_TIME * mt = (MYSQL_TIME *)calloc(sizeof(MYSQL_TIME),1);
                  if( ! mt ) return cpdo_rc.AllocError;
                  rc = cpdo_bind_val_custom( bv, mt,
                                             cpdo_my5_MYSQL_TIME_free,
                                             cpdo_bind_val_tag_type_hash(&cpdo_my5_driver_api,fld->type));
                  if( rc )
                  {
                      free(mt);
                      return rc;
                  }
                  bin->buffer = (char *)mt;
                  bin->buffer_length = sizeof(MYSQL_TIME);
                  bin->length = &bv->valu.custom.length;
                  break;
              }
              default:
                  MARKER("WARNING: UNHANDLED MYSQL_TYPE_XXX #%d IN PARAMETER DATA\n",fld->type);
                  return cpdo_rc.TypeError;
            };
            bin->buffer_type = fld->type;
        }
        return 0;
    }

}

static int cpdo_my5_prepare( cpdo_driver * self, cpdo_stmt ** tgt, char const * sql, uint32_t len  )
{
    int rc = 0;
    cpdo_my5_stmt * st = NULL;
    MYSQL_STMT * myst = NULL;
    MYSQL_RES * meta = NULL;
    char * xlate = NULL /* "translated" SQL, for named params support. */;
    uint32_t xlateLen = 0 /* length of xlate */;
    char ** nameList = NULL /* bound params names. Memory is owned by xlate's block! */;
    uint16_t paramCount = 0;
    uint16_t fieldCount = 0;
    DRV_DECL(cpdo_rc.ArgError);
    RESET_DRV_ERR(drv);
    myst = mysql_stmt_init( drv->conn );
    if( NULL == myst )
    {
        cpdo_my5_drv_err( drv );
        return cpdo_rc.CheckDbError;
    }
    if( drv->enableNamedParams )
    { /* Do custom bound parameter name parsing... */
        rc = cpdo_named_params_to_qmarks( sql, len, ':', 1, &paramCount,
                                          &xlate, &xlateLen, &nameList );
        if( rc )
        {
            assert( NULL == xlate );
            mysql_stmt_close( myst );
            return rc;
        }
    }
    rc = xlate
        ? mysql_stmt_prepare( myst, xlate, xlateLen )
        : mysql_stmt_prepare( myst, sql, len );
    if( rc )
    {
        /*
          The client has no way to get this error info. We must stuff
          it into the driver object.

          MySQL does not document (and google doesn't know) the
          lifetime of the bytes returned by mysql_stmt_error().
        */
#if MEGADEBUG
        MARKER("prepare failed: mysql_stmt_error(): %s\n", mysql_stmt_error(myst));
#endif
        free(xlate);
        cpdo_my5_drv_err2( drv, mysql_stmt_errno(myst), mysql_stmt_error(myst) );
        mysql_stmt_close( myst );
        return cpdo_rc.CheckDbError;
    }
    if( drv->enableNamedParams )
    {
        if( (uint16_t)mysql_stmt_param_count( myst ) != paramCount )
        { /* indicative of a bug in our parsing. */
            MARKER("cpdo mysql driver error: param count check mismatch: %lu, %u\n",
                   mysql_stmt_param_count( myst ),
                   paramCount );
            free( xlate );
            mysql_stmt_close( myst );
            return cpdo_rc.InternalError;
        }
    }
    else
    {
        paramCount = (uint16_t)mysql_stmt_param_count( myst );
    }
    fieldCount = (uint16_t)mysql_stmt_field_count( myst );
    meta = mysql_stmt_result_metadata( myst )/* can legally be NULL*/;
    st = cpdo_my5_stmt_alloc();
    if( NULL == st )
    {
        if( meta ) mysql_free_result( meta );
        mysql_stmt_close( myst );
        free(xlate);
        return cpdo_rc.AllocError;
    }
    st->stmt = myst;
    st->sql = xlate;
    st->pbind.names = nameList;
    st->driver = drv;
    st->colMeta = meta;
    rc = cpdo_my5_setup_bind_buffers( st, paramCount, fieldCount );
    assert( paramCount == st->pbind.count );
    if( 0 == rc ) rc = cpdo_my5_setup_result_bindings( st );
    if( 0 == rc ) *tgt = &st->self;
    else cpdo_my5_stmt_free(st);
    if( MEGADEBUG && (0 == rc) )
    {
        MARKER("Created statement @%p\n", (void const *)self);
    }

    return rc;
}

int cpdo_my5_connect( cpdo_driver * self, cpdo_connect_opt const * opt )
{
    DRV_DECL(cpdo_rc.ArgError);
    if( ! opt || !opt->dsn ) return cpdo_rc.ArgError;
    else
    {
        enum {
            /** Max size for parameter keys and values, including
            trailing NUL.
            */
            BufSize = 128U
        };
        char const * tokBegin = opt->dsn;
        char const * tokEnd = NULL;
        char kbuf[BufSize] = {0,0};
        char pDbName[BufSize] = {0,0};
        char pHost[BufSize] = {0,0};
        int port = 0;
        int rc = 0;
        if( drv->isConnected ) return cpdo_rc.ConnectionError;
        for( ; *tokBegin && (*tokBegin != ':'); ++tokBegin ) {
            /* skip driver name part of dsn. */
        }
        if( ':' != *tokBegin ) return cpdo_rc.RangeError;
        ++tokBegin /* skip ':'*/;
        port = rc = 0;
        while( cpdo_next_token( &tokBegin, ';', &tokEnd) )
        { /* TODO: wrap most of this into a helper function
             which does the key/value splitting. We'll need
             this in other drivers.
          */
            if( tokBegin == tokEnd ) break;
            else
            {
                char const * key = tokBegin;
                char const * value = NULL;
                char * at = kbuf;
                if( (tokEnd - tokBegin) >= BufSize ) return cpdo_rc.RangeError;
                memset( kbuf, 0, BufSize );
                /* Write the key part to the buffer... */
                for( ; (key<tokEnd) && *key && ('='!=*key); ++key ) {
                    *(at++) = *key;
                }
                *(at++) = 0;
                value = at;
                if( '=' == *key ) {
                    ++key;
                }
                /* Write the value part to the buffer... */
                for( ; (key<tokEnd) && *key; ++key ) {
                    *(at++) = *key;
                }
                key = kbuf;
                /*MARKER("key=[%s] value=[%s]\n", key, value);*/

                /* Done parsing. Now see if we understand how to use
                   this option... */
                if( 0 == strcmp("port",key) )
                { /* remember that mysql ignores the port number when
                     connecting to localhost via a UNIX socket.
                  */
                    port = *value ? atoi(value) : 0;
                    if( port < 0 ) port = 0;
                }
                else if( 0 == strcmp("dbname",key) )
                {
                    size_t const slen = strlen(value);
                    if( slen >= BufSize ) return cpdo_rc.RangeError;
                    memcpy( pDbName, value, slen );
                    pDbName[slen] = 0;
                }
                else if( 0 == strcmp("host",key) )
                {
                    size_t const slen = strlen(value);
                    if( slen >= BufSize ) return cpdo_rc.RangeError;
                    memcpy( pHost, value, slen );
                    pHost[slen] = 0;
                }
                else if( 0 == strcmp("autocommit",key) )
                {
                    drv->explicitAutoCommit = cpdo_token_bool_val(value);
                }
                else if( 0 == strcmp("fieldbuffersize",key) )
                {
                    size_t const slen = strlen(value);
                    int32_t i;
                    if( slen >= BufSize ) return cpdo_rc.RangeError;
                    i = atoi(value);
                    if( i > 64 )
                    {
                        drv->fieldBufferSize = (uint32_t)i;
                    }
                }
                else if( 0 == strcmp("enablenamedparams",key) )
                {
                    drv->enableNamedParams = cpdo_token_bool_val(value);
                }
                else if( 0 == strcmp("enableblobtostring",key) )
                {
                    drv->blobsAllowGetString = cpdo_token_bool_val(value);
                }
                else
                {
                    /* ignore unknown keys: this is optional in the CPDO
                       interface. If we add warning support, i'll add the
                       warning here. Or if i'm feeling pedantic later i'll
                       throw the error here.
                    */
                }
                /* set up for the next token... */
                tokBegin = tokEnd;
                tokEnd = NULL;
            }
        } /* options parsing */
        rc = (drv->conn ==
              mysql_real_connect( drv->conn, pHost,
                                  opt->user_name, opt->password,
                                  pDbName, port,
                                  NULL/*unix socket*/, 0 /*"client flag"*/))
            ? 0 :  cpdo_rc.CheckDbError
            ;
        if( (0 == rc)
            && (drv->explicitAutoCommit!=-1
                /* only set autocommit explicitly if we got an autocommit parameter*/)
            )
        {
            mysql_autocommit( drv->conn, drv->explicitAutoCommit );
        }
        if( 0 == rc )
        {
            drv->isConnected = 1;
        }
        else if( cpdo_rc.CheckDbError == rc )
        {
            cpdo_my5_drv_err( drv );
        }
        return rc;
    }
}


static int cpdo_my5_driver_begin_transaction( cpdo_driver * self )
{
    int rc;
    DRV_DECL(cpdo_rc.ArgError);
    RESET_DRV_ERR(drv);
    if( drv->inTransaction ) return cpdo_rc.UnsupportedError;
    /**
        i can find no API for BEGINING a transaction in MySQL,
        but there are mysql_commit() and mysql_rollback().
        Though the API docs don't SAY THIS, turning off autocommit
        is _apparently_ the same as "BEGIN", except that when
        i run "BEGIN" from here the statement preparation fails
        with error code 1. (From the command-line mysql client it works
        fine.)
    */
    rc = mysql_autocommit(drv->conn, 0);
    /*rc = cpdo_exec( self, "BEGIN", 5 );*/
    if( 0 == rc ) drv->inTransaction = 1;
    if( rc )
    {
        cpdo_my5_drv_err( drv );
        return cpdo_rc.CheckDbError;
    }
    else return 0;
}

static int cpdo_my5_driver_opt_set( cpdo_driver * self, char const * key, va_list vargs )
{
    return cpdo_rc.NYIError;
}
static int cpdo_my5_driver_opt_get( cpdo_driver * self, char const * key, va_list vargs )
{
    return cpdo_rc.NYIError;
}

static int cpdo_my5_driver_commit( cpdo_driver * self )
{
    int rc;
    DRV_DECL(cpdo_rc.ArgError);
    RESET_DRV_ERR(drv);
    rc = mysql_commit( drv->conn );
    if( drv->explicitAutoCommit >= 0 /* == was explicitly set by the client.*/ )
    {
        mysql_autocommit(drv->conn, drv->explicitAutoCommit );
    }
    drv->inTransaction = 0;
    if( rc )
    {
        cpdo_my5_drv_err( drv );
        return cpdo_rc.CheckDbError;
    }
    else return 0;
}

static int cpdo_my5_driver_rollback( cpdo_driver * self )
{
    int rc;
    DRV_DECL(cpdo_rc.ArgError);
    RESET_DRV_ERR(drv);
    rc = mysql_rollback( drv->conn );
    if( drv->explicitAutoCommit >= 0 )
    {
        mysql_autocommit(drv->conn, drv->explicitAutoCommit);
    }
    drv->inTransaction = 0;
    if( rc )
    {
        cpdo_my5_drv_err( drv );
        return cpdo_rc.CheckDbError;
    }
    else return 0;
}

static char cpdo_my5_driver_in_trans( cpdo_driver * self )
{
    DRV_DECL(0);
    return drv->inTransaction;
}

/**
   Cleans up the innards of st->rbind.convBuf. To be called
   before stepping the cursor.
*/
static void cpdo_my5_cleanup_scratchpads( cpdo_my5_stmt * st )
{
    uint16_t i;
    assert( st );
    if(st->rbind.convBuf) {
        for( i = 0; i < st->rbind.count; ++i )
        {
            cpdo_bind_val_clean( &st->rbind.convBuf[i] );
        }
    }
}

static cpdo_step_code cpdo_my5_stmt_step( cpdo_stmt * self )
{
    STMT_DECL(CPDO_STEP_ERROR);
    if( ! stmt->stmt ) return CPDO_STEP_ERROR;
    if( stmt->pbind.count )
    {
        if( mysql_stmt_bind_param( stmt->stmt, stmt->pbind.myBinders ) )
        {
            return CPDO_STEP_ERROR;
        }
    }
    if( stmt->rbind.count )
    { /* fetching data */
        int rc;
        cpdo_my5_cleanup_scratchpads( stmt );
        if( mysql_stmt_bind_result( stmt->stmt, stmt->rbind.myBinders ) )
        {
            return CPDO_STEP_ERROR;
        }
        if( stmt->needsExec )
        {
            if( mysql_stmt_execute( stmt->stmt ) ) return CPDO_STEP_ERROR;
            stmt->needsExec = 0;
        }
        rc = mysql_stmt_fetch( stmt->stmt );
        if( 0 == rc ) return CPDO_STEP_OK;
        else if( MYSQL_NO_DATA == rc ) return CPDO_STEP_DONE;
        else return CPDO_STEP_ERROR;
    }
    else
    { /* non-fetching query */
        if( mysql_stmt_execute( stmt->stmt ) ) return CPDO_STEP_ERROR;
        stmt->needsExec = 0;
        return CPDO_STEP_DONE;
    }
}

static int cpdo_my5_stmt_reset( cpdo_stmt * self )
{
    STMT_DECL(cpdo_rc.ArgError);
    RESET_DRV_ERR(stmt->driver);
    if( 0==mysql_stmt_reset(stmt->stmt) )
    {
        stmt->needsExec = 1;
        return 0;
    }
    else
    {
        cpdo_my5_stmt_err( stmt );
        return cpdo_rc.CheckDbError;
    }
}

static uint16_t cpdo_my5_stmt_column_count( cpdo_stmt * self )
{
    STMT_DECL(cpdo_rc.ArgError);
    return (uint32_t)mysql_stmt_field_count(stmt->stmt);
}

static char const * cpdo_my5_stmt_column_name( cpdo_stmt * self, uint16_t ndx )
{
    STMT_DECL(NULL);
    return
    /* these should all be equivalent... */
#if 0
        ( ndx >= mysql_stmt_field_count(stmt->stmt) )
#elif 1
        ( ndx >= stmt->stmt->field_count )
#elif 0
        ( ndx >= cpdo_my5_stmt_column_count( stmt )
#else
        ( ndx >= stmt->api->column_count(stmt) )
#endif        
        ? NULL
        : stmt->stmt->fields[ndx].name
        ;
}

static uint16_t cpdo_my5_stmt_bind_count( cpdo_stmt * self )
{
    STMT_DECL(0);
    return /*stmt->pbind.count;*/ stmt->pbind.uniqueCount;
}

static uint16_t cpdo_my5_stmt_param_index( cpdo_stmt * self, char const * name )
{
    /*
      We need this function:

      http://dev.mysql.com/doc/refman/5.0/en/mysql-stmt-param-metadata.html

      But its docs say: "This function currently does nothing."
    */
    uint16_t i;
    char const * key;
    STMT_DECL(0);
    if( ! name || !*name || ('?'==*name) )
    {
        /* Reminder: we catch '?' here because this routine will only
           ever match the first one with that name. */
        return cpdo_rc.ArgError;
    }
    for( i = 0; i < stmt->pbind.count; ++i )
    {
        key = stmt->pbind.names[i];
        if( key && (0==strcmp(key,name)) )
        {
            return i+1;
        }
    }
    return 0;
}


static char const * cpdo_my5_stmt_param_name( cpdo_stmt * self, uint16_t ndx )
{
    STMT_DECL(NULL);
    if( !ndx || (ndx > stmt->pbind.count) ) return NULL;
    assert( stmt->pbind.names && stmt->pbind.names[ndx-1] );
    return stmt->pbind.names[ndx-1];
}

/**
   Internal helper for implementing multiple bindings to the same
   parameter name. This is only intended for use in the
   cpdo_my5_stmt_bind_xxx() family of functions.  If the current index
   has a named parameter, this code iterates to the right through the
   indexes and binds the same value to any other indexes which have
   that same name. FWD_FUNC(params...) must be the function to call to
   make for setting the value in subsequent columns. The parameters
   may include the symbol 'i' to mean the current index of the
   iteration (starting one to the right of the current ndx value).
*/
#define PBIND_FORWARD_SAME_NAME(FWD_FUNC)      \
    if(!rc && stmt->pbind.names && (ndx < stmt->pbind.count)){ \
        char const * name = stmt->pbind.names[ndx-1]; \
        rc = 0; \
        if( name && *name && (':'==*name) ){ \
            uint16_t i = ndx+1; \
            for( ;!rc && (i <= stmt->pbind.count); ++i ){ \
                char const * n2 = stmt->pbind.names[i-1]; \
                if( (n2 && *n2 && (':'==*n2)) \
                    && (0==strcmp(name,n2))){ \
                    rc = FWD_FUNC; \
                } \
            } \
        } \
    }




static int cpdo_my5_stmt_bind_null_2( cpdo_stmt * self, uint16_t ndx )
{
    PBIND_DECL(ndx);
    memset(mybin, 0, sizeof(MYSQL_BIND));
    mybin->buffer_type = MYSQL_TYPE_NULL;
    cpdo_bind_val_null( cbin );
    return 0;
}

static int cpdo_my5_stmt_bind_null( cpdo_stmt * self, uint16_t ndx )
{
    int rc = 0;
    PBIND_DECL(ndx);
    rc = cpdo_my5_stmt_bind_null_2( self, ndx );
    PBIND_FORWARD_SAME_NAME(cpdo_my5_stmt_bind_null_2( self, i ));
    return rc;
}

static int cpdo_my5_stmt_bind_int8_2( cpdo_stmt * self, uint16_t ndx, int8_t v )
{
    PBIND_DECL(ndx);
    memset(mybin, 0, sizeof(MYSQL_BIND));
    cpdo_bind_val_int8( cbin, v );
    mybin->buffer_type = MYSQL_TYPE_TINY;
    mybin->buffer = (char *)&cbin->valu.i8;
    mybin->is_null = 0;
    return 0;
}

static int cpdo_my5_stmt_bind_int8( cpdo_stmt * self, uint16_t ndx, int8_t v )
{
    int rc;
    PBIND_DECL(ndx);
    rc = cpdo_my5_stmt_bind_int8_2(self,ndx,v);
    PBIND_FORWARD_SAME_NAME(cpdo_my5_stmt_bind_int8_2(self,i,v));
    return rc;
}


static int cpdo_my5_stmt_bind_int16_2( cpdo_stmt * self, uint16_t ndx, int16_t v )
{
    PBIND_DECL(ndx);
    memset(mybin, 0, sizeof(MYSQL_BIND));
    cpdo_bind_val_int16( cbin, v );
    mybin->buffer_type = MYSQL_TYPE_SHORT;
    mybin->buffer = (char *)&cbin->valu.i16;
    mybin->is_null = 0;
    return 0;
}

static int cpdo_my5_stmt_bind_int16( cpdo_stmt * self, uint16_t ndx, int16_t v )
{
    int rc;
    PBIND_DECL(ndx);
    rc = cpdo_my5_stmt_bind_int16_2(self,ndx,v);
    PBIND_FORWARD_SAME_NAME(cpdo_my5_stmt_bind_int16_2(self,i,v));
    return rc;
}


static int cpdo_my5_stmt_bind_int32_2( cpdo_stmt * self, uint16_t ndx, int32_t v )
{
    PBIND_DECL(ndx);
    memset(mybin, 0, sizeof(MYSQL_BIND));
    cpdo_bind_val_int32( cbin, v );
    mybin->buffer_type = MYSQL_TYPE_LONG;
    mybin->buffer = (char *)&cbin->valu.i32;
    mybin->is_null = 0;
    return 0;
}

static int cpdo_my5_stmt_bind_int32( cpdo_stmt * self, uint16_t ndx, int32_t v )
{
    int rc;
    PBIND_DECL(ndx);
    rc = cpdo_my5_stmt_bind_int32_2(self, ndx, v);
    PBIND_FORWARD_SAME_NAME(cpdo_my5_stmt_bind_int32_2(self,i,v));
    return rc;
}
                    
static int cpdo_my5_stmt_bind_int64_2( cpdo_stmt * self, uint16_t ndx, int64_t v )
{
    PBIND_DECL(ndx);
    memset(mybin, 0, sizeof(MYSQL_BIND));
    cpdo_bind_val_int64( cbin, v );
    mybin->buffer_type = MYSQL_TYPE_LONGLONG;
    mybin->buffer = (char *)&cbin->valu.i64;
    mybin->is_null = 0;
    return 0;
}
static int cpdo_my5_stmt_bind_int64( cpdo_stmt * self, uint16_t ndx, int64_t v )
{
    int rc;
    PBIND_DECL(ndx);
    rc = cpdo_my5_stmt_bind_int64_2(self,ndx,v);
    PBIND_FORWARD_SAME_NAME(cpdo_my5_stmt_bind_int64_2(self,i,v));
    return rc;
}

static int cpdo_my5_stmt_bind_float_2( cpdo_stmt * self, uint16_t ndx, float v )
{
    PBIND_DECL(ndx);
    memset(mybin, 0, sizeof(MYSQL_BIND));
    cpdo_bind_val_float( cbin, v );
    mybin->buffer_type = MYSQL_TYPE_FLOAT;
    mybin->buffer = (char *)&cbin->valu.flt;
    mybin->is_null = 0;
    return 0;
}

static int cpdo_my5_stmt_bind_float( cpdo_stmt * self, uint16_t ndx, float v )
{
    int rc;
    PBIND_DECL(ndx);
    rc = cpdo_my5_stmt_bind_float_2(self,ndx,v);
    PBIND_FORWARD_SAME_NAME(cpdo_my5_stmt_bind_float_2(self,i,v));
    return rc;
}


static int cpdo_my5_stmt_bind_double_2( cpdo_stmt * self, uint16_t ndx, double v )
{
    PBIND_DECL(ndx);
    memset(mybin, 0, sizeof(MYSQL_BIND));
    cpdo_bind_val_double( cbin, v );
    mybin->buffer_type = MYSQL_TYPE_DOUBLE;
    mybin->buffer = (char *)&cbin->valu.dbl;
    mybin->is_null = 0;
    return 0;
}
static int cpdo_my5_stmt_bind_double( cpdo_stmt * self, uint16_t ndx, double v )
{
    int rc;
    PBIND_DECL(ndx);
    rc = cpdo_my5_stmt_bind_double_2(self,ndx,v);
    PBIND_FORWARD_SAME_NAME(cpdo_my5_stmt_bind_double_2(self,i,v));
    return rc;
}


static int cpdo_my5_stmt_bind_string_2( cpdo_stmt * self, uint16_t ndx, char const * v, uint32_t len )
{
    static char isNullYes = 1;
    static char isNullNo = 0;
    int rc;
    PBIND_DECL(ndx);
    if(!v) len = 0;
    rc = cpdo_bind_val_string( cbin, v, len );
    if( rc ) return rc;
    else
    {
        memset(mybin, 0, sizeof(MYSQL_BIND));
        mybin->buffer_type = MYSQL_TYPE_STRING;
        mybin->buffer = (char *)cbin->valu.blob.mem;
        mybin->is_null = v
            ? &isNullNo
            : &isNullYes;
        mybin->buffer_length = v ? len : 0;
        return 0;
    }
}

static int cpdo_my5_stmt_bind_string( cpdo_stmt * self, uint16_t ndx, char const * v, uint32_t len )
{
    int rc;
    PBIND_DECL(ndx);
    rc = cpdo_my5_stmt_bind_string_2(self,ndx,v,len);
    PBIND_FORWARD_SAME_NAME(cpdo_my5_stmt_bind_string_2(self,i,v,len));
    return rc;
}



static int cpdo_my5_stmt_bind_blob_2( cpdo_stmt * self, uint16_t ndx, void const * v, uint32_t len )
{
    static char isNullYes = 1;
    static char isNullNo = 0;
    int rc;
    PBIND_DECL(ndx);
    if(!v) len = 0;
    rc = cpdo_bind_val_blob( cbin, v, len );
    if( rc ) return rc;
    else
    {
        memset(mybin, 0, sizeof(MYSQL_BIND));
        mybin->buffer_type = MYSQL_TYPE_BLOB;
        mybin->buffer = (char *)cbin->valu.blob.mem;
        mybin->is_null = v
            ? &isNullNo
            : &isNullYes;
        mybin->buffer_length = v ? len : 0;
        return 0;
    }
}

static int cpdo_my5_stmt_bind_blob( cpdo_stmt * self, uint16_t ndx, void const * v, uint32_t len )
{
    int rc;
    PBIND_DECL(ndx);
    rc = cpdo_my5_stmt_bind_blob_2(self,ndx,v,len);
    PBIND_FORWARD_SAME_NAME(cpdo_my5_stmt_bind_blob_2(self,i,v,len));
    return rc;
}


static int cpdo_my5_stmt_get_type_ndx( cpdo_stmt * self, uint16_t ndx, cpdo_data_type * val )
{
    STMT_DECL(cpdo_rc.ArgError);
    if( ! val ) return cpdo_rc.ArgError;
    else if( ndx >= stmt->rbind.count ) return cpdo_rc.RangeError;
    else
    {
        cpdo_bind_val const * bv = &stmt->rbind.cBinders[ndx];
        *val = (bv->is_null) ? CPDO_TYPE_NULL : bv->type;
        return 0;
    }
}

/**
   Get-by-index impl for the int8..int64 number types.
*/
static int cpdo_my5_stmt_get_int_types( cpdo_my5_stmt * stmt,
                                        uint16_t ndx,
                                        int64_t * val )
{
    assert( stmt && (ndx<stmt->rbind.count));
    if( ! val ) return cpdo_rc.ArgError;
    else if( ndx >= stmt->rbind.count ) return cpdo_rc.RangeError;
    else
    {
        cpdo_bind_val * bv = &stmt->rbind.cBinders[ndx];
        if( bv->is_null )
        {
            *val = 0;
            return 0;
        }
        switch( bv->type )
        {
          case CPDO_TYPE_INT8:
              *val = bv->valu.i8;
              break;
          case CPDO_TYPE_INT16:
              *val = bv->valu.i16;
              break;
          case CPDO_TYPE_INT32:
              *val = bv->valu.i32;
              break;
          case CPDO_TYPE_INT64:
              *val = bv->valu.i64;
              break;
          case CPDO_TYPE_FLOAT:
              *val = (int64_t)bv->valu.flt;
              break;
          case CPDO_TYPE_DOUBLE:
              *val = (int64_t)bv->valu.dbl;
              break;
          case CPDO_TYPE_NULL:
              *val = 0;
              break;
          default:
              return cpdo_rc.TypeError;
        }
        return 0;
    }
}



static int cpdo_my5_stmt_get_int8_ndx( cpdo_stmt * self, uint16_t ndx, int8_t * val )
{
    int64_t x;
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    rc = cpdo_my5_stmt_get_int_types( stmt, ndx, &x );
    if( 0 == rc )
    {
        *val = (int8_t)x;
    }
    return rc;
}

static int cpdo_my5_stmt_get_int16_ndx( cpdo_stmt * self, uint16_t ndx, int16_t * val )
{
    int64_t x;
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    rc = cpdo_my5_stmt_get_int_types( stmt, ndx, &x );
    if( 0 == rc )
    {
        *val = (int16_t)x;
    }
    return rc;
}

static int cpdo_my5_stmt_get_int32_ndx( cpdo_stmt * self, uint16_t ndx, int32_t * val )
{
    int64_t x;
    int rc;
    STMT_DECL(cpdo_rc.ArgError);
    rc = cpdo_my5_stmt_get_int_types( stmt, ndx, &x );
    if( 0 == rc )
    {
        *val = (int32_t)x;
    }
    return rc;
}

static int cpdo_my5_stmt_get_int64_ndx( cpdo_stmt * self, uint16_t ndx, int64_t * val )
{
    STMT_DECL(cpdo_rc.ArgError);
    return cpdo_my5_stmt_get_int_types( stmt, ndx, val );
}

static int cpdo_my5_stmt_get_double_ndx( cpdo_stmt * self, uint16_t ndx, double * val )
{
    STMT_DECL(cpdo_rc.ArgError);
    if( ! val ) return cpdo_rc.ArgError;
    else if( ndx >= stmt->rbind.count ) return cpdo_rc.RangeError;
    else
    {
        cpdo_bind_val * bv = &stmt->rbind.cBinders[ndx];
        if( bv->is_null || (bv->type==CPDO_TYPE_NULL) )
        {
            *val = 0.0;
            return 0;
        }
        switch( bv->type )
        {
          case CPDO_TYPE_INT8:
          case CPDO_TYPE_INT16:
          case CPDO_TYPE_INT32:
          case CPDO_TYPE_INT64: {
              int64_t i64 = 0;
              int rc = cpdo_my5_stmt_get_int_types( stmt, ndx, &i64 );
              if( rc ) return rc;
              *val = (double)i64;
              break;
          }
          case CPDO_TYPE_FLOAT:
              *val = bv->valu.flt;
              break;
          case CPDO_TYPE_DOUBLE:
              *val = bv->valu.dbl;
              break;
          case CPDO_TYPE_NULL:
              *val = 0.0;
              break;
          default:
              return cpdo_rc.TypeError;
        }
        return 0;
    }
}

static int cpdo_my5_stmt_get_float_ndx( cpdo_stmt * self, uint16_t ndx, float * val )
{
    double d = 0.0;
    int rc = cpdo_my5_stmt_get_double_ndx( self, ndx, &d );
    if( (0 == rc) && val ) *val = (float)d;
    return rc;
}

#if !CPDO_MY5_HAS_PRINT64
typedef struct {
    char * str;
    long maxLen;
    long pos;
} My5StringAppender;

static long cpdo_my5_appender_string( void * arg, char const * data, long n )
{
    long rc = -1;
    My5StringAppender * my = (My5StringAppender*) arg;
    if( ! arg || ((my->pos+n) > my->maxLen) || (n<0) ) return rc;
    for( rc = 0; rc < n; ++data, ++rc )
    {
        my->str[my->pos++] = *data;
    }
    return rc;
}
#endif /*!CPDO_MY5_HAS_PRINT64*/

static int cpdo_my5_stmt_get_string_ndx( cpdo_stmt * self, uint16_t ndx, char const ** val, uint32_t * len )
{
    STMT_DECL(cpdo_rc.ArgError);
    if( ! val ) return cpdo_rc.ArgError;
    else if( ndx >= stmt->rbind.count ) return cpdo_rc.RangeError;
    else
    {
        enum { NumBufSize = 80 };
        int rc;
        cpdo_bind_val * scratch = NULL;
        cpdo_bind_val * bv = &stmt->rbind.cBinders[ndx];
        if( bv->is_null || (bv->type==CPDO_TYPE_NULL) )
        {
            *val = NULL;
            if( len ) *len = 0;
            return 0;
        }
        else if( (CPDO_TYPE_STRING == bv->type)
                 || (stmt->driver->blobsAllowGetString && (CPDO_TYPE_BLOB == bv->type))
                 ) {
            /* shortcut - no need to convert this */
            *val = (char const *)bv->valu.blob.mem;
            if( len ) *len = bv->valu.blob.length;
            return 0;
        }
        else if(!stmt->rbind.convBuf)
        {
            stmt->rbind.convBuf = cpdo_bind_val_list_new(stmt->rbind.count);
            if( ! stmt->rbind.convBuf ) return cpdo_rc.AllocError;
        }
        else {
            /* check for a cached value */
            scratch = &stmt->rbind.convBuf[ndx];
            if( CPDO_TYPE_STRING == scratch->type )
            {/* already done the conversion, so re-use it.. */
                *val = (char const *)scratch->valu.blob.mem;
                if(len) *len = scratch->valu.blob.length;
                return 0;
            }
        }

        switch( bv->type ){
          case CPDO_TYPE_STRING: { /* shortcut - no need to convert this */
              *val = (char const *)bv->valu.blob.mem;
              if( len ) *len = bv->valu.blob.length;
            return 0;
          }
#if 1
          case CPDO_TYPE_BLOB:
              if(stmt->driver->blobsAllowGetString) { /* Arguable: optimistically assume blob is a legal string */
                  *val = (char const *)bv->valu.blob.mem;
                  if( len ) *len = bv->valu.blob.length;
                  return 0;
              }
              else goto unhandled_type;
#endif
          case CPDO_TYPE_INT8:
          case CPDO_TYPE_INT16:
          case CPDO_TYPE_INT32:
          case CPDO_TYPE_INT64:
          case CPDO_TYPE_DOUBLE:
          case CPDO_TYPE_FLOAT: { /* convert numbers to strings... */
              cpdo_bind_val * scratch = &stmt->rbind.convBuf[ndx];
              int sprc;
              char buf[NumBufSize];
              assert( (CPDO_TYPE_STRING != scratch->type) && "This should have been caught by an earlier block.");
              switch( bv->type )
              {
                case CPDO_TYPE_INT8:
                    sprc = sprintf( buf, "%"PRIi8, bv->valu.i8 );
                    break;
                case CPDO_TYPE_INT16:
                    sprc = sprintf( buf, "%"PRIi16, bv->valu.i16 );
                    break;
                case CPDO_TYPE_INT32:
                    sprc = sprintf( buf, "%"PRIi32, bv->valu.i32 );
                    break;
                case CPDO_TYPE_INT64:
#if CPDO_MY5_HAS_PRINT64
                    
                    sprc = sprintf( buf, "%"PRIi64, bv->valu.i64 );
#else
                    /*
                      We  use cpdo_printf() to implement this on 32-bit platforms
                      because the PRIi64 specifier can map to a value which is not
                      specified in C89.
                    */
                    {
                        enum { I64BufLen = 80 };
                        char buf[I64BufLen];
                        My5StringAppender myStr;
                        myStr.str = buf;
                        myStr.maxLen = I64BufLen;
                        myStr.pos = 0;
                        sprc = cpdo_printf( cpdo_my5_appender_string, &myStr, "%"PRIi64, bv->valu.i64 );
                    }
#endif
                    break;
                case CPDO_TYPE_DOUBLE:
                    sprc = sprintf( buf, "%f", bv->valu.dbl );
                  break;
                case CPDO_TYPE_FLOAT:
                    sprc = sprintf( buf, "%f", bv->valu.flt );
                  break;
                default:
                    assert(0 && "Unhandled statement column data type in switch().");
                    return cpdo_rc.TypeError;
              };
              if(sprc>0)
              { /* Strip trailing zeroes before passing it on... */
                  unsigned int urc = (unsigned int)sprc;
                  char * pos = buf + urc - 1;
                  for( ; ('0' == *pos) && urc && (*(pos-1) != '.')&& (*(pos-1) != ',');
                       --pos, --urc )
                  {
                      *pos = 0;
                  }
                  assert(urc && *pos);
              }
              rc = cpdo_bind_val_string( scratch, buf, strlen(buf) );
              if( rc ) return rc;
              *val = (char const *)scratch->valu.blob.mem;
              if( len ) *len = scratch->valu.blob.length;
              return 0;
          }
          case CPDO_TYPE_CUSTOM:{ /* convert date/time fields to strings... */
              enum { BufSize = 20 /* for time/date-to-string (timestamp sz=19)*/ };
              MYSQL_BIND * bin = &stmt->rbind.myBinders[ndx];
              if(! cpdo_bind_val_tag_type_check_origin(&cpdo_my5_driver_api,
                                                       bv->valu.custom.type_tag ))
              {
                  return cpdo_rc.TypeError;
              }
              switch( bin->buffer_type )
              {
                case MYSQL_TYPE_DATE:
                case MYSQL_TYPE_TIME:
                case MYSQL_TYPE_DATETIME:
                case MYSQL_TYPE_TIMESTAMP: {
                    MYSQL_TIME * tm;
                    scratch = &stmt->rbind.convBuf[ndx];
                    assert( (CPDO_TYPE_STRING != scratch->type) && "This should have been caught by an earlier block.");
                    rc = cpdo_bind_val_string( scratch, NULL, BufSize );
                    if( rc ) return rc;
                    tm = (MYSQL_TIME*)bv->valu.custom.mem;
                    assert( tm );
                    if( ! tm ) return cpdo_rc.InternalError;
                    if( MYSQL_TYPE_DATE == bin->buffer_type )
                    {
                        sprintf( (char *)scratch->valu.blob.mem,
                                 "%04d-%02d-%02d",
                                 tm->year, tm->month, tm->day);
                    }
                    else if( MYSQL_TYPE_TIME == bin->buffer_type )
                    {
                        sprintf( (char *)scratch->valu.blob.mem,
                                 "%02d:%02d:%02d",
                                 tm->hour, tm->minute, tm->second);
                    }
                    else
                    {
                        assert( (MYSQL_TYPE_TIMESTAMP == bin->buffer_type)
                                || (MYSQL_TYPE_DATETIME == bin->buffer_type)
                                );
                        sprintf( (char *)scratch->valu.blob.mem,
                                 "%04d-%02d-%02d %02d:%02d:%02d",
                                 tm->year, tm->month, tm->day,
                                 tm->hour, tm->minute, tm->second);
                    }
                    scratch->valu.blob.length = strlen( (char *)scratch->valu.blob.mem );
                    *val = (char const *)scratch->valu.blob.mem;
                    if(len) *len = scratch->valu.blob.length;
                    return 0;                  
                }
                default:
                    break;
              }
              return cpdo_rc.TypeError;
          }
          default:
          unhandled_type:
              MARKER("WARNING: UNHANDLED MYSQL_TYPE_XXX #%d IN RESULT DATA\n",
                     stmt->rbind.myBinders[ndx].buffer_type);
              return cpdo_rc.TypeError;
        }
    }
}

static int cpdo_my5_stmt_get_blob_ndx( cpdo_stmt * self, uint16_t ndx, void const ** val, uint32_t * len )
{
    STMT_DECL(cpdo_rc.ArgError);
    if( ! val ) return cpdo_rc.ArgError;
    else if( ndx >= stmt->rbind.count ) return cpdo_rc.RangeError;
    else
    {
        static char zeroLenVal = {0};
        /*MYSQL_BIND * bin = &stmt->rbind.myBinders[ndx];*/
        cpdo_bind_val * bv = &stmt->rbind.cBinders[ndx];
        if( bv->is_null )
        {
            *val = NULL;
            if(len) *len = 0;
            return 0;
        }
        else if( !bv->valu.blob.length ){
            *val = &zeroLenVal;
            if(len) *len = 0;
            return 0;
        }
        else switch( bv->type ) {
          case CPDO_TYPE_BLOB:
              *val = (void const *)bv->valu.blob.mem;
              if(len) *len = bv->valu.blob.length;
              break;
          case CPDO_TYPE_CUSTOM:
              *val = (void const *)bv->valu.custom.mem;
              if(len) *len = bv->valu.custom.length;
              break;
          case CPDO_TYPE_NULL:
              assert(0 && "Should have been handled above,");
              *val = 0;
              if(len) *len = 0;
              break;
          default:
              return cpdo_rc.TypeError;
        }
        return 0;
    }
}

static int cpdo_my5_stmt_error_info( cpdo_stmt * self, char const ** dest, uint32_t * len, int * errorCode )
{
    STMT_DECL(cpdo_rc.ArgError);
    else
    {
        if( errorCode ) *errorCode = mysql_stmt_errno( stmt->stmt );
        if( dest )
        {
            *dest = mysql_stmt_error( stmt->stmt );
            if( len )
            {
                *len = *dest ? strlen(*dest) : 0;
            }
        }
        return 0;
    }
}

static int cpdo_my5_stmt_finalize( cpdo_stmt * self )
{
    STMT_DECL(cpdo_rc.ArgError);
    if(MEGADEBUG)
    {
        MARKER("Finalizing statement @%p\n", (void const *)self);
    }
    return cpdo_my5_stmt_free(stmt);
}

static cpdo_driver_details const * cpdo_my5_driver_details()
{
    static const cpdo_driver_details bob = {
    CPDO_DRIVER_NAME/*driver_name*/,
    "20130507"/*driver_version*/,
    "Same as your MySQL"/*license*/,
    "http://fossil.wanderinghorse.net/repos/cpdo/" /*url*/,
    "Stephan Beal (http://wanderinghorse.net)" /*authors*/
    };
    return &bob;
}

int cpdo_driver_mysql5_register()
{
    return cpdo_driver_register( CPDO_DRIVER_NAME, cpdo_my5_driver_new );
}


#if defined(__cplusplus)
namespace {
    struct RegPlaceholder{
        int registered;
        RegPlaceholder(int v):registered(v){}
    };
    const RegPlaceholder regPlaceholder_my5 = RegPlaceholder(cpdo_driver_mysql5_register());
}
} /*extern "C"*/
#endif

#undef DRV_DECL
#undef STMT_DECL
#undef PBIND_DECL
#undef MARKER
#undef CPDO_DRIVER_NAME
#undef CPDO_MY5_HAS_PRINT64
#undef MEGADEBUG
#undef RESET_DRV_ERR
#undef TRY_SHARED_STRINGS
#undef PBIND_FORWARD_SAME_NAME
#endif
/*CPDO_ENABLE_MYSQL5*/
/* end file cpdo_amalgamation.c */
/* end file cson_amalgamation_session.c */
/* begin file cson_amalgamation_cgi.c */
/* auto-generated! Do not edit! */
/* begin file cgi/whuuid.h */
#if !defined(WANDERGINHORSE_NET_WHUUID_H_INCLUDED)
#define WANDERGINHORSE_NET_WHUUID_H_INCLUDED 1
#include <stdio.h> /* only for decl of FILE. */
/************************************************************************
An experiment in creating random UUIDs (http://wikipedia.org/wiki/Uuid).


Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

License: Public Domain


Features:

- Small API. Only two relevant classes and a handful of functions.

- Uses a client-specified RNG source. Two are provided with the
library. The RNG source may be arbitrarily stateful, and each may have
instance-specific data.

- State objects have a uniform cleanup interface, but each implementation
defines which cleanup behaviours need to be performed (e.g. closing
an input file).

- Fairly fast, assuming your RNG is. (My 2.6GHz PC can generate, and send
them to stdout, just over 1.3 million UUIDs per second.)


Misfeatures:

- Does not support a specific version of UUID, as detailed at
[http://wikipedia.org/wiki/Uuid]. Its UUIDs have random data in all
positions, as opposed to reserving certain positions for specific
values or using specified algorithms to generate the values. Thus the
UUIDs it generates are similar to Version 4 UUIDs except that no bytes
are reserved for specific values.

PS: i don't really consider that to be a mis-feature. IMHO UUIDs
should be completely random, with no reserved bytes.


------------------------------------------------------------------------
TIP: checking for duplicate UUIDs

The sqlite3 tool can be used for checking for duplicate UUIDs. Simply
print the UUIDs, one per line, and feed them into sqlite3 like so:

@code
sqlite3> create table ids (id,unide(id));
sqlite3> .import myUUIDListFile ids
@endcode

If sqlite3 does not complain, there were no duplicates.

You can also test by sorting the list, removing duplicates, and
checking the length of the list. e.g. assume we have a file named "1m"
containing 1 million UUIDs. From a Unix shell we could do:

@code
~> sort -u < 1m > 1ms
~> ls -la 1m 1ms
@endcode

If the files have the same size then there were no duplicates.

In my tests i have not encountered duplicates except when testing
a deterministic RNG with a specific seed.
************************************************************************/

/** @def WHUUID_CONFIG_KEEP_METRICS

    If WHUUID_CONFIG_KEEP_METRICS is a true value then the library keeps track
    of how many times a given hex digit value is generated by the
    whuuid_rng class. It has a minimal performance hit, but if
    the data will never be used then it can be turned off.
*/
#define WHUUID_CONFIG_KEEP_METRICS 1

/** @enum whuuid_constants

A list of constant values used by the whuuid API.

*/
enum whuuid_constants {
/**
   The length of a UUID canonical-form string, not including
   a trailing NULL bytes. e.g.:

   00000000-0000-0000-0000-000000000000
*/
whuuid_length_canonical = 36,
/**
   The length of a UUID in canonical form, including
   a trailing NULL byte.
*/
whuuid_length_cstring = whuuid_length_canonical + 1,
/**
   The number of bytes of data necessary to store
   a UUID in "raw" form.
*/
whuuid_length_bytes = 16
};

/**
   Represents a single UUID.
*/
struct whuuid_t
{
    unsigned char bytes[whuuid_length_bytes];
};
typedef struct whuuid_t whuuid_t;
/**
   A zero-initialized whuiid_t initialization object.
*/
extern const whuuid_t whuuid_t_empty;

/**
   A class holding RNG information. Each instance manages a single RNG
   source, which is used to populate any number of whuiid_t objects
   with random data. They may or may not need to dynamically allocate
   resources (e.g. open a file containing random data), depending
   on the implementation.   

   They should normally be initialized via factory functions, and
   those functions should:

   a) Allocate any private resources the object needs and store them in
   self->impl.

   b) Set the cleanup() member function to a function which knows how
   to clean up any resources stored in self->impl.

   c) Set the rand() member to a function which knows how to use
   the private state to generate random data.


   The most basic usage looks something like this:

   @code
   whuuid_rng st = whuuid_rng_lcrng; // a Linear Congruent RNG
   whuuid_t u = whuuid_t_empty;
   char buffer[whuuid_length_canonical+1]; // buffer for UUID string
   buffer[whuuid_length_canonical] = 0; // add trailing NULL
   for( int i =0; i < 100; ++i )
   {// generate 100 UUIDs to print them
       whuuid_fill_rand( &u, &st ); // generate UUID using st->rand()
       whuuid_to_string( &u, buffer );
       puts(buffer);
   }
   st.cleanup(&st); // see below.
   @endcode

   In that particular case the state object has no state which
   needs cleaning, but we could also set up a FILE as an input source,
   in which case we need to clean up the object:

   @code
   st = whuuid_rng_FILE;
   st.impl = fopen("/dev/urandom", "r");
   ... use st ...
   st.cleanup(&st); // will fclose() the file
   @endcode

   If a state object is dynamically allocated then it should be freed
   after calling its cleanup() member to free any
   implementation-specific resources.
*/
struct whuuid_rng
{
    /**
       Must set *tgt to sizeof(unsigned int) random bytes. Must return
       0 on success or non-zero if something goes wrong (e.g. the
       input source has failed or run out of numbers). How it uses (or
       ignores) the self argument is implementation-specific.
    */
    int (*rand)( struct whuuid_rng * self, unsigned int  * tgt );
    /**
       Must clean up self, but not free self itself. How it does this
       is implementation-specific. If it has no private state,
       this function may be NULL.

       whuuid_rng objects can be allocated on the stack or via
       arbitrary mechanisms, so the cleanup routine must not free the
       self object. How it is freed (after it is cleaned up) depends
       on how it was allocated.
    */
    void (*cleanup)( struct whuuid_rng * self );
    /**
       Implementations may store any private state here. This member is
       not for public use.
    */
    void * impl;
    /**
       Stores the distribution of values created by this state
       object. whuuid_fill_rand() updates these values.
    */
    unsigned long distribution[whuuid_length_bytes];
};


/** Convenience typedef. */
typedef struct whuuid_rng whuuid_rng;

/**
   A zero-initialized whuiid_state initialization object.
*/
extern const whuuid_rng whuuid_rng_empty;

/**
   An almost-empty whuiid_state initialization object with
   its rand() member set to whuuid_lc_rand.
*/
extern const whuuid_rng whuuid_rng_lcrng;

/**
   A whuuid_state initialization object with its rand() member set to
   whuuid_FILE_rand and its cleanup() member set to
   whuuid_FILE_cleanup.  Clients may copy this then set the impl
   member to point it to an opened FILE handle. The FILE handle will
   be closed when the cleanup() member is called. If the state object
   should not close the file when it cleans up, set the cleanup()
   member to NULL.
*/
extern const whuuid_rng whuuid_rng_FILE;

/**
   Implements the whuuid_rng::rand() interface.

   This implementaion uses/abuses st->impl to store a numeric state
   value for a linear congruent RNG. If st->impl is NULL then a seed
   value is generated using some external source (we malloc() a few
   bytes to get a random address, and we use that address as a
   seed). The state value is stored directly in st->impl and does not
   need to be cleaned up. (The memory malloc()ed to get the initial
   seed is free()d immediately after it is malloc()ed.)

   Returns 0 on success, non-zero on error. The only error conditions
   are !st or !tgt. A malloc() error on the initial seeding will not
   cause an error (but causes a determinate (but unspecified) seed
   value to be used).

   In my (informal/unscientific) tests, this RNG works very well for
   generating UUIDs, out-performing /dev/urandom in terms of even
   numeric distribution most of the time.
*/
int whuuid_lc_rand( whuuid_rng * st, unsigned int *tgt );

/**
   Implements the whuuid_rng::rand() interface.

   If st->impl is not NULL it is assumed to be-a (FILE*) and
   sizeof(unsigned int) bytes are read from it and returned via the
   tgt argument.

   Returns non-zero if !st or !st->impl, or if reading from the file
   fails.

   Results are undefined if st->impl is non-null but is-not-a FILE.

   Note that this implementation does nothing fancy like buffering
   some larger amount of random input. Each call reads sizeof(int)
   bytes. If performance is of a concern, create an implementation
   which stores a struct containing the FILE and the buffer somewhere
   in st->impl and reads the input in larger blocks. Also implement a
   cleanup function which can free the buffer.

   @see whuuid_FILE_cleanup()
   @see whuuid_rng_FILE
*/
int whuuid_FILE_rand( whuuid_rng * st, unsigned int * tgt );

/**
   Implements the whuuid_rng::cleanup() interface for state
   objects where obj->impl is-a FILE handle opened via
   fopen() (or equivalent).
   
   Assumes self->impl is-a (FILE*) and calls fclose() on it.
*/
void whuuid_FILE_cleanup( whuuid_rng * self );

/**
   Converts src->bytes to a canonical-form UUID string.  dest must be
   valid memory at least whuuid_length_canonical bytes long, and on
   success exactly whuuid_length_canonical bytes will be written to it.
   No terminating null is added.

   Returns 0 on success, non-zero on error. The only error conditions
   are (!src) or (!dest).
*/
int whuuid_to_string( whuuid_t const * src, char * dest );

/**
   Populates all of dest->bytes, using st->rand() to collect the
   random bytes. It calls st->rand() enough times to collect
   whuuid_length_bytes bytes.

   Returns 0 on success, non-0 on error. The error conditions are:

   - !st or !dest

   - st->rand() returns non-0, in which case that error code is passed
   back to the caller.

   st->distribution is modified by this function to record the number
   of times any given digit (hex 0..f) is generated via a call to
   rand() (but note that each call to st->rand() is used to generate
   (sizeof(unsigning int)*2) digits).

   This routine does not guaranty that the bytes returned by
   st->rand() are used in the exact same order as they are returned.
*/
int whuuid_fill_rand( whuuid_t * dest, whuuid_rng * st );

/**
   Copies whuuid_length_bytes bytes from src to dest->bytes.

   Returns 0 on success. The only error cases are !dest or !src.
*/
int whuuid_fill( whuuid_t * dest, unsigned char const * src );


/**
   Compares lhs->bytes and rhs->bytes and
   returns 0, less than 0, or greater than 0 depending on whether
   lhs equals, is less than, or is greater to rhs, respectively.
   i.e. it behaves like memcmp(3).

   A NULL value for lhs or rhs compares as less-than any other value
   except NULL, to which it compares equal.
*/
short whuuid_compare( whuuid_t const * lhs, whuuid_t const * rhs );

/**
   Debugging/testing function which dumps the RNG distribution counts
   of st to the given FILE handle. The stats are updated on each call
   to whuuid_fill_rand() IF the WHUUID_CONFIG_KEEP_METRICS macro is
   set to a true value when the library is built.

   If full is non-zero then a full list of metrics is dumped,
   otherwise just an overview.

   Returns 0 on success, non-zero on error (!dest, !st, or
   WHUUID_CONFIG_KEEP_METRICS is false).
*/
int whuuid_dump_distribution( whuuid_rng const * st, short full, FILE * dest );

#endif /* WANDERGINHORSE_NET_WHUUID_H_INCLUDED */
/* end file cgi/whuuid.h */
/* begin file cgi/whuuid.c */
#include <assert.h>
#include <string.h> /* memset() */

#include <stdlib.h> /* malloc(), free() */


#if WHUUID_CONFIG_KEEP_METRICS
#  include <stdio.h> /* fprintf(), FILE */
#endif

const whuuid_t whuuid_t_empty = {
{0,0,0,0,
 0,0,0,0,
 0,0,0,0,
 0,0,0,0}/*bytes*/
};


static void whuuid_noop_cleanup( whuuid_rng * self )
{
    /* does nothing */
}
/**
   An almost-empty-initialized whuuid_rng object which uses
   whuuid_rand_uuint() as its random data source. It has no resources
   associated with it.
*/
const whuuid_rng whuuid_rng_empty = {
NULL/*rand*/,
whuuid_noop_cleanup/*cleanup*/,
NULL/*impl*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}/*distribution*/
};

const whuuid_rng whuuid_rng_lcrng = {
whuuid_lc_rand/*rand*/,
whuuid_noop_cleanup/*cleanup*/,
NULL/*impl*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}/*distribution*/
};

const whuuid_rng whuuid_rng_FILE = {
whuuid_FILE_rand/*rand*/,
whuuid_FILE_cleanup/*cleanup*/,
NULL/*impl*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}/*distribution*/
};

/** BITS2CHAR(X) expects (X<=15). Returns the hex-code char for it
    ('0'..'f'), or 0 if X is out of range. */
#define BITS2CHAR(X) ( ((X)<=0x09) ? ('0'+(X)) : (((X)<=0xF) ? ('a'+((X)-10)) : 0))


void whuuid_FILE_cleanup( whuuid_rng * self )
{
    if( self && self->impl )
    {
        fclose( (FILE*)self->impl );
        self->impl = 0;
    }
}

int whuuid_FILE_rand( whuuid_rng * st, unsigned int * tgt )
{
    if( st && st->impl )
    {
        unsigned int d = 0;
        if( 1 != fread( &d, sizeof(d), 1, (FILE*)st->impl ) )
        {
            return -1;
        }
        *tgt = d;
        return 0;
    }
    return -1;
}

#include <time.h>
int whuuid_lc_rand( whuuid_rng * st, unsigned int  * tgt )
{
    typedef unsigned long NumType;
    NumType num = (NumType)st->impl;
    if( ! st || ! tgt ) return -1;
#define RNG(SEED) (NumType)( (NumType)((NumType)(SEED) * (NumType)1103515245) + 12345)
    /* ^^^^^ This RNG Works very well for this use case (comparable
       with /dev/urandom on my box). Algo found in Angband sources. */
    if( ! num )
    {
        void * x;
        num = (NumType) st;
        /* Generate a unique seed. */
        x = malloc( (num % 13)+9 );
        free(x);
        num = (NumType)(RNG(x) ^ num) >> 6
            /*
              The bitshift part is to work around the problem that the
              left-most byte of generated UUIDs always have the same
              starting sequences.
             */
            ;
    }
    else
    {
        num = RNG(num);
    }
#undef RNG
    st->impl = (void *)num;
    *tgt = num;
    return 0;
}

int whuuid_to_string( whuuid_t const * src, char * dest )
{
    unsigned int i = 0;
    int part = 1;
    int span = 0;
    char byte = 0;
    char nibble = 0;
    if( ! src || ! dest ) return -1;
    for( i = 0; i < whuuid_length_bytes; )
    {
        int x;
        if( 1 == part ) span = 8;
        else if( (part>1) && (part<5) ) span = 4;
        else if( 5 == part ) span = 12;
        for( x = 0; x < (span/2); ++x )
        {
            byte = src->bytes[i++];
            nibble = (byte >> 4) & 0x0F;
            *(dest++) = BITS2CHAR(nibble);
            nibble = (byte & 0x0F);
            *(dest++) = BITS2CHAR(nibble);
        }
        if( part < 5 )
        {
            *(dest++) = '-';
            ++part;
        }
        else break;
    }
    return 0;
}

int whuuid_fill( whuuid_t * dest, unsigned char const * src )
{
    if( ! dest || ! src ) return -1;
    else
    {
        memcpy( dest, src, whuuid_length_bytes );
        return 0;
    }
}

int whuuid_fill_rand( whuuid_t * dest, whuuid_rng * st )
{
    unsigned int i = 0, x = 0;
    unsigned char * c = 0;
    unsigned int r;
    unsigned char nibble;
    int rc = 0;
    if( ! st || ! dest ) return -1;
    if( ! dest ) return -1;
    for( ; i < whuuid_length_bytes; )
    {
        rc = st->rand(st, &r);
        if( rc ) break;
        c = (unsigned char *)&r;
        for( x = sizeof(r); (x > 0) && (i < whuuid_length_bytes); --x, ++i, ++c )
        {
            dest->bytes[i] = *c;
#if WHUUID_CONFIG_KEEP_METRICS
            nibble = (*c >> 4) & 0x0F;
            ++st->distribution[nibble];
            nibble = (*c & 0x0F);
            ++st->distribution[nibble];
#endif
        }
    }
    return rc;
}

short whuuid_compare( whuuid_t const * lhs, whuuid_t const * rhs )
{
    if( ! lhs ) return rhs ? -1 : 0;
    else if( ! rhs ) return 1;
    else if( lhs == rhs ) return 0;
    else
    {
#if 0
        unsigned int i = 0;
        unsigned char const * l = lhs->bytes;
        unsigned char const * r = rhs->bytes;
        unsigned char bl = 0, br = 0; /* current byte of lhs/rhs*/
        unsigned char nl = 0, nr = 0;/* 4-bit part of bl/br*/
        for( ; i < whuuid_length_bytes; ++i )
        {
            bl = l[i];
            br = r[i];
            nl = (bl >> 4);
            nr = (br >> 4);
            if( nl < nr ) return -1;
            else if( nl > nr ) return 1;
            nl = (bl & 0x0F);
            nr = (br & 0x0F);
            if( nl < nr ) return -1;
            else if( nl > nr ) return 1;
        }
        return 0;
#else
        return memcmp( lhs->bytes, rhs->bytes, whuuid_length_bytes );
#endif
    }
}

int whuuid_dump_distribution( whuuid_rng const * st, short full, FILE * dest )
{
#if ! WHUUID_CONFIG_KEEP_METRICS
    fprintf("WHUUID_CONFIG_KEEP_METRICS is false, so whuuid_dump_distribution() cannot work!\n");
    return -1;
#else
    unsigned short i = 0;
    double total = 0;
    unsigned long int max = 0, min = st->distribution[0];
    unsigned long int x = 0;
    char minL = 0, maxL = 0;
    if( full )
    {
        fprintf(dest,"Random number distribution:\nDigit:\tCount:\n");
    }
    for( ; i < 16; ++i )
    {
        x = st->distribution[i];
        total += x;
        if( max < x )
        {
            max = x;
            maxL = BITS2CHAR(i);
        }
        else if( min >= x )
        {
            min = x;
            minL = BITS2CHAR(i);
        }
    }
    if( full )
    {
        for( i = 0; i < 16; ++i )
        {
            x = st->distribution[i];
            fprintf(dest,"%c\t%lu (%0.6f%%)\n",
                    BITS2CHAR(i),
                    x, (x/ total) *100 );
        }
    }
    fprintf(dest,"Least Hits: '%c' (%lu)\nMost Hits: '%c' (%lu)\n",
           minL, min, maxL, max );
    if( max == min )
    {
        fprintf(dest,"Least==Most == best possible random distribution!\n" );
    }
    else
    {
        fprintf(dest,"Max-Min diff = %lu (%0.4f%% of Max)\n", max - min, ((max - min)/(double)max)*100 );
    }
    fprintf(dest,"Total random 4-bit UUID digits: %0.0f\n\n",total);
    return 0;
#endif
}

#undef BITS2CHAR
/* end file cgi/whuuid.c */
/* begin file cgi/cson_cgi.c */
#include <assert.h>
#include <stdlib.h> /* environ, getenv(), atexit() */
#include <ctype.h> /* isspace() */
#include <string.h> /* strlen() */
#include <stdarg.h>
#include <time.h>
#include <locale.h> /* setlocale(), needed for JSON parser. */

#if CSON_ENABLE_UNIX
#  define CSON_CGI_USE_SIGNALS 1
#else
#  define CSON_CGI_USE_SIGNALS 0
#endif

/* If RNG_FILENAME evaluates to true then we use that file for getting
   random bytes for session IDs. FIXME: we effectively leak a file
   handle if this is enabled.
*/
#if 0
#  define RNG_FILENAME "/dev/urandom"
#else
#  define RNG_FILENAME NULL
#endif


#if 1
#define MARKER if(1) printf("MARKER: %s:%d:%s():\t",__FILE__,__LINE__,__func__); if(1) printf
#else
static void noop_printf(char const * fmt, ...) {}
#define MARKER if(0) printf
#endif

#if CSON_CGI_USE_SIGNALS
#  include <signal.h> /* signal() */
#endif

const cson_cgi_init_opt cson_cgi_init_opt_empty = cson_cgi_init_opt_empty_m;

/**
   Some cson_cgi-internal value keys.
*/
static const struct {
    char const * ENV_GET;
    char const * ENV_POST;
    char const * ENV_COOKIE;
    char const * ENV_SYS;
    char const * ENV_APP;
    char const * ENV_ARGV;
    char const * ENV_CONFIG;
    char const * ENV_SESSION;
    char const * RESPONSE_HEADERS;
} cson_cgi_keys = {
"$GET",
"$POST",
"$COOKIE",
"$ENV",
"$APP",
"$ARGV",
"$CONFIG",
"$SESSION",
"response.headers"
};


/**
   Shared state used by the cson_cgi API.
*/
const cson_cgi_cx cson_cgi_cx_empty = cson_cgi_cx_empty_m;

static int cson_cgi_printf(cson_cgi_cx * cx, char const * fmt, ... )
{
    if( ! fmt ) return 0;
    else
    {
        int rc;
        va_list vargs;
        assert( NULL != cx->opt.outStream );
        va_start( vargs, fmt );
        rc = vfprintf( cx->opt.outStream, fmt, vargs );
        /*if( rc > 0 ) fflush( cx->opt.outStream );*/
        va_end( vargs );
        return rc;
    }
}

static int cson_cgi_puts(cson_cgi_cx * cx, char const * str)
{
    size_t const slen = str ? strlen(str) : 0;
    if( slen )
    {
        if( 1 != fwrite( str, slen, 1, cx->opt.outStream ) )
        {
            return -1;
        }
    }
    if( 1 != fwrite( "\n", 1, 1, cx->opt.outStream ) )
    {
        return -2;
    }
    return (int) (slen + 1);
}

static int cson_cgi_putchar(cson_cgi_cx * cx, char ch)
{
    return ( 1 == fwrite( &ch, 1, 1, cx->opt.outStream ) )
        ? 1
        : -1;
}


cson_value * cson_cgi_argv(cson_cgi_cx *cx)
{
    return cx ? cx->argv.jval : NULL;
}

cson_array * cson_cgi_argv_array(cson_cgi_cx * cx)
{
    return cx ? cson_value_get_array( cx->argv.jval ) : NULL;
}

int cson_cgi_gc_add( cson_cgi_cx * cx, char const * key, cson_value * v, char freeOnError )
{
    int const rc = cson_object_set( cx->gc.jobj, key, v );
    if( (0 != rc) && freeOnError )
    {
        cson_value_free( v );
    }
    return rc;
}

int cson_cgi_response_root_set( cson_cgi_cx * cx, cson_value * v )
{
    if( ! cx ) return cson_rc.ArgError;
    else if( v && !cson_value_is_object(v) && !cson_value_is_array(v) )
    {
        return cson_rc.TypeError;
    }
    else if( cx->response.root != v )
    {
        int rc = 0;
        rc = cson_cgi_gc_add(cx, "response.root", v, 0 )
            /** TODO: confirm that cson_object_set() does not
                clean up the original object if insertion fails.
                If it does, we've just hosed the root node.
            */
            ;
        if( 0 != rc )
        {
            return rc;
        }
        else
        {
            cx->response.root = v;
            return 0;
        }
    }
    else
    {
        return 0;
    }
    
}
cson_value * cson_cgi_response_root_get( cson_cgi_cx * cx, char createMode )
{
    if( ! cx ) return NULL;
    else if( cx->response.root ) return cx->response.root;
    else
    {
        if( 0 != createMode )
        {
            if( createMode > 0 )
            {
                cx->response.root = cson_value_new_object();
            }
            else if( createMode < 0 )
            {
                cx->response.root = cson_value_new_array();
            }
            if( cx->response.root &&
                (0 != cson_cgi_gc_add(cx, "response.root", cx->response.root, 1 )) )
            {
                cx->response.root = NULL /* was cleaned up by cson_cgi_gc_add() */;
            }
        }
        return cx->response.root;
    }
}


/** @internal

Tokenizes an input string on a given separator. Inputs are:

- (inp) = is a pointer to the pointer to the start of the input.

- (separator) = the separator character

- (end) = a pointer to NULL. i.e. (*end == NULL)

This function scans *inp for the given separator char or a NULL char.
Successive separators at the start of *inp are skipped. The effect is
that, when this function is called in a loop, all neighboring
separators are ignored. e.g. the string "aa.bb...cc" will tokenize to
the list (aa,bb,cc) if the separator is '.' and to (aa.,...cc) if the
separator is 'b'.

Returns 0 (false) if it finds no token, else non-0 (true).

Output:

- (*inp) will be set to the first character of the next token.

- (*end) will point to the one-past-the-end point of the token.

If (*inp == *end) then the end of the string has been reached
without finding a token.

Post-conditions:

- (*end == *inp) if no token is found.

- (*end > *inp) if a token is found.

It is intolerant of NULL values for (inp, end), and will assert() in
debug builds if passed NULL as either parameter.

When looping, one must be sure to re-set the inp and end
parameters. For example:

@code
char const * head = input;
char const * tail = NULL;
while( cson_cgi_next_token( &inp, '/', &tail ) ) {
  ...
  head = tail;
  tail = NULL;
}
@endcode

If the loop calls 'continue', it must be careful to
ensure that the parameters are re-set, to avoid an endless
loop. This can be simplified with a goto:

@code
while( cson_cgi_next_token( &inp, '/', &tail ) ) {
  if( some condition ) {
     ... do something ...
     goto next_iter;
  }
  else {
    ....
  }
  next_iter;
  head = tail;
  tail = NULL;
}
@endcode

*/
char cson_cgi_next_token( char const ** inp, char separator, char const ** end )
{
    char const * pos = NULL;
    assert( inp && end && *inp );
    if( ! inp || !end ) return 0;
    else if( *inp == *end ) return 0;
    pos = *inp;
    if( !*pos )
    {
        *end = pos;
        return 0;
    }
    for( ; *pos && (*pos == separator); ++pos) { /* skip preceeding splitters */ }
    *inp = pos;
    for( ; *pos && (*pos != separator); ++pos) { /* find next splitter */ }
    *end = pos;
    return (pos > *inp) ? 1 : 0;
}

/**
   If map->jval is NULL then map->jval is created using
   cson_value_new_object() and map->jobj is assigned to its object
   reference. The newly-created map->jval is appended to
   cx->gc to ensure that map->jval lives a full life (as
   opposed to potentially being prematurly GC'd if a client later adds
   map->jval to his own container).

   If map->jval is not NULL then this function is a no-op.

   This function will assert() if map is NULL.

   Returns 0 on success, else cson_rc.AllocError. On error map->jval
   will be NULL after this call.

   On success, ownership of map->jval is transfered to (or potentially
   shared with) cx->gc.
*/
static int cson_cgi_init_env_map( cson_cgi_cx * cx, char const * gckey, cson_cgi_env_map * map )
{
    int rc = 0;
    assert( NULL != map );
    if( NULL == map->jval )
    {
        assert( NULL == map->jobj );
        map->jval = cson_value_new_object();
        if( NULL == map->jval ) return cson_rc.AllocError;
        rc = cson_cgi_gc_add( cx, gckey, map->jval, 1 )
            /* We do this to avoid a corner case in cleanup logic
               if the client stores this object in another container.
            */;
        if( 0 != rc )
        {
            map->jval = NULL /* was cleaned up by cson_cgi_gc_add() */;
        }
        else
        {
            map->jobj = cson_value_get_object( map->jval );
            assert( NULL != map->jobj );
        }
    }
    return rc;
}

char const * cson_cgi_getenv_cstr( cson_cgi_cx * cx, char const * where, char const * key )
{
    return cson_string_cstr( cson_value_get_string( cson_cgi_getenv(cx, where, key) ) );
}

cson_value * cson_cgi_path_part( cson_cgi_cx * cx, unsigned short ndx )
{
    cson_value * piV = cson_cgi_getenv( cx, "e", "PATH_INFO_SPLIT" );
    if( ! piV ) return NULL;
    else
    {
        unsigned int alen;
        cson_array * ar = cson_value_get_array(piV);
        assert( NULL != ar );
        alen = cson_array_length_get( ar );
        return ( ndx >= alen )
            ? NULL
            : cson_array_get( ar, ndx );
    }
}

char const * cson_cgi_path_part_cstr( cson_cgi_cx * cx, unsigned short ndx )
{
    return cson_string_cstr( cson_value_get_string( cson_cgi_path_part( cx, ndx ) ) );
}

/**
   cson_cgi_hexchar_to_int():

   For 'a'-'f', 'A'-'F' and '0'-'9', returns the appropriate decimal
   number.  For any other character it returns -1.
*/
static int cson_cgi_hexchar_to_int( int ch )
{
    if( (ch>='a' && ch<='f') ) return ch-'a'+10;
    else if( (ch>='A' && ch<='F') ) return ch-'A'+10;
    else if( (ch>='0' && ch<='9') ) return ch-'0';
    return -1;
}

int cson_cgi_urldecode_inline( char * str )
{
    unsigned char ch = 0;
    unsigned char cx1 = 0;
    unsigned char cx2 = 0;
    int decoded;
    unsigned char * pos = (unsigned char *)str;
    unsigned char * out = pos;
    unsigned char const * end;
    size_t slen = (str && *str) ? strlen(str) : 0;
    if( !slen ) return 0;
    end = pos + slen;
    for( ; pos < end; ++pos )
    {
        ch = *pos;
        if( ch == '%' )
        {
            cx1 = *(pos+1);
            /* FIXME: with only minor refactoring we can remove the
               isxdigit() calls and use cson_cgi_hexchar_to_int()
               instead, checking for a negative return value. That
               would potentially save us 2 extra function calls here.
             */
            if( isxdigit(cx1) )
            {
                cx2 = *(pos+2);
                if( isxdigit(cx2) )
                {
                    decoded = (cson_cgi_hexchar_to_int( cx1 ) * 16)
                        + cson_cgi_hexchar_to_int( cx2 );
                    *(out++) = (char)decoded;
                    pos += 2;
                    continue;
                }
                /* else fall through... */
            }
            /* else fall through... */
        }
        else if( ch == '+' )
        {
            *(out++) = ' ';
            continue;
        }
        *(out++) = ch;
    }
    *out = 0;
    return 0;
}

/**
   If PATH_INFO is set, this function splits it on '/'
   characters and creates an array out of the elements.
   The array is stored as $ENV["PATH_INFO_SPLIT"].

   Returns non-0 on error. If PATH_INFO is not set,
   0 is returned. If it is set but has no entries,
   an empty array is created.

   A return value of cson_rc.RangeError probably means that a path
   element was longer than our internal buffer size, in which case
   processing ends and PATH_INFO_SPLIT is not set. That error can
   probably be ignored by the caller, but all others are probably
   serious (e.g. AllocError).
*/
static int cson_cgi_import_path_info(cson_cgi_cx *cx)
{
    char const * pi = cson_cgi_getenv_cstr(cx, "e","PATH_INFO");
    if( NULL == pi ) return 0;
    else
    {
        cson_value * arV = cson_value_new_array();
        cson_array * ar;
        char const * head = pi;
        char const * tail = NULL;
        if( ! arV ) return cson_rc.AllocError;
        else
        {
            enum { BufSize = 128 };
            char buf[BufSize];
            cson_value * partV;
            unsigned int slen;
            int rc = 0;
            ar = cson_value_get_array(arV);
            while( cson_cgi_next_token( &head, '/', &tail ) )
            {
                slen = (tail-head);
                if( slen >= BufSize )
                {
                    rc = cson_rc.RangeError;
                    goto end_clean;
                }
                memcpy( buf, head, slen );
                buf[slen] = 0;
                cson_cgi_urldecode_inline( buf );
                partV = cson_value_new_string( buf, strlen(buf) );
                if( ! partV )
                {
                    rc = cson_rc.AllocError;
                    goto end_clean;
                }
                rc = cson_array_append( ar, partV );
                if( rc )
                {
                    cson_value_free( partV );
                    goto end_clean;
                }
                partV = NULL;
                head = tail;
                tail = NULL;
            }
            assert( 0 == rc );
            rc = cson_object_set( cx->request.env.jobj,
                                  "PATH_INFO_SPLIT",
                                  arV );
            end_clean:
            if( rc )
            {
                cson_value_free( arV );
            }
            return rc;
        }
    }
}

/**
   Imports (extern char ** environ) into cx->request.env, initializing
   cx->request.env if needed. If called multiple times the environment
   is re-read each time, but old entries which are no longer in the
   new environment are not removed from cx->request.env.

   Returns 0 on success.
*/
static int cson_cgi_import_environ(cson_cgi_cx * cx)
{
    extern char ** environ;
    int i = 0;
    char const * e = environ[0];
    char const * v = NULL;
    enum { KeyBufSize = 512 };
    char keybuf[KeyBufSize];
    char * kpos = NULL;
    int rc = 0;
    cson_value * jv = NULL;
    rc = cson_cgi_init_env_map( cx, cson_cgi_keys.ENV_SYS, &cx->request.env );
    if( 0 != rc ) return rc;
    for( ; e && *e; e = environ[++i] )
    {
        v = NULL;
        memset( keybuf, 0, KeyBufSize );
        kpos = keybuf;
        for( ; *e && ('=' != *e); ++e )
        {
            *(kpos++) = *e;
            assert( kpos < (keybuf+KeyBufSize) );
            if( kpos >= (keybuf+KeyBufSize) )
            {
                return cson_rc.RangeError;
            }
        }
        if( '=' == *e )
        {
            v = e+1;
        }
        else
        {
            v = "";
        }
        jv = cson_value_new_string( v, strlen(v) );
        if( NULL == jv )
        {
            rc = cson_rc.AllocError;
            break;
        }
        rc = cson_object_set( cx->request.env.jobj, keybuf, jv );
        if( 0 != rc ) break;
    }
    if( 0 == rc )
    {
        rc = cson_cgi_import_path_info(cx);
    }
    return rc;
}

/**
   Tries to save the current session data, if any, using the
   configured session manager.

   Returns 0 on success. If the environment has no session,
   it is treated as success but nothing is actually saved.

   If no session manager has been configured then
   cson_rc.UnsupportedError is returned.
*/
static int cson_cgi_session_save(cson_cgi_cx * cx)
{
    if( ! cx->session.mgr )
    {
        return cson_rc.UnsupportedError;
    }
    else if( !cx->session.id || !cx->session.env.jval )
    {
        return 0;
    }
    else
    {
        return cx->session.mgr->api->save( cx->session.mgr,
                                           cx->session.env.jval,
                                           cx->session.id );
    }
}

cson_cgi_cx * cson_cgi_cx_alloc()
{
    cson_cgi_cx * rc = (cson_cgi_cx *)malloc(sizeof(cson_cgi_cx));
    if( rc )
    {
        *rc = cson_cgi_cx_empty;
        rc->misc.allocStamp = rc;
    }
    return rc;
}

char cson_cgi_cx_clean( cson_cgi_cx * cx )
{
    if( !cx ) return 0;
    else
    {
        void const * allocStamp = NULL;
        if( cx->session.mgr )
        {
            cson_cgi_session_save(cx) /* ignoring error code */;
            cx->session.mgr->api->finalize( cx->session.mgr );
            cx->session.mgr = NULL;
        }
        if(NULL != cx->gc.jval)
        {
            cson_value_free( cx->gc.jval );
            cx->gc.jval = NULL;
            cx->gc.jobj = NULL;
        }
        if( cx->session.id )
        {
            free( cx->session.id );
            cx->session.id = NULL;
        }
        cson_buffer_reserve( &cx->tmpBuf, 0 );
        allocStamp = cx->misc.allocStamp;
        if( cx->opt.inStream && (stdin != cx->opt.inStream) ) fclose(cx->opt.inStream);
        if( cx->opt.outStream && (stderr != cx->opt.outStream) && (stdout != cx->opt.outStream) ) fclose(cx->opt.outStream);
        if( cx->opt.errStream && (stderr != cx->opt.errStream) && (stdout != cx->opt.errStream) ) fclose(cx->opt.errStream);
        *cx = cson_cgi_cx_empty;
        return ( allocStamp == cx )
            ? (free( cx ), 1)
            : 0;
    }
}

cson_value * cson_cgi_env_get_val( cson_cgi_cx * cx, char which, char createIfNeeded )
{
    cson_cgi_env_map * map = NULL;
    cson_value * v = NULL;
    char const * gckey = NULL;
    switch( which )
    {
      case 'c':
      case 'C':
          map = &cx->request.cookie;
          gckey = cson_cgi_keys.ENV_COOKIE;
          break;
      case 'e':
      case 'E':
          gckey = cson_cgi_keys.ENV_SYS;
          map = &cx->request.env;
          break;
      case 'g':
      case 'G':
          gckey = cson_cgi_keys.ENV_GET;
          map = &cx->request.get;
          break;
      case 'f':
      case 'F':
          gckey = cson_cgi_keys.ENV_CONFIG;
          map = &cx->config;
          break;
      case 'p':
      case 'P':
          gckey = cson_cgi_keys.ENV_POST;
          map = &cx->request.post;
          break;
      case 'a':
      case 'A':
          gckey = cson_cgi_keys.ENV_APP;
          map = &cx->clientEnv;
          break;
      case 's':
      case 'S':
          gckey = cson_cgi_keys.ENV_SESSION;
          map = &cx->session.env;
          break;
      default:
          break;
    }
    if( map )
    {
        v = map->jval;
        if( !v && createIfNeeded )
        {
            assert( NULL != gckey );
            cson_cgi_init_env_map( cx, gckey, map );
            v = map->jval;
        }
    }
    return v;
}

cson_object * cson_cgi_env_get_obj( cson_cgi_cx * cx, char which, char createIfNeeded )
{
    return cson_value_get_object( cson_cgi_env_get_val( cx, which, createIfNeeded ) );
}

/**
   Sets a variable in one of the environment objects.

   env must be the conventional character representation
   (case-insensitive) for on of the following environment objects:

   - g = GET
   - p = POST
   - e = ENV
   - c = COOKIE
   - u = USER

   On success 0 is returned and ownership of v is transfered to (or
   shared with) the appropriate environment object. On error non-zero
   is returned and ownership of v is not modified.
*/
static int cson_cgi_setenv_x( cson_cgi_cx * cx, char env, char const * key, cson_value * v )
{
    if( ! key || !*key ) return cson_rc.ArgError;
    else
    {
        cson_object * jo = cson_cgi_env_get_obj( cx, env, 1 );
        return ( NULL == jo )
            ? cson_rc.RangeError /* FIXME: expand the above code so we
                                    can distinguish between invalid
                                    env and allocation error. (Except that
                                    there is no allocation on get_obj().*/
            : cson_object_set( jo, key, v );
    }
}

int cson_cgi_setenv( cson_cgi_cx * cx, char const * key, cson_value * v )
{
    return cson_cgi_setenv_x( cx, 'a', key, v );
}

int cson_cgi_cookie_set( cson_cgi_cx * cx, char const * key, cson_value * v )
{

    if( ! key || !*key ) return cson_rc.ArgError;
    else
    {
        cson_object * jo = cson_cgi_env_get_obj( cx, 'c', 1 );
        return (NULL == jo)
            ? cson_rc.AllocError
            : cson_object_set( jo, key, v ? v : cson_value_null() );
    }
}

int cson_cgi_cookie_set2( cson_cgi_cx * cx,
                          char const * key, cson_value * v,
                          char const * domain, char const * path,
                          unsigned int expires, char secure, char httponly )
{
    if( ! key || !*key ) return cson_rc.ArgError;
    else
    {
        int rc;
        cson_value * jv = cson_value_new_object();
        cson_object * jo = cson_value_get_object(jv);
        cson_value * x = NULL;
        if( ! jo ) return cson_rc.AllocError;
        if( ! v ) v = cson_value_null() /* reminder: does not allocate */;

#define SET(KEY) if( 0 != (rc = cson_object_set( jo, KEY, x) ) ) {      \
            cson_value_free(x); \
            cson_value_free( jv ); \
            return rc; \
        }

        if( NULL != domain )
        {
            x = cson_value_new_string( domain, strlen(domain) );
            SET("domain");
        }
        if( NULL != path )
        {
            x = cson_value_new_string( path, strlen(path) );
            SET("path");
        }

        if( cson_value_is_null(v) )
        {
            x = cson_value_new_integer( 1 );
            SET("expires");
        }
        else if( expires )
        {
            x = cson_value_new_integer( (cson_int_t) expires );
            SET("expires");
        }
        if( secure )
        {
            x = cson_value_new_bool(secure);
            SET("secure");
        }
        if( httponly )
        {
            x = cson_value_new_bool(httponly);
            SET("httponly");
        }
#undef SET
        rc = cson_cgi_cookie_set( cx, key, jv );
        if( 0 != rc )
        {
            cson_value_free( jv );
        }
        else
        { /* set "value" last so that we can avoid tricky
             ownership/lifetime problems in error cases.
          */
            if( 0 != (rc = cson_object_set( jo, "value", v) ) )
            { /* remove the cookie. Note that this particular case
                 does not remove it from the HTTP client. In order to do that
                 we have to keep the existing path/domain/etc info.
              */
                cson_object * cookies = cson_cgi_env_get_obj( cx, 'c', 0 );
                if( cookies )
                {
                    cson_object_set( cookies, key, cson_value_null() )
                        /* Ignore error code, since we have no fallback
                           and cson_value_null() does not allocate.
                           Worst-case is that removing it fails, but when we
                           emit the cookie headers that cookie will be skipped
                           because it has no "value" field.
                        */
                        ;
                }
            }
        }
        return rc;
    }
}

cson_value * cson_cgi_getenv( cson_cgi_cx * cx, char const * fromWhere, char const * key )
{
    cson_value * jv = NULL;
    cson_object * map = NULL;
    if( (NULL == fromWhere) || !*fromWhere ) fromWhere = CSON_CGI_GETENV_DEFAULT;
    if( !key || !*key ) return NULL;
    for( ; *fromWhere ; ++fromWhere )
    {
        map = cson_cgi_env_get_obj( cx, *fromWhere, 0 );
        if( (NULL == map) && (('r'==*fromWhere)||('R'==*fromWhere)) )
        {
            jv = cson_cgi_getenv( cx, "gpc", key );
        }
        if( NULL != jv ) /* only in 'R' case */ break;
        else if( NULL == map ) continue /* invalid character or NULL map */;
        jv = cson_object_get( map, key );
        if( NULL != jv ) break;
    }
    return jv;
}


int cson_cgi_response_header_add( cson_cgi_cx * cx, char const * key, cson_value * v )
{
    int rc = 0;
    if( !cx || ! key || !*key ) return cson_rc.ArgError;
    rc = cson_cgi_init_env_map( cx, cson_cgi_keys.RESPONSE_HEADERS, &cx->response.headers );
    if( 0 == rc )
    {
        assert( NULL != cx->response.headers.jobj );
        rc = cson_object_set( cx->response.headers.jobj, key, v );
    }
    return rc;
}


char cson_cgi_is_jsonp(cson_cgi_cx * cx)
{
    if( ! cx ) return 0;
    else if( cx->misc.isJSONP < 0 )
    { /* guess */
        cx->misc.isJSONP = (NULL == cson_cgi_getenv( cx, "agp", CSON_CGI_KEY_JSONP ))
            ? 0 : 1;
    }
    return cx->misc.isJSONP;
}

void cson_cgi_enable_jsonp( cson_cgi_cx * cx, char b )
{
    if( cx ) cx->misc.isJSONP = b ? 1 : 0;
}

char const * cson_cgi_guess_content_type(cson_cgi_cx * cx)
{
    char const * cset;
    char doUtf8;
    cset = getenv("HTTP_ACCEPT_CHARSET");
    doUtf8 = ((NULL == cset) || (NULL!=strstr("utf-8",cset)))
        ? 1 : 0;
    if( cson_cgi_is_jsonp(cx) )
    {
        return doUtf8
            ? "application/javascript; charset=utf-8"
            : "application/javascript";
    }
    else
    {
        /*
          Content-type

          If the browser does not sent an ACCEPT for application/json
          then we fall back to text/plain.
        */
        char const * cstr;
        cstr = getenv("HTTP_ACCEPT");
        if( NULL == cstr )
        {
            return doUtf8
                ? "application/json; charset=utf-8"
                : "application/json";
        }
        else
        {
            if( strstr( cstr, "application/json" )
                || strstr( cstr, "*/*" ) )
            {
                return doUtf8
                    ? "application/json; charset=utf-8"
                    : "application/json";
            }
            else
            {
                return "text/plain";
            }
        }
    }
}


/**
   URL-encodes src to dest and NUL-terminates it. dest must be at
   least *destLen bytes long. Upon a successful return, *destLen
   will be modified to hold the new string's length.

   Returns 0 on success. On error dest might be partially populated.

   Returns cson_rc.RangeError if dest is not long enough to hold
   the conversion and a terminating NUL.
*/
static int cson_cgi_urlencode( char const * src, char * dest_, size_t * destLen )
{
#define needs_escape \
    ( (ch >= 32 && ch <=47)                       \
      || ( ch>=58 && ch<=64)                      \
      || ( ch>=91 && ch<=96)                      \
      || ( ch>=123 && ch<=126)                    \
      || ( ch<32 || ch>=127)                      \
      )
    char const * pos = src;
    char ch;
    size_t dpos = 0;
    char * dest = dest_;
    static char const * hex = "0123456789ABCDEF";
    if( ! dest || !destLen ) return cson_rc.RangeError;
    for( ; pos && *pos; ++pos )
    {
        ch = *pos;
        if( ! needs_escape )
        {
            if( ++dpos >= *destLen ) return cson_rc.RangeError;
            *(dest++) = ch;
            continue;
        }
        else
        {
            if( (dpos+=3) >= *destLen ) return cson_rc.RangeError;
            *(dest++) = '%';
            *(dest++) = hex[((ch>>4)&0xf)];
            *(dest++) = hex[(ch&0xf)];
        }
    }
    if( ++dpos >= *destLen ) return cson_rc.RangeError;
    *dest = 0;
    *destLen = dest - dest_;
    return 0;
#undef needs_escape
}

/**
   If orig is one of the types (string,double,bool,undef,null) then
   a pointer to its string representation is returned, else NULL
   is returned.

   For non-string types, dest must be at least destLen bytes of memory, and
   if destLen is not long enough to hold the string form then NULL is returned.

   On success a pointer to a string is returned. It will be one of:

   - if orig is-a string then it's underlying string.

   - for (double,integer,bool,undef,null), dest will be returned. The encoded
   form is decimal for (double,integer), the number 0 or 1 for bool, and the
   number 0 for (undef,null).

   Ownership of dest is not modified by this call.

   The returned value is valid until either orig or dest are modified.

   On error dest is not modified. Dest is also not modified if orig
   is-a string, as its own string bytes are returned instead.
*/
static char const * cson_cgi_pod_to_string( cson_value const * orig,
                                            char * dest, unsigned int destLen )
{
    if( ! orig || !dest || !destLen ) return NULL;
    else
    {/* FIXME? use cson's output support for the numeric types. i
        _think_ those bits might not be in the public API, though.
        We could use it for serializing objects/arrays, in any case.
      */
        enum { NumBufSize = 80 };
        if( cson_value_is_string(orig) )
        {
            cson_string const * jstr = cson_value_get_string(orig);
            assert( NULL != jstr );
            return cson_string_cstr( jstr ); 
        }
        else if( cson_value_is_integer(orig) )
        {
            char tmp[NumBufSize] = {0};
            int const sc = sprintf( tmp, "%"CSON_INT_T_PFMT, cson_value_get_integer(orig));
            if( sc <= 0 ) return NULL;
            else if( (unsigned int)sc >= destLen ) return NULL;
            else
            {
                strcpy( dest, tmp );
                return dest;
            }
        }
        else if( cson_value_is_double(orig) )
        {
            char tmp[NumBufSize] = {0};
            int const sc = sprintf( tmp, "%"CSON_DOUBLE_T_PFMT, cson_value_get_double(orig));
            if( sc <= 0 ) return NULL;
            else if( (unsigned int)sc >= destLen ) return NULL;
            else
            {
                strcpy( dest, tmp );
                if(1)
                { /* Strip trailing zeroes... */
                    unsigned int urc = strlen(dest);
                    char * pos = dest + urc - 1;
                    for( ; ('0' == *pos) && urc && (*(pos-1) != '.'); --pos, --urc )
                    {
                        *pos = 0;
                    }
                    assert(urc && *pos);
                }
                return dest;
            }
        }
        else if( cson_value_is_bool( orig ) )
        {
            char const bv = cson_value_get_bool(orig);
            if( destLen < 2 ) return NULL;
            *dest = bv ? '1' : '0';
            *(dest+1) = 0;
            return dest;
        }
        else if( cson_value_is_null( orig ) || cson_value_is_undef( orig ) )
        {
            if( destLen < 2 ) return NULL;
            *dest = '0';
            *(dest+1) = 0;
            return dest;
        }
        else
        {
            return NULL;
        }
    }
}


/**
   Writes an RFC822 timestamp string to dest, which must be at least destLen bytes long.
   On success returns dest, else NULL. destLen must be at least 31.
*/
static char * cson_cgi_rfc822_timedate( time_t now, char * dest, unsigned int destLen )
{
    static const char * dayNames[] = 
        {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
         0 };
    static const char * monthNames[] =
        {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
         "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
         0};

    struct tm * t = (dest && (destLen>30)) ? gmtime(&now) : NULL;
    if( ! t || (destLen<31) ) return NULL;
    else
    {
        int const rc = sprintf( dest,
                                "%s, %d %s %02d %02d:%02d:%02d GMT",
                                dayNames[t->tm_wday], t->tm_mday,
                                monthNames[t->tm_mon],
                                t->tm_year+1900, t->tm_hour,
                                t->tm_min, t->tm_sec
                                );
        assert( (rc>0) && ((unsigned int)rc) < destLen );
        return dest;
    }
}

/**
   Outputs the cookie-specific HTTP headers.

   Returns 0 on success.
*/
static int cson_cgi_response_output_cookies(cson_cgi_cx * cx)
{
    cson_kvp * kvp = NULL;
    cson_object * jo = NULL;
    cson_object_iterator iter = cson_object_iterator_empty;
    assert(cx);
    jo = cx->request.cookie.jobj;
    if( ! jo ) return 0;
    else
    {
        enum { CookieBufSize = 1024 * 8,
               ValBufSize = 1024 * 4,
               TSBufSize = 32
        };
        char cookieBuf[CookieBufSize] = {0} /* buffer for whole cookie string */;
        char valBuf[ValBufSize] = {0} /* buffer for value encoding */;
        char urlBuf[ValBufSize] = {0} /* buffer for urlencoding */;
        char tsBuf[TSBufSize] = {0} /* buffer for expiry timestamp */;
        int rc = cson_object_iter_init( jo, &iter );
        assert( CookieBufSize > ValBufSize );
        if( 0 != rc ) return rc;
        while( (kvp = cson_object_iter_next(&iter)) )
        {
            cson_string const * key = cson_kvp_key(kvp);
            cson_value const * val = cson_kvp_value(kvp);
            if( cson_value_is_null(val) )
            {
#if 1
                cson_cgi_printf(cx,"Set-Cookie: %s=; Expires=Thu, 01-Jan-1970 00:00:01 GMT\r\n", cson_string_cstr(key));
#else
                cson_cgi_printf(cx,"Set-Cookie: %s=\r\n", cson_string_cstr(key));
#endif
                continue;
            }
            if( cson_value_is_object(val) )
            {
                /*
                  Accept in Object in the form:

                  {
                  value: VALUE,
                  domain: string,
                  path: string,
                  secure: bool,
                  httponly: bool,
                  expires: integer
                  }
                 */
                cson_object const * obj = cson_value_get_object( val );
                cson_value const * cv = cson_object_get( obj, "value" );
                char const * valstr = NULL;
                char const isNull = !cv || cson_value_is_null( cv );
                if( isNull )
                {
                    cson_cgi_printf(cx, "Set-Cookie: %s=", cson_string_cstr(key));
                }
                else
                {
                    /* FIXME: streamify urlencode so we can get around fixed buffer size. */
                    valstr = cson_cgi_pod_to_string( cv, valBuf, ValBufSize );
                    if( ! valstr ) continue;
                    else
                    {
                        size_t bSize = ValBufSize;
                        memset( urlBuf, 0, ValBufSize );
                        if( 0 != cson_cgi_urlencode( valstr, urlBuf, &bSize ) )
                        {
                            /* buffer is too small. Skip it. */
                            continue;
                        }
                        assert( bSize <= ValBufSize );
                        cson_cgi_printf(cx, "Set-Cookie: %s=%s", cson_string_cstr(key), urlBuf);
                    }
                }

#define DOPART(KEY,KEY2) cv = cson_object_get( obj, KEY );  \
                if( cv ) { \
                    valstr = cson_cgi_pod_to_string( cv, valBuf, ValBufSize ); \
                    if( valstr ) { \
                        cson_cgi_printf( cx, "; "KEY2"=%s", valstr );  \
                    } } (void)0
                DOPART("domain","Domain");
                DOPART("path","Path");
#undef DOPART

                cv = cson_object_get( obj, "expires" );
                if( cv || isNull )
                {
                    cson_int_t const intVal = isNull ? 1 : cson_value_get_integer(cv);
                    if( intVal )
                    {
                        valstr = cson_cgi_rfc822_timedate( (time_t)intVal, tsBuf, TSBufSize );
                        if( valstr )
                        {
                            cson_cgi_printf( cx, "; Expires=%s", valstr );
                        }
                    }
#if 0
                    else if( cson_value_is_string(cv) )
                    {
                        /* TODO?: assume it's already propery formatted. */
                    }
                    else
                    {
                        /* skip it.*/
                    }
#endif
                }
                cv = cson_object_get( obj, "secure" );
                if( cson_value_get_bool(cv) )
                {
                    cson_cgi_printf( cx, "; Secure" );
                }
                
                cv = cson_object_get( obj, "httponly" );
                if( cson_value_get_bool(cv) )
                {
                    cson_cgi_printf( cx, "; HttpOnly" );
                }
                cson_cgi_puts(cx, "\r");
            }
            else
            {
                char const * valstr;
                memset( valBuf, 0, ValBufSize );
                valstr = cson_cgi_pod_to_string( val, valBuf, ValBufSize );
                if( ! valstr ) continue;
                else
                {
                    size_t bSize = CookieBufSize;
                    memset( cookieBuf, 0, CookieBufSize );
                    rc = cson_cgi_urlencode( valstr, cookieBuf, &bSize );
                    if( 0 != rc )
                    {
                        /* too beaucoup. skip it */
                        continue;
                    }
                    assert( bSize < CookieBufSize );
                    cson_cgi_printf(cx,"Set-Cookie: %s=%s\r\n", cson_string_cstr(key), cookieBuf);
                }
            }
        }
        return 0;
    }

}
int cson_cgi_response_output_headers(cson_cgi_cx * cx)
{
    enum { BufSize = 64 };
    cson_object * jo = NULL;
    int rc;
    rc = cson_cgi_printf(cx, "Content-type: %s\r\n", cson_cgi_guess_content_type(cx) );
    if( rc <= 0 ) return rc;
    rc = cson_cgi_puts(cx, "Status: 200 OK\r");
    if( rc <= 0 ) return rc;
    jo = cx->response.headers.jobj;
    if( jo )
    {
        char buf[BufSize] = {0};
        cson_object_iterator iter = cson_object_iterator_empty;
        cson_kvp * kvp;
        cson_string const * key;
        cson_value const * val;
        char const * valcstr;
        rc = cson_object_iter_init( jo, &iter );
        if( 0 != rc ) return rc;
        while( (kvp = cson_object_iter_next(&iter)) )
        {
            key = cson_kvp_key(kvp);
            val = cson_kvp_value(kvp);
            valcstr = cson_cgi_pod_to_string( val, buf, BufSize );
            if( ! valcstr ) continue;
            assert( NULL != key );
            assert( NULL != val );
            cson_cgi_printf(cx, "%s: %s\r\n",
                            cson_string_cstr(key),
                            valcstr ? valcstr : "");
        }
    }
    rc = cson_cgi_response_output_cookies(cx);
    return rc;
}

int cson_cgi_response_output_root(cson_cgi_cx * cx)
{
    return ( !cx || !cx->response.root )
        ? cson_rc.ArgError
        : cson_output_FILE( cx->response.root, cx->opt.outStream, &cx->opt.outOpt );
}

int cson_cgi_response_output_all(cson_cgi_cx * cx)
{
    int rc = 0;
    char isJP = 0;
    char doHeaders = cx->opt.httpHeadersMode;
    if( NULL == cx->response.root )
    {
        return cson_rc.ArgError;
    }
    isJP = cson_cgi_is_jsonp(cx);
    if( doHeaders < 0 )
    {
        if( NULL!=getenv("GATEWAY_INTERFACE") )
        {
            doHeaders = 1;
        }
    }
    if( doHeaders > 0 )
    {
        rc = cson_cgi_response_output_headers(cx);
        if( 0 == rc )
        {
            cson_cgi_puts(cx,"\r")/*yes, putS, not putCHAR!*/;
        }
        else return rc;
    }
    if( isJP )
    {
        cson_cgi_printf(cx,"%s(", "FIXME_JSONP_CALLBACK_NAME" );
    }
    rc = cson_cgi_response_output_root(cx);
    if( 0 == rc )
    {
        if( isJP )
        {
            cson_cgi_putchar(cx,')');
        }
        cson_cgi_putchar(cx,'\n');
        fflush( cx->opt.outStream );
    }
    return rc;
}

/**
   Parses inp as a delimited list, separated by the given
   separator character. Each item in the list is treated
   as a key/value pair in the form KEY=VALUE, and inserted
   into the target cson_object (which must not be NULL).

   This is intended for parsing HTTP GET-style parameter lists.

   If doUrlDecode is true (non-zero) then the VALUE part of the
   key/value pair gets url-decoded before insertion. (FIXME? Also
   decode the keys?)

   If firstOneWins is non-0 then if a given key in the parameters is
   duplicated, entries after the first are ignored. If it is 0 then
   the "last one wins." This is basically a workaround for when we
   have multiple session ID cookies hanging around :/.
   
   On success it returns 0.

   If a given key contains the string "[]", that part is stripped and
   the entry is treated like an array element. e.g. a query string of
   "a[]=3&a[]=7" would result in an array property named "a" with the
   (string) entries ("3", "7").
   
*/
static int cson_cgi_parse_param_list( cson_cgi_cx * cx,
                                      cson_object * tgt,
                                      char const * inp,
                                      char separator,
                                      char doUrlDecode,
                                      char firstOneWins)
{
    if( ! tgt || !separator ) return cson_rc.ArgError;
    else if( !inp || !*inp ) return 0;
    else
    {
        char const * head = inp;
        char const * tail = NULL;
        char * out = NULL;
        unsigned int inLen = strlen( inp );
        unsigned int valLen;
        cson_value * jval = NULL;
        cson_value * listV = NULL;
        cson_array * list = NULL;
        int rc = cson_buffer_reserve( &cx->tmpBuf, inLen+1 );
        if( 0 != rc ) return rc;
        while( cson_cgi_next_token( &head, separator, &tail ) )
        {
            char const * key = head;
            char * value = NULL;
            rc = 0;
            if( head == tail ) break;
            out = (char *)cx->tmpBuf.mem;
            memset( cx->tmpBuf.mem, 0, cx->tmpBuf.capacity );
            for( ; (key<tail) && *key && isspace(*key); ++key )
            {
                /* strip leading spaces in the key name
                   (happens in cookie values). */
            }
            if( key==tail ) break;
            else if( '='==*key )
            {
                /* all-space key. Just skip it. */
                goto next_iter;
            }
            /* Write the key part to the buffer... */
            for( ; (key<tail) && *key && ('='!=*key); ++key ) {
                *(out++) = *key;
            }
            *(out++) = 0;
            if( '=' == *key )
            {
                ++key;
            }
            value = out;
            valLen = 0;
            /* Write the value part to the buffer... */
            for( ; (key<tail) && *key; ++key, ++valLen ) {
                *(out++) = *key;
            }
            key = (char const *)cx->tmpBuf.mem;
            if( firstOneWins && (NULL != cson_object_get( tgt, key )) )
            {
                goto next_iter;
            }
            if( doUrlDecode && valLen )
            {
                cson_cgi_urldecode_inline( value );
            }
            /*MARKER("key=[%s], valLen=%u, value=[%s]\n", key, valLen, value );*/
            jval = cson_value_new_string( value, valLen );
            if( NULL == jval )
            {
                rc = cson_rc.AllocError;
                goto the_end;
            }
            if( NULL != (out = strstr(key,"[]")) )
            { /* Treat key as an array entry, like PHP does... */
                cson_value * freeThisOnErr = NULL;
                *out = 0;
                list = NULL;
                listV = cson_object_get( tgt, key );
                if( listV )
                {
                    if( ! cson_value_is_array( listV ) )
                    {
                        /* skip it to avoid hosing a different entry. */
                        cson_value_free( jval );
                        jval = NULL;
                        goto next_iter;
                    }
                }
                else
                { /* create a new array to hold the value */
                    listV = cson_value_new_array();
                    if( ! listV )
                    {
                        cson_value_free( jval );
                        rc = cson_rc.AllocError;
                        goto the_end;
                    }
                    rc = cson_object_set( tgt, key, listV );
                    if( 0 != rc )
                    {
                        cson_value_free( listV );
                        cson_value_free( jval );
                        goto the_end;
                    }
                    freeThisOnErr = listV;
                }
                list = cson_value_get_array( listV );
                assert( NULL != list );
                rc = cson_array_append( list, jval );
                if( 0 != rc )
                {
                    cson_value_free( jval );
                    cson_value_free( freeThisOnErr );
                    goto the_end;
                }
            }
            else
            {
                rc = cson_object_set( tgt, key, jval );
                if( 0 != rc )
                {
                    cson_value_free( jval );
                    goto the_end;
                }
            }
            next_iter:
            head = tail;
            tail = NULL;
        }
        the_end:
        cson_buffer_reserve( &cx->tmpBuf, 0 );
        return rc;
    }
}
                             

/**
   Parses key/value pairs from a QUERY_STRING-formatted
   string.

   Returns 0 on success. The "most likely" error condition, in terms
   of potential code paths, is is an allocation error.
   
   TODO: if the key part of any entry ends with "[]", treat it as an
   array entry, like PHP does.
*/
static int cson_cgi_parse_query_string( cson_cgi_cx * cx, char const * qstr )
{
    cson_object * env = NULL;
    if( !qstr || !*qstr ) return 0;
    assert(cx);
    env = cson_cgi_env_get_obj( cx, 'g', 1 );
    if( NULL == env ) return cson_rc.AllocError /* guess! */;
    return cson_cgi_parse_param_list( cx, env, qstr, '&', 1, 0 );
}

#if CSON_CGI_ENABLE_POST_FORM_URLENCODED
static int cson_cgi_parse_post_urlencoded( cson_cgi_cx * cx, char const * qstr )
{
    cson_object * env = NULL;
    if( !qstr || !*qstr ) return 0;
    assert(cx);
    env = cson_cgi_env_get_obj( cx, 'p', 1 );
    if( NULL == env ) return cson_rc.AllocError /* guess! */;
    return cson_cgi_parse_param_list( cx, env, qstr, '&', 1, 0 );
}
#endif

/**
   Like cson_cgi_parse_query_string(), but expects qstr to be in COOKIE
   format.
*/
static int cson_cgi_parse_cookies( cson_cgi_cx * cx, char const * qstr )
{
    cson_object * env = NULL;
    if( !qstr || !*qstr ) return 0;
    assert(cx);
    env = cson_cgi_env_get_obj(cx, 'c', 1 );
    if( NULL == env ) return cson_rc.AllocError /* guess! */;
    return cson_cgi_parse_param_list( cx, env, qstr, ';', 1, 1 );
}


/**
   Initializes cx->argv.jval and cx->argv.jarr, adds them to the
   garbage collector, then copies argv to cx->argv.jarr as an
   array of JSON strings.

   Returns 0 on success.

   Results are undefined if argv is not a properly initialized array
   of NUL-terminated strings with at least argc entries.

   If argc is 0 or less then cx->argv is still initialized but has
   a length of 0.

   After the first call, further arguments are appended to the current
   list.
*/
static int cson_cgi_init_argv( cson_cgi_cx * cx, int argc, char const * const * argv )
{
    int rc = 0;
    int i;
    assert( NULL != cx->gc.jobj );
    if( cx->argv.jval == NULL )
    {
        cson_value * v = cson_value_new_array();
        if( NULL == v ) return cson_rc.AllocError;
        rc = cson_cgi_gc_add( cx, cson_cgi_keys.ENV_ARGV, v, 1 );
        if( 0 != rc )
        {
            /* reminder: v was freed by cson_cgi_gc_add() */
            return rc;
        }
        cx->argv.jval = v;
        cx->argv.jarr = cson_value_get_array( v );
        assert( NULL != cx->argv.jarr );
    }
    for( i = 0; i < argc; ++i )
    {
        char const * arg = argv[i];
        cson_value * vstr = cson_value_new_string( arg ? arg : "",
                                                   arg ? strlen(arg) : 0 );
        if( NULL == vstr ) return cson_rc.AllocError;
        rc = cson_array_append( cx->argv.jarr, vstr );
        if( 0 != rc )
        {
            cson_value_free( vstr );
            break;
        }
    }
    return rc;
}

typedef struct CgiPostReadState_ {
    FILE * fh;
    unsigned int len;
    unsigned int pos;
} CgiPostReadState;

static int cson_data_source_FILE_n( void * state, void * dest, unsigned int * n )
{
    if( ! state || !dest || !n ) return cson_rc.ArgError;
    else
    {
        CgiPostReadState * st = (CgiPostReadState *)state;
        if( st->pos >= st->len )
        {
            *n = 0;
            return 0;
        }
        else if( !*n || ((st->pos + *n) > st->len) ) return cson_rc.RangeError;
        else
        {
            unsigned int rsz = (unsigned int)fread( dest, 1, *n, st->fh );
            if( ! rsz )
            {
                *n = rsz;
                return feof(st->fh) ? 0 : cson_rc.IOError;
            }
            else
            {
                *n = rsz;
                st->pos += *n;
                return 0;
            }
        }
    }
}


static int cson_cgi_parse_POST_JSON(cson_cgi_cx * cx, FILE * src, unsigned int contentLen)
{
    cson_value * jv = NULL;
    int rc = 0;
    CgiPostReadState state;
    cson_parse_info pinfo = cson_parse_info_empty;
    assert( 0 != contentLen );
    assert( NULL == cx->request.post.jval );
    state.fh = src;
    state.len = contentLen;
    state.pos = 0;
    rc = cson_parse( &jv, cson_data_source_FILE_n, &state, NULL, &pinfo );
    if( rc )
    {
#if 0
        fprintf(stderr, "%s: Parsing POST as JSON failed: code=%d (%s) line=%u, col=%u\n",
                __FILE__, rc, cson_rc_string(rc), pinfo.line, pinfo.col );
#endif
        return rc;
    }
    rc = cson_cgi_gc_add( cx, cson_cgi_keys.ENV_POST, jv, 1 );
    if( 0 == rc )
    {
        cx->request.post.jval = jv;
        cx->request.post.jobj = cson_value_get_object( jv );
        assert( cx->request.post.jobj && "FIXME: also support an Array as POST data node." ); 
    }
    return rc;
}

static int cson_cgi_init_POST(cson_cgi_cx * cx)
{
    if( ! cx || !cx->opt.inStream ) return cson_rc.ArgError;
    else
    {
        FILE * src = cx->opt.inStream;
        char const * ctype = cson_string_cstr( cson_value_get_string( cson_cgi_getenv( cx, "e", "CONTENT_TYPE" ) ) );
        if( NULL == ctype ) return 0;
        else
        {
            char const * clen = cson_string_cstr( cson_value_get_string( cson_cgi_getenv( cx, "e", "CONTENT_LENGTH" ) ) );
            if( NULL == clen ) return cson_rc.ArgError;
            else
            {
                char * endpt = NULL;
                long len = strtol( clen, &endpt, 10 );
                if( (endpt && *endpt) || (len<=0) ) return cson_rc.RangeError;
#if CSON_CGI_ENABLE_POST_FORM_URLENCODED
                else if( 0 == strncmp(ctype,"application/x-www-form-urlencoded",33) )
                {
                    cson_buffer buf = cson_buffer_empty;
                    int rc = cson_buffer_fill_from( &buf, cson_data_source_FILE, src );
                    if( rc )
                    {
                        goto end_clean;
                        return rc;
                    }
                    if( buf.mem && buf.used )
                    {
#if 1
                        if( strlen((char const *)buf.mem)
                            != buf.used )
                        {
                            /* assume bad/malicious input. */
                            rc = cson_rc.RangeError;
                            goto end_clean;
                        }
#endif
                        rc = cson_cgi_parse_post_urlencoded( cx, (char const *)buf.mem );
                    }
                    end_clean:
                    cson_buffer_reserve( &buf, 0 );
                    return rc;
                }
#endif
                else if( (0 == strncmp(ctype,"application/json",16))
                         || (0 == strncmp(ctype,"text/plain",10))
                         || (0 == strncmp(ctype,"application/javascript",22))
                         )
                {
                    return cson_cgi_parse_POST_JSON(cx, src, len);
                }
                else
                {
                    return cson_rc.TypeError;
                }
            }
        }
    }
}

static int cson_cgi_init_config( cson_cgi_cx * cx, char const * fname )
{
    int rc;
    cson_value * root = NULL;
    rc = cson_parse_filename( &root, fname, NULL, NULL );
    if( 0 == rc )
    {
        assert( NULL != root );
        if( ! cson_value_is_object(root) )
        {
            cson_value_free( root );
            rc = cson_rc.TypeError;
        }
        else
        {
            rc = cson_cgi_gc_add( cx,cson_cgi_keys.ENV_CONFIG, root, 1 );
            if( 0 == rc )
            {
                cx->config.jval = root;
                cx->config.jobj = cson_value_get_object( root );
                assert( NULL != cx->config.jobj );
            }
        }
    }
    return rc;
}

static char * cson_cgi_strdup( char const * src )
{
    size_t const n = src ? strlen(src) : 0;
    char * rc = src ? (char *)malloc(n+1) : NULL;
    if( ! rc ) return NULL;
    memcpy( rc, src, n );
    rc[n] = 0;
    return rc;
}

/**
   Writes a 36-byte (plus one NUL byte) UUID value to dest. dest
   must be at least 37 bytes long. If dest is NULL this function
   has no side effects.

   Not thread-safe.
*/
void cson_cgi_generate_uuid( cson_cgi_cx * cx, char * dest )
{
    static whuuid_rng rng = {
    NULL/*rand*/,
    NULL/*cleanup*/,
    NULL/*impl*/
#if WHUUID_CONFIG_KEEP_METRICS
    ,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}/*distribution*/
#endif
    };
    whuuid_t u = whuuid_t_empty;
    if( NULL == dest ) return;
    else if( (NULL==rng.rand) && (NULL != RNG_FILENAME) )
    { /* try to open rng file... */
        /* FIXME: we're missing a cleanup handler for the RNG_FILENAME case. */
        FILE * f = fopen(RNG_FILENAME, "rb");
        if( NULL != f )
        {
            rng = whuuid_rng_FILE;
            rng.impl = f;
        }
    }
    if( NULL == rng.rand )
    { /* fall back to LC rng */
        extern char ** environ;
        void * addr;
        unsigned long seed;
        rng = whuuid_rng_lcrng;
        addr = malloc(
                      (((unsigned long)environ) % 13) + 9
                      );
        free(addr) /* but keep the address as a seed value */;
        seed = (unsigned long)addr * (unsigned long)time(NULL);
        rng.impl = (void *)seed;
    }
    whuuid_fill_rand( &u, &rng );
    whuuid_to_string( &u, dest );
}

char const * cson_cgi_session_id(cson_cgi_cx * cx)
{
    return cx ? cx->session.id : NULL;
}


static int cson_cgi_init_session_mgr(cson_cgi_cx * cx)
{
    /*
       Check for this config structure:

       {
       manager:"mgrID",
       managers:{
           mgrID:{
               sessionDriver: "back-end-name" (e.g. "cpdo" or "file"),
               ... back-end-specific options ...
           },
           otherManager: { ... }
       }
    */
    cson_object const * conf = cson_cgi_env_get_obj(cx,  'f', 0 );
    cson_string const * aString;
    cson_value const * optV = NULL;
    cson_object const * optObj = NULL;
    if( NULL == conf ) return 0;
    assert( cx && !cx->session.mgr );

    /* get "manager" part... */
    aString = cson_value_get_string( cson_object_get_sub( conf, "session.manager", '.' ) );
    if( NULL == aString ) return 0;

    /* Fetch that manager config ... */
    optV = cson_object_get_sub( conf, "session.managers", '.' );
    if( optV )
    {
        optV = cson_object_get( cson_value_get_object( optV ), cson_string_cstr( aString ) );
    }
    optObj = cson_value_get_object( optV );
    if( ! optObj ) return 0;

    /* Get the "sessionDriver" part ... */
    aString = cson_value_get_string( cson_object_get( optObj, "sessionDriver" ) );
    if( NULL == aString ) return 0;

    return cson_sessmgr_load( cson_string_cstr(aString), &cx->session.mgr, optObj );
}


static char const * cson_cgi_get_session_key(cson_cgi_cx * cx)
{
    cson_object const * conf = cson_cgi_env_get_obj( cx, 'f', 0 );
    char const * sessKey = CSON_CGI_KEY_SESSION;
    assert( NULL != cx );
    if( conf )
    {
        cson_string const * k = cson_value_get_string( cson_object_get_sub( conf, "session.cookieName", '.' ) );
        char const * ck = k ? cson_string_cstr(k) : NULL;
        if( ck ) sessKey = ck;
    }
    return sessKey;
}

static int cson_cgi_gen_session_id(cson_cgi_cx * cx)
{
    char buf[37] = {0};
    if( cx->session.id )
    {
        free( cx->session.id );
        cx->session.id = NULL;
    }
    cson_cgi_generate_uuid( cx, buf );
    cx->session.id = cson_cgi_strdup( buf );
    return ( NULL == cx->session.id )
        ? cson_rc.AllocError
        : 0;
}

static int cson_cgi_init_session( cson_cgi_cx * cx, char const * forceID )
{
    char const * idstr;
    char const * sessKey;
    int rc = cson_cgi_init_session_mgr(cx);
    if( 0 != rc ) return rc;
    else if( NULL == cx->session.mgr ) return 0
        /* treat as non-fatal error */;
    sessKey = cson_cgi_get_session_key(cx);
    assert( sessKey && *sessKey );
    /* Try to get the session ID ... */
    idstr = (forceID && *forceID)
        ? forceID
        : cson_string_cstr( cson_value_get_string( cson_cgi_getenv( cx, "cegp", sessKey ) ) );
    if( NULL == idstr )
    { /* Generate a session ID but defer creation of the session
         object until the client does it. If they never use it,
         we won't bother saving the session.
      */
        rc = cson_cgi_gen_session_id(cx);
        if( 0 != rc ) return rc;
    }
    else
    { /* try to load the session */
        cson_value * sessV = NULL;
        free( cx->session.id );
        cx->session.id = cson_cgi_strdup( idstr );
        if( ! cx->session.id ) return cson_rc.AllocError;
        rc = cx->session.mgr->api->load( cx->session.mgr, &sessV,
                                         cx->session.id );
        if( (0 == rc) && sessV )
        {
            rc = cson_cgi_gc_add( cx, cson_cgi_keys.ENV_SESSION, sessV, 1 );
            if( 0 != rc )
            { /* almost certainly an alloc error */
                return rc;
            }
            cx->session.env.jval = sessV;
            cx->session.env.jobj = cson_value_get_object( sessV );
        }
        else
        {
            if( !forceID || !*forceID )
            {
                /* On load error, assume the session ID is
                   stale. Re-generate it to avoid potential future
                   collisions. This heuristic will cause us intermittent
                   grief when loading does not work for a second or three
                   due to network-related problems. Each time that
                   happens, the caller will lose his session.
                */
                rc = cson_cgi_gen_session_id(cx);
                if( 0 != rc ) return rc;
            }
        }
    }
    assert( NULL != cx->session.id );
    { /* make sure the session ID is set in the cookies and has an updated
         expiry time... */
        unsigned int expiry = 0;
        cson_object const * conf;
        cson_value * jstr = cson_value_new_string( cx->session.id,
                                                   strlen(cx->session.id) );
        if( ! jstr ) return cson_rc.AllocError;
        conf = cson_cgi_env_get_obj( cx, 'f', 0 );
        if( conf )
        {
            expiry = cson_value_get_integer( cson_object_get_sub( conf, "session.cookieLifetimeMinutes", '.' ) );
            if( expiry ) expiry *= 60 /* convert to seconds */;
        }
        if( ! expiry )
        {
            expiry = (60*60*24);
        }
        expiry += (unsigned int)time(NULL);
        
        rc = cson_cgi_cookie_set2( cx, sessKey, jstr,
                                   NULL, NULL,
                                   expiry,
                                   0/*FIXME: set 'secure' option in HTTPS mode.*/,
                                   0/*FIXME: make the httponly flag configurable*/ );
        if( 0 != rc )
        {
            cson_value_free( jstr );
            if( cson_rc.AllocError == rc ) return rc;
            rc = 0 /* else treat as non-fatal */;
        }
    }
    return rc;
}



int cson_cgi_init(cson_cgi_cx * cx, int argc, char const * const * argv, cson_cgi_init_opt * opt )
{
    int rc = 0;
    static int hasInited = 0;
    if( NULL == cx ) return cson_rc.ArgError;
    else if( NULL != cx->gc.jval )
    { /* we've already done this or object was mal-initialized... */
        return cson_rc.ArgError;
    }

    assert( NULL != CSON_CGI_GETENV_DEFAULT );

#if CSON_CGI_USE_SIGNALS
    {
        /* FIXME: use sigaction() instead of signal() */
        typedef void (*sighnd)(int);
        sighnd oldSigPipe;
        oldSigPipe = signal(SIGPIPE, SIG_IGN) /* to try avoid unclean termination if client disconnects. */;
        if( SIG_ERR == oldSigPipe )
        {
            return cson_rc.UnknownError;
        }
    }
#endif

    if( ! hasInited )
    {
        hasInited = 1;
        setlocale( LC_ALL, "C" )
            /* supposedly important for underlying JSON parser.
               FIXME: only do this init once!
            */;
    }

    cx->gc.jval = cson_value_new_object();
    if( NULL == cx->gc.jval )
    {
        return cson_rc.AllocError;
    }
    cx->gc.jobj = cson_value_get_object( cx->gc.jval );
    assert( NULL != cx->gc.jobj );

    if( opt )
    {
        cx->opt = *opt;
    }
    if( NULL == cx->opt.inStream ) cx->opt.inStream = stdin;
    if( NULL == cx->opt.outStream ) cx->opt.outStream = stdout;
    if( NULL == cx->opt.errStream ) cx->opt.errStream = stderr;

#define CHECKRC if(rc) goto end
    rc = cson_cgi_import_environ(cx);
    CHECKRC;
    rc = cson_cgi_init_argv( cx, argc, argv );
    CHECKRC;
    if( cx->opt.configObj ){
        cson_value * configV = cson_object_value( cx->opt.configObj );
        rc = cson_cgi_gc_add( cx,cson_cgi_keys.ENV_CONFIG, configV, 0 );
        if(rc) return cson_rc.AllocError;
        cx->config.jobj = cx->opt.configObj;
        cx->opt.configObj = NULL;
        cx->config.jval = configV;
    }
    else { /* read config file */
        char const * conffile = cx->opt.configFile;
        if( ! conffile )
        {
            cson_value const * v = cson_cgi_getenv( cx, "e", "CSON_CGI_CONFIG" );
            if( v && cson_value_is_string(v) )
            {
                conffile = cson_string_cstr( cson_value_get_string( v ) );
            }
        }
        if( conffile )
        {
            cson_cgi_init_config( cx, conffile )
                /* Ignore error code.

                TODO:

                - use argv[0]+".json" as the default config file.
                */
                ;
        }
    }

    rc = cson_cgi_parse_query_string( cx, getenv("QUERY_STRING") );
    CHECKRC;
    rc = cson_cgi_parse_cookies( cx, getenv("HTTP_COOKIE") );
    CHECKRC;
    rc = cson_cgi_init_POST(cx);
    if( cson_rc.AllocError == rc ) goto end
        /* this can fail for several reasons which are non-fatal. */
        ;

    if( (NULL == opt) )
    {
        /* TODO: read these values from cx->config, if available. */
        cx->opt.outOpt.indentation = 1;
        cx->opt.outOpt.addNewline = 1;
        cx->opt.outOpt.addSpaceAfterColon = 1;
        cx->opt.outOpt.indentSingleMemberValues = 1;
    }

    rc = cson_cgi_init_session( cx, opt ? opt->sessionID : NULL )
        /* ignore non-OOM error codes. Not fatal. */;
    if( cson_rc.AllocError == rc ) goto end;
    else rc = 0;

    /*
      TODOs:

      - Read form-urlencoded POST data. (Do this BEFORE
      restoring the session, so that we can get the session
      ID from there if needed.)
    */
    end:
    return rc;
#undef CHECKRC
}

int cson_cgi_replace_POST( cson_cgi_cx * cx, cson_value * newPost )
{
    if(!cx || !newPost) return cson_rc.ArgError;
    else if( !cson_value_is_object(newPost) && !cson_value_is_array(newPost)){
        return cson_rc.TypeError;
    }
    else {
        int rc = cson_cgi_gc_add( cx, cson_cgi_keys.ENV_POST, newPost, 0 )
            /* will free up previous post data */
            ;
        if(rc) return rc;
        cx->request.post.jval = newPost;
        cx->request.post.jobj = cson_value_get_object( newPost )
            /* reminder: we currently "don't do" arrays, but eventually
               should.
            */
            ;
        return 0;
    }
}

int cson_cgi_replace_GET( cson_cgi_cx * cx, cson_object * newGet )
{
    if(!cx || !newGet) return cson_rc.ArgError;
    else {
        cson_value * v = cson_object_value(newGet);
        int rc = cson_cgi_gc_add( cx, cson_cgi_keys.ENV_GET, v, 0 )
            /* will free up previous GET data */
            ;
        if(rc) return rc;
        cx->request.get.jval = v;
        cx->request.get.jobj = newGet;
        return 0;
    }
}


#undef cson_cgi_env_map_empty_m
#undef CSON_CGI_USE_SIGNALS
#undef RNG_FILENAME
/* end file cgi/cson_cgi.c */
/* end file cson_amalgamation_cgi.c */