/* auto-generated! Do not edit! */
/* begin file cson_amalgamation_core.h */
/* auto-generated! Do not edit! */
/* begin file include/wh/cson/cson.h */
#if !defined(WANDERINGHORSE_NET_CSON_H_INCLUDED)
#define WANDERINGHORSE_NET_CSON_H_INCLUDED 1

/*#include <stdint.h> C99: fixed-size int types. */
#include <stdio.h> /* FILE decl */

/** @page page_cson cson JSON API

cson (pronounced "season") is an object-oriented C API for generating
and consuming JSON (http://www.json.org) data.

Its main claim to fame is that it can parse JSON from, and output it
to, damned near anywhere. The i/o routines use a callback function to
fetch/emit JSON data, allowing clients to easily plug in their own
implementations. Implementations are provided for string- and
FILE-based i/o.

Project home page: http://fossil.wanderinghorse.net/repos/cson

Author: Stephan Beal (http://www.wanderinghorse.net/home/stephan/)

License: Dual Public Domain/MIT

The full license text is at the bottom of the main header file
(cson.h).

Examples of how to use the library are scattered throughout
the API documentation, in the test.c file in the source repo,
and in the wiki on the project's home page.


*/

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
#  define CSON_ENABLE_UNIX 0
#else
#  define CSON_ENABLE_UNIX 1
#endif


/** @typedef some_long_int_type cson_int_t

Typedef for JSON-like integer types. This is (long long) where feasible,
otherwise (long).
*/
#ifdef _WIN32
typedef __int64 cson_int_t;
#define CSON_INT_T_SFMT "I64d"
#define CSON_INT_T_PFMT "I64d"
#elif (__STDC_VERSION__ >= 199901L) || (HAVE_LONG_LONG == 1)
typedef long long cson_int_t;
#define CSON_INT_T_SFMT "lld"
#define CSON_INT_T_PFMT "lld"
#else 
typedef long cson_int_t;
#define CSON_INT_T_SFMT "ld"
#define CSON_INT_T_PFMT "ld"
#endif

/** @typedef double_or_long_double cson_double_t

This is the type of double value used by the library.
It is only lightly tested with long double, and when using
long double the memory requirements for such values goes
up.
*/
#if 0
typedef long double cson_double_t;
#define CSON_DOUBLE_T_SFMT "Lf"
#define CSON_DOUBLE_T_PFMT "Lf"
#else
typedef double cson_double_t;
#define CSON_DOUBLE_T_SFMT "f"
#define CSON_DOUBLE_T_PFMT "f"
#endif

/** @def CSON_VOID_PTR_IS_BIG

ONLY define this to a true value if you know that

(sizeof(cson_int_t) <= sizeof(void*))

If that is the case, cson does not need to dynamically
allocate integers. However, enabling this may cause
compilation warnings in 32-bit builds even though the code
being warned about cannot ever be called. To get around such
warnings, when building on a 64-bit environment you can define
this to 1 to get "big" integer support. HOWEVER, all clients must
also use the same value for this macro. If i knew a halfway reliable
way to determine this automatically at preprocessor-time, i would
automate this. We might be able to do halfway reliably by looking
for a large INT_MAX value?
*/
#if !defined(CSON_VOID_PTR_IS_BIG)

/* Largely taken from http://predef.sourceforge.net/prearch.html

See also: http://poshlib.hookatooka.com/poshlib/trac.cgi/browser/posh.h
*/
#  if defined(_WIN64) || defined(__LP64__)/*gcc*/ \
    || defined(_M_X64) || defined(__amd64__) || defined(__amd64) \
    ||  defined(__x86_64__) || defined(__x86_64) \
    || defined(__ia64__) || defined(__ia64) || defined(_IA64) || defined(__IA64__) \
    || defined(_M_IA64) \
    || defined(__sparc_v9__) || defined(__sparcv9) || defined(_ADDR64) \
    || defined(__64BIT__)
#    define CSON_VOID_PTR_IS_BIG 1
#  else
#    define CSON_VOID_PTR_IS_BIG 0
#  endif
#endif

/** @def CSON_INT_T_SFMT

scanf()-compatible format token for cson_int_t.
*/

/** @def CSON_INT_T_PFMT

printf()-compatible format token for cson_int_t.
*/


/** @def CSON_DOUBLE_T_SFMT

scanf()-compatible format token for cson_double_t.
*/

/** @def CSON_DOUBLE_T_PFMT

printf()-compatible format token for cson_double_t.
*/

/**
    Type IDs corresponding to JavaScript/JSON types.

    These are only in the public API to allow O(1) client-side
    dispatching based on cson_value types.
*/
enum cson_type_id {
  /**
    The special "undefined" value constant.

    Its value must be 0 for internal reasons.
 */
 CSON_TYPE_UNDEF = 0,
 /**
    The special "null" value constant.
 */
 CSON_TYPE_NULL = 1,
 /**
    The bool value type.
 */
 CSON_TYPE_BOOL = 2,
 /**
    The integer value type, represented in this library
    by cson_int_t.
 */
 CSON_TYPE_INTEGER = 3,
 /**
    The double value type, represented in this library
    by cson_double_t.
 */
 CSON_TYPE_DOUBLE = 4,
 /** The immutable string type. This library stores strings
    as immutable UTF8.
 */
 CSON_TYPE_STRING = 5,
 /** The "Array" type. */
 CSON_TYPE_ARRAY = 6,
 /** The "Object" type. */
 CSON_TYPE_OBJECT = 7
};
/**
   Convenience typedef.
*/
typedef enum cson_type_id cson_type_id;


/**
   Convenience typedef.
*/
typedef struct cson_value cson_value;

/** @struct cson_value
   
   The core value type of this API. It is opaque to clients, and
   only the cson public API should be used for setting or
   inspecting their values.

   This class is opaque because stack-based usage can easily cause
   leaks if one does not intimately understand the underlying
   internal memory management (which sometimes changes).

   It is (as of 20110323) legal to insert a given value instance into
   multiple containers (they will share ownership using reference
   counting) as long as those insertions do not cause cycles. However,
   be very aware that such value re-use uses a reference to the
   original copy, meaning that if its value is changed once, it is
   changed everywhere. Also beware that multi-threaded write
   operations on such references leads to undefined behaviour.
   
   PLEASE read the ACHTUNGEN below...

   ACHTUNG #1:

   cson_values MUST NOT form cycles (e.g. via object or array
   entries).

   Not abiding th Holy Law Of No Cycles will lead to double-frees and
   the like (i.e. undefined behaviour, likely crashes due to infinite
   recursion or stepping on invalid (freed) pointers).

   ACHTUNG #2:
   
   ALL cson_values returned as non-const cson_value pointers from any
   public functions in the cson API are to be treated as if they are
   heap-allocated, and MUST be freed by client by doing ONE of:
   
   - Passing it to cson_value_free().
   
   - Adding it to an Object or Array, in which case the object/array
   takes over ownership. As of 20110323, a value may be inserted into
   a single container multiple times, or into multiple containers,
   in which case they all share ownership (via reference counting)
   of the original value (meaning any changes to it are visible in
   all references to it).
   
   Each call to cson_value_new_xxx() MUST eventually be followed up
   by one of those options.
   
   Some cson_value_new_XXX() implementations do not actually allocate
   memory, but this is an internal implementation detail. Client code
   MUST NOT rely on this behaviour and MUST treat each object
   returned by such a function as if it was a freshly-allocated copy
   (even if their pointer addresses are the same).
   
   ACHTUNG #3:

   Note that ACHTUNG #2 tells us that we must always free (or transfer
   ownership of) all pointers returned bycson_value_new_xxx(), but
   that two calls to (e.g.) cson_value_new_bool(1) will (or might)
   return the same address. The client must not rely on the
   "non-allocation" policy of such special cases, and must pass each
   returned value to cson_value_free(), even if two of them have the
   same address.  Some special values (e.g. null, true, false, integer
   0, double 0.0, and empty strings) use shared copies and in other
   places reference counting is used internally to figure out when it
   is safe to destroy an object.


   @see cson_value_new_array()
   @see cson_value_new_object()
   @see cson_value_new_string()
   @see cson_value_new_integer()
   @see cson_value_new_double()
   @see cson_value_new_bool()
   @see cson_value_true()
   @see cson_value_false()
   @see cson_value_null()
   @see cson_value_free()
   @see cson_value_type_id()
*/

/** @var cson_rc

   This object defines the error codes used by cson.

   Library routines which return int values almost always return a
   value from this structure. None of the members in this struct have
   published values except for the OK member, which has the value 0.
   All other values might be incidentally defined where clients
   can see them, but the numbers might change from release to
   release, so clients should only use the symbolic names.

   Client code is expected to access these values via the shared
   cson_rc object, and use them as demonstrated here:

   @code
   int rc = cson_some_func(...);
   if( 0 == rc ) {...success...}
   else if( cson_rc.ArgError == rc ) { ... some argument was wrong ... }
   else if( cson_rc.AllocError == rc ) { ... allocation error ... }
   ...
   @endcode
   
   The entries named Parse_XXX are generally only returned by
   cson_parse() and friends.
*/

/** @struct cson_rc_
   See \ref cson_rc for details.
*/
static const struct cson_rc_
{
    /** The generic success value. Guaranteed to be 0. */
    const int OK;
    /** Signifies an error in one or more arguments (e.g. NULL where it is not allowed). */
    const int ArgError;
    /** Signifies that some argument is not in a valid range. */
    const int RangeError;
    /** Signifies that some argument is not of the correct logical cson type. */
    const int TypeError;
    /** Signifies an input/ouput error. */
    const int IOError;
    /** Signifies an out-of-memory error. */
    const int AllocError;
    /** Signifies that the called code is "NYI" (Not Yet Implemented). */
    const int NYIError;
    /** Signifies that an internal error was triggered. If it happens, please report this as a bug! */
    const int InternalError;
    /** Signifies that the called operation is not supported in the
        current environment. e.g.  missing support from 3rd-party or
        platform-specific code.
    */
    const int UnsupportedError;
    /**
       Signifies that the request resource could not be found.
     */
    const int NotFoundError;
    /**
       Signifies an unknown error, possibly because an underlying
       3rd-party API produced an error and we have no other reasonable
       error code to convert it to.
     */
    const int UnknownError;
    /**
       Signifies that the parser found an unexpected character.
     */
    const int Parse_INVALID_CHAR;
    /**
       Signifies that the parser found an invalid keyword (possibly
       an unquoted string).
     */
    const int Parse_INVALID_KEYWORD;
    /**
       Signifies that the parser found an invalid escape sequence.
     */
    const int Parse_INVALID_ESCAPE_SEQUENCE;
    /**
       Signifies that the parser found an invalid Unicode character
       sequence.
     */
    const int Parse_INVALID_UNICODE_SEQUENCE;
    /**
       Signifies that the parser found an invalid numeric token.
     */
    const int Parse_INVALID_NUMBER;
    /**
       Signifies that the parser reached its maximum defined
       parsing depth before finishing the input.
     */
    const int Parse_NESTING_DEPTH_REACHED;
    /**
       Signifies that the parser found an unclosed object or array.
     */
    const int Parse_UNBALANCED_COLLECTION;
    /**
       Signifies that the parser found an key in an unexpected place.
     */
    const int Parse_EXPECTED_KEY;
    /**
       Signifies that the parser expected to find a colon but
       found none (e.g. between keys and values in an object).
     */
    const int Parse_EXPECTED_COLON;
} cson_rc = {
0/*OK*/,
1/*ArgError*/,
2/*RangeError*/,
3/*TypeError*/,
4/*IOError*/,
5/*AllocError*/,
6/*NYIError*/,
7/*InternalError*/,
8/*UnsupportedError*/,
9/*NotFoundError*/,
10/*UnknownError*/,
11/*Parse_INVALID_CHAR*/,
12/*Parse_INVALID_KEYWORD*/,
13/*Parse_INVALID_ESCAPE_SEQUENCE*/,
14/*Parse_INVALID_UNICODE_SEQUENCE*/,
15/*Parse_INVALID_NUMBER*/,
16/*Parse_NESTING_DEPTH_REACHED*/,
17/*Parse_UNBALANCED_COLLECTION*/,
18/*Parse_EXPECTED_KEY*/,
19/*Parse_EXPECTED_COLON*/
};

/**
   Returns the string form of the cson_rc code corresponding to rc, or
   some unspecified, non-NULL string if it is an unknown code.

   The returned bytes are static and do not changing during the
   lifetime of the application.
*/
char const * cson_rc_string(int rc);

/** @struct cson_parse_opt
   Client-configurable options for the cson_parse() family of
   functions.
*/
struct cson_parse_opt
{
    /**
       Maximum object/array depth to traverse.
    */
    unsigned short maxDepth;
    /**
       Whether or not to allow C-style comments.  Do not rely on this
       option being available. If the underlying parser is replaced,
       this option might no longer be supported.
    */
    char allowComments;
};
typedef struct cson_parse_opt cson_parse_opt;

/**
   Empty-initialized cson_parse_opt object.
*/
#define cson_parse_opt_empty_m { 25/*maxDepth*/, 0/*allowComments*/}


/**
   A class for holding JSON parser information. It is primarily
   intended for finding the position of a parse error.
*/
struct cson_parse_info
{
    /**
       1-based line number.
    */
    unsigned int line;
    /**
       0-based column number.
     */
    unsigned int col;

    /**
       Length, in bytes.
    */
    unsigned int length;
    
    /**
       Error code of the parse run (0 for no error).
    */
    int errorCode;

    /**
       The total number of object keys successfully processed by the
       parser.
    */
    unsigned int totalKeyCount;

    /**
       The total number of object/array values successfully processed
       by the parser, including the root node.
     */
    unsigned int totalValueCount;
};
typedef struct cson_parse_info cson_parse_info;

/**
   Empty-initialized cson_parse_info object.
*/
#define cson_parse_info_empty_m {1/*line*/,\
            0/*col*/,                                   \
            0/*length*/,                                \
            0/*errorCode*/,                             \
            0/*totalKeyCount*/,                         \
            0/*totalValueCount*/                        \
            }
/**
   Empty-initialized cson_parse_info object.
*/
extern const cson_parse_info cson_parse_info_empty;

/**
   Empty-initialized cson_parse_opt object.
*/
extern const cson_parse_opt cson_parse_opt_empty;

/**
    Client-configurable options for the cson_output() family of
    functions.
*/
struct cson_output_opt
{
    /**
       Specifies how to indent (or not) output. The values
       are:

       (0) == no extra indentation.
       
       (1) == 1 TAB character for each level.

       (>1) == that number of SPACES for each level.
    */
    unsigned char indentation;

    /**
       Maximum object/array depth to traverse. Traversing deeply can
       be indicative of cycles in the object/array tree, and this
       value is used to figure out when to abort the traversal.
    */
    unsigned short maxDepth;
    
    /**
       If true, a newline will be added to generated output,
       else not.
    */
    char addNewline;

    /**
       If true, a space will be added after the colon operator
       in objects' key/value pairs.
    */
    char addSpaceAfterColon;

    /**
       If set to 1 then objects/arrays containing only a single value
       will not indent an extra level for that value (but will indent
       on subsequent levels if that value contains multiple values).
    */
    char indentSingleMemberValues;

    /**
       The JSON format allows, but does not require, JSON generators
       to backslash-escape forward slashes. This option enables/disables
       that feature. According to JSON's inventor, Douglas Crockford:

       <quote>
       It is allowed, not required. It is allowed so that JSON can be
       safely embedded in HTML, which can freak out when seeing
       strings containing "</". JSON tolerates "<\/" for this reason.
       </quote>

       (from an email on 2011-04-08)

       The default value is 0 (because it's just damned ugly).
    */
    char escapeForwardSlashes;
};
typedef struct cson_output_opt cson_output_opt;

/**
   Empty-initialized cson_output_opt object.
*/
#define cson_output_opt_empty_m { 0/*indentation*/,\
            25/*maxDepth*/,                             \
            0/*addNewline*/,                            \
            0/*addSpaceAfterColon*/,                    \
            0/*indentSingleMemberValues*/,              \
            0/*escapeForwardSlashes*/                   \
            }

/**
   Empty-initialized cson_output_opt object.
*/
extern const cson_output_opt cson_output_opt_empty;

/**
   Typedef for functions which act as an input source for
   the cson JSON parser.

   The arguments are:

   - state: implementation-specific state needed by the function.

   - n: when called, *n will be the number of bytes the function
   should read and copy to dest. The function MUST NOT copy more than
   *n bytes to dest. Before returning, *n must be set to the number of
   bytes actually copied to dest. If that number is smaller than the
   original *n value, the input is assumed to be completed (thus this
   is not useful with non-blocking readers).

   - dest: the destination memory to copy the data do.

   Must return 0 on success, non-0 on error (preferably a value from
   cson_rc).

   The parser allows this routine to return a partial character from a
   UTF multi-byte character. The input routine does not need to
   concern itself with character boundaries.
*/
typedef int (*cson_data_source_f)( void * state, void * dest, unsigned int * n );

/**
   Typedef for functions which act as an output destination for
   generated JSON.

   The arguments are:

   - state: implementation-specific state needed by the function.

   - n: the length, in bytes, of src.

   - src: the source bytes which the output function should consume.
   The src pointer will be invalidated shortly after this function
   returns, so the implementation must copy or ignore the data, but not
   hold a copy of the src pointer.

   Must return 0 on success, non-0 on error (preferably a value from
   cson_rc).

   These functions are called relatively often during the JSON-output
   process, and should try to be fast.   
*/
typedef int (*cson_data_dest_f)( void * state, void const * src, unsigned int n );

/**
    Reads JSON-formatted string data (in ASCII, UTF8, or UTF16), using the
    src function to fetch all input. This function fetches each input character
    from the source function, which is calls like src(srcState, buffer, bufferSize),
    and processes them. If anything is not JSON-kosher then this function
    fails and returns one of the non-0 cson_rc codes.

    This function is only intended to read root nodes of a JSON tree, either
    a single object or a single array, containing any number of child elements.

    On success, *tgt is assigned the value of the root node of the
    JSON input, and the caller takes over ownership of that memory.
    On error, *tgt is not modified and the caller need not do any
    special cleanup, except possibly for the input source.


    The opt argument may point to an initialized cson_parse_opt object
    which contains any settings the caller wants. If it is NULL then
    default settings (the values defined in cson_parse_opt_empty) are
    used.

    The info argument may be NULL. If it is not NULL then the parser
    populates it with information which is useful in error
    reporting. Namely, it contains the line/column of parse errors.
    
    The srcState argument is ignored by this function but is passed on to src,
    so any output-destination-specific state can be stored there and accessed
    via the src callback.
    
    Non-parse error conditions include:

    - (!tgt) or !src: cson_rc.ArgError
    - cson_rc.AllocError can happen at any time during the input phase

    Here's a complete example of using a custom input source:

    @code
    // Internal type to hold state for a JSON input string.
    typedef struct
    {
        char const * str; // start of input string
        char const * pos; // current internal cursor position
        char const * end; // logical EOF (one-past-the-end)
    } StringSource;

    // cson_data_source_f() impl which uses StringSource.
    static int cson_data_source_StringSource( void * state, void * dest,
                                              unsigned int * n )
    {
        StringSource * ss = (StringSource*) state;
        unsigned int i;
        unsigned char * tgt = (unsigned char *)dest;
        if( ! ss || ! n || !dest ) return cson_rc.ArgError;
        else if( !*n ) return cson_rc.RangeError;
        for( i = 0;
             (i < *n) && (ss->pos < ss->end);
             ++i, ++ss->pos, ++tgt )
        {
             *tgt = *ss->pos;
        }
        *n = i;
        return 0;
    }

    ...
    // Now use StringSource together with cson_parse()
    StringSource ss;
    cson_value * root = NULL;
    char const * json = "{\"k1\":123}";
    ss.str = ss.pos = json;
    ss.end = json + strlen(json);
    int rc = cson_parse( &root, cson_data_source_StringSource, &ss, NULL, NULL );
    @endcode

    It is recommended that clients wrap such utility code into
    type-safe wrapper functions which also initialize the internal
    state object and check the user-provided parameters for legality
    before passing them on to cson_parse(). For examples of this, see
    cson_parse_FILE() or cson_parse_string().

    TODOs:

    - Buffer the input in larger chunks. We currently read
    byte-by-byte, but i'm too tired to write/test the looping code for
    the buffering.
    
    @see cson_parse_FILE()
    @see cson_parse_string()
*/
int cson_parse( cson_value ** tgt, cson_data_source_f src, void * srcState,
                cson_parse_opt const * opt, cson_parse_info * info );
/**
   A cson_data_source_f() implementation which requires the state argument
   to be a readable (FILE*) handle.
*/
int cson_data_source_FILE( void * state, void * dest, unsigned int * n );

/**
   Equivalent to cson_parse( tgt, cson_data_source_FILE, src, opt ).

   @see cson_parse_filename()
*/
int cson_parse_FILE( cson_value ** tgt, FILE * src,
                     cson_parse_opt const * opt, cson_parse_info * info );

/**
   Convenience wrapper around cson_parse_FILE() which opens the given filename.

   Returns cson_rc.IOError if the file cannot be opened.

   @see cson_parse_FILE()
*/
int cson_parse_filename( cson_value ** tgt, char const * src,
                         cson_parse_opt const * opt, cson_parse_info * info );

/**
   Uses an internal helper class to pass src through cson_parse().
   See that function for the return value and argument semantics.

   src must be a string containing JSON code, at least len bytes long,
   and the parser will attempt to parse exactly len bytes from src.

   If len is less than 2 (the minimum length of a legal top-node JSON
   object) then cson_rc.RangeError is returned.
*/
int cson_parse_string( cson_value ** tgt, char const * src, unsigned int len,
                       cson_parse_opt const * opt, cson_parse_info * info );



/**
   Outputs the given value as a JSON-formatted string, sending all
   output to the given callback function. It is intended for top-level
   objects or arrays, but can be used with any cson_value.

   If opt is NULL then default options (the values defined in
   cson_output_opt_empty) are used.

   If opt->maxDepth is exceeded while traversing the value tree,
   cson_rc.RangeError is returned.

   The destState parameter is ignored by this function and is passed
   on to the dest function.

   Returns 0 on success. On error, any amount of output might have been
   generated before the error was triggered.
   
   Example:

   @code
   int rc = cson_output( myValue, cson_data_dest_FILE, stdout, NULL );
   // basically equivalent to: cson_output_FILE( myValue, stdout, NULL );
   // but note that cson_output_FILE() actually uses different defaults
   // for the output options.
   @endcode
*/
int cson_output( cson_value const * src, cson_data_dest_f dest, void * destState, cson_output_opt const * opt );


/**
   A cson_data_dest_f() implementation which requires the state argument
   to be a writable (FILE*) handle.
*/
int cson_data_dest_FILE( void * state, void const * src, unsigned int n );

/**
   Almost equivalent to cson_output( src, cson_data_dest_FILE, dest, opt ),
   with one minor difference: if opt is NULL then the default options
   always include the addNewline option, since that is normally desired
   for FILE output.

   @see cson_output_filename()
*/
int cson_output_FILE( cson_value const * src, FILE * dest, cson_output_opt const * opt );
/**
   Convenience wrapper around cson_output_FILE() which writes to the given filename, destroying
   any existing contents. Returns cson_rc.IOError if the file cannot be opened.

   @see cson_output_FILE()
*/
int cson_output_filename( cson_value const * src, char const * dest, cson_output_opt const * fmt );

/**
   Returns the virtual type of v, or CSON_TYPE_UNDEF if !v.
*/
cson_type_id cson_value_type_id( cson_value const * v );


/** Returns true if v is null, v->api is NULL, or v holds the special undefined value. */
char cson_value_is_undef( cson_value const * v );
/** Returns true if v contains a null value. */
char cson_value_is_null( cson_value const * v );
/** Returns true if v contains a bool value. */
char cson_value_is_bool( cson_value const * v );
/** Returns true if v contains an integer value. */
char cson_value_is_integer( cson_value const * v );
/** Returns true if v contains a double value. */
char cson_value_is_double( cson_value const * v );
/** Returns true if v contains a number (double, integer) value. */
char cson_value_is_number( cson_value const * v );
/** Returns true if v contains a string value. */
char cson_value_is_string( cson_value const * v );
/** Returns true if v contains an array value. */
char cson_value_is_array( cson_value const * v );
/** Returns true if v contains an object value. */
char cson_value_is_object( cson_value const * v );

/** @struct cson_object

    cson_object is an opaque handle to an Object value.

    They are used like:

    @code
    cson_object * obj = cson_value_get_object(myValue);
    ...
    @endcode

    They can be created like:

    @code
    cson_value * objV = cson_value_new_object();
    cson_object * obj = cson_value_get_object(objV);
    // obj is owned by objV and objV must eventually be freed
    // using cson_value_free() or added to a container
    // object/array (which transfers ownership to that container).
    @endcode

    @see cson_value_new_object()
    @see cson_value_get_object()
    @see cson_value_free()
*/

typedef struct cson_object cson_object;

/** @struct cson_array

    cson_array is an opaque handle to an Array value.

    They are used like:

    @code
    cson_array * obj = cson_value_get_array(myValue);
    ...
    @endcode

    They can be created like:

    @code
    cson_value * arV = cson_value_new_array();
    cson_array * ar = cson_value_get_array(arV);
    // ar is owned by arV and arV must eventually be freed
    // using cson_value_free() or added to a container
    // object/array (which transfers ownership to that container).
    @endcode

    @see cson_value_new_array()
    @see cson_value_get_array()
    @see cson_value_free()

*/
typedef struct cson_array cson_array;

/** @struct cson_string

   cson-internal string type, opaque to client code. Strings in cson
   are immutable and allocated only by library internals, never
   directly by client code.

   The actual string bytes are to be allocated together in the same
   memory chunk as the cson_string object, which saves us 1 malloc()
   and 1 pointer member in this type (because we no longer have a
   direct pointer to the memory).

   Potential TODOs:

   @see cson_string_cstr()
*/
typedef struct cson_string cson_string;

/**
   Converts the given value to a boolean, using JavaScript semantics depending
   on the concrete type of val:

   undef or null: false
   
   boolean: same
   
   integer, double: 0 or 0.0 == false, else true
   
   object, array: true

   string: length-0 string is false, else true.

   Returns 0 on success and assigns *v (if v is not NULL) to either 0 or 1.
   On error (val is NULL) then v is not modified.
*/
int cson_value_fetch_bool( cson_value const * val, char * v );

/**
   Similar to cson_value_fetch_bool(), but fetches an integer value.

   The conversion, if any, depends on the concrete type of val:

   NULL, null, undefined: *v is set to 0 and 0 is returned.
   
   string, object, array: *v is set to 0 and
   cson_rc.TypeError is returned. The error may normally be safely
   ignored, but it is provided for those wanted to know whether a direct
   conversion was possible.

   integer: *v is set to the int value and 0 is returned.
   
   double: *v is set to the value truncated to int and 0 is returned.
*/
int cson_value_fetch_integer( cson_value const * val, cson_int_t * v );

/**
   The same conversions and return values as
   cson_value_fetch_integer(), except that the roles of int/double are
   swapped.
*/
int cson_value_fetch_double( cson_value const * val, cson_double_t * v );

/**
   If cson_value_is_string(val) then this function assigns *str to the
   contents of the string. str may be NULL, in which case this function
   functions like cson_value_is_string() but returns 0 on success.

   Returns 0 if val is-a string, else non-0, in which case *str is not
   modified.

   The bytes are owned by the given value and may be invalidated in any of
   the following ways:

   - The value is cleaned up or freed.

   - An array or object containing the value peforms a re-allocation
   (it shrinks or grows).

   And thus the bytes should be consumed before any further operations
   on val or any container which holds it.

   Note that this routine does not convert non-String values to their
   string representations. (Adding that ability would add more
   overhead to every cson_value instance.)
*/
int cson_value_fetch_string( cson_value const * val, cson_string ** str );

/**
   If cson_value_is_object(val) then this function assigns *obj to the underlying
   object value and returns 0, otherwise non-0 is returned and *obj is not modified.

   obj may be NULL, in which case this function works like cson_value_is_object()
   but with inverse return value semantics (0==success) (and it's a few
   CPU cycles slower).

   The *obj pointer is owned by val, and will be invalidated when val
   is cleaned up.

   Achtung: for best results, ALWAYS pass a pointer to NULL as the
   second argument, e.g.:

   @code
   cson_object * obj = NULL;
   int rc = cson_value_fetch_object( val, &obj );

   // Or, more simply:
   obj = cson_value_get_object( val );
   @endcode

   @see cson_value_get_object()
*/
int cson_value_fetch_object( cson_value const * val, cson_object ** obj );

/**
   Identical to cson_value_fetch_object(), but works on array values.

   @see cson_value_get_array()
*/
int cson_value_fetch_array( cson_value const * val, cson_array ** tgt );

/**
   Simplified form of cson_value_fetch_bool(). Returns 0 if val
   is NULL.
*/
char cson_value_get_bool( cson_value const * val );

/**
   Simplified form of cson_value_fetch_integer(). Returns 0 if val
   is NULL.
*/
cson_int_t cson_value_get_integer( cson_value const * val );

/**
   Simplified form of cson_value_fetch_double(). Returns 0.0 if val
   is NULL.
*/
cson_double_t cson_value_get_double( cson_value const * val );

/**
   Simplified form of cson_value_fetch_string(). Returns NULL if val
   is-not-a string value.
*/
cson_string * cson_value_get_string( cson_value const * val );

/**
   Returns a pointer to the NULL-terminated string bytes of str.
   The bytes are owned by string and will be invalided when it
   is cleaned up.

   If str is NULL then NULL is returned. If the string has a length
   of 0 then "" is returned.

   @see cson_string_length_bytes()
   @see cson_value_get_string()
*/
char const * cson_string_cstr( cson_string const * str );

/**
   Convenience function which returns the string bytes of
   the given value if it is-a string, otherwise it returns
   NULL. Note that this does no conversion of non-string types
   to strings.

   Equivalent to cson_string_cstr(cson_value_get_string(val)).
*/
char const * cson_value_get_cstr( cson_value const * val );

/**
   Equivalent to cson_string_cmp_cstr_n(lhs, cson_string_cstr(rhs), cson_string_length_bytes(rhs)).
*/
int cson_string_cmp( cson_string const * lhs, cson_string const * rhs );

/**
   Compares lhs to rhs using memcmp()/strcmp() semantics. Generically
   speaking it returns a negative number if lhs is less-than rhs, 0 if
   they are equivalent, or a positive number if lhs is greater-than
   rhs. It has the following rules for equivalence:

   - The maximum number of bytes compared is the lesser of rhsLen and
   the length of lhs. If the strings do not match, but compare equal
   up to the just-described comparison length, the shorter string is
   considered to be less-than the longer one.
   
   - If lhs and rhs are both NULL, or both have a length of 0 then they will
   compare equal.

   - If lhs is null/length-0 but rhs is not then lhs is considered to be less-than
   rhs.

   - If rhs is null/length-0 but lhs is not then rhs is considered to be less-than
   rhs.

   - i have no clue if the results are exactly correct for UTF strings.

*/
int cson_string_cmp_cstr_n( cson_string const * lhs, char const * rhs, unsigned int rhsLen );

/**
   Equivalent to cson_string_cmp_cstr_n( lhs, rhs, (rhs&&*rhs)?strlen(rhs):0 ).
*/
int cson_string_cmp_cstr( cson_string const * lhs, char const * rhs );

/**
   Returns the length, in bytes, of str, or 0 if str is NULL. This is
   an O(1) operation.

   TODO: add cson_string_length_chars() (is O(N) unless we add another
   member to store the char length).
   
   @see cson_string_cstr()
*/
unsigned int cson_string_length_bytes( cson_string const * str );

/**
    Returns the number of UTF8 characters in str. This value will
    be at most as long as cson_string_length_bytes() for the
    same string, and less if it has multi-byte characters.

    Returns 0 if str is NULL.
*/
unsigned int cson_string_length_utf8( cson_string const * str );

/**
   Like cson_value_get_string(), but returns a copy of the underying
   string bytes, which the caller owns and must eventually free
   using free().
*/
char * cson_value_get_string_copy( cson_value const * val );

/**
   Simplified form of cson_value_fetch_object(). Returns NULL if val
   is-not-a object value.
*/
cson_object * cson_value_get_object( cson_value const * val );

/**
   Simplified form of cson_value_fetch_array(). Returns NULL if val
   is-not-a array value.
*/
cson_array * cson_value_get_array( cson_value const * val );

/**
   Const-correct form of cson_value_get_array().
*/
cson_array const * cson_value_get_array_c( cson_value const * val );

/**
   If ar is-a array and is at least (pos+1) entries long then *v (if v is not NULL)
   is assigned to the value at that position (which may be NULL).

   Ownership of the *v return value is unchanged by this call. (The
   containing array may share ownership of the value with other
   containers.)

   If pos is out of range, non-0 is returned and *v is not modified.

   If v is NULL then this function returns 0 if pos is in bounds, but does not
   otherwise return a value to the caller.
*/
int cson_array_value_fetch( cson_array const * ar, unsigned int pos, cson_value ** v );

/**
   Simplified form of cson_array_value_fetch() which returns NULL if
   ar is NULL, pos is out of bounds or if ar has no element at that
   position.
*/
cson_value * cson_array_get( cson_array const * ar, unsigned int pos );

/**
   Ensures that ar has allocated space for at least the given
   number of entries. This never shrinks the array and never
   changes its logical size, but may pre-allocate space in the
   array for storing new (as-yet-unassigned) values.

   Returns 0 on success, or non-zero on error:

   - If ar is NULL: cson_rc.ArgError

   - If allocation fails: cson_rc.AllocError
*/
int cson_array_reserve( cson_array * ar, unsigned int size );

/**
   If ar is not NULL, sets *v (if v is not NULL) to the length of the array
   and returns 0. Returns cson_rc.ArgError if ar is NULL.
*/
int cson_array_length_fetch( cson_array const * ar, unsigned int * v );

/**
   Simplified form of cson_array_length_fetch() which returns 0 if ar
   is NULL.
*/
unsigned int cson_array_length_get( cson_array const * ar );

/**
   Sets the given index of the given array to the given value.

   If ar already has an item at that index then it is cleaned up and
   freed before inserting the new item.

   ar is expanded, if needed, to be able to hold at least (ndx+1)
   items, and any new entries created by that expansion are empty
   (NULL values).

   On success, 0 is returned and ownership of v is transfered to ar.
  
   On error ownership of v is NOT modified, and the caller may still
   need to clean it up. For example, the following code will introduce
   a leak if this function fails:

   @code
   cson_array_append( myArray, cson_value_new_integer(42) );
   @endcode

   Because the value created by cson_value_new_integer() has no owner
   and is not cleaned up. The "more correct" way to do this is:

   @code
   cson_value * v = cson_value_new_integer(42);
   int rc = cson_array_append( myArray, v );
   if( 0 != rc ) {
      cson_value_free( v );
      ... handle error ...
   }
   @endcode

*/
int cson_array_set( cson_array * ar, unsigned int ndx, cson_value * v );

/**
   Appends the given value to the given array, transfering ownership of
   v to ar. On error, ownership of v is not modified. Ownership of ar
   is never changed by this function.

   This is functionally equivalent to
   cson_array_set(ar,cson_array_length_get(ar),v), but this
   implementation has slightly different array-preallocation policy
   (it grows more eagerly).
   
   Returns 0 on success, non-zero on error. Error cases include:

   - ar or v are NULL: cson_rc.ArgError

   - Array cannot be expanded to hold enough elements: cson_rc.AllocError.

   - Appending would cause a numeric overlow in the array's size:
   cson_rc.RangeError.  (However, you'll get an AllocError long before
   that happens!)

   On error ownership of v is NOT modified, and the caller may still
   need to clean it up. See cson_array_set() for the details.

*/
int cson_array_append( cson_array * ar, cson_value * v );


/**
   Creates a new cson_value from the given boolean value.

   Ownership of the new value is passed to the caller, who must
   eventually either free the value using cson_value_free() or
   inserting it into a container (array or object), which transfers
   ownership to the container. See the cson_value class documentation
   for more details.

   Semantically speaking this function Returns NULL on allocation
   error, but the implementation never actually allocates for this
   case. Nonetheless, it must be treated as if it were an allocated
   value.
*/
cson_value * cson_value_new_bool( char v );


/**
   Alias for cson_value_new_bool(v).
*/
cson_value * cson_new_bool(char v);

/**
   Returns the special JSON "null" value. When outputing JSON,
   its string representation is "null" (without the quotes).
   
   See cson_value_new_bool() for notes regarding the returned
   value's memory.
*/
cson_value * cson_value_null();

/**
   Equivalent to cson_value_new_bool(1).
*/
cson_value * cson_value_true();

/**
   Equivalent to cson_value_new_bool(0).
*/
cson_value * cson_value_false();

/**
   Semantically the same as cson_value_new_bool(), but for integers.
*/
cson_value * cson_value_new_integer( cson_int_t v );

/**
   Alias for cson_value_new_integer(v).
*/
cson_value * cson_new_int(cson_int_t v);

/**
   Semantically the same as cson_value_new_bool(), but for doubles.
*/
cson_value * cson_value_new_double( cson_double_t v );

/**
   Alias for cson_value_new_double(v).
*/
cson_value * cson_new_double(cson_double_t v);

/**
   Semantically the same as cson_value_new_bool(), but for strings.
   This creates a JSON value which copies the first n bytes of str.
   The string will automatically be NUL-terminated.
   
   Note that if str is NULL or n is 0, this function still
   returns non-NULL value representing that empty string.
   
   Returns NULL on allocation error.
   
   See cson_value_new_bool() for important information about the
   returned memory.
*/
cson_value * cson_value_new_string( char const * str, unsigned int n );

/**
   Allocates a new "object" value and transfers ownership of it to the
   caller. It must eventually be destroyed, by the caller or its
   owning container, by passing it to cson_value_free().

   Returns NULL on allocation error.

   Post-conditions: cson_value_is_object(value) will return true.

   @see cson_value_new_array()
   @see cson_value_free()
*/
cson_value * cson_value_new_object();

/**
   This works like cson_value_new_object() but returns an Object
   handle directly.

   The value handle for the returned object can be fetched with
   cson_object_value(theObject).
   
   Ownership is transfered to the caller, who must eventually free it
   by passing the Value handle (NOT the Object handle) to
   cson_value_free() or passing ownership to a parent container.

   Returns NULL on error (out of memory).
*/
cson_object * cson_new_object();

/**
   Identical to cson_new_object() except that it creates
   an Array.
*/
cson_array * cson_new_array();

/**
   Identical to cson_new_object() except that it creates
   a String.
*/
cson_string * cson_new_string(char const * val, unsigned int len);

/**
   Equivalent to cson_value_free(cson_object_value(x)).
*/
void cson_free_object(cson_object *x);

/**
   Equivalent to cson_value_free(cson_array_value(x)).
*/
void cson_free_array(cson_array *x);

/**
   Equivalent to cson_value_free(cson_string_value(x)).
*/
void cson_free_string(cson_string *x);


/**
   Allocates a new "array" value and transfers ownership of it to the
   caller. It must eventually be destroyed, by the caller or its
   owning container, by passing it to cson_value_free().

   Returns NULL on allocation error.

   Post-conditions: cson_value_is_array(value) will return true.

   @see cson_value_new_object()
   @see cson_value_free()
*/
cson_value * cson_value_new_array();

/**
   Frees any resources owned by v, then frees v. If v is a container
   type (object or array) its children are also freed (recursively).

   If v is NULL, this is a no-op.

   This function decrements a reference count and only destroys the
   value if its reference count drops to 0. Reference counts are
   increased by either inserting the value into a container or via
   cson_value_add_reference(). Even if this function does not
   immediately destroy the value, the value must be considered, from
   the perspective of that client code, to have been
   destroyed/invalidated by this call.

   
   @see cson_value_new_object()
   @see cson_value_new_array()
   @see cson_value_add_reference()
*/
void cson_value_free(cson_value * v);

/**
   Alias for cson_value_free().
*/
void cson_free_value(cson_value * v);


/**
   Functionally similar to cson_array_set(), but uses a string key
   as an index. Like arrays, if a value already exists for the given key,
   it is destroyed by this function before inserting the new value.

   If v is NULL then this call is equivalent to
   cson_object_unset(obj,key). Note that (v==NULL) is treated
   differently from v having the special null value. In the latter
   case, the key is set to the special null value.

   The key may be encoded as ASCII or UTF8. Results are undefined
   with other encodings, and the errors won't show up here, but may
   show up later, e.g. during output.
   
   Returns 0 on success, non-0 on error. It has the following error
   cases:

   - cson_rc.ArgError: obj or key are NULL or strlen(key) is 0.

   - cson_rc.AllocError: an out-of-memory error

   On error ownership of v is NOT modified, and the caller may still
   need to clean it up. For example, the following code will introduce
   a leak if this function fails:

   @code
   cson_object_set( myObj, "foo", cson_value_new_integer(42) );
   @endcode

   Because the value created by cson_value_new_integer() has no owner
   and is not cleaned up. The "more correct" way to do this is:

   @code
   cson_value * v = cson_value_new_integer(42);
   int rc = cson_object_set( myObj, "foo", v );
   if( 0 != rc ) {
      cson_value_free( v );
      ... handle error ...
   }
   @endcode

   Potential TODOs:

   - Add an overload which takes a cson_value key instead. To get
   any value out of that we first need to be able to convert arbitrary
   value types to strings. We could simply to-JSON them and use those
   as keys.
*/
int cson_object_set( cson_object * obj, char const * key, cson_value * v );

/**
   Functionaly equivalent to cson_object_set(), but takes a
   cson_string() as its KEY type. The string will be reference-counted
   like any other values, and the key may legally be used within this
   same container (as a value) or others (as a key or value) at the
   same time.

   Returns 0 on success. On error, ownership (i.e. refcounts) of key
   and value are not modified. On success key and value will get
   increased refcounts unless they are replacing themselves (which is
   a harmless no-op).
*/
int cson_object_set_s( cson_object * obj, cson_string * key, cson_value * v );

/**
   Removes a property from an object.
   
   If obj contains the given key, it is removed and 0 is returned. If
   it is not found, cson_rc.NotFoundError is returned (which can
   normally be ignored by client code).

   cson_rc.ArgError is returned if obj or key are NULL or key has
   a length of 0.

   Returns 0 if the given key is found and removed.

   This is functionally equivalent calling
   cson_object_set(obj,key,NULL).
*/
int cson_object_unset( cson_object * obj, char const * key );

/**
   Searches the given object for a property with the given key. If found,
   it is returned. If no match is found, or any arguments are NULL, NULL is
   returned. The returned object is owned by obj, and may be invalidated
   by ANY operations which change obj's property list (i.e. add or remove
   properties).

   FIXME: allocate the key/value pairs like we do for cson_array,
   to get improve the lifetimes of fetched values.

   @see cson_object_fetch_sub()
   @see cson_object_get_sub()
*/
cson_value * cson_object_get( cson_object const * obj, char const * key );

/**
   Equivalent to cson_object_get() but takes a cson_string argument
   instead of a C-style string.
*/
cson_value * cson_object_get_s( cson_object const * obj, cson_string const *key );

/**
   Similar to cson_object_get(), but removes the value from the parent
   object's ownership. If no item is found then NULL is returned, else
   the object (now owned by the caller or possibly shared with other
   containers) is returned.

   Returns NULL if either obj or key are NULL or key has a length
   of 0.

   This function reduces the returned value's reference count but has
   the specific property that it does not treat refcounts 0 and 1
   identically, meaning that the returned object may have a refcount
   of 0. This behaviour works around a corner-case where we want to
   extract a child element from its parent and then destroy the parent
   (which leaves us in an undesireable (normally) reference count
   state).
*/
cson_value * cson_object_take( cson_object * obj, char const * key );

/**
    Fetches a property from a child (or [great-]*grand-child) object.

    obj is the object to search.

    path is a delimited string, where the delimiter is the given
    separator character.

    This function searches for the given path, starting at the given object
    and traversing its properties as the path specifies. If a given part of the
    path is not found, then this function fails with cson_rc.NotFoundError.

    If it finds the given path, it returns the value by assiging *tgt
    to it.  If tgt is NULL then this function has no side-effects but
    will return 0 if the given path is found within the object, so it can be used
    to test for existence without fetching it.
    
    Returns 0 if it finds an entry, cson_rc.NotFoundError if it finds
    no item, and any other non-zero error code on a "real" error. Errors include:

   - obj or path are NULL: cson_rc.ArgError
    
    - separator is 0, or path is an empty string or contains only
    separator characters: cson_rc.RangeError

    - There is an upper limit on how long a single path component may
    be (some "reasonable" internal size), and cson_rc.RangeError is
    returned if that length is violated.

    
    Limitations:

    - It has no way to fetch data from arrays this way. i could
    imagine, e.g., a path of "subobj.subArray.0" for
    subobj.subArray[0], or "0.3.1" for [0][3][1]. But i'm too
    lazy/tired to add this.

    Example usage:
    

    Assume we have a JSON structure which abstractly looks like:

    @code
    {"subobj":{"subsubobj":{"myValue":[1,2,3]}}}
    @endcode

    Out goal is to get the value of myValue. We can do that with:

    @code
    cson_value * v = NULL;
    int rc = cson_object_fetch_sub( object, &v, "subobj.subsubobj.myValue", '.' );
    @endcode

    Note that because keys in JSON may legally contain a '.', the
    separator must be specified by the caller. e.g. the path
    "subobj/subsubobj/myValue" with separator='/' is equivalent the
    path "subobj.subsubobj.myValue" with separator='.'. The value of 0
    is not legal as a separator character because we cannot
    distinguish that use from the real end-of-string without requiring
    the caller to also pass in the length of the string.
   
    Multiple successive separators in the list are collapsed into a
    single separator for parsing purposes. e.g. the path "a...b...c"
    (separator='.') is equivalent to "a.b.c".

    @see cson_object_get_sub()
    @see cson_object_get_sub2()
*/
int cson_object_fetch_sub( cson_object const * obj, cson_value ** tgt, char const * path, char separator );

/**
   Similar to cson_object_fetch_sub(), but derives the path separator
   character from the first byte of the path argument. e.g. the
   following arg equivalent:

   @code
   cson_object_fetch_sub( obj, &tgt, "foo.bar.baz", '.' );
   cson_object_fetch_sub2( obj, &tgt, ".foo.bar.baz" );
   @endcode
*/
int cson_object_fetch_sub2( cson_object const * obj, cson_value ** tgt, char const * path );

/**
   Convenience form of cson_object_fetch_sub() which returns NULL if the given
   item is not found.
*/
cson_value * cson_object_get_sub( cson_object const * obj, char const * path, char sep );

/**
   Convenience form of cson_object_fetch_sub2() which returns NULL if the given
   item is not found.
*/
cson_value * cson_object_get_sub2( cson_object const * obj, char const * path );

/** @enum CSON_MERGE_FLAGS

    Flags for cson_object_merge().
*/
enum CSON_MERGE_FLAGS {
    CSON_MERGE_DEFAULT = 0,
    CSON_MERGE_REPLACE = 0x01,
    CSON_MERGE_NO_RECURSE = 0x02
};

/**
   "Merges" the src object's properties into dest. Each property in
   src is copied (using reference counting, not cloning) into dest. If
   dest already has the given property then behaviour depends on the
   flags argument:

   If flag has the CSON_MERGE_REPLACE bit set then this function will
   by default replace non-object properties with the src property. If
   src and dest both have the property AND it is an Object then this
   function operates recursively on those objects. If
   CSON_MERGE_NO_RECURSE is set then objects are not recursed in this
   manner, and will be completely replaced if CSON_MERGE_REPLACE is
   set.

   Array properties in dest are NOT recursed for merging - they are
   either replaced or left as-is, depending on whether flags contains
   he CSON_MERGE_REPLACE bit.

   Returns 0 on success. The error conditions are:

   - dest or src are NULL or (dest==src) returns cson_rc.ArgError.

   - dest or src contain cyclic references - this will likely cause a
   crash due to endless recursion.

   Potential TODOs:

   - Add a flag to copy clones, not the original values.
*/
int cson_object_merge( cson_object * dest, cson_object const * src, int flags );


/**
   An iterator type for traversing object properties.

   Its values must be considered private, not to be touched by client
   code.

   @see cson_object_iter_init()
   @see cson_object_iter_next()
*/
struct cson_object_iterator
{
    
    /** @internal
        The underlying object.
    */
    cson_object const * obj;
    /** @internal
        Current position in the property list.
     */
    unsigned int pos;
};
typedef struct cson_object_iterator cson_object_iterator;

/**
   Empty-initialized cson_object_iterator object.
*/
#define cson_object_iterator_empty_m {NULL/*obj*/,0/*pos*/}

/**
   Empty-initialized cson_object_iterator object.
*/
extern const cson_object_iterator cson_object_iterator_empty;

/**
   Initializes the given iterator to point at the start of obj's
   properties. Returns 0 on success or cson_rc.ArgError if !obj
   or !iter.

   obj must outlive iter, or results are undefined. Results are also
   undefined if obj is modified while the iterator is active.

   @see cson_object_iter_next()
*/
int cson_object_iter_init( cson_object const * obj, cson_object_iterator * iter );

/** @struct cson_kvp

This class represents a key/value pair and is used for storing
object properties. It is opaque to client code, and the public
API only uses this type for purposes of iterating over cson_object
properties using the cson_object_iterator interfaces.
*/

typedef struct cson_kvp cson_kvp;

/**
   Returns the next property from the given iterator's object, or NULL
   if the end of the property list as been reached.

   Note that the order of object properties is undefined by the API,
   and may change from version to version.

   The returned memory belongs to the underlying object and may be
   invalidated by any changes to that object.

   Example usage:

   @code
   cson_object_iterator it;
   cson_object_iter_init( myObject, &it ); // only fails if either arg is 0
   cson_kvp * kvp;
   cson_string const * key;
   cson_value const * val;
   while( (kvp = cson_object_iter_next(&it) ) )
   {
       key = cson_kvp_key(kvp);
       val = cson_kvp_value(kvp);
       ...
   }
   @endcode

   There is no need to clean up an iterator, as it holds no dynamic resources.
   
   @see cson_kvp_key()
   @see cson_kvp_value()
*/
cson_kvp * cson_object_iter_next( cson_object_iterator * iter );


/**
   Returns the key associated with the given key/value pair,
   or NULL if !kvp. The memory is owned by the object which contains
   the key/value pair, and may be invalidated by any modifications
   to that object.
*/
cson_string * cson_kvp_key( cson_kvp const * kvp );

/**
   Returns the value associated with the given key/value pair,
   or NULL if !kvp. The memory is owned by the object which contains
   the key/value pair, and may be invalidated by any modifications
   to that object.
*/
cson_value * cson_kvp_value( cson_kvp const * kvp );

/** @typedef some unsigned int type cson_size_t

*/
typedef unsigned int cson_size_t;

/**
   A generic buffer class.

   They can be used like this:

   @code
   cson_buffer b = cson_buffer_empty;
   int rc = cson_buffer_reserve( &buf, 100 );
   if( 0 != rc ) { ... allocation error ... }
   ... use buf.mem ...
   ... then free it up ...
   cson_buffer_reserve( &buf, 0 );
   @endcode

   To take over ownership of a buffer's memory:

   @code
   void * mem = b.mem;
   // mem is b.capacity bytes long, but only b.used
   // bytes of it has been "used" by the API.
   b = cson_buffer_empty;
   @endcode

   The memory now belongs to the caller and must eventually be
   free()d.
*/
struct cson_buffer
{
    /**
       The number of bytes allocated for this object.
       Use cson_buffer_reserve() to change its value.
     */
    cson_size_t capacity;
    /**
       The number of bytes "used" by this object. It is not needed for
       all use cases, and management of this value (if needed) is up
       to the client. The cson_buffer public API does not use this
       member. The intention is that this can be used to track the
       length of strings which are allocated via cson_buffer, since
       they need an explicit length and/or null terminator.
     */
    cson_size_t used;

    /**
       This is a debugging/metric-counting value
       intended to help certain malloc()-conscious
       clients tweak their memory reservation sizes.
       Each time cson_buffer_reserve() expands the
       buffer, it increments this value by 1.
    */
    cson_size_t timesExpanded;

    /**
       The memory allocated for and owned by this buffer.
       Use cson_buffer_reserve() to change its size or
       free it. To take over ownership, do:

       @code
       void * myptr = buf.mem;
       buf = cson_buffer_empty;
       @endcode

       (You might also need to store buf.used and buf.capacity,
       depending on what you want to do with the memory.)
       
       When doing so, the memory must eventually be passed to free()
       to deallocate it.
    */
    unsigned char * mem;
};
/** Convenience typedef. */
typedef struct cson_buffer cson_buffer;

/** An empty-initialized cson_buffer object. */
#define cson_buffer_empty_m {0/*capacity*/,0/*used*/,0/*timesExpanded*/,NULL/*mem*/}
/** An empty-initialized cson_buffer object. */
extern const cson_buffer cson_buffer_empty;

/**
   Uses cson_output() to append all JSON output to the given buffer
   object. The semantics for the (v, opt) parameters, and the return
   value, are as documented for cson_output(). buf must be a non-NULL
   pointer to a properly initialized buffer (see example below).

   Ownership of buf is not changed by calling this.

   On success 0 is returned and the contents of buf.mem are guaranteed
   to be NULL-terminated. On error the buffer might contain partial
   contents, and it should not be used except to free its contents.

   On error non-zero is returned. Errors include:

   - Invalid arguments: cson_rc.ArgError

   - Buffer cannot be expanded (runs out of memory): cson_rc.AllocError
   
   Example usage:

   @code
   cson_buffer buf = cson_buffer_empty;
   // optional: cson_buffer_reserve(&buf, 1024 * 10);
   int rc = cson_output_buffer( myValue, &buf, NULL );
   if( 0 != rc ) {
       ... error! ...
   }
   else {
       ... use buffer ...
       puts((char const*)buf.mem);
   }
   // In both cases, we eventually need to clean up the buffer:
   cson_buffer_reserve( &buf, 0 );
   // Or take over ownership of its memory:
   {
       char * mem = (char *)buf.mem;
       buf = cson_buffer_empty;
       ...
       free(mem);
   }
   @endcode
   
   @see cson_output()
   
*/
int cson_output_buffer( cson_value const * v, cson_buffer * buf,
                        cson_output_opt const * opt );

/**
   This works identically to cson_parse_string(), but takes a
   cson_buffer object as its input.  buf->used bytes of buf->mem are
   assumed to be valid JSON input, but it need not be NUL-terminated
   (we only read up to buf->used bytes). The value of buf->used is
   assumed to be the "string length" of buf->mem, i.e. not including
   the NUL terminator.

   Returns 0 on success, non-0 on error.

   See cson_parse() for the semantics of the tgt, opt, and err
   parameters.
*/
int cson_parse_buffer( cson_value ** tgt, cson_buffer const * buf,
                       cson_parse_opt const * opt, cson_parse_info * err );


/**
   Reserves the given amount of memory for the given buffer object.

   If n is 0 then buf->mem is freed and its state is set to
   NULL/0 values.

   If buf->capacity is less than or equal to n then 0 is returned and
   buf is not modified.

   If n is larger than buf->capacity then buf->mem is (re)allocated
   and buf->capacity contains the new length. Newly-allocated bytes
   are filled with zeroes.

   On success 0 is returned. On error non-0 is returned and buf is not
   modified.

   buf->mem is owned by buf and must eventually be freed by passing an
   n value of 0 to this function.

   buf->used is never modified by this function unless n is 0, in which case
   it is reset.
*/
int cson_buffer_reserve( cson_buffer * buf, cson_size_t n );

/**
   Fills all bytes of the given buffer with the given character.
   Returns the number of bytes set (buf->capacity), or 0 if
   !buf or buf has no memory allocated to it.
*/
cson_size_t cson_buffer_fill( cson_buffer * buf, char c );

/**
    Uses a cson_data_source_f() function to buffer input into a
    cson_buffer.

   dest must be a non-NULL, initialized (though possibly empty)
   cson_buffer object. Its contents, if any, will be overwritten by
   this function, and any memory it holds might be re-used.

   The src function is called, and passed the state parameter, to
   fetch the input. If it returns non-0, this function returns that
   error code. src() is called, possibly repeatedly, until it reports
   that there is no more data.

   Whether or not this function succeeds, dest still owns any memory
   pointed to by dest->mem, and the client must eventually free it by
   calling cson_buffer_reserve(dest,0).

   dest->mem might (and possibly will) be (re)allocated by this
   function, so any pointers to it held from before this call might be
   invalidated by this call.
   
   On error non-0 is returned and dest has almost certainly been
   modified but its state must be considered incomplete.

   Errors include:

   - dest or src are NULL (cson_rc.ArgError)

   - Allocation error (cson_rc.AllocError)

   - src() returns an error code

   Whether or not the state parameter may be NULL depends on
   the src implementation requirements.

   On success dest will contain the contents read from the input
   source. dest->used will be the length of the read-in data, and
   dest->mem will point to the memory. dest->mem is automatically
   NUL-terminated if this function succeeds, but dest->used does not
   count that terminator. On error the state of dest->mem must be
   considered incomplete, and is not guaranteed to be NUL-terminated.

    Example usage:

    @code
    cson_buffer buf = cson_buffer_empty;
    int rc = cson_buffer_fill_from( &buf,
                                    cson_data_source_FILE,
                                    stdin );
    if( rc )
    {
        fprintf(stderr,"Error %d (%s) while filling buffer.\n",
                rc, cson_rc_string(rc));
        cson_buffer_reserve( &buf, 0 );
        return ...;
    }
    ... use the buf->mem ...
    ... clean up the buffer ...
    cson_buffer_reserve( &buf, 0 );
    @endcode

    To take over ownership of the buffer's memory, do:

    @code
    void * mem = buf.mem;
    buf = cson_buffer_empty;
    @endcode

    In which case the memory must eventually be passed to free() to
    free it.    
*/
int cson_buffer_fill_from( cson_buffer * dest, cson_data_source_f src, void * state );


/**
   Increments the reference count for the given value. This is a
   low-level operation and should not normally be used by client code
   without understanding exactly what side-effects it introduces.
   Mis-use can lead to premature destruction or cause a value instance
   to never be properly destructed (i.e. a memory leak).

   This function is probably only useful for the following cases:

   - You want to hold a reference to a value which is itself contained
   in one or more containers, and you need to be sure that your
   reference outlives the container(s) and/or that you can free your
   copy of the reference without invaliding any references to the same
   value held in containers.

   - You want to implement "value sharing" behaviour without using an
   object or array to contain the shared value. This can be used to
   ensure the lifetime of the shared value instance. Each sharing
   point adds a reference and simply passed the value to
   cson_value_free() when they're done. The object will be kept alive
   for other sharing points which added a reference.

   Normally any such value handles would be invalidated when the
   parent container(s) is/are cleaned up, but this function can be
   used to effectively delay the cleanup.
   
   This function, at its lowest level, increments the value's
   reference count by 1.

   To decrement the reference count, pass the value to
   cson_value_free(), after which the value must be considered, from
   the perspective of that client code, to be destroyed (though it
   will not be if there are still other live references to
   it). cson_value_free() will not _actually_ destroy the value until
   its reference count drops to 0.

   Returns 0 on success. The only error conditions are if v is NULL
   (cson_rc.ArgError) or if the reference increment would overflow
   (cson_rc.RangeError). In theory a client would get allocation
   errors long before the reference count could overflow (assuming
   those reference counts come from container insertions, as opposed
   to via this function).

   Insider notes which clients really need to know:
   
   For shared/constant value instances, such as those returned by
   cson_value_true() and cson_value_null(), this function has no side
   effects - it does not actually modify the reference count because
   (A) those instances are shared across all client code and (B) those
   objects are static and never get cleaned up. However, that is an
   implementation detail which client code should not rely on. In
   other words, if you call cson_value_add_reference() 3 times using
   the value returned by cson_value_true() (which is incidentally a
   shared cson_value instance), you must eventually call
   cson_value_free() 3 times to (semantically) remove those
   references. However, internally the reference count for that
   specific cson_value instance will not be modified and those
   objects will never be freed (they're stack-allocated).

   It might be interesting to note that newly-created objects
   have a reference count of 0 instead of 1. This is partly because
   if the initial reference is counted then it makes ownership
   problematic when inserting values into containers. e.g. consider the
   following code:

   @code
   // ACHTUNG: this code is hypothetical and does not reflect
   // what actually happens!
   cson_value * v =
        cson_value_new_integer( 42 ); // v's refcount = 1
   cson_array_append( myArray, v ); // v's refcount = 2
   @endcode

   If that were the case, the client would be forced to free his own
   reference after inserting it into the container (which is a bit
   counter-intuitive as well as intrusive). It would look a bit like
   the following and would have to be done after every create/insert
   operation:

   @code
   // ACHTUNG: this code is hypothetical and does not reflect
   // what actually happens!
   cson_array_append( myArray, v ); // v's refcount = 2
   cson_value_free( v ); // v's refcount = 1
   @endcode

   (As i said: it's counter-intuitive and intrusive.)

   Instead, values start with a refcount of 0 and it is only increased
   when the value is added to an object/array container or when this
   function is used to manually increment it. cson_value_free() treats
   a refcount of 0 or 1 equivalently, destroying the value
   instance. The only semantic difference between 0 and 1, for
   purposes of cleaning up, is that a value with a non-0 refcount has
   been had its refcount adjusted, whereas a 0 refcount indicates a
   fresh, "unowned" reference.
*/
int cson_value_add_reference( cson_value * v );

#if 0
/**
   DO NOT use this unless you know EXACTLY what you're doing.
   It is only in the public API to work around a couple corner
   cases involving extracting child elements and discarding
   their parents.

   This function sets v's reference count to the given value.
   It does not clean up the object if rc is 0.

   Returns 0 on success, non-0 on error.
*/
int cson_value_refcount_set( cson_value * v, unsigned short rc );
#endif

/**
   Deeply copies a JSON value, be it an object/array or a "plain"
   value (e.g. number/string/boolean). If cv is not NULL then this
   function makes a deep clone of it and returns that clone. Ownership
   of the clone is identical t transfered to the caller, who must
   eventually free the value using cson_value_free() or add it to a
   container object/array to transfer ownership to the container. The
   returned object will be of the same logical type as orig.

   ACHTUNG: if orig contains any cyclic references at any depth level
   this function will endlessly recurse. (Having _any_ cyclic
   references violates this library's requirements.)
   
   Returns NULL if orig is NULL or if cloning fails. Assuming that
   orig is in a valid state, the only "likely" error case is that an
   allocation fails while constructing the clone. In other words, if
   cloning fails due to something other than an allocation error then
   either orig is in an invalid state or there is a bug.

   When this function clones Objects or Arrays it shares any immutable
   values (including object keys) between the parent and the
   clone. Mutable values (Objects and Arrays) are copied, however.
   For example, if we clone:

   @code
   { a: 1, b: 2, c:["hi"] }
   @endcode

   The cloned object and the array "c" would be a new Object/Array
   instances but the object keys (a,b,b) and the values of (a,b), as
   well as the string value within the "c" array, would be shared
   between the original and the clone. The "c" array itself would be
   deeply cloned, such that future changes to the clone are not
   visible to the parent, and vice versa, but immutable values within
   the array are shared (in this case the string "hi"). The
   justification for this heuristic is that immutable values can never
   be changed, so there is no harm in sharing them across
   clones. Additionally, such types can never contribute to cycles in
   a JSON tree, so they are safe to share this way. Objects and
   Arrays, on the other hand, can be modified later and can contribute
   to cycles, and thus the clone needs to be an independent instance.
   Note, however, that if this function directly passed a
   non-Object/Array, that value is deeply cloned. The sharing
   behaviour only applies when traversing Objects/Arrays.
*/
cson_value * cson_value_clone( cson_value const * orig );

/**
   Returns the value handle associated with s. The handle itself owns
   s, and ownership of the handle is not changed by calling this
   function. If the returned handle is part of a container, calling
   cson_value_free() on the returned handle invoked undefined
   behaviour (quite possibly downstream when the container tries to
   use it).

   This function only returns NULL if s is NULL. The length of the
   returned string is cson_string_length_bytes().
*/
cson_value * cson_string_value(cson_string const * s);
/**
   The Object form of cson_string_value(). See that function
   for full details.
*/
cson_value * cson_object_value(cson_object const * s);

/**
   The Array form of cson_string_value(). See that function
   for full details.
*/
cson_value * cson_array_value(cson_array const * s);


/**
   Calculates the approximate in-memory-allocated size of v,
   recursively if it is a container type, with the following caveats
   and limitations:

   If a given value is reference counted then it is only and multiple
   times within a traversed container, each reference is counted at
   full cost. We have no way of knowing if a given reference has been
   visited already and whether it should or should not be counted, so
   we pessimistically count them even though the _might_ not really
   count for the given object tree (it depends on where the other open
   references live).

   This function returns 0 if any of the following are true:

   - v is NULL

   - v is one of the special singleton values (null, bools, empty
   string, int 0, double 0.0)

   All other values require an allocation, and this will return their
   total memory cost, including the cson-specific internals and the
   native value(s).

   Note that because arrays and objects might have more internal slots
   allocated than used, the alloced size of a container does not
   necessarily increase when a new item is inserted into it. An interesting
   side-effect of this is that when cson_clone()ing an array or object, the
   size of the clone can actually be less than the original.
*/
unsigned int cson_value_msize(cson_value const * v);

/**
   Parses command-line-style arguments into a JSON object.

   It expects arguments to be in any of these forms, and any number
   of leading dashes are treated identically:

   --key : Treats key as a boolean with a true value.

   --key=VAL : Treats VAL as either a double, integer, or string.

   --key= : Treats key as a JSON null (not literal NULL) value.

   Arguments not starting with a dash are skipped.
   
   Each key/value pair is inserted into an object.  If a given key
   appears more than once then only the final entry is actually
   stored.

   argc and argv are expected to be values from main() (or similar,
   possibly adjusted to remove argv[0]).

   tgt must be either a pointer to NULL or a pointer to a
   client-provided Object. If (NULL==*tgt) then this function
   allocates a new object and on success it stores the new object in
   *tgt (it is owned by the caller). If (NULL!=*tgt) then it is
   assumed to be a properly allocated object. DO NOT pass a pointer to
   an unitialized pointer, as that will fool this function into
   thinking it is a valid object and Undefined Behaviour will ensue.

   If count is not NULL then the number of arugments parsed by this
   function are assigned to it. On error, count will be the number of
   options successfully parsed before the error was encountered.

   On success:

   - 0 is returned.

   - If (*tgt==NULL) then *tgt is assigned to a newly-allocated
   object, owned by the caller. Note that even if no arguments are
   parsed, the object is still created.

   On error:

   - non-0 is returned

   - If (*tgt==NULL) then it is not modified.

   - If (*tgt!=NULL) (i.e., the caller provides his own object) then
   it might contain partial results.
*/
int cson_parse_argv_flags( int argc, char const * const * argv,
                           cson_object ** tgt, unsigned int * count );


/* LICENSE

This software's source code, including accompanying documentation and
demonstration applications, are licensed under the following
conditions...

Certain files are imported from external projects and have their own
licensing terms. Namely, the JSON_parser.* files. See their files for
their official licenses, but the summary is "do what you want [with
them] but leave the license text and copyright in place."

The author (Stephan G. Beal [http://wanderinghorse.net/home/stephan/])
explicitly disclaims copyright in all jurisdictions which recognize
such a disclaimer. In such jurisdictions, this software is released
into the Public Domain.

In jurisdictions which do not recognize Public Domain property
(e.g. Germany as of 2011), this software is Copyright (c) 2011 by
Stephan G. Beal, and is released under the terms of the MIT License
(see below).

In jurisdictions which recognize Public Domain property, the user of
this software may choose to accept it either as 1) Public Domain, 2)
under the conditions of the MIT License (see below), or 3) under the
terms of dual Public Domain/MIT License conditions described here, as
they choose.

The MIT License is about as close to Public Domain as a license can
get, and is described in clear, concise terms at:

    http://en.wikipedia.org/wiki/MIT_License

The full text of the MIT License follows:

--
Copyright (c) 2011 Stephan G. Beal (http://wanderinghorse.net/home/stephan/)

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

--END OF MIT LICENSE--

For purposes of the above license, the term "Software" includes
documentation and demonstration source code which accompanies
this software. ("Accompanies" = is contained in the Software's
primary public source code repository.)

*/

#if defined(__cplusplus)
} /*extern "C"*/
#endif

#endif /* WANDERINGHORSE_NET_CSON_H_INCLUDED */
/* end file include/wh/cson/cson.h */
/* end file cson_amalgamation_core.h */
/* begin file cson_amalgamation_session.h */
/* auto-generated! Do not edit! */
/* begin file include/wh/cson/cson_session.h */
#if !defined(WANDERINGHORSE_NET_CSON_SESSION_H_INCLUDED)
#define WANDERINGHORSE_NET_CSON_SESSION_H_INCLUDED 1

/** @page page_cson_session cson Session API

The cson_session API provides a small interface,
called cson_sessmgr, which defines the basic operations
needed for implementent persistent application state,
across application sessions, by storing the state as
JSON data in "some back-end storage." The exact underlying
storage is not specified by the interface, but two
implementations are provided by the library:

- File-based sessions.

- Database-based sessions, using libcpdo for connection
abstraction.

libcpdo is included, in full, in the cson source tree,
but can also be found on its web page:

    http://fossil.wanderinghorse.net/repos/cpdo/

@see cson_sessmgr_register()
@see cson_sessmgr_load()
@see cson_sessmgr_names()
@see cson_sessmgr
@see cson_sessmgr_api
*/


#if defined(__cplusplus)
extern "C" {
#endif

    typedef struct cson_sessmgr cson_sessmgr;
    typedef struct cson_sessmgr_api cson_sessmgr_api;

    /** @struct cson_sessmgr_api
        
       Defines operations required by "session managers." Session managers
       are responsible for loading and saving cson session information
       in the form of JSON data.

       @see cson_sessmgr
    */
    struct cson_sessmgr_api
    {
        /**
           Loads/creates a session object (JSON data). The
           implementation must use the given identifier for loading an
           existing session, creating the session if createIfNeeded is
           true and the session data is not found. If createIfNeeded
           is true then the implementation must create an Object for
           the session root, as opposed to an Array or other JSON
           value. Clients are allowed to use non-Objects as their
           sessions but doing so would seem to have no benefit, and is
           not recommended.

           If the given id cannot be found then cson_rc.NotFoundError
           must be returned. On success it must assign the root node
           of the session tree to *tgt and return 0.  On error *tgt
           must not be modified and non-zero must be returned.

           On success ownership of *tgt is transfered to the caller.

           Error conditions include:

           - self, tgt, or id are NULL: cson_rc.ArgError

           - id is "not valid" (the meaning of "valid" is
           implementation-dependent): cson_rc.ArgError

           The identifier string must be NUL-terminated. Its maximum
           length, if any, is implementation-dependent.
        */
        int (*load)( cson_sessmgr * self, cson_value ** tgt, char const * id );

        /**
           Must save the given JSON object tree to the underlying storage, using the given identifier
           as its unique key. It must overwrite any existing session with that same identifier.
        */
        int (*save)( cson_sessmgr * self, cson_value const * root, char const * id );

        /**
           Must remove all session data associated with the given id.

           Must return 0 on success, non-0 on error.
        */
        int (*remove)( cson_sessmgr * self, char const * id );
        /**
           Must free up any resources used by the self object and then
           free self. After calling this, further use of the self
           object invokes undefined behaviour.
        */
        void (*finalize)( cson_sessmgr * self );
    };

    /**
       cson_sessmgr is base interface type for concrete
       cson_sessmgr_api implementations.  Each holds a pointer to its
       underlying implementation and to implementation-private
       data.
    
       @see cson_sessmgr_register()
       @see cson_sessmgr_load()
       @see cson_sessmgr_names()
       @see cson_sessmgr_api
    */
    struct cson_sessmgr
    {
        /**
           The concrete implementation functions for this
           session manager instance.
        */
        const cson_sessmgr_api * api;
        /**
           Private implementation date for this session manager
           instance. It is owned by this object and will be freed when
           thisObject->api->finalize(thisObject) is called. Client
           code must never use nor rely on the type/contents of the
           memory stored here.
        */
        void * impl;
    };

    /**
       A typedef for factory functions which instantiate cson_sessmgr
       instances.

       The semantics are:

       - tgt must be a non-NULL pointer where the result object can be
       stored. If it is NULL, cson_rc.ArgError must be returned.

       - opt (configuration options) may or may not be required,
       depending on the manager. If it is required and not passed in,
       cson_rc.ArgError must be returned. If the config options are
       required but the passed-in object is missing certain values, or
       has incorrect values, the implementation may substitute
       sensible defaults (if possible) or return cson_rc.ArgError.

       - On error non-0 (one of the cson_rc values) must be returned
       and tgt must not be modified.

       - On success *tgt must be pointed to the new manager object,
       zero must be returned, and the caller takes over ownership of
       the *tgt value (and must eventually free it with
       obj->api->finalize(obj)).
    */
    typedef int (*cson_sessmgr_factory_f)( cson_sessmgr ** tgt, cson_object const * config );
    
#define cson_sessmgr_empty_m { NULL/*api*/, NULL/*impl*/ }

    /**
       Registers a session manager by name. The given name must be a
       NUL-terminaed string shorter than some internal limit
       (currently 32 bytes, including the trailing NUL). f must be a
       function conforming to the cson_sessmgr_factory_f() interface.

       On success returns 0.

       On error either one of the arguments was invalid, an entry with
       the given name was already found, or no space is left in the
       internal registration list. The API guarantees that at least 10
       slots are initially available, and it is not anticipated that
       more than a small handful of them will ever be used.

       This function is not threadsafe - do not register factories
       concurrently from multiple threads.

       By default the following registrations are (possibly)
       pre-installed:

       - "file" = cson_sessmgr_file()

       - "cpdo" = cson_sessmgr_cpdo() IF this library is compiled with
       the macro CSON_ENABLE_CPDO set to a true value. Exactly which
       databases are supported by that back-end (if any) are
       determined by how the cpdo library code is compiled.

       - "whio_ht" = cson_sessmgr_whio_ht() IF this library is compiled
       with whio support.

       - "whio_epfs" = cson_sessmgr_whio_epfs() IF this library is
       compiled with whio support.
    */
    int cson_sessmgr_register( char const * name, cson_sessmgr_factory_f f );

    /**
       A front-end to loading cson_sessmgr intances by their
       cson_session-conventional name. The first arguments must be a
       NUL-terminated string holding the name of the session manager
       driver. The other two arguments have the same semantics as for
       cson_sessmgr_factory_f(), so see that typedef's documentation
       regarding, e.g., ownership of the *tgt value.

       This function is thread-safe with regards to itself but not
       with regards to cson_sessmgr_register(). That is, it is legal
       to call this function concurrently from multiple threads,
       provided the arguments themselves are not being used
       concurrently. However, it is not safe to call this function
       when cson_sessmgr_register() is being called from another
       thread, as that function modifies the lookup table used by this
       function.

       On success 0 is returned and the ownership of *tgt is as
       documented for cson_sessmgr_factory_f(). On error non-0 is
       returned and tgt is not modified.
    */
    int cson_sessmgr_load( char const * name, cson_sessmgr ** tgt, cson_object const * opt );

    /**
       Returns the list of session managers registered via
       cson_sessmgr_register(). This function is not thread-safe in
       conjunction with cson_sessmgr_register(), and results are
       undefined if that function is called while this function is
       called or the results of this function call are being used.

       The returned array is never NULL but has a NULL as its final
       entry.

       Example usage:

       @code
       char const * const * mgr = cson_sessmgr_names();
       for( ; *mgr; ++mgr ) puts( *mgr );
       @endcode
    */
    char const * const * cson_sessmgr_names();
    
    /**
       A cson_sessmgr_factory_f() implementation which returns a new
       session manager which uses local files for storage.

       tgt must be a non-NULL pointer where the result can be stored.
       
       The opt object may be NULL or may be a configuration object
       with the following structure:

       @code
       {
       dir: string (directory to store session files in),
       prefix: string (prefix part of filename),
       suffix: string (file extension, including leading '.')
       }
       @endcode

       Any missing options will assume (unspecified) default values.
       This routine does not ensure the validity of the option values,
       other than to make sure they are strings.

       The returned object is owned by the caller, who must eventually
       free it using obj->api->finalize(obj). If it returns NULL,
       the error was an out-of-memory condition or tgt was NULL.

       On error non-0 is returned, but the only error conditions are
       allocation errors and (NULL==tgt), which will return
       cson_rc.AllocError resp. cson_rc.ArgError.

       Threading notes:

       - As long as no two operations on these manager instances use
       the same JSON object and/or session ID at the same time,
       multi-threaded usage should be okay. All save()/load()/remove()
       data is local to those operations, with the exception of the
       input arguments (which must not be used concurrently to those
       calls).

       Storage locking:

       - No locking of input/output files is done, under the
       assumption that only one thread/process will be using a given
       session ID (which should, after all, be unique world-wide).  If
       sessions will only be read, not written, there is little danger
       of something going wrong vis-a-vis locking (assuming the
       session files exists and can be read).

       TODO:

       - Add a config option to enable storage locking. If we'll
       re-implement this to use the whio API under the hood then we
       could use the (slightly simpler) whio_lock API for this.
    */
    int cson_sessmgr_file( cson_sessmgr ** tgt, cson_object const * opt );

    /**
       This is only available if cson is compiled with cpdo support.

       Implements the cson_sessmgr_factory_f() interface.
       
       This function tries to create a database connection using the options
       supplied in the opt object. The opt object must structurarly look like:

       @code
       {
       "dsn": "cpdo dsn string",
       "user": "string",
       "password": "string",
       "table": "table_name_where_sessions_are_stored",
       "fieldId": "field_name_for_session_id (VARCHAR/STRING)",
       "fieldTimestamp": "field_name_for_last_saved_timestamp (INTEGER)",
       "fieldSession": "field_name_for_session_data (TEXT)"
       }
       @endcode

       On success it returns 0 and sets *tgt to the new session manager,
       which is owned by the caller and must eventually be freed by calling
       obj->api->finalize(obj).

       This function can fail for any number of reasons:

       - Any parameters are NULL (cson_rc.ArgError).

       - cpdo cannot connect to the given DSN with the given
       username/password. Any error in establishing a connection causes
       cson_rc.IOError to be returned, as opposed to the underlying
       cpdo error code.

       - Any of the "table" or "fieldXXX" properties are NULL. It
       needs these data in order to know where to load/save sessions.


       If any required options are missing, cson_rc.ArgError is
       returned.

       TODO: add option "preferBlob", which can be used to set the db
       field type preference for the fieldSession field to
       blob. Currently it prefers string but will try blob operations
       if string ops fail.  Blobs have the disadvantage of much larger
       encoded sizes but the advantage that the JSON data is encoded
       (at least by sqlite3) as a hex number stream, making it
       unreadable to casual observers.       

       @endcode
    */
    int cson_sessmgr_cpdo( cson_sessmgr ** tgt, cson_object const * opt );

    /**
       This cson_sessmgr_factory_f() implementation might or might not
       be compiled in, depending on the mood of the cson
       maintainer. It is very niche-market, and primarily exists just
       to test (and show off) the whio_ht code.

       It uses libwhio's on-storage hashtable (called whio_ht)
       as the underlying storage:
       
       http://fossil.wanderinghorse.net/repos/whio/index.cgi/wiki/whio_ht

       The opt object must not be NULL and must contain a single
       string property named "file" which contains the path to the
       whio_ht file to use for sessions. That file must have been
       previously created, either programatically using the whio_ht
       API or using whio-ht-tool:

       http://fossil.wanderinghorse.net/repos/whio/index.cgi/wiki/whio_ht_tool

       See cson_sessmgr_factory_f() for the semantics of the tgt
       argument and the return value.

       Threading notes:

       While the underlying hashtable supports a client-defined mutex,
       this usage of it does not set one (because we have no default
       one to use). What this means for clients is that they must not
       use this session manager from multiple threads, nor may they
       use multiple instances in the same process which use the same
       underlying hashtable file from multiple threads. How best to
       remedy this (allowing the client to tell this API what mutex to
       use) is not yet clear. Maybe a global whio_mutex object which
       the client must initialize before instantiating these session
       managers.
       
       Storage Locking:

       If the underlying filesystem reports that it supports file
       locking (via the whio_lock API, basically meaning POSIX
       fcntl()-style locking) the the session manager will use it. For
       the load() operation a read lock is acquired and for
       save()/remove() a write lock. The operations will fail if
       locking fails, the exception being if the device reports that
       it doesn't support locking, in which case we optimistically
       save/load/remove without locking.

       Remember that in POSIX-style locking, a single process does not
       see its own locks and can overwrite locks set via other
       threads. This means that multi-threaded use of a given
       instance, or multiple instances in the same process using the
       same underlying hashtable file, will likely eventually corrupt
       the hashtable.

       TODO:

       - Add a config option to disable storage locking, for clients
       who really don't want to use it.
    */
    int cson_sessmgr_whio_ht( cson_sessmgr ** tgt, cson_object const * opt );

    /**
       This cson_sessmgr_factory_f() implementation might or might not
       be compiled in, depending on the mood of the cson
       maintainer. It is very niche-market, and primarily exists just
       to test (and show off) the whio_epfs code.

       It uses libwhio's embedded filesystem (called whio_epfs) as the
       underlying storage:
       
       http://fossil.wanderinghorse.net/repos/whio/index.cgi/wiki/whio_epfs

       The opt object must not be NULL and must contain a single
       string property named "file" which contains the path to the
       whio_epfs "container file" to use for storing sessions. That
       file must have been previously created, either programatically
       using the whio_epfs API or using whio-epfs-mkfs:

       http://fossil.wanderinghorse.net/repos/whio/index.cgi/wiki/whio_epfs_mkfs

       The EPFS container file MUST be created with a "namer"
       installed. See the above page for full details and examples.
       
       See cson_sessmgr_factory_f() for the semantics of the tgt
       argument and the return value.

       Threading notes:

       - It is not legal to use this session manager from multiple threads.
       Doing so will eventually corrupt the underlying EFS if multiple writers
       work concurrently, and will also eventually _appear_ corrupt to multiple
       readers.

       Storage locking:

       The underlying storage (EFS container file) is locked (with a
       write lock) for the lifetime the the returned session manager
       IF the storage reports that it supports locking. Unlocked
       write access from an outside application will corrupt the EFS.

       TODOs:

       - Add config option to explicitly disable locking support.
    */
    int cson_sessmgr_whio_epfs( cson_sessmgr ** tgt, cson_object const * opt );
    
#if 0
    /** TODO? dummy manager which has no i/o support. */
    int cson_sessmgr_transient( cson_sessmgr ** tgt );
#endif

/* LICENSE

This software's source code, including accompanying documentation and
demonstration applications, are licensed under the following
conditions...

Certain files are imported from external projects and have their own
licensing terms. Namely, the JSON_parser.* files. See their files for
their official licenses, but the summary is "do what you want [with
them] but leave the license text and copyright in place."

The author (Stephan G. Beal [http://wanderinghorse.net/home/stephan/])
explicitly disclaims copyright in all jurisdictions which recognize
such a disclaimer. In such jurisdictions, this software is released
into the Public Domain.

In jurisdictions which do not recognize Public Domain property
(e.g. Germany as of 2011), this software is Copyright (c) 2011 by
Stephan G. Beal, and is released under the terms of the MIT License
(see below).

In jurisdictions which recognize Public Domain property, the user of
this software may choose to accept it either as 1) Public Domain, 2)
under the conditions of the MIT License (see below), or 3) under the
terms of dual Public Domain/MIT License conditions described here, as
they choose.

The MIT License is about as close to Public Domain as a license can
get, and is described in clear, concise terms at:

    http://en.wikipedia.org/wiki/MIT_License

The full text of the MIT License follows:

--
Copyright (c) 2011 Stephan G. Beal (http://wanderinghorse.net/home/stephan/)

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

--END OF MIT LICENSE--

For purposes of the above license, the term "Software" includes
documentation and demonstration source code which accompanies
this software. ("Accompanies" = is contained in the Software's
primary public source code repository.)

*/

#if defined(__cplusplus)
} /*extern "C"*/
#endif

#endif /* WANDERINGHORSE_NET_CSON_SESSION_H_INCLUDED */
/* end file include/wh/cson/cson_session.h */
/* begin file include/wh/cson/cson_sqlite3.h */
/** @file cson_sqlite3.h

This file contains cson's public sqlite3-to-JSON API declarations
and API documentation. If CSON_ENABLE_SQLITE3 is not defined,
or is defined to 0, then including this file will have no side-effects
other than defining CSON_ENABLE_SQLITE3 (if it was not defined) to 0
and defining a few include guard macros. i.e. if CSON_ENABLE_SQLITE3
is not set to a true value then the API is not visible.

This API requires that <sqlite3.h> be in the INCLUDES path and that
the client eventually link to (or directly embed) the sqlite3 library.
*/
#if !defined(WANDERINGHORSE_NET_CSON_SQLITE3_H_INCLUDED)
#define WANDERINGHORSE_NET_CSON_SQLITE3_H_INCLUDED 1
#if !defined(CSON_ENABLE_SQLITE3)
#  if defined(DOXYGEN)
#    define CSON_ENABLE_SQLITE3 1
#  else
#    define CSON_ENABLE_SQLITE3 0
#  endif
#endif

#if CSON_ENABLE_SQLITE3 /* we do this here for the sake of the amalgamation build */
#include <sqlite3.h>

#if defined(__cplusplus)
extern "C" {
#endif

/**
   Converts a single value from a single 0-based column index to its JSON
   equivalent.

   On success it returns a new JSON value, which will have a different concrete
   type depending on the field type reported by sqlite3_column_type(st,col):

   Integer, double, null, or string (TEXT and BLOB data, though not
   all blob data is legal for a JSON string).

   st must be a sqlite3_step()'d row and col must be a 0-based column
   index within that result row.
 */       
cson_value * cson_sqlite3_column_to_value( sqlite3_stmt * st, int col );

/**
   Creates a JSON Array object containing the names of all columns
   of the given prepared statement handle. 
    
   Returns a new array value on success, which the caller owns. Its elements
   are in the same order as in the underlying query.

   On error NULL is returned.
    
   st is not traversed or freed by this function - only the column
   count and names are read.
*/
cson_value * cson_sqlite3_column_names( sqlite3_stmt * st );

/**
   Creates a JSON Object containing key/value pairs corresponding
   to the result columns in the current row of the given statement
   handle. st must be a sqlite3_step()'d row result.

   On success a new Object is returned which is owned by the
   caller. On error NULL is returned.

   cson_sqlite3_column_to_value() is used to convert each column to a
   JSON value, and the column names are taken from
   sqlite3_column_name().
*/
cson_value * cson_sqlite3_row_to_object( sqlite3_stmt * st );
/**
   Functionally almost identical to cson_sqlite3_row_to_object(), the
   only difference being how the result objects gets its column names.
   st must be a freshly-step()'d handle holding a result row.
   colNames must be an Array with at least the same number of columns
   as st. If it has fewer, NULL is returned and this function has
   no side-effects.

   For each column in the result set, the colNames entry at the same
   index is used for the column key. If a given entry is-not-a String
   then conversion will fail and NULL will be returned.

   The one reason to prefer this over cson_sqlite3_row_to_object() is
   that this one can share the keys across multiple rows (or even
   other JSON containers), whereas the former makes fresh copies of
   the column names for each row.

*/
cson_value * cson_sqlite3_row_to_object2( sqlite3_stmt * st,
                                          cson_array * colNames );

/**
   Similar to cson_sqlite3_row_to_object(), but creates an Array
   value which contains the JSON-form values of the given result
   set row.
*/
cson_value * cson_sqlite3_row_to_array( sqlite3_stmt * st );
/**
    Converts the results of an sqlite3 SELECT statement to JSON,
    in the form of a cson_value object tree.
    
    st must be a prepared, but not yet traversed, SELECT query.
    tgt must be a pointer to NULL (see the example below). If
    either of those arguments are NULL, cson_rc.ArgError is returned.
    
    This walks the query results and returns a JSON object which
    has a different structure depending on the value of the 'fat'
    argument.
    
    
    If 'fat' is 0 then the structure is:
    
    @code
    {
        "columns":["colName1",..."colNameN"],
        "rows":[
            [colVal0, ... colValN],
            [colVal0, ... colValN],
            ...
        ]
    }
    @endcode
    
    In the "non-fat" format the order of the columns and row values is
    guaranteed to be the same as that of the underlying query.
    
    If 'fat' is not 0 then the structure is:
    
    @code
    {
        "columns":["colName1",..."colNameN"],
        "rows":[
            {"colName1":value1,..."colNameN":valueN},
            {"colName1":value1,..."colNameN":valueN},
            ...
        ]
    }
    @endcode

    In the "fat" format, the order of the "columns" entries is guaranteed
    to be the same as the underlying query fields, but the order
    of the keys in the "rows" might be different and might in fact
    change when passed through different JSON implementations,
    depending on how they implement object key/value pairs.

    On success it returns 0 and assigns *tgt to a newly-allocated
    JSON object tree (using the above structure), which the caller owns.
    If the query returns no rows, the "rows" value will be an empty
    array, as opposed to null.
    
    On error non-0 is returned and *tgt is not modified.
    
    The error code cson_rc.IOError is used to indicate a db-level
    error, and cson_rc.TypeError is returned if sqlite3_column_count(st)
    returns 0 or less (indicating an invalid or non-SELECT statement).
    
    The JSON data types are determined by the column type as reported
    by sqlite3_column_type():
    
    SQLITE_INTEGER: integer
    
    SQLITE_FLOAT: double
    
    SQLITE_TEXT or SQLITE_BLOB: string, and this will only work if
    the data is UTF8 compatible.
    
    If the db returns a literal or SQL NULL for a value it is converted
    to a JSON null. If it somehow finds a column type it cannot handle,
    the value is also converted to a NULL in the output.

    Example
    
    @code
    cson_value * json = NULL;
    int rc = cson_sqlite3_stmt_to_json( myStatement, &json, 1 );
    if( 0 != rc ) { ... error ... }
    else {
        cson_output_FILE( json, stdout, NULL );
        cson_value_free( json );
    }
    @endcode
*/
int cson_sqlite3_stmt_to_json( sqlite3_stmt * st, cson_value ** tgt, char fat );

/**
    A convenience wrapper around cson_sqlite3_stmt_to_json(), which
    takes SQL instead of a sqlite3_stmt object. It has the same
    return value and argument semantics as that function.
*/
int cson_sqlite3_sql_to_json( sqlite3 * db, cson_value ** tgt, char const * sql, char fat );

/**
   Binds a JSON value to a 1-based parameter index in a prepared SQL
   statement. v must be NULL or one of one of the types (null, string,
   integer, double, boolean, array). Booleans are bound as integer 0
   or 1. NULL or null are bound as SQL NULL. Integers are bound as
   64-bit ints. Strings are bound using sqlite3_bind_text() (as
   opposed to text16), but we could/should arguably bind them as
   blobs.

   If v is an Array then ndx is is used as a starting position
   (1-based) and each item in the array is bound to the next parameter
   position (starting and ndx, though the array uses 0-based offsets).

   TODO: add Object support for named parameters.

   Returns 0 on success, non-0 on error.
 */
int cson_sqlite3_bind_value( sqlite3_stmt * st, int ndx, cson_value const * v );
    
#if defined(__cplusplus)
} /*extern "C"*/
#endif
    
#endif /* CSON_ENABLE_SQLITE3 */
#endif /* WANDERINGHORSE_NET_CSON_SQLITE3_H_INCLUDED */
/* end file include/wh/cson/cson_sqlite3.h */
/* begin file include/wh/cpdo/cpdo_amalgamation.h */
#if !defined(CPDO_AMALGAMATION_BUILD)
#  define CPDO_AMALGAMATION_BUILD 1
#endif
#if !defined(CPDO_ENABLE_SQLITE3)
#  define CPDO_ENABLE_SQLITE3 0
#endif
#if !defined(CPDO_ENABLE_SQLITE4)
#  define CPDO_ENABLE_SQLITE4 0
#endif
#if !defined(CPDO_ENABLE_MYSQL5)
#  define CPDO_ENABLE_MYSQL5 0
#endif
/* start of file cpdo_printf.h */
#ifndef WANDERINGHORSE_NET_CPDO_PRINTF_H_INCLUDED
#define WANDERINGHORSE_NET_CPDO_PRINTF_H_INCLUDED 1
#include <stdarg.h>
#include <stdio.h> /* FILE handle */
#ifdef __cplusplus
extern "C" {
#endif
/** @page cpdo_printf_page_main cpdo_printf printf-like API

   This API contains a printf-like implementation which supports
   aribtrary data destinations.

   Authors: many, probably. This code supposedly goes back to the
   early 1980's.

   Current maintainer: Stephan Beal (http://wanderinghorse.net/home/stephan)

   License: Public Domain.

   The primary functions of interest are cpdo_printfv() and cpdo_printf(), which works
   similarly to printf() except that they take a callback function which they
   use to send the generated output to arbitrary destinations. e.g. one can
   supply a callback to output formatted text to a UI widget or a C++ stream
   object.
*/

/**
   @typedef long (*cpdo_printf_appender)( void * arg, char const * data, long n )


   The cpdo_printf_appender typedef is used to provide cpdo_printfv()
   with a flexible output routine, so that it can be easily
   send its output to arbitrary targets.

   The policies which implementations need to follow are:

   - arg is an implementation-specific pointer (may be 0) which is
   passed to vappendf. cpdo_printfv() doesn't know what this argument is
   but passes it to its cpdo_printf_appender. Typically it will be an
   object or resource handle to which string data is pushed or output.

   - The 'data' parameter is the data to append. If it contains
   embedded nulls, this function will stop at the first one. Thus
   it is not binary-safe.

   - n is the number of bytes to read from data. If n<0 then
   strlen(data) should be used.

   - Returns, on success, the number of bytes appended (may be 0).

   - Returns, on error, an implementation-specified negative number.
   Returning a negative error code will cause cpdo_printfv() to stop the
   processing of that string. Note that 0 is a success value (some
   printf format specifiers do not add anything to the output).
*/
typedef long (*cpdo_printf_appender)( void * arg,
				   char const * data,
				   long n );


/**
  This function works similarly to classical printf implementations,
  but instead of outputing somewhere specific, it uses a callback
  function to push its output somewhere. This allows it to be used for
  arbitrary external representations. It can be used, for example, to
  output to an external string, a UI widget, or file handle (it can
  also emulate printf by outputing to stdout this way).

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
 function "func", or a negative number on a pre-output error. If this
 function returns an integer greater than 1 it is in general
 impossible to know if all of the elements were output. As such
 failure can only happen if the callback function returns an error,
 and this type of error is very rare in a printf-like context, this is
 not considered to be a significant problem. (The same is true for any
 classical printf implementations, as far as i'm aware.)


 CURRENT (documented) PRINTF EXTENSIONS:

 %%z works like %%s, but takes a non-const (char *) and vappendf
 deletes the string (using free()) after appending it to the output.

 %%h (HTML) works like %s but converts certain characters (like '<' and '&' to
 their HTML escaped equivalents.

 %%t (URL encode) works like %%s but converts certain characters into a representation
 suitable for use in an HTTP URL. (e.g. ' ' gets converted to %%20)

 %%T (URL decode) does the opposite of %t - it decodes URL-encoded
 strings.

 %%r requires an int and renders it in "ordinal form". That is,
 the number 1 converts to "1st" and 398 converts to "398th".

 %%q quotes a string as required for SQL. That is, '\'' characters get
 doubled.

 %%Q as %%q, but includes the outer '\'' characters and null pointers
 replaced by SQL NULL.

 (The %%q and %%Q specifiers are options inherited from this printf
 implementation's sqlite3 genes.)

 These extensions may be disabled by setting certain macros when
 compiling vappendf.c (see that file for details).
*/
long cpdo_printfv(
  cpdo_printf_appender pfAppend,          /* Accumulate results here */
  void * pfAppendArg,                /* Passed as first arg to pfAppend. */
  const char *fmt,                   /* Format string */
  va_list ap                         /* arguments */
  );

/**
   Identical to cpdo_printfv() but takes a (...) ellipses list instead of a
   va_list.
*/
long cpdo_printf(cpdo_printf_appender pfAppend,
	     void * pfAppendArg,
	     const char *fmt,
	     ... );

/**
   Emulates fprintf() using cpdo_printfv().
*/
long cpdo_printf_file( FILE * fp, char const * fmt, ... );


/**
   Works like cpdo_printfv(), but appends all output to a
   dynamically-allocated string, expanding the string as necessary to
   collect all formatted data. The returned null-terminated string is
   owned by the caller and it must be cleaned up using free(). If !fmt
   or if the expanded string evaluates to empty, null is returned, not
   a 0-byte string.
*/
char * cpdo_printfv_str( char const * fmt, va_list vargs );

/**
   Equivalent to cpdo_printfv_str(), but takes elipsis arguments instead
   of a va_list.
*/
char * cpdo_printf_str( char const * fmt, ... );

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* WANDERINGHORSE_NET_CPDO_PRINTF_H_INCLUDED */
/* end of file cpdo_printf.h */
/* start of file include/wh/cpdo/cpdo.h */
#if !defined(WANDERINGHORSE_NET_CPDO_H_INCLUDED)
#define WANDERINGHORSE_NET_CPDO_H_INCLUDED

/** @file cpdo.h

This file contains the public API for the "cpdo" library, a database
access abstraction API written in C and modelled heavily off of PHP's
PDO API (see http://php.net/manual/en/book.pdo.php).

Author: Stephan Beal (http://www.wanderinghorse.net/home/stephan/)

License: Dual Public Domain/MIT. The full license text is at the
bottom of the main header file (cpdo.h).

Requirements:

- C99, but only for the fixed-size integer defined in stdint.h.  The rest of
the code "should" work fine in C89 mode. It works fine in C89 mode when
including the C99-specified stdint.h, but the portability of that approach
is questionable.

- A db back-end for which we have a driver, including the required
headers and libraries needed for using that back-end.


Current code status:

- The public API is more or less complete. There will likely be some
additions to the cpdo_driver_api. e.g. for fetching table lists and similar
driver-specific utilities. Some utility (non-member) functions are probably
missing, but those will be added as the need for them is found.

- The sqlite3 driver implementation seems to work as documented for
the cpdo abstraction interface.

- Consider whether we want to support get-db-name and get-table-name
operations.

Design notes:

There are basically two ways to fire off SQL queries to a server:

- Send "plain SQL". This is the most common approach, and the easiest
for most purposes.

- Use "prepared statements."

This library is built upon the guiding principal of always using
prepared statements, and implements "plain queries" internally
on top of prepared statements.

*/

/** @page page_cpdo_main cpdo

This is the documentation for the cpdo library. The API
is completely documented in \ref cpdo.h.


*/

#include <stdint.h> /* fixed-size integers */
#include <stddef.h> /* NULL! */
#include <stdarg.h> /* va_list */

/** @def CPDO_VERSION_NUMBER

CPDO_VERSION_NUMBER contains the version
number of cpdo, encoded in the form
YYYYMMDD of the most recent changes,
in decimal format. e.g. the number
20110209 represents February 9th, 2011.
*/
#define CPDO_VERSION_NUMBER 20110209
/** @def CPDO_VERSION_STRING

CPDO_VERSION_STRING contains the string form
of CPDO_VERSION_NUMBER
*/
#define CPDO_VERSION_STRING "20110209"


/** @def CPDO_ENABLE_64_BIT

    Some internal code works either on 32-bit or 64-bit, but
    not both (well, not portably). We try to detect 64-bit
    with this macro, but if you get errors about "pointer being
    cast to integer of different size" while building cpdo
    then try defining this before including this file (e.g. from
    the compiler command line or in your impl file before you
    include this).
    
    In 64-bit mode, some internal masking operations may
    work differently. In 32-bit mode support for int64_t might
    be limited or unavailable in some contexts.
*/
#if !defined(CPDO_ENABLE_64_BIT)
/* Largely taken from http://predef.sourceforge.net/prearch.html

See also: http://poshlib.hookatooka.com/poshlib/trac.cgi/browser/posh.h
*/
#  if defined(_WIN64) || defined(__LP64__/*gcc*/) \
    || defined(_M_X64) || defined(__amd64__) || defined(__amd64) \
    ||  defined(__x86_64__) || defined(__x86_64) \
    || defined(__ia64__) || defined(__ia64) || defined(_IA64) || defined(__IA64__) \
    || defined(_M_IA64) \
    || defined(__sparc_v9__) || defined(__sparcv9) || defined(_ADDR64) \
    || defined(__64BIT__)
#    define CPDO_ENABLE_64_BIT 1
#  else
#    define CPDO_ENABLE_64_BIT 0
#  endif
#endif


#if defined(__cplusplus)
extern "C" {
#endif

    /** @var cpdo_rc

    This object defines the error codes used by cpdo.

    Library routines which return int values almost always return a
    value from this structure. None of the members in this struct have
    published values except for the OK member, which has the value 0.
    All other values might be incidentally defined where clients
    can see them, but the numbers might change from release to
    release, so clients must only use the symbolic names.

    Client code is expected to access these values via the shared
    cpdo_rc object, and use them as demonstrated here:

    @code
    int rc = cpdo_some_func(...);
    if( 0 == rc ) {...success...}
    else if( cpdo_rc.ArgError == rc ) { ... some argument was wrong ... }
    else if( cpdo_rc.AllocError == rc ) { ... allocation error ... }
    ...
    @endcode

    Design notes: i "should have" gone with an enum from the start, and at
    some point may migrate to an enum and keep cpdo_rc as a compatiblity
    option. The advantage of the enum is O(1) dispatching, as opposed to
    O(N) if/else-ing (as shown above).
    */

    /** @struct cpdo_rc_
        See \ref cpdo_rc for details.
    */
    static const struct cpdo_rc_
    {
        /** The generic success value. Guaranteed to be 0. */
        const int OK;
        /** Signifies an error in one or more arguments (e.g. NULL where it is not allowed). */
        const int ArgError;
        /** Signifies that some argument is not in a valid range. */
        const int RangeError;
        /** Signifies that some argument is not of the correct logical type. */
        const int TypeError;
        /** Signifies an input/ouput error. */
        const int IOError;
        /** Signifies an out-of-memory error. */
        const int AllocError;
        /** Signifies that the called code is "NYI" (Not Yet Implemented). */
        const int NYIError;
        /** Signifies that an internal error was triggered. If it happens, please report this as a bug! */
        const int InternalError;
        /** Signifies that the called operation is not supported in the
            current environment. e.g.  missing support from 3rd-party or
            platform-specific code.
        */
        const int UnsupportedError;
        /**
           Signifies that the request resource could not be found.
        */
        const int NotFoundError;
        /**
           Signifies an unknown error, possibly because an underlying
           3rd-party API produced an error and we have no other reasonable
           error code to convert it to.
        */
        const int UnknownError;
        /**
           A generic connection-related error, e.g. trying to prepare an
           SQL statement when the driver is not connected to the database.
        */
        const int ConnectionError;
        /**
           Signifies that the caller should check the db-specific or
           statement-specific error code (depending on the context)
           for more details.
        */
        const int CheckDbError;
        /**
           Signifies that some form of access was denied. What exactly
           what denied depends on the function returning this value.
        */
        const int AccessError;
        
        /**
            Indicates some form of usage error _other_ than those
            represented by ArgError. An example is trying to use
            a db connection after it has been closed (but often such
            a case cannot be caught, and might lead to a crash).
        */
        const int UsageError;
    } cpdo_rc = {
    0/*OK*/,
    1/*ArgError*/,
    2/*RangeError*/,
    3/*TypeError*/,
    4/*IOError*/,
    5/*AllocError*/,
    6/*NYIError*/,
    7/*InternalError*/,
    8/*UnsupportedError*/,
    9/*NotFoundError*/,
    10/*UnknownError*/,
    11/*ConnectionError*/,
    12/*CheckDbError*/,
    13/*AccessError*/,
    14/*UsageError*/
    };

    /**
       Flags specifying what optional features a driver supports
       (required features are always assumed to be supported!).
       Used with cpdo_driver_api::capabilities().
    */
    enum cpdo_capability_e {
    CPDO_CAPABILITY_ERROR = -1,
    CPDO_CAPABILITY_NONE = 0,
    /**
       Flag indicating whether AUTOINCREMENT support of integer
       primary keys is available.
    */
    CPDO_CAPABILITY_AUTOINCREMENT = 1 << 0,
    /**
       Flag indicating whether the last-insert-id
       (cpdo_driver_api::last_insert_id()) is supported.
    */
    CPDO_CAPABILITY_LAST_INSERT_ID = 1 << 1,
    /**
       Flag indicating whether integers can be fetched via the string
       APIs, converted automatically.
     */
    CPDO_CAPABILITY_INT_TO_STRING = 1 << 2,
    /**
       Flag indicating whether doubles/floats can be fetched via the
       string APIs, converted automatically.
     */
    CPDO_CAPABILITY_DOUBLE_TO_STRING = 1 << 3,
    /**
       Convenience entry for integer/double-to-string conversions.
    */
    CPDO_CAPABILITY_NUMBER_TO_STRING = CPDO_CAPABILITY_INT_TO_STRING | CPDO_CAPABILITY_DOUBLE_TO_STRING,
    /**
       Flag indicating whether string columns can be fetched via the
       integer interfaces, converted to the appropriate numeric type
       automatically.
    */
    CPDO_CAPABILITY_STRING_TO_INT = 1 << 4,
    /**
       Flag indicating whether string columns can be fetched via the
       float/double interfaces, converted to the appropriate numeric
       type automatically.
    */
    CPDO_CAPABILITY_STRING_TO_DOUBLE = 1 << 5,
    /**
       Convenience entry for string-to-integer/double conversions.
    */
    CPDO_CAPABILITY_STRING_TO_NUMBER = CPDO_CAPABILITY_STRING_TO_INT | CPDO_CAPABILITY_STRING_TO_DOUBLE,
    /**
       Flag indicating whether named parameter support is available.
    */
    CPDO_CAPABILITY_NAMED_PARAMETERS = 1 << 6,
    /**
       Flag indicating whether blob support is available.
     */
    CPDO_CAPABILITY_BLOBS = 1 << 7,
    /**
       Flag indicating whether multiple statements may be running on a
       given driver at one time.
    */
    CPDO_CAPABILITY_MULTIPLE_STATEMENTS = 1 << 8
    };
    
    /**
       Returns the string form of the cpdo_rc code corresponding to rc,
       or some unspecified, non-NULL string if it is an unknown code.
       The returned bytes are static, and live as long as the application is
       running.
    */
    char const * cpdo_rc_string(int rc);

    typedef struct cpdo_driver_api cpdo_driver_api;
    typedef struct cpdo_stmt_api cpdo_stmt_api;
    typedef struct cpdo_driver cpdo_driver;
    typedef struct cpdo_stmt cpdo_stmt;
    typedef struct cpdo_driver_opt cpdo_driver_opt;
    typedef struct cpdo_connect_opt cpdo_connect_opt;
    typedef struct cpdo_driver_details cpdo_driver_details;
#if 0
    typedef int64_t cpdo_int_t;
    typedef double cpdo_double_t;
#endif

    /**
       SQL type identifiers. Note that none of them has a value of 0,
       to avoid confusion with 0 as a success code returned from most
       of the cpdo API. Clients must not rely on any specific values
       for these members, and i might just randomly change the values
       to make sure code is behaving as it should.

       Though this type makes a distinction between 8-, 16-, 32-, and
       64-bit integers, drivers which do not support the full range
       are encouraged to "fudge it" a little, and try to do what would
       be intuitive (or least surprising) for the user.

       Notes regarding type conversions:

       - Implementations are expected to be able to convert, for
       field-fetching purposes, numeric types to other numeric
       types, even if it means a loss in precision. e.g. a user
       should be able to fetch a double-typed field into an
       int16_t value if he wishes.

       - Converting non-string data to a string is an optional
       feature, but drivers are encouraged to implement it if
       feasible.

       - Conversions from strings to numeric data is also an optional
       feature, but may require more effort on the part of the driver
       implementor.
    */
    enum cpdo_data_type {
    CPDO_TYPE_ERROR = -1,
    /** Represents SQL NULL. */
    CPDO_TYPE_NULL = 1,
    /** Represents 1-byte integers. */
    CPDO_TYPE_INT8,
    /** Represents 2-byte integers. */
    CPDO_TYPE_INT16,
    /** Represents 4-byte integers. */
    CPDO_TYPE_INT32,
    /** Represents 8-byte integers. */
    CPDO_TYPE_INT64,
    /** Represents single-precision floating point values. */
    CPDO_TYPE_FLOAT,
    /** Represents double-precision floating point values. */
    CPDO_TYPE_DOUBLE,
    /** Represents SQL CHAR and VARCHAR values. The library assumes
        an encoding of UTF8.
    */
    CPDO_TYPE_STRING,
    /**
       Represents SQL BLOB/CLOB data.
     */
    CPDO_TYPE_BLOB,
    /** For internal use by driver implementations, so that they
        can support driver-specific types with some transparency
        (e.g. MySQL's TIMESTAMP field handling uses this).
     */
    CPDO_TYPE_CUSTOM
    };
    /** Convenience typedef. */
    typedef enum cpdo_data_type cpdo_data_type;

    /**
       Return values for cpdo_stmt::step(). Note that no entry has the
       value 0. This is to avoid accidental confusion with the 0 success
       value returned by most functions in this API. Clients must not rely
       on these have any specific value, and i may change them just to
       make sure test code is behaving how it's supposed to.
    */
    enum cpdo_step_code {
    /**
       Means the next row in the result set was fetched successfully.
       Some non-fetchable queries might return this value, instead of
       CPDO_STEP_DONE, because it is not always possible to know if a
       driver-specific SQL construct wants to return results or not.  In
       such cases, trying to access the field data will not work (error
       codes, as opposed to crashes, unless the driver is poorly written).
    */
    CPDO_STEP_OK = 1,
    /**
       Means the end of a result set or non-fetchable statement, was
       reached without encountering an error. For fetchable results, the
       result set is not valid when this value is returned.  See the notes
       for CPDO_STEP_OK.
    */
    CPDO_STEP_DONE = 2,
    /**
       Signifies that an error occurred while fetching
       the next row of the result set. This is normally
       one of two things:

       - The client passed an invalid statement handle.

       - The statement is not fetchable or we have already fetched all the
       data. In that case, the cpdo_stmt::error_info() routine should give
       more information about the error (or it might not, depending on
       internal details of the driver).
    */
    CPDO_STEP_ERROR = 3
    };
    /** Convenience typedef. */
    typedef enum cpdo_step_code cpdo_step_code;

    /**
       A place for storing miscellaneous human-readable
       info about a driver.
    */
    struct cpdo_driver_details
    {
        /**
           The name used by DSNs for identifying this
           driver. e.g. "sqlite3" or "mysql5".
        */
        char const * driver_name;
        /**
           Version of the driver and optionally the version
           of the underlying database driver.
        */
        char const * driver_version;
        /**
           Brief description of the driver. e.g. "MySQL 5.x".
        */
        char const * description;
        /**
           License conditions of the driver. Ideally a URL
           to a well-known license, or simply its name
           (e.g. "GNU GPL version 2").
        */
        char const * license;
        /**
           Home page of the driver.
        */
        char const * url;
        /**
           Holds author names/email/URL/whatever.
           Entries should be separated by a newline.
        */
        char const * authors;
    };
    /** Empty-initialized cpdo_driver_details object. */
#define cpdo_driver_details_empty_m {           \
        "Unnamed"/*driver_name*/,                \
            "Unversioned"/*driver_version*/,     \
            "Undescribed"/*description*/,       \
            "Unknown"/*license*/,               \
            "Unknown"/*authors*/,               \
            "Unknown"/*url*/                    \
            }
    /** Empty-initialized cpdo_driver_details object. */
    extern const cpdo_driver_details cpdo_driver_details_empty;

    /**
       Holds connection-related options.

       TODO: how are we gonna get driver-specific options passed via this
       object? Still thinking about how to handle them.
    */
    struct cpdo_connect_opt
    {
        /**
           DSN for the connection.
        */
        char const * dsn;
        /**
           User name for the connection login.
         */
        char const * user_name;
        /**
           Password for the connection login.
         */
        char const * password;
    };
#define cpdo_connect_opt_empty_m {              \
        NULL/*dsn*/,                    \
            NULL/*user_name*/,                  \
            NULL/*password*/,                   \
            }
    extern const cpdo_connect_opt cpdo_connect_opt_empty;

    /**
       This type holds the "vtbl", or "virtual" operations, specified for
       the cpdo_driver interface. All instances of a specific driver type
       share a single instance of this class.
    */
    struct cpdo_driver_api
    {
        /**
           Returns human-readable details about the driver.

           Implementations are required to use an immutable/const instance
           of cpdo_driver_details so that we have no lifetime-related
           issues involving the string data in the returned object.
        */
        cpdo_driver_details const * (*driver_details)();

        /**
           Establishes the connection to the database. Whether or not the
           opt parameter is used, and which parts of it are used,
           are driver-dependent.

           Ownership of self is not changed by this call.

           Returns 0 on success, non-0 (probably cpdo_rc.CheckDbError) on
           error.

           Regardless of success or failure, this function must not
           be called more than once for the same connection object.
           Violating this may lead to resource leaks. Implementations
           are encourages to return an error code if they detect
           that connect() is being called a second time.

           Implementations "should" try to ensure that if connection
           fails, the client can use error_info() to get the
           error information. i.e. don't clean up the underlying
           connection object if this fails - wait until close()
           is called.

           Drivers which do not require login information must ignore
           the user/password info, as opposed to returning an error.

           Drivers may ignore DSN-specified options which they do not
           understand or they may treat them as an error. If they
           treat it as an error, the next call to
           self->api->error_info(self) MUST return the reason for the
           error (if it does not, the user will have no indication as
           to why the connection, using otherwise valid valid login
           info, failed).
        */
        int (*connect)( cpdo_driver * self, cpdo_connect_opt const * opt );

        /**
           str is the string to quote. *len must be its length.
   
           Allocates a new string with the contents of str, contained in
           single-quotes, and all single quotes in the string get
           doubled. Drivers may opt to use double-quotes instead.

           If str is NULL then drivers must treat that as an SQL NULL value,
           assign *dest to the string "NULL" (without any quotes) and assign
           *len to 4.

           On success, 0 is returned and *dest is pointed to the new
           string, and *len is assigned to the length of the new
           string. The caller owns the string and must eventually free
           it using self->api->free_string(self, str). The bytes are
           owned by the driver and must not be modified by the caller
           (they may refer to a shared string).

           On error non-0 is returned (one of the cpdo_rc values) and *len and
           *dest are unmodified. The most likely error codes are cpdo_rc.ArgError
           (an invalid argument or combination of arguments) and cpdo_rc.AllocError
           (if the new string cannot be allocated).

           ACHTUNG: this is itended for use with individual SQL values,
           not whole SQL statements.
        */
        int (*quote)( cpdo_driver * self, char const * str, uint32_t * len, char ** dest );

        /**
           Given a table or field name, this function must allocate a
           new copy quoted using the mechanism conventional for the
           driver. On success the new string is assigned to *dest and
           *len is set to the length of the escaped name. The caller
           owns the *dest memory and must eventually free it using
           self->api->free_string().

           dest may not be NULL but len may be (in which case len is
           simply ignored). If self, identifier, or dest are NULL, or
           if identifier is empty, then cpdo_rc.ArgError must be
           returned.

           Implementations are not required to perform any validation
           on the identifier other than to confirm that it is not
           NULL/empty.

           On error *dest and *len must not be modified.

           On success 0 is returned, else one of the cpdo_rc values
           describing the problem.
         */
        int (*qualify)( cpdo_driver * self, char const * identifier, uint32_t * len, char ** dest );

        /**
           Frees a string allocated by quote() or qualify(). Results are
           undefined if the string came from another source. It must return
           0 on success. It must, like free(), gracefully ignore a NULL
           string value.
        */
        int (*free_string)( cpdo_driver * self, char * str);

        /**
           Must initialize a statement object for the given self from the
           first len bytes of the given sql, and populte *tgt object with
           the results.  Must return 0 on success, non-zero on error. On
           error, *tgt must not be modified.

           On success the caller must eventually clean up the result with
           result->api->finalize(result).

           Whether or not the given sql may be legally terminated with a
           semicolon is driver-dependent. For best results, do not include
           a semicolon (e.g. Oracle OCI says it's an invalid character).
           Most drivers, in my experience, do not care one way or the
           other.
        */
        int (*prepare)( cpdo_driver * self, cpdo_stmt ** tgt, char const * sql, uint32_t len );

        /**
           Must return the last error information associated with the
           connection.

           IFF the dest parameter is non null then it must point *dest at
           the error string and set *len (if it is not NULL) to the length
           the message.

           IFF errorCode is not null then it must assign *errorCode to the
           driver-dependent error code.

           Implementations must allow arbitrary combinations of NULL for
           the dest/len/errorCode parameters, to allow clients to specify
           exactly which information they're interested in.
       
           Returns 0 on success (meaning that it fetched the info from the
           underlying driver). On success *dest and/or *len and/or
           *errorCode may be set to 0 if the driver has no error to report.
           Some drivers may return other strings on non-error (sqlite3
           infamously uses "not an error" for this case).

           It is ASSUMED that the underlying driver owns the returned
           string, and this API allows the lifetime of the returned
           bytes to expire at the next call into the underlying driver
           (including this function!). Thus clients must copy it if
           needed.

           Implementations may invalidate/reset the error state for any
           operations, any string fetched here may be invalidated
           (e.g. its memory cleaned up) at the next database operation via
           this driver.

           Portability notes:

           - Some drivers (e.g. Oracle OCI) automatically add a newline to
           the end of the error message (which is somewhat tasteles, IMO).

           - sqlite3 infamously uses the error string "not an error" when
           there is no error, instead of using a NULL/empty string. A driver
           for sqlite3 MAY convert that to a null/empty string.
        */
        int (*error_info)( cpdo_driver * self, char const ** dest,
                           uint32_t * len, int * errorCode );

        /**
           Must return non-zero (true) if the self object is connected, or
           false if !self or self is not connected. A return value of true is
           NOT a guaranty that the connection actually works at the
           moment, but that a connection has, at some point, been
           established.

           Implementations may optionally check whether the connection is
           live here, but are not required to do so.
        */
        char (*is_connected)( cpdo_driver * self );

        /**
           Must clean up all internal resources and then free the self
           object using a mechanism appropriate for its allocation.

           After calling close(), the self object is invalidated and must
           not be used further by the client.
        */
        int (*close)( cpdo_driver * self );

        /**
           This is an optional feature.

           Should return the value of the most recent insertion
           operation by writing it to *v. If this feature is not
           supported, v must not be modified and
           cpdo_rc.UnsupportedError must be returned.

           The hint parameter is required by drivers which need, e.g.,
           a sequence name from which to extract the id. Drivers which
           do not need this may ignore the parameter (and must not
           treat its existence as an error).

           Note that some databases use signed integers as IDs, but as
           far as i am aware those do not actually employ negative ID
           numbers. This API makes the assumption that all IDs of this
           type are positive values.
           
           Must return 0 on success. On error v must not be modified.
           If v is NULL implementations must return cpdo_rc.ArgError.
        */
        int (*last_insert_id)( cpdo_driver * self, uint64_t * v, char const * hint );

        /**
           Must return a bitmask of capabilities for the drivers, from
           the cpdo_capability_e enum. If a capability bit is returned
           which the driver is not capable of (i.e., if the driver
           lies), results of using that feature later are undefined.

           On error (e.g. self is not the proper type)
           CPDO_CAPABILITY_ERROR must be returned.

           Depending on the driver, the returned value might be the
           same for all instances.
        */
        int (*capabilities)( cpdo_driver const * self );
        
        /**
           Transaction-related functions. Implementations which do not
           support transactions must return cpdo_rc.UnsupportedError
           from these functions.

           Note that drivers which support transactions do not
           necessarily allow nested transactions. This API assumes
           that only one level of transaction is supported, and
           clients should also assume that.

           ACHTUNG: this part of the API _might_ be removed because...
           The only private state, other than the underlying db
           handle, which the driver impls currently hold has to do
           with the transaction-related APIs (they need to remember if
           they're in a transaction or not). If we remove this API
           then we can allow those drivers to be multi-thread-capable
           as long as the underlying driver supports it.

           Additionally, the is_in() operation relies 100% on the client
           using only this API to start transactions, so it is not reliable
           if clients manually send BEGIN/COMMIT commands.
        */
        struct cpdo_driver_api_transaction
        {
            /**
               Tries to start an SQL transaction. Note that
               not all drivers, or not all storage engines supported
               by a given driver, may support transactions.

               If the driver is requested to start a transaction while
               it is already in one, it must return
               cpdo_rc.UnsupportedError if it does not support nested
               transactions.
               
               Returns 0 on success.
            */
            int (*begin)( cpdo_driver * self );
            /**
               Commits the current transaction.
       
               Returns 0 on success.
            */
            int (*commit)( cpdo_driver * self );
            /**
               Rolls back the current transaction.
           
               Returns 0 on success.
            */
            int (*rollback)( cpdo_driver * self );
            /**
               Returns true if, in the driver's humble opinion, it is
               within an active transaction.  This determination is
               not fool-proof, and can only work consistently when
               used in conjunction with this class'
               transaction-related APIs, as opposed to manually
               sending BEGIN/COMMIT SQL.

               Implementations must return 0 if self is NULL.
            */
            char (*is_in)( cpdo_driver * self );
        } transaction;
        /**
            Holds functions related to setting or fetching driver-level
            options.
        */
        struct cpdo_driver_api_options
        {
            /** Do not use - i have not settled on the interface conventions
            yet.
            */
            int (*set)( cpdo_driver * self, char const * key, va_list vargs );

            /** Do not use - i have not settled on the interface conventions
            yet.
            */
            int (*get)( cpdo_driver * self, char const * key, va_list vargs );
        } opt;
        /**
           Holds non-function constants used by the driver implementation.

           FIXME: the table_quote member should be a function
           so that driver-specific options can be used to change them. e.g.
           MySQL supports (it turns out) an option to enable ANSI-style quoting
           (which is not its native string format). Alternately, we could
           implement such differences by using a separate instance of
           cpdo_driver_api for those cases, but that quickly adds complications
           to internal code which otherwise doesn't care about these configurable
           options.
        */
        struct cpdo_driver_api_nonfunc
        {
            /**
               Must be the driver name string associated with this
               driver. It must not change. Ever.
            */
            char const * driver_name;
        } constants;
    };

    /**
       This type holds the "vtbl", or "virtual" operations, specified for
       the cpdo_stmt interface. All cpdo_stmt instances originating from a
       given cpdo_driver implementation share a single instance of this class.
    */
    struct cpdo_stmt_api
    {
        /**
           "Steps" the cursor one place. For INSERT, DROP, and DELETE
           operations and such, this executes the insert. For SELECT, it
           steps one row in the result set. Certain DB-specific SQL constructs
           (e.g. MySQL's "SHOW TABLES") are also traversable using
           this mechanism.

           Must return:

           CPDO_STEP_ERROR on error.

           CPDO_STEP_OK means a row was fetched and its data can be
           pulled from the self object's getter functions.
       
           CPDO_STEP_DONE means the end of the result set has been
           reached without an notable error. No result set is actually
           fetched, and the client must not try to fetch row data from
           the self object after CPDO_STEP_DONE is returned. This is
           also returned for non-fetching queries.

           Using the various getter routines on a statement has undefined
           results if step() does not return CPDO_STEP_OK.

           Looping over result sets is done like:

           @code
           cpdo_step_code scode;
           while( CPDO_STEP_OK == (scode=st->api->step(st)) ) {
               ... fetch row data ...
           }
           // scode will be CPDO_STEP_DONE or CPDO_STEP_ERR now
           // unless we prematurely ended the loop.
           if( CPDO_STEP_ERROR == scode ) {
               ... there was an error during fetch.
               ... Use st->api->error_info() for (hopefully) details.
           }
           @endcode

        */
        cpdo_step_code (*step)( cpdo_stmt * self );


        /** Has semantics identical to cpdo_driver::error_info(),
            and may produce the same results as that function,
            depending on the driver.
        */
        int (*error_info)( cpdo_stmt * self, char const ** dest,
                           uint32_t * len, int * errorCode );

    
        /**
           Must clean up any memory owned by self and then free the self
           the statement object.

           All statements opened for a given database MUST be finalized
           by the client before the db is closed, or undefined behaviour
           ensues. When used in environments which use garbage
           collection for object destruction (e.g. a script language
           binding), implementor may have to take extra care to ensure
           that statements are not finalized after their parent
           connection has been finalized.
        */
        int (*finalize)( cpdo_stmt * self );

        /**
           The "bind" API is used for binding parameters to prepared
           statements.

           Note that these use 1-based indexes, whereas the getter
           functions use 0-based indexes. i didn't invent the
           conventions, i just try to follow them. The bind functions
           will return cpdo_rc.RangeError if the index is out of
           range.

           Note that not all drivers support binding parameters
           by name (e.g. MySQL 5). For those which don't,
           cpdo_driver implementors may wish to take a look at
           cpdo_named_params_to_qmarks(), which takes a huge number of
           parameters and can be used to add this feature on top of
           APIs which do not natively support it.
        */
        struct cpdo_stmt_api_bind
        {

            /**
               Must re-set the statement so that it can be executed
               again.  This is necessary after binding values to a
               prepared INSERT statement, executing the insert (with
               step()), and then you want to do another insert with
               the same statement but different values.

               Whether or not this un-binds bound values is
               implementation-defined. For portable behaviour, always
               re-bind them between resets.
            */
            int (*reset)( cpdo_stmt * self );
            /**
               Returns the number of bound parameters, or 0 if it has
               none, on error, or if self is NULL.
            */
            uint16_t (*param_count)( cpdo_stmt * self );

            /**
               Returns the 1-based index of the given parameter name
               (not column name!), or 0 on error or if there is no
               such named parameter.

               This is an OPTIONAL feature, because some drivers
               do not support it natively (e.g. MySQL 5).

               The name provided here must exactly match the name in
               the prepared query. e.g. if the query uses ":name" then
               that same string (including the ':' prefix) must be
               passed here. Drivers which internally do not use the
               prefix may simply skip the prefix character.

               Also beware that different drivers support different
               naming conventions. e.g. sqlite3 supports several
               different ones (see
               http://sqlite.org/c3ref/bind_parameter_name.html).
            */
            uint16_t (*param_index)( cpdo_stmt * self, char const * name );

            /**
               Returns the parameter name (not column name!) bound to
               the given 1-based column index, or NULL on error.

               This is an OPTIONAL feature, because some drivers
               do not support it natively (e.g. MySQL 5).
            */
            char const * (*param_name)( cpdo_stmt * self, uint16_t ndx );

            /**
               Binds a NULL value to the given 1-based index.

               Returns 0 on success.
            */
            int (*null)( cpdo_stmt * self, uint16_t ndx );

            /**
               Binds an int8_t value to the given 1-based index.

               Returns 0 on success.
            */
            int (*i8)( cpdo_stmt * self, uint16_t ndx, int8_t v );

            /**
               Binds an int16_t value to the given 1-based index.

               Returns 0 on success.
            */
            int (*i16)( cpdo_stmt * self, uint16_t ndx, int16_t v );

            /**
               Binds an int32_t value to the given 1-based index.

               Returns 0 on success.
            */
            int (*i32)( cpdo_stmt * self, uint16_t ndx, int32_t v );

            /**
               Binds an int64_t value to the given 1-based index.

               Returns 0 on success.
            */
            int (*i64)( cpdo_stmt * self, uint16_t ndx, int64_t v );


            /**
               Binds a float value to the given 1-based index.

               Returns 0 on success.
            */
            int (*flt)( cpdo_stmt * self, uint16_t ndx, float v );

            /**
               Binds a double value to the given 1-based index.

               Returns 0 on success.
            */
            int (*dbl)( cpdo_stmt * self, uint16_t ndx, double v );
        
            /**
               Binds the first len bytes of v as a string value to the
               given 1-based index.

               Implementations must (as of 20120413) treat (v==NULL)
               as an SQL NULL and (v!=NULL) with (0==len) as a
               zero-length value (as opposed to an SQL NULL). If v is
               NULL then len must be ignored.
           
               Returns 0 on success.
            */
            int (*string)( cpdo_stmt * self, uint16_t ndx, char const * v, uint32_t len );

            /**
               Binds the first len bytes of v as a BLOB value at the given
               index.  Blob support is not required to be implemented, and
               drivers which do not support it must return
               cpdo_rc.UnsupportedError from this function.
               
               Implementations must (as of 20120413) treat (v==NULL)
               as an SQL NULL and (v!=NULL) with (0==len) as a
               zero-length value (as opposed to an SQL NULL). If v is
               NULL then len must be ignored.

               Returns 0 on success.
            */
            int (*blob)( cpdo_stmt * self, uint16_t ndx, void const * v, uint32_t len );
        } bind;

        /**
           The "getter" API, for fetching column data from SELECT-like
           queries. i appologize for the cryptic names, like i8() and
           flt(), but i had to avoid reserved words.

           Note that the lifetime of fetched string/blob bytes
           may (depending on the driver) expire the next time
           the statement is stepped or when it is cleaned up,
           so clients must copy such data if they need it for
           later.

           Note that these use 0-based indexes, whereas the bind
           functions use 1-based indexes. i didn't invent the
           conventions, i just try to follow them. The getter
           functions must return cpdo_rc.RangeError if the client-provided
           index is out of range.

           Implementations of the index-based getter functions SHOULD
           return cpdo_rc.RangeError if the index is out of range. However,
           some wrappers might not have the range information without the
           overhead of another call into the underlying driver, so they MAY
           simply pass on the index to the driver and accept what it returns.
        */
        struct cpdo_stmt_api_get
        {
            /**
               Returns the number of columns in the query, or 0 on
               error or if the statement has no columns (e.g. a DROP
               TABLE command). The "on error" case would be caught
               during statement preparation, and only applies if the
               client mis-uses the self object (calling this without
               having successfully prepared the self object).
            */
            uint16_t (*column_count)( cpdo_stmt * self );
            /**
               Returns the name of the 0-based result column index, or
               NULL on error.

               Note that not all DB drivers guaranty the exact string
               returned for any given column name will have any
               particular value unless the SQL explicitly uses 'AS' to
               alias the column. sqlite3, in particular, has that
               property, and can legally return "possibly unexpected"
               results if the query does not explicitly alias each
               column.
   
               What that means is:

               @code
               // This might provide unexpected column names for (a,b):
               SELECT a, b FROM t;
               // That might return "t.a" and "t.b", or something
               // entirely different, for the column names.

               // This approach guarantees a specific column name will be returned:
               SELECT a AS a, b AS b FROM t;
               @endcode

               Thus clients should always alias all column names with
               'AS' if they want this function to behave predictably.
            */
            char const * (*column_name)( cpdo_stmt * self, uint16_t ndx );
            /**
               Assigns the cpdo data type of the given 0-based column index
               to *val.

               Returns 0 on success. On error, *val is not modified.

               Implementations "should" return the type of the value at
               the current row in the same result set. e.g. a
               driver should return the type NULL for a STRING field
               if the current record has an SQL NULL value. However, it is
               easy to see how unclear driver-level documentation can lead
               to confusion about about exactly which type data is
               returned for such a case (the declared type or the NULL
               type).

                TODO:

                - Make sure the existing drivers return CPDO_TYPE_NULL
                for NULL values.

                - And make sure that's the philosophically correct thing
                to do.
            */
            int (*type)( cpdo_stmt * self, uint16_t ndx, cpdo_data_type * val );

            /**
               Assigns the int8_t value of the given 0-based column index
               to *val.

               Returns 0 on success. On error, *val is not modified.

               Implementations are required to convert other numeric
               types to int8_t for this purpose. NULL values must
               convert to 0.
            */
            int (*i8)( cpdo_stmt * self, uint16_t ndx, int8_t * val );

            /**
               Assigns the int16_t value of the given 0-based column index
               to *val.

               Returns 0 on success. On error, *val is not modified.

               Implementations are required to convert other numeric
               types to int16_t for this purpose. NULL values must
               convert to 0.
            */
            int (*i16)( cpdo_stmt * self, uint16_t ndx, int16_t * val );

            /**
               Assigns the int32_t value of the given 0-based column index
               to *val.

               Returns 0 on success. On error, *val is not modified.

               Implementations are required to convert other numeric
               types to int32_t for this purpose. NULL values must
               convert to 0.
            */
            int (*i32)( cpdo_stmt * self, uint16_t ndx, int32_t * val );
            /**
               Assigns the int64_t value of the given 0-based column index
               to *val.

               Returns 0 on success. On error, *val is not modified.

               Implementations are required to convert other numeric
               types to int64_t for this purpose. NULL values must
               convert to 0.
            */
            int (*i64)( cpdo_stmt * self, uint16_t ndx, int64_t * val );

            /**
               Assigns the double value of the given 0-based column index
               to *val.

               Returns 0 on success. On error, *val is not modified.

               Implementations are required to convert other numeric
               types to float for this purpose. NULL values must
               convert to 0.0.
            */
            int (*flt)( cpdo_stmt * self, uint16_t ndx, float * val );

            /**
               Assigns the double value of the given 0-based column index
               to *val.

               Returns 0 on success. On error, *val is not modified.

               Implementations are required to convert other numeric
               types to double for this purpose. NULL values must
               convert to 0.0.
            */
            int (*dbl)( cpdo_stmt * self, uint16_t ndx, double * val );
        
            /**
               Assigns the string value of the given 0-based column
               index to *val. Note that some (most?) drivers will
               convert just about anything to a string, so this can
               normally be used to fetch numeric fields (but that is
               not a required behaviour).

               If len is not NULL then on success it is set to the
               length of the fetched string. (Returned strings are
               always NUL-terminated, so the length is not always
               required by the client.)
           
               The memory is owned by the statement handle, and it may be
               invalidated the next time the handle is step()'d or
               when it is cleaned up. Thus clients must copy the data
               if needed.

               Drivers are not required to convert arbitrary
               non-string fields to strings for this purpose, but they
               are encouraged to do so if feasible. If they cannot do
               a conversion to string, they must return cpdo_rc.TypeError
               from this function.
           
               The driver is responsible for ensuring that the string
               is NUL-terminated.

               The data will be encoded using whatever format the
               server sent. Generally speaking, the library assumes an
               encoding of UTF8, but it does not enforce this (nor
               does it really care).

               Returns 0 on success. On error, *val is not modified.

               Drivers must return SQL NULL values by setting *val to
               NULL if at all possible. It is recognized, however,
               that some underlying APIs may not allow clients to
               distinguish between an empty string and an SQL NULL, in
               which case implementations should assign *val to an
               empty string. In either case they must set *len (if len
               is not NULL) to 0.

               FIXME? Use (unsigned char const **)?
            */
            int (*string)( cpdo_stmt * self, uint16_t ndx, char const ** val, uint32_t * len );

            /**
               Assigns the blob value of the given 0-based column index
               to *val.

               Blob support is optional, and implementations which don't
               support it must return cpdo_rc.UnsupportedError from this
               function.
               
               Implementations must return SQL NULL values by setting
               *val to NULL if at all possible. It is recognized,
               however, that some underlying APIs may not allow
               clients to distinguish between an empty blobs and an
               SQL NULL, in which case implementations should assign
               *val to "an empty value" (which reading from is
               invalid). In either case they must set *len (if len is
               not NULL) to 0. Drivers may, out of concern for safety,
               return cpdo_rc.ArgError if len is NULL and the result
               would be a zero-length blob.

               The returned memory is owned by the statement handle,
               and it may be invalidated the next time the handle is
               step()'d or when it is cleaned up. Thus clients must
               copy the data if needed. *len (if len is not NULL) is
               assigned to the length, in bytes, of the memory.

               Implementations must allow a NULL len value, under the
               assumption that the client knows that the hell he's
               doing and has some way to count the number of bytes
               himself. A NULL len can be especially problematic for
               the caller if the row holds a length-zero blob because
               the caller will get memory which is not legal to read
               (the user gets a *len=0 in that case if len!=NULL).

               Note that drivers may have "relatively small" limits on
               blobs.  e.g. sqlite3 uses int as the blob size, and
               therefore cannot handle "really big" blobs.
           
               Returns 0 on success. On error, *val and *len must not be
               modififed.

               See string() for more notes.
            */
            int (*blob)( cpdo_stmt * self, uint16_t ndx, void const ** val, uint32_t * len );
        } get;
    };

    /** @struct cpdo_driver

        This class represents a single instance of a concrete database
        driver implementation.

       Clients may use the 'api' member but must not touch
       the 'impl' member.

    */
    struct cpdo_driver
    {
        /**
           The "vtbl" of "virtual" operations. That is, those
           which are specified by the cpdo interface but which require
           driver-specific implementations. This must not be NULL.

           All instances of a given driver share the same instance
           of cpdo_driver_api.
        */
        cpdo_driver_api const * api;

        /**
           Holds all implementation-specific data. Client code must
           _never_ use this field, and any modification from anwhere other
           than the concrete driver implementation invokes undefined
           behaviour.
        */
        void * impl;
    };

#if 0
#define cpdo_driver_empty_m {NULL/*api*/, NULL/*impl*/}
#endif

    /**@struct cpdo_stmt

       Represents a single "statement" _and_ result set,
       similar to PHP's PDOStatement.

       Clients may use the 'api' member but must not touch
       the 'impl' member.
    */
    struct cpdo_stmt
    {
        /**
           The "vtbl" for statement handles. That is, those
           which are specified by the cpdo interface but which require
           driver-specific implementations. This must not be NULL.

           All instances of cpdo_stmt which originate from the same
           driver share the same instance of cpdo_stmt_api.
        */
        cpdo_stmt_api const * api;

        /**
           Arbitrary implementation-dependent data. Client code
           must not touch this.
        */
        void * impl;
    };

    /** Empty-initialized cpdo_stmt object. */
#define cpdo_stmt_empty_m {                     \
        NULL/*api*/,                            \
        NULL/*impl*/                        \
    }

    /** Empty-initialized cpdo_stmt object. */
    extern const cpdo_stmt cpdo_stmt_empty;

    /**
       Tries to load a new database driver instance, using the
       given driver name, but does not connect to a database.

       On success *tgt is assigned to the new instance, which the
       caller owns and must eventually free using
       driver->api->close(driver).

       On error, non-0 is returned and *tgt is not modified.

       The opt is currently ignored and may be NULL. This may
       eventually be used to pass PDO-style option/parameter strings,
       be replaced by varargs, or it might be removed altogether.

       After this returns, use driver->api->connect(driver,...) to
       connect to a specific database.

       @code
       cpdo_driver * drv = NULL; // do not forget the NULL!
       int rc = cpdo_driver_new( &drv, "sqlite3", NULL );
       if( rc ) {
           printf("Error: #%d: %s\n",rc, cpdo_rc_string(rc));
           return ...;
       }
       else {
          ... use db ...
          ... eventually close it ...
          drv->api->close(drv);
       }
       @endcode

       Note that in the error case, drv is NULL. This is in contrast
       to cpdo_driver_new_connect().
       
       @see cpdo_driver_new_connect()
    */
    int cpdo_driver_new( cpdo_driver ** tgt, char const * driver,
                         char const * opt );

    /**
       Similar to cpdo_driver_new(), but also tries to connect
       and has slightly different return value semantics
       for the *drv parameter.

       Neither of drv or driver may be NULL. The user and password
       parameters are given to the driver, and may or may not be
       required for that driver (see its documentation).
       
       Tries to load a new db driver using the given dsn string, and
       then connect to it.

       On success, returns 0 and assigns *drv to the new connection,
       which the caller owns.

       On error non-zero is returned and the caller STILL owns *drv.
       The reason for this is so that he can fetch any db-level
       error information before destroying it. If the function
       fails before it can create a driver, *drv is not modified,
       so the client should assign it to NULL before calling this
       to ensure that he can distinguish that case.

       Note that not all drivers require user/password parameters, and
       these arguments may be NULL for such drivers. errMsg may always
       be NULL, but is the only way to get any useful error information
       when connection fails, because the client cannot later fetch
       the driver-level error info
       info

       Example:

       @code
       cpdo_driver * drv = NULL; // do not forget the NULL!
       int rc = cpdo_driver_new_connect( &drv, .... );
       if( rc ) {
           if( drv ) { // means driver was loaded but connect failed
               int err = 0;
               char const * msg = NULL;
               drv->api->error_info( drv, &msg, NULL, &err );
               printf("Error: #%d: %s\n",err,msg);
               drv->api->close(drv);
           }
           else {
               puts("Could not load driver!")
           }
           return ...;
       }
       else {
          ... use db ...
          ... eventually close it ...
          drv->api->close(drv);
       }
       @endcode

       
       Notice that in the error case drv might not be NULL and we
       still have to clean it up. This is the only way to allow
       clients to fetch the connection error information.  We say
       "might" be NULL: if it is NULL then the error happened before a
       connection attempt could be made. e.g. no driver could be found
       for the given driver name, some required argument was NULL,
       or a memory allocation error prevented the object from being
       created.

       @see cpdo_driver_new_connect()
    */
    int cpdo_driver_new_connect( cpdo_driver ** drv, char const * dsn,
                                 char const * user, char const * passwd );

    /**
       A generic SQL quoting algorithm, intended mainly for use by driver
       implementations which use ANSI SQL-style quoting.

       str is the string to quote. *len must be its length.
   
       Allocates a new string with the contents of str, and all quoteChar
       characters in the string gets escaped by escapeChar. If addQuotes
       is not 0 then the returned value is also wrapped in the quote
       character defined by quoteChar, otherwise it is returned without
       surrounding quotes.

       Results are undefined if str is not ASCII encoded. It MIGHT
       work as-is with UTF8, but i haven't tried it.
   
       On success, 0 is returned, *dest is pointed to the new string, and
       *len is assigned to the length of the new string. The caller owns
       the string and must eventually free() it.

       If str is NULL it is treated as an SQL NULL value. *len will be set
       to 4 and *dest will be set to "NULL" (without quotes).

       On error non-0 is returned (one of the cpdo_rc values) and *len and
       *dest are unmodified.

       This implementation sacrifices a bit of speed in favour of
       doing a single, best-fit allocation. It first counts how many
       characters it has to escape, the allocates the string, then
       copies/escapes the bytes.

       FIXME: port in the cson/sqlite3 utf8 traversal code, so we can
       make this safe for UTF8.
    */
    int cpdo_sql_escape( char const * str, uint32_t * len, char ** dest,
                         char quoteChar, char escapeChar, char addQuotes );

    /**
       Prepares and executes the first len bytes of the given SQL code in
       a single step. This is only useful for query types which do not
       have any sensible result set, or where one does not care about the
       result set. e.g. CREATE TABLE or INSERT.
    */
    int cpdo_exec( cpdo_driver * drv, char const * sql, uint32_t len );

    /**
       Returns the driver's name, or NULL if drv is NULL.
    */
    char const * cpdo_driver_name( cpdo_driver const * drv );

    /**
       A typedef for cpdo_driver factory functions.

       Implementations must try to instantiate a new instance, assign
       it to *tgt, and return 0 on success. On error they must return
       non-0 (preferably a value from cpdo_rc,
       e.g. whio_rc.AllocError) and must not modify *tgt.

       tgt must not be NULL and *tgt should be NULL.
    */
    typedef int (*cpdo_driver_factory_f)( cpdo_driver ** tgt );

    /**
       Registers a factory for a cpdo_driver using the given name.

       On success it returns 0. On error the factory is not registered,
       and the return code should give you some idea about why.

       If a registration already exists with this name,
       cpdo_rc.AccessError is returned.
   
       Caveats:

       - This is not thread-safe. If called multiple times concurrently,
       or concurrent to cpdo_available_drivers(), results are undefined.
       See cpdo_available_drivers() for more details.

       - The internal list has a fixed maximum size. It "should" be "large
       enough", but if it fills up then subsequent registrations will
       cause cpdo_rc.AllocError to be returned (though there is no actual
       allocation going on, it's a logical allocation error because we
       cannot (or do not) expand the list at runtime). The API guarantees
       that at least 20 slots are available for client use. It's not
       that we expect to have that many drivers, but to allow us to add
       aliases. e.g. if MySQL 6 DBs can be used with the MySQL 5 driver
       then we might want to alias "mysql6" to the "mysql5" driver.

    */
    int cpdo_driver_register( char const * name, cpdo_driver_factory_f );

    /**
       Returns an array containing the names of all registered drivers.

       The last entry in the array is NULL.

       This function is not thread-safe. If called concurrent to
       cpdo_driver_register(), results are undefined. The return result
       itself is also not thread-safe in conjunction with
       cpdo_driver_register(), and calling that function may change the
       values pointed to by this array. It won't change their addresses
       (the pointers will still be valid) but might change their
       contents. In such a case, nothing horribly drastic will happen. e.g.
       if cpdo_driver_register() is called concurrently with
       cpdo_driver_new() using the same driver name, the worst that will
       happen is that cpdo_driver_new() won't find the driver.
    */
    char const * const * cpdo_available_drivers();

    /** Do not use - its interface is as yet undecided. */
    int cpdo_driver_opt_set( cpdo_driver * self, char const * key, ... );
    /** Do not use - its interface is as yet undecided. */
    int cpdo_driver_opt_get( cpdo_driver * self, char const * key, ... );

    /**
       Returns true (non-0) if st is NULL or if st->api->get.type(st,ndx)
       is CPDO_TYPE_NULL. This only has meaningful results when used
       on fetchable statements (e.g. SELECT).
    */
    char cpdo_stmt_col_is_null( cpdo_stmt const * st, uint16_t ndx );

    /**
       Convenience form of st->api->get.type() which returns
       CPDO_TYPE_ERROR on error.
    */
    cpdo_data_type cpdo_get_type( cpdo_stmt * st, uint16_t ndx );
    
    /**
       Convenience form of st->api->get.i8() which returns 0
       on error.
    */
    int8_t cpdo_get_int8( cpdo_stmt * st, uint16_t ndx );

    /**
       Convenience form of st->api->get.i16() which returns 0
       on error.
    */
    int16_t cpdo_get_int16( cpdo_stmt * st, uint16_t ndx );

    /**
       Convenience form of st->api->get.i32() which returns 0
       on error.
    */
    int32_t cpdo_get_int32( cpdo_stmt * st, uint16_t ndx );

    /**
       Convenience form of st->api->get.i64() which returns 0
       on error.
    */
    int64_t cpdo_get_int64( cpdo_stmt * st, uint16_t ndx );

    /**
       Convenience form of st->api->get.dbl() which returns 0
       on error.
    */
    double cpdo_get_double( cpdo_stmt * st, uint16_t ndx );

    /**
       Convenience form of st->api->get.flt() which returns 0
       on error.
    */
    float cpdo_get_float( cpdo_stmt * st, uint16_t ndx );

    /**
       Convenience form of st->api->get.string() which returns NULL on
       error.
    */
    char const * cpdo_get_string( cpdo_stmt * st, uint16_t ndx, uint32_t * len );
    
    /**
       Convenience form of st->api->get.string() which returns NULL on
       error.
    */
    void const * cpdo_get_blob( cpdo_stmt * st, uint16_t ndx, uint32_t * len );

    /**
       Convenience form of st->api->bind.int8().
    */
    int cpdo_bind_int8( cpdo_stmt * st, uint16_t ndx, int8_t v );

    /**
       Convenience form of st->api->bind.int16().
    */
    int cpdo_bind_int16( cpdo_stmt * st, uint16_t ndx, int16_t v );

    /**
       Convenience form of st->api->bind.int32().
    */
    int cpdo_bind_int32( cpdo_stmt * st, uint16_t ndx, int32_t v );

    /**
       Convenience form of st->api->bind.int64().
    */
    int cpdo_bind_int64( cpdo_stmt * st, uint16_t ndx, int64_t v );

    /**
       Convenience form of st->api->bind.flt().
    */
    int cpdo_bind_float( cpdo_stmt * st, uint16_t ndx, float v );

    /**
       Convenience form of st->api->bind.double().
    */
    int cpdo_bind_double( cpdo_stmt * st, uint16_t ndx, double v );

    /**
       Convenience form of st->api->bind.string().
    */
    int cpdo_bind_string( cpdo_stmt * st, uint16_t ndx, char const * v, uint32_t length );

    /**
       Convenience form of st->api->bind.blob().
    */
    int cpdo_bind_blob( cpdo_stmt * st, uint16_t ndx, void const * v, uint32_t length );
    
    /**
       Convenience form of drv->api->close(drv).

       After calling this, drv is no longer valid.
    */
    int cpdo_close( cpdo_driver * drv );

    /**
       Convenience form of drv->api->connect( drv, opt );
    */
    int cpdo_driver_connect( cpdo_driver * drv, cpdo_connect_opt const * opt );

    /**
       Convenience form of drv->api->prepare(drv, tgt, sql, length).
    */
    int cpdo_prepare( cpdo_driver * drv, cpdo_stmt ** tgt, char const * sql, uint32_t length );

    /**
       Convenience form of st->api->step(st).
    */
    cpdo_step_code cpdo_step( cpdo_stmt * st );

    /**
       Convenience form of st->api->finalize(st).

       After calling this, st is no longer valid.

       All statements opened for a given database MUST be finalized
       before the db is closed, or undefined behaviour ensues.
    */
    int cpdo_stmt_finalize( cpdo_stmt * st );

    /**
       Convenience form of drv->api->close(drv).

       After calling this, drv is no longer valid.

       All statements opened for a given database MUST be finalized
       before the db is closed, or undefined behaviour ensues.
    */
    int cpdo_close( cpdo_driver * drv );

    /**
       Convenience form of drv->api->last_insert_id(drv,...,hint)
       which returns 0 on error.
    */
    uint64_t cpdo_last_insert_id( cpdo_driver * drv, char const * hint );


    /**
       Works like vprintf(), but appends all output to a
       dynamically-allocated string, expanding the string as necessary to
       collect all formatted data. The returned nul-terminated string is
       owned by the caller and it must be cleaned up using free(). If !fmt
       or if the expanded string evaluates to empty, null is returned, not
       a length-0 (1-byte) string.
       
       This implementation of printf() has several custom format
       specifiers:
       
        %%z works like %%s, but takes a non-const (char *) and this 
        code deletes the string (using free()) after appending it to 
        the output.

         %%h (HTML) works like %s but converts certain characters 
         (like '<' and '&' to their HTML escaped equivalents.

         %%t (URL encode) works like %%s but converts certain 
         characters into a representation suitable for use in an 
         HTTP URL. (e.g. ' ' gets converted to %%20)

         %%T (URL decode) does the opposite of %t - it decodes 
         URL-encoded strings.

         %%r requires an int and renders it in "ordinal form". That 
         is, the number 1 converts to "1st" and 398 converts to 
         "398th".

         %%q quotes a string as required for SQL. That is, '\'' 
         characters get doubled. ONLY use this when you know that the
         underlying driver supports ANSI-SQL-style strings.

         %%Q as %%q, but includes the outer '\'' characters and null pointers
         are replaced by SQL NULL.

         (The %%q and %%Q specifiers are options inherited from this printf
         implementation's sqlite3 genes.)

         These extensions may be disabled by setting certain macros when
         compiling the library (see cpdo_printf.c for the gory details).
    */
    char * cpdo_mprintf_v( char const * fmt, va_list vargs );

    /**
        Equivalent to cpdo_mprintf_v(), but takes elipsis arguments instead
        of a va_list.
    */
    char * cpdo_mprintf( char const * fmt, ... );

    /**
        A convenience wrapper around cpdo_mprintf() and 
        cpdo_prepare(), which allows one to create SQL using 
        printf-style formatting specifiers.
        
        Returns 0 on success.
    */
    int cpdo_prepare_f_v( cpdo_driver * drv, cpdo_stmt ** tgt, char const * fmt, va_list );

    /**
        Equivalent to cpdo_prepare_f_v() but takes elipsis arguments.
    */
    int cpdo_prepare_f( cpdo_driver * drv, cpdo_stmt ** tgt, char const * fmt, ... );

    /**
        A convenience wrapper around cpdo_mprintf() and 
        cpdo_exec(), which allows one to create SQL using 
        printf-style formatting specifiers.
        
        Returns 0 on success.
    */
    int cpdo_exec_f_v(  cpdo_driver * drv, char const * fmt, va_list );

    /**
        Equivalent to cpdo_exec_f_v() but takes elipsis arguments.
    */
    int cpdo_exec_f(  cpdo_driver * drv, char const * fmt, ... );

    /**
        A convenience wrapper around cpdo_mprintf() and 
        cpdo_bind_string(), which allows one to set string values using 
        printf-style formatting specifiers.
        
        Returns 0 on success.
    */
    int cpdo_bind_string_f_v( cpdo_stmt * st, uint16_t ndx, char const *fmt, va_list );

    /**
        Equivalent to cpdo_bind_string_f_v() but takes elipsis arguments.
    */
    int cpdo_bind_string_f( cpdo_stmt * st, uint16_t ndx, char const * fmt, ... );
    
    /**
        Convenience form of st->api->bind.param_name(st, name).
        
        Returns 0 if st is NULL.
    */
    uint16_t cpdo_param_index( cpdo_stmt * st, char const * name );

    /**
        convenience form of st->api->bind.param_name(st, ndx).
        
        Returns NULL if st is NULL.
    */
    char const * cpdo_param_name( cpdo_stmt * st, uint16_t ndx );

    /**
        Returns the value of CPDO_VERSION_NUMBER this library was
        compiled against.

        Added in version 20110209.
    */
    int cpdo_version_number();

    /**
       Convenience form of cpdo_driver::qualify() which returns the
       new string on success and NULL on error.
     */
    char * cpdo_qualify_identifier( cpdo_driver * drv, char const * iden );

    /**
       Convenience form of drv->api->free_string(drv,str).
    */
    int cpdo_free_string( cpdo_driver * drv, char * str );

    /**
       Callback type used by cpdo_step_each(). It is called only in
       response to a successful step() operation on st, and this function
       may use st to fetch the current row's data, but must not step()
       the statement or otherwise modify it. The state argument is the
       3rd argument passed to cpdo_step_each().

       Implementations must return 0 on success, non-zero on error,
       preferably a value from cson_rc.

       @see cpdo_step_each()
       @see cpdo_exec_each()
    */
    typedef int (*cpdo_step_each_f)( cpdo_stmt * st, void * state );

    /**
       Iterates over a SELECT-style curstor and calls a callback for
       each iteration.

       For each row it calls f(stmt,state), stopping if f() returns
       non-0. Returns 0 on success, non-0 on error. If f() returns
       non-0, that value is returned. If step() fails then
       cpdo_rc.CheckDbError will be returned, a sign that the caller
       should ask the statement for the driver-level error
       information.

       The client is free to return some magic value (e.g. -9999) from
       his step function to indicate to the caller of this function
       that it aborted early on purpose, as opposed to due to an
       error.

       @see cpdo_exec_each()
       @see cpdo_exec_each_f()
       @see cpdo_exec_each_f_v()
    */
    int cpdo_step_each( cpdo_stmt * stmt, cpdo_step_each_f f, void * state );

    /**
       Convenience form of cpdo_step_each() which steps through the
       first sqlLength bytes of the given SQL string, which is assumed
       to be a statement which is capable of returning results. The
       SQL must represent a single statement.

       Returns 0 on success.

       @see cpdo_step_each()
       @see cpdo_exec_each_f()
       @see cpdo_exec_each_f_v()
    */
    int cpdo_exec_each( cpdo_driver * db, cpdo_step_each_f f, void * state,
                        char const * sql, unsigned int sqlLength );

    /**
       Variadic form of cpdo_exec_each(), accepts the same formatting
       specifies supported by cpdo_prepare_f_v().

       @see cpdo_step_each()
       @see cpdo_exec_each()
       @see cpdo_exec_each_f()
    */
    int cpdo_exec_each_f_v( cpdo_driver * db, cpdo_step_each_f f, void * state,
                            char const * sql, va_list args );

    /**
       Variadic form of cpdo_exec_each(), accepts the same formatting
       specifies supported by cpdo_prepare_f().

       @see cpdo_step_each()
       @see cpdo_exec_each()
       @see cpdo_exec_each_f_v()
    */
    int cpdo_exec_each_f( cpdo_driver * db, cpdo_step_each_f f, void * state,
                          char const * sql, ... );

    /**
       A generic buffer class.

       They can be used like this:

       @code
       cpdo_buffer b = cpdo_buffer_empty;
       int rc = cpdo_buffer_reserve( &buf, 100 );
       if( 0 != rc ) { ... allocation error ... }
       ... use buf.mem ...
       ... then free it up ...
       cpdo_buffer_reserve( &buf, 0 );
       @endcode

       To take over ownership of a buffer's memory:

       @code
       void * mem = b.mem;
       // mem is b.capacity bytes long, but only b.used
       // bytes of it has been "used" by the API.
       b = cpdo_buffer_empty;
       @endcode

       The memory now belongs to the caller and must eventually be
       free()d.

       @see cpdo_buffer_reserve()
       @see cpdo_buffer_fill()
       @see cpdo_buffer_fill_from()
    */
    struct cpdo_buffer
    {
        /**
           The number of bytes allocated for this object.
           Use cpdo_buffer_reserve() to change its value.
        */
        unsigned int capacity;
        /**
           The number of bytes "used" by this object. It is not needed for
           all use cases, and management of this value (if needed) is up
           to the client. The cpdo_buffer public API does not use this
           member. The intention is that this can be used to track the
           length of strings which are allocated via cpdo_buffer, since
           they need an explicit length and/or null terminator.
        */
        unsigned int used;

        /**
           The memory allocated for and owned by this buffer.
           Use cpdo_buffer_reserve() to change its size or
           free it. To take over ownership, do:

           @code
           void * myptr = buf.mem;
           buf = cpdo_buffer_empty;
           @endcode

           (You might also need to store buf.used and buf.capacity,
           depending on what you want to do with the memory.)
       
           When doing so, the memory must eventually be passed to free()
           to deallocate it.
        */
        unsigned char * mem;
    };
    /** Convenience typedef. */
    typedef struct cpdo_buffer cpdo_buffer;

    /** An empty-initialized cpdo_buffer object. */
#define cpdo_buffer_empty_m {0/*capacity*/,0/*used*/,NULL/*mem*/}
    /** An empty-initialized cpdo_buffer object. */
    extern const cpdo_buffer cpdo_buffer_empty;

    /**
       Typedef for functions which act as a generic byte input source.

       The arguments are:

       - state: implementation-specific state needed by the function.

       - n: when called, *n will be the number of bytes the function
       should read and copy to dest. The function MUST NOT copy more than
       *n bytes to dest. Before returning, *n must be set to the number of
       bytes actually copied to dest. If that number is smaller than the
       original *n value, the input is assumed to be completed (thus this
       is not useful with non-blocking readers).

       - dest: the destination memory to copy the data do.

       Must return 0 on success, non-0 on error (preferably a value from
       cpdo_rc).

       Callers of this routine are not required to have any knowledge
       of multi-byte characters, and this routine is allowed to return
       partial characters when reading UTF multi-byte character.
    */
    typedef int (*cpdo_data_source_f)( void * state, void * dest, unsigned int * n );

    /**
       Reserves the given amount of memory for the given buffer object.

       If n is 0 then buf->mem is freed and its state is set to
       NULL/0 values.

       If buf->capacity is less than or equal to n then 0 is returned and
       buf is not modified.

       If n is larger than buf->capacity then buf->mem is (re)allocated
       and buf->capacity contains the new length. Newly-allocated bytes
       are filled with zeroes.

       On success 0 is returned. On error non-0 is returned and buf is not
       modified.

       buf->mem is owned by buf and must eventually be freed by passing an
       n value of 0 to this function.

       buf->used is never modified by this function unless n is 0, in which case
       it is reset.
    */
    int cpdo_buffer_reserve( cpdo_buffer * buf, unsigned int n );

    /**
       Fills all bytes of the given buffer with the given character.
       Returns the number of bytes set (buf->capacity), or 0 if
       !buf or buf has no memory allocated to it.
    */
    unsigned int cpdo_buffer_fill( cpdo_buffer * buf, char c );

    /**
       Uses a cpdo_data_source_f() function to buffer input into a
       cpdo_buffer.

       dest must be a non-NULL, initialized (though possibly empty)
       cpdo_buffer object. Its contents, if any, will be overwritten by
       this function, and any memory it holds might be re-used.

       The src function is called, and passed the state parameter, to
       fetch the input. If it returns non-0, this function returns that
       error code. src() is called, possibly repeatedly, until it reports
       that there is no more data.

       Whether or not this function succeeds, dest still owns any memory
       pointed to by dest->mem, and the client must eventually free it by
       calling cpdo_buffer_reserve(dest,0).

       dest->mem might (and possibly will) be (re)allocated by this
       function, so any pointers to it held from before this call might be
       invalidated by this call.
   
       On error non-0 is returned and dest has almost certainly been
       modified but its state must be considered incomplete.

       Errors include:

       - dest or src are NULL (cpdo_rc.ArgError)

       - Allocation error (cpdo_rc.AllocError)

       - src() returns an error code

       Whether or not the state parameter may be NULL depends on
       the src implementation requirements.

       On success dest will contain the contents read from the input
       source. dest->used will be the length of the read-in data, and
       dest->mem will point to the memory. dest->mem is automatically
       NUL-terminated if this function succeeds, but dest->used does not
       count that terminator. On error the state of dest->mem must be
       considered incomplete, and is not guaranteed to be NUL-terminated.

       Example usage:

       @code
       cpdo_buffer buf = cpdo_buffer_empty;
       int rc = cpdo_buffer_fill_from( &buf, cpdo_data_source_FILE, stdin );
       if( rc )
       {
         fprintf(stderr,"Error %d (%s) while filling buffer.\n", rc, cpdo_rc_string(rc));
         cpdo_buffer_reserve( &buf, 0 );
         return ...;
       }
       ... use the buf->mem ...
       ... clean up the buffer ...
       cpdo_buffer_reserve( &buf, 0 );
       @endcode

       To take over ownership of the buffer's memory, do:

       @code
       void * mem = buf.mem;
       buf = cpdo_buffer_empty;
       @endcode

       In which case the memory must eventually be passed to free() to
       free it.
    */
    int cpdo_buffer_fill_from( cpdo_buffer * dest, cpdo_data_source_f src, void * state );

    /**
       A cpdo_data_source_f() implementation which requires the state argument
       to be a readable (FILE*) handle.
    */
    int cpdo_data_source_FILE( void * state, void * dest, unsigned int * n );
    
    
    /** LICENSE

    This software's source code, including accompanying documentation and
    demonstration applications, are licensed under the following
    conditions...

    The author (Stephan G. Beal [http://wanderinghorse.net/home/stephan/])
    explicitly disclaims copyright in all jurisdictions which recognize
    such a disclaimer. In such jurisdictions, this software is released
    into the Public Domain.

    In jurisdictions which do not recognize Public Domain property
    (e.g. Germany as of 2011), this software is Copyright (c) 2011
    by Stephan G. Beal, and is released under the terms of the MIT License
    (see below).

    In jurisdictions which recognize Public Domain property, the user of
    this software may choose to accept it either as 1) Public Domain, 2)
    under the conditions of the MIT License (see below), or 3) under the
    terms of dual Public Domain/MIT License conditions described here, as
    they choose.

    The MIT License is about as close to Public Domain as a license can
    get, and is described in clear, concise terms at:

    http://en.wikipedia.org/wiki/MIT_License

    The full text of the MIT License follows:

    --
    Copyright (c) 2011 Stephan G. Beal (http://wanderinghorse.net/home/stephan/)

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    --END OF MIT LICENSE--

    For purposes of the above license, the term "Software" includes
    documentation and demonstration source code which accompanies
    this software. ("Accompanies" = is contained in the Software's
    primary public source code repository.)

    */

#if defined(__cplusplus)
} /*extern "C"*/
#endif

#endif /* WANDERINGHORSE_NET_CPDO_H_INCLUDED */
/* end of file include/wh/cpdo/cpdo.h */
/* start of file include/wh/cpdo/cpdo_internal.h */
#if !defined(WANDERINGHORSE_NET_CPDO_INTERNAL_H_INCLUDED)
#define WANDERINGHORSE_NET_CPDO_INTERNAL_H_INCLUDED

/** @file cpdo_internal.h

This function contains the "semi-private" API for the cpdo library.
This mainly includes rountines and types which might be useful
for specific cpdo_driver implementations.

Author: Stephan Beal (http://www.wanderinghorse.net/home/stephan/)

License: Dual Public Domain/MIT. The full license text is at the
bottom of the main header file (cpdo.h).

*/


#if defined(__cplusplus)
extern "C" {
#endif

    /** @internal

    Semi-internal: intended for use by concrete driver implementations,
    for parsing DSN parameters.

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
    debug builds if passed NULL as either parameter. In non-debug builds
    it will return 0 in that case.

    Sample usage (because it might not be obvious based on the details
    given above:


    @code
    char const * tokBegin = "foo/bar///baz";
    char const * tokEnd = NULL;
    while( cpdo_next_token( &tokBegin, '/', &tokEnd) ) {
    if( tokBegin == tokEnd ) break;
    else {
    ... the "half-open" range [tokBegin,tokEnd) now
    ... contains token bytes. Note that they are not
    ... NUL-terminated and they immediately preceed the
    ... next token's bytes, so they must normally be
    ... copied to a NUL-terminated buffer in order to
    ... be useful.

    ... When finished, set up the pointers for the next
    ... loop iteration:

    tokBegin = tokEnd;
    tokEnd = NULL;
    }
    }
    @endcode

    Given those sample inputs, the above loop would run 3 times, fetching
    the tokens "foo", "bar", and "baz".

    */
    char cpdo_next_token( char const ** inp, char separator, char const ** end );

    /**
       Returns non-0 if str is not NULL and starts with any of
       [1tTyY], else returns 1. Intended primarily for use with tokens
       parsed from cpdo_next_token(), e.g. boolean DSN parameter
       values.
    */
    char cpdo_token_bool_val(char const * str);

    /**
       Semi-internal: intended for driver implementations.

       This function takes a DSN-style string and splits it
       into two parts.

       out must be valid memory at least outLen bytes long.  If dsn is
       as long or longer than outLen then cpdo_rc.RangeError is
       returned (we need one extra byte to ensure a NUL terminator).
       The contents of out (up to outLen bytes) will be initialized by
       this function, so the caller need not memset() it before
       calling this.

       On success, will contain two parts: the part of the dsn up to the
       first ':' character. That ':' will be replaced by a NUL byte. The
       *paramPos parameter (if not NULL) will be assigned to point to the
       byte directly after the new NUL byte, i.e. the parameters part of
       the DSN.

       On error, non-0 is returned and the output must not be used.

       Error conditions include:

       - dsn==NULL: cpdo_rc.ArgError

       - dsn with a length of less than 2 or dsn length>=outLen:
       cpdo_rc.RangeError. (2 is the minimum legal length length for
       a driver taking no parameters, e.g. "x:".)

       This function does not validate that the driver part of the
       string or the parameter parts are actually syntactically or
       symantically valid.
       
       Example:

       Given the DSN "drivername:foo=bar...", after this returns,
       out would contain "drivername<NUL>foo=bar...", and *paramPos
       (if not NULL) will point to "foo=bar...".
    */
    int cpdo_split_dsn( char const * dsn, char * out, uint32_t outLen,
                        char const ** paramPos );

    
    typedef struct cpdo_bind_val cpdo_bind_val;

    /** @struct cpdo_bind_val

       Semi-internal - intended only for driver developers.
    
       This type exists to give driver bindings a place to store
       result buffers (for drivers which require the client to
       supply such buffers).

       To properly manage their memory, use the cpdo_bind_val_TYPE()
       family of functions to set their values and
       cpdo_bind_val_clean() or cpdo_bind_val_free() (see their docs
       for the difference) to clean them up.

       These objects, if used at all, are for internal use within a
       given driver binding, and are not intended to leak out into
       client code. Thus the driver code is free to take a few
       liberties with these objects (in terms of memory management and
       whatnot), provided he does not violate the rules specified in
       the documentation.
    */
    struct cpdo_bind_val
    {
        /**
           Specifies which member of the valu member was last used to
           store a value in this object.

           If this type is CPDO_TYPE_NULL or CPDO_TYPE_ERROR, then it
           is not legal to read any of the valu union fields.
        */
        cpdo_data_type type;

        /**
           Drivers MAY set this to non-0 to signify that the value is
           SQL NULL. They MUST set type=CPDO_TYPE_NULL for that, but
           this member exists because MySQL wants a pointer to a spot
           where it can set the value itself.
         */
        char is_null;

        /**
           Drivers MAY set this to non-0 to signify that the value has
           some error condition. e.g. the MySQL driver wants a pointer
           to a spot where it can set the value itself, and it sets
           this flag for us on fetch errors.
        */
        char has_error;

        /**
           The raw value for this object.  Note that it's a union and
           that this class' "type" member specifies which member of
           valu is legal to read. Reading any other values results, as
           specified by the rules for unions, in undefined behaviour.
         */
        union cpdo_bind_val_union_t
        {
            int8_t i8;
            int16_t i16;
            int32_t i32;
            int64_t i64;
            float flt;
            double dbl;
            /**
               Used for both string and binary data.
            */
            struct cpdo_bind_val_blob_t
            {
                /**
                   Memory for the blob, owned by this
                   object.
                */
                void * mem;
                /**
                   The number of bytes in mem.
                */
                unsigned long length;
            } blob;

            /**
               Low-low-level place to store pointers to custom data
               types.
             */
            struct cpdo_bind_val_custom_t
            {
                /**
                   The raw memory. If dtor is not NULL then
                   this object owns the memory, otherwise
                   the user must manage it.
                 */
                void * mem;
                /**
                   Arbitrary value. e.g. a value from
                   enum_mysql_timestamp_type for a
                   MYSQL_TYPE_TIMESTAMP value (or equivalent for a
                   given driver). Drivers can use this to distinguish
                   between multiple types bound with the cpdo type
                   CPDO_TYPE_CUSTOM.
                   
                   See cpdo_bind_val_custom() for more notes on this.
                 */
                int type_tag;
                /**
                   This only exists to please MySQL and similar 
                   drivers. It writes the length of inbound data 
                   (stored in the mem pointer) here.
                 */
                unsigned long length;
                /**
                   Must free the memory passed it (at least
                   semantically). Must accept a parameter of NULL
                   without crashing. i.e. it must behave like free().
                 */
                void (*dtor)( void * );
            } custom;
        } valu;
    };

    /** An empty-initialized cpdo_bind_val object. */    
#define cpdo_bind_val_empty_m { \
        CPDO_TYPE_NULL/*type*/,\
            0/*is_null*/, \
            0/*has_error*/,\
        {/*valu*/0}\
    }

    /** An empty-initialized cpdo_bind_val object. */
    extern const cpdo_bind_val cpdo_bind_val_empty;

    /**
       Cleans any internals owned by b and sets it to a "non-value"
       with the type CPOD_TYPE_NULL. Does not free b.  Returns 0 on
       success. The only detectable error condition is if b is NULL,
       and passing an object with invalid internal state
       (e.g. b->valu.blob.mem is pointed to memory b does not own)
       results in undefined behaviour.
    */
    int cpdo_bind_val_clean( cpdo_bind_val * b );

    /**
       Calls cpdo_bind_val_clean() and then frees b.
       See cpdo_bind_val_clean() for the return values.
    */
    int cpdo_bind_val_free( cpdo_bind_val * b );

    /**
       Calls cpdo_bind_val_clean() and then sets b to be of type
       CPDO_TYPE_NULL. See cpdo_bind_val_clean() for the return
       values.
    */
    int cpdo_bind_val_null( cpdo_bind_val * b );

    /**
       Calls cpdo_bind_val_clean() and then sets b to be of type
       CPDO_TYPE_CUSTOM, with the given memory, destructor, and
       "typeTag" (which can be whatever the implementor wants.

       mem must point to the bound object. dtor may be NULL
       or may point to a function which will be used to free
       mem when b is cleaned up.

       Any of the parameters may be NULL/0 except for b.

       The inspiration here is that we want to be able to support
       MySql TIMESTAMP fields.

       Please don't use this unless you really know what
       you're doing with it!

       See cpdo_bind_val_clean() for the return values.
       
       Be VERY AWARE that the downstream interpretation of the mem 
       pointer depends entirely on a unique combination of the 
       underlying cpdo_driver (which is presumably using this 
       function) and the value of typeTag. Since 20 different 
       drivers could all have a custom type where they use typeTag 
       2, a more robust mechanism for dolling out typeTags might be 
       in order. On idea is to hash the driver's name, use (say) 24 
       bits from that hash as the top 24 bits of typeTag, and use a 
       value 0-255 for the lower byte. It ain't pretty, but it 
       should help avoid collisions. The small size of the hashing 
       space (all available driver names) is so small that it's 
       highly unlikely that a collision would occur unless a very 
       mismatched hash routine is used. An alternate, which is 
       easier to  implement: use the high bytes of some internally-used
       static object (e.g. the local cpdo_driver::api's address)
       as the mask, and map the custom type ID into the lower byte. 
       The only collision cases might be when two internally-defined 
       cpdo_driver_api instances are allocated with the same
       mask of its upper 24 bytes (we lose bits after 64 because
       of the conversion to int tagType). Since each 
       driver is implemented in its own file (with the excepion of 
       during the cpdo Amalgamation Build), it is exceedingly 
       unlikely that multiple instances will fall into the same 
       255-byte range. By inserting a dummy char array 255 bytes 
       long immediately after the private cpdo_driver_api is set up, 
       we can probably (maybe?) even guaranty no collisions.
       
       The purpose of such an exercise would be to future-proof the 
       driver for when i eventually (possibly) add CPDO_TYPE_CUSTOM 
       support to the public API (remember, this function is 
       semi-private, intended for driver developers). If that is added,
       client code could easily inadvertently pass a type tag from
       a different type, and we can check that by using the masking
       option. The client would probably need to call into the driver
       API to generate such a mask, or we could add it to the public API,
       and use driver->api as the address to mask against. Hmmm...

       FIXME: use a (void const *) as the type tag here, and let the
       driver choose some internal static pointer to associate with
       each type. Disadvantage: can't use it in a
       switch/case. Alternately, take 2 params here: one identifies
       the driver name and one a driver-specific type id. We could
       then construct an internal tag with no collisions based on
       that. BUT... since we only have MySQL for now (and the
       foreseeable future), that might be getting ahead of myself.
       And, in hindsight, since the only way for values bound this way
       to cross into another driver's implementation is if a client
       swaps the impl or api members of cpdo_stmt object, we could
       safely just allow each driver to pass any value as typeTag, and
       not do any type of hashing/verification on it.
    */
    int cpdo_bind_val_custom( cpdo_bind_val * b, void * mem,
                              void (*dtor)(void *), int typeTag );

    
    /**
       Calls cpdo_bind_val_clean() and then sets b to be of type
       CPDO_TYPE_INT8. See cpdo_bind_val_clean() for the return
       values.

       On success, sets b->valu.i8 to v.
    */
    int cpdo_bind_val_int8( cpdo_bind_val * b, int8_t v );

    /**
       Calls cpdo_bind_val_clean() and then sets b to be of type
       CPDO_TYPE_INT16. See cpdo_bind_val_clean() for the return
       values.

       On success, sets b->valu.i16 to v.
    */
    int cpdo_bind_val_int16( cpdo_bind_val * b, int16_t v );

    /**
       Calls cpdo_bind_val_clean() and then sets b to be of type
       CPDO_TYPE_INT32. See cpdo_bind_val_clean() for the return
       values.

       On success, sets b->valu.i132 to v.
    */
    int cpdo_bind_val_int32( cpdo_bind_val * b, int32_t v );

    /**
       Calls cpdo_bind_val_clean() and then sets b to be of type
       CPDO_TYPE_INT64. See cpdo_bind_val_clean() for the return
       values.

       On success, sets b->valu.i64 to v.
    */
    int cpdo_bind_val_int64( cpdo_bind_val * b, int64_t v );

    /**
       Calls cpdo_bind_val_clean() and then sets b to be of type
       CPDO_TYPE_DOUBLE. See cpdo_bind_val_clean() for the return
       values.

       On success, sets b->valu.dbl to v.
    */
    int cpdo_bind_val_double( cpdo_bind_val * b, double v );

    /**
       Calls cpdo_bind_val_clean() and then sets b to be of type
       CPDO_TYPE_FLOAT. See cpdo_bind_val_clean() for the return
       values.

       On success, sets b->valu.dbl to v.
    */
    int cpdo_bind_val_float( cpdo_bind_val * b, float v );

    /**
       Calls cpdo_bind_val_clean() and then sets b to be of type
       CPDO_TYPE_STRING. See cpdo_bind_val_clean() for the return
       values.

       The first len bytes of v are copied by this function and the
       copy is owned by b.  A NUL terminator is added to the copied
       data.

       If v is NULL and len is not then len bytes are allocated
       dynamically and set to 0.

       On success, sets b->valu.blob.mem to the copied memory
       and b->valu.blob.length to len.
    */
    int cpdo_bind_val_string( cpdo_bind_val * b, char const * v, uint32_t len );

    /**
       Calls cpdo_bind_val_clean() and then sets b to be of type
       CPDO_TYPE_BLOB. See cpdo_bind_val_clean() for the return
       values.

       The first len bytes of v are copied by this function and the
       copy is owned by b. The library may, at its descretion, add a
       NUL terminator to the copied data, for sanity's sake.

       If v is NULL and len is not then len bytes are allocated
       dynamically and set to 0.

       On success, sets b->valu.blob.mem to the copied memory
       and b->valu.blob.length to len.
    */
    int cpdo_bind_val_blob( cpdo_bind_val * b, void const * v, uint32_t len );

    /**
       Allocates a list of n cpdo_bind_val object, each initialized to
       hold the default state (that of cpdo_bind_val_empty).

       Returns NULL on allocation error or if n is 0.

       On success the caller owns the returned object and must
       eventually free it using cpdo_bind_val_list_free().
    */
    cpdo_bind_val * cpdo_bind_val_list_new( uint16_t n );

    /**
       Cleans up and frees lists created using
       cpdo_bind_val_list_new().  list must be an array len items
       long, and it must have been allocated via
       cpdo_bind_val_list_new() (or compatible).  Each item in the
       list is passed to cpdo_value_clean(), and then list is freed.

       Returns 0 on success, non-0 on error. The only detectable error
       conditions are if list is NULL or len in 0. It cannot detect if
       someone mucked about incorrectly with the individual values'
       pointer data.
    */
    int cpdo_bind_val_list_free( cpdo_bind_val * list, uint16_t len );


    /**
       Semi-internal: intended to support the MySQL driver but might
       find uses elsewhere.
       
       Searches an SQL string for named parameters in the form :paramName,
       where the ':' character is defined by the paramChar parameter.

       If alsoDoQMarks is true then embedded question-mark
       placeholders are also treated as being identifiers, otherwise
       they are not counted. This is required if SQL uses a mixture of
       names and unnamed bound parameters and you need to know the
       exact bind index of a named parameter.  In such cases, the
       matched parameter name will simply be the question mark.
       
       This code assumes backslash-style escaping rules, a-la MySQL,
       and not ANSI SQL escaping rules. That said: it will behave
       correctly with ANSI-style escaped SQL, but only if the SQL is
       100% well-formed.  See below for more details.
       
       On success non-0 (true) is returned *paramBegin will point to
       the start of the named parameter and *paramEnd points to the
       one-after-the-end position. On error (bad parameters or no
       matches found) *paramBegin and *paramEnd are not modified.

       This function tries to do robust handling of escaped strings,
       strings-within-string, and whatnot, but it can be fooled if
       passed ANSI-SQL-quoted strings which themselves have
       bound-parameter-like substrings within them.

       inpBegin must point to the begining of the input string, which
       is assumed to be well-formed SQL (i.e., that a "prepare"
       operation would succeed as far as the syntax is concerned, not
       counting missing support for named bound parameters).

       len is the number of bytes to scan.

       paramChar must be the prefix character used for binding
       parameters. If it is either a backlash, question mark, single
       quote, or double quote, it is rejected and false is returned
       (to avoid amgbuities in the rest of the parser).

       paramBegin must be a pointer to a string to hold the starting
       position of the result. Likewise, paramEnd must point to place
       where we can store the result position.

       Parameter identifiers are assumed to follow conventional
       identifier naming rules: they start with a alpha- or underscore
       character and containing any number alphanumeric or underscore
       characters.
       
       Example:

       @code
       char const * sql =
           "INSERT INTO T VALUES("
            "\"string 'stuff'\""
            ", :param1"
            ", ':nonParam'"
            ", \"\\\":nonParam\\\"\""
            ", :finalParam"
            ")";
       uint32_t slen = strlen(sql);
       char const * begin = sql;
       char const * end = NULL;
       while( cpdo_find_next_named_param( begin, slen, ':', 0, &begin, &end ) ) {
           printf("Found parameter: ");
           for( ; begin < end; ++begin ) putchar(*begin);
           putchar('\n');
           end = NULL; // set up for next loop iteration.
       }
       @endcode

       If fed input which is not syntactically valid SQL, it _may_ produce
       incorrect results.

       The following legal SQL will actually work here, by a weird
       accident of design:

       @code
       "'''ANSI-style :thisWillNOTLookLikeAParameter''\"
       @endcode

       But the following will produce unexpected results because the enclosing
       string is not quoted properly:
    
       @code
       "''ANSI-style :thisWILLLookLikeAParameter''\"
       @endcode

       Such a statement cannot be prepared by the database (we hope!), so
       it is arguable that a mis-match here will have any real effect on
       usage.

    */
    char cpdo_find_next_named_param( char const * inpBegin,
                                     uint32_t len,
                                     char paramChar,
                                     char alsoDoQMarks,
                                     char const **paramBegin,
                                     char const **paramEnd );

    /**
       Semi-internal: intended to support the MySQL driver but might
       find uses elsewhere.

       This function uses cpdo_find_next_named_param() to parse the
       first len bytes of inp for named parameter markers.  It
       replaces those markers with question marks.  See
       cpdo_find_next_named_param() for the meanings of the first 4
       parameters.

       If any arguments are illegal, *out is not modified and
       cpdo_rc.ArgError is returned.
       
       If it [thinks it] finds any named parameters, it returns the
       number of markers replaced by assigning *count to that value,
       and places a copy of the translated, NUL-terminated string in
       *out.  If outLen is not NULL, the length of the new string is
       written there. The caller must eventually free() the
       *out-stored string.

       If it finds no matches, it returns 0 (success), sets *out
       to NULL, and outLen (if not NULL) to 0.
       
       SQL comments (starting with "--" and going to the next newline character)
       are skipped, and comments inside a quoted string are seen as part
       of the string, not part of a comment.
       
       If the nameList parameter is not NULL and parameters are found
       then the names of all found params are stored as a list here
       (in the order they are found). The list will have the same
       number of entries as *count, and will contain NUL-terminated
       strings. If this function returns an error or finds no matches,
       nameList is never modified. The name list, if used, is
       allocated into the same block of memory as the *out memory, and
       is freed when that memory is freed by the caller.

       This function has an internal hard-coded limit on the number of
       parameters it will parse, and if this limit is exceeded it is
       treated as an error (i.e. *out is not modified), returning
       cpdo_rc.RangeError. The limit is guaranteed to be at least 100.

       This function takes the record (within in the cpdo API) for the
       number of parameters it takes. It also represents my very first
       (char ***) parameter.
    */
    int cpdo_named_params_to_qmarks( char const * inp,
                                     uint32_t len,
                                     char paramChar,
                                     char alsoDoQMarks,
                                     uint16_t * count,
                                     char ** out,
                                     uint32_t * outLen,
                                     char *** nameList 
                                     );


    /**
        Given a "tag type" (see cpdo_bind_val::custom::tag_type and 
        cpdo_bind_val_custom) and the address of some internal 
        static data (doesn't really matter what, as long as it's 
        unique per driver), this function returns a hashcode which 
        can later (in the same app session) be used to help ensure 
        that the driver is later working with the proper custom data 
        type.
        
        i know it sounds confusing... i'm trying.
        
        This can be used to "mangle" custom type IDs in such a way 
        that collisions with type_tags used by other drivers is 
        essentially ... well, highly, highly unlikely.
        
        The return value is intended to be passed to 
        cpdo_hash_type_tag_validate() at some point.
        
        This hash is not guaranteed to be persistent across app
        sessions (and almost certainly won't be).
        
        It is assumed that driver-level type IDs (e.g. MYSQL_TYPE_STRING)
        have values less than 256, occupying only 1 byte of tagType.
        If they have higher values, the hash routine will have a higher
        chance of producing collisions across different keys. (Still not high,
        but higher.)
        
        This can be used in conjunction with cpdo_bind_val_custom() so that 
        we can start future-proofing the code to avoid problems of 
        getting incorrect types passed to us from client code when/if i 
        add CPDO_TYPE_CUSTOM support to the public API. With this, we 
        can validate whether a given CPDO_TYPE_CUSTOM/type_flag 
        combination came from this driver or a different one.
        
        Maybe, anyway.
    */
    int cpdo_bind_val_tag_type_hash( cpdo_driver_api const * key, int tagType );

    /**
        Expects a hash value calculated by cpdo_bind_val_tag_type_hash(key). 
        If tagHash appears to have been generated by that function, as 
        opposed to coming from another source, this function returns 
        non-0 (true), else false. This does not verify the exact type
        encoded in tagHash, but verifies (to the best of its ability)
        that the given hashcode was generated by a call to
        cpdo_bind_val_tag_type_hash(key,someValue).
    */
    char cpdo_bind_val_tag_type_check_origin( cpdo_driver_api const * key, int tagHash );
    
    /* LICENSE

    This software's source code, including accompanying documentation
    and demonstration applications, are licensed under the following
    conditions...

    The author (Stephan G. Beal [http://wanderinghorse.net/home/stephan/])
    explicitly disclaims copyright in all jurisdictions which recognize
    such a disclaimer. In such jurisdictions, this software is released
    into the Public Domain.

    In jurisdictions which do not recognize Public Domain property
    (e.g. Germany as of 2011), this software is Copyright (c) 2011-2012
    by Stephan G. Beal, and is released under the terms of the MIT License
    (see below).

    In jurisdictions which recognize Public Domain property, the user of
    this software may choose to accept it either as 1) Public Domain, 2)
    under the conditions of the MIT License (see below), or 3) under the
    terms of dual Public Domain/MIT License conditions described here, as
    they choose.

    The MIT License is about as close to Public Domain as a license can
    get, and is described in clear, concise terms at:

    http://en.wikipedia.org/wiki/MIT_License

    The full text of the MIT License follows:

    --
    Copyright (c) 2011 Stephan G. Beal (http://wanderinghorse.net/home/stephan/)

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    --END OF MIT LICENSE--

    For purposes of the above license, the term "Software" includes
    documentation and demonstration source code which accompanies
    this software. ("Accompanies" = is contained in the Software's
    primary public source code repository.)

    */

#if defined(__cplusplus)
} /*extern "C"*/
#endif

#endif /* WANDERINGHORSE_NET_CPDO_INTERNAL_H_INCLUDED */
/* end of file include/wh/cpdo/cpdo_internal.h */
#if CPDO_ENABLE_SQLITE3
#if !defined(WANDERINGHORSE_NET_CPDO_CPDO_SQLITE3_H_INCLUDED)
#define WANDERINGHORSE_NET_CPDO_CPDO_SQLITE3_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
/**
    Registers the sqlite3 driver with the cpdo library.
    Returns 0 on success, non-0 on error (see cpdo_driver_register() for
    the return codes).
*/
int cpdo_driver_sqlite3_register();

#if defined(__cplusplus)
} /*extern "C"*/
#endif

#endif /* WANDERINGHORSE_NET_CPDO_CPDO_SQLITE3_H_INCLUDED */

#endif
/*CPDO_ENABLE_SQLITE3*/
#if CPDO_ENABLE_SQLITE4
#if !defined(WANDERINGHORSE_NET_CPDO_CPDO_SQLITE4_H_INCLUDED)
#define WANDERINGHORSE_NET_CPDO_CPDO_SQLITE4_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
/**
    Registers the sqlite4 driver with the cpdo library.
    Returns 0 on success, non-0 on error (see cpdo_driver_register() for
    the return codes).
*/
int cpdo_driver_sqlite4_register();

#if defined(__cplusplus)
} /*extern "C"*/
#endif

#endif /* WANDERINGHORSE_NET_CPDO_CPDO_SQLITE4_H_INCLUDED */

#endif
/*CPDO_ENABLE_SQLITE4*/
#if CPDO_ENABLE_MYSQL5
#if !defined(WANDERINGHORSE_NET_CPDO_CPDO_MYSQL5_H_INCLUDED)
#define WANDERINGHORSE_NET_CPDO_CPDO_MYSQL5_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
/**
    Registers the mysql5 driver with the cpdo library.
    Returns 0 on success, non-0 on error (see cpdo_driver_register() for
    the return codes).
*/
int cpdo_driver_mysql5_register();

#if defined(__cplusplus)
} /*extern "C"*/
#endif
#endif /* WANDERINGHORSE_NET_CPDO_CPDO_MYSQL5_H_INCLUDED */

#endif
/*CPDO_ENABLE_MYSQL5*/
/* end file include/wh/cpdo/cpdo_amalgamation.h */
/* begin file include/wh/cson/cson_cpdo.h */
/** @file cson_cpdo.h

This file contains cson's public cpdo-to-JSON API declarations
and API documentation. If CSON_ENABLE_CPDO is not defined,
or is defined to 0, then including this file will have no side-effects
other than defining CSON_ENABLE_CPDO (if it was not defined) to 0
and defining a few include guard macros. i.e. if CSON_ENABLE_CPDO
is not set to a true value then the API is not visible.

This API requires that <wh/cpdo/cpdo_amalgamation.h> be in the
INCLUDES path and that the client eventually link to (or directly
embed) the cpdo library.
*/
#if !defined(WANDERINGHORSE_NET_CSON_CPDO_H_INCLUDED)
#define WANDERINGHORSE_NET_CSON_CPDO_H_INCLUDED 1
#if !defined(CSON_ENABLE_CPDO)
#  if defined(DOXYGEN)
#    define CSON_ENABLE_CPDO 1
#  else
#    define CSON_ENABLE_CPDO 0
#  endif
#endif

#if CSON_ENABLE_CPDO /* we do this here for the sake of the amalgamation build */

#if defined(__cplusplus)
extern "C" {
#endif

/**
    Converts the results of an cpdo SELECT statement to JSON,
    in the form of a cson_value object tree.
    
    st must be a prepared, but not yet traversed, SELECT query.
    tgt must be a pointer to NULL (see the example below). If
    either of those arguments are NULL, cson_rc.ArgError is returned.
    
    This walks the query results and returns a JSON object which
    has a different structure depending on the value of the 'fat'
    argument.
    
    
    If 'fat' is 0 then the structure is:
    
    @code
    {
        "columns":["colName1",..."colNameN"],
        "rows":[
            [colVal0, ... colValN],
            [colVal0, ... colValN],
            ...
        ]
    }
    @endcode
    
    In the "non-fat" format the order of the columns and row values is
    guaranteed to be the same as that of the underlying query.
    
    If 'fat' is not 0 then the structure is:
    
    @code
    {
        "columns":["colName1",..."colNameN"],
        "rows":[
            {"colName1":value1,..."colNameN":valueN},
            {"colName1":value1,..."colNameN":valueN},
            ...
        ]
    }
    @endcode

    In the "fat" format, the order of the "columns" entries is guaranteed
    to be the same as the underlying query fields, but the order
    of the keys in the "rows" might be different and might in fact
    change when passed through different JSON implementations,
    depending on how they implement object key/value pairs. For most
    data sets with small values, the "thin" format produces much
    less output than the fat format.

    On success it returns 0 and assigns *tgt to a newly-allocated
    JSON object tree (using the above structure), which the caller owns.
    If the query returns no rows, the "rows" value will be an empty
    array, as opposed to null.
    
    On error non-0 is returned and *tgt is not modified.
    
    The error code cson_rc.IOError is used to indicate a db-level
    error, and cson_rc.ArgError is returned if the column count of the
    statement is 0 (indicating an invalid or non-SELECT statement).
    
    The JSON data types are determined by the column type as reported
    by cpdo column type:
    
    CPDO_TYPE_INT8...INT64: integer
    
    CPDO_TYPE_DOUBLE: double
    
    CPDO_TYPE_TEXT or CPDO_TYPE_BLOB: string, and this will only work if
    the data is UTF8 compatible. We treat BLOB as strings for the sake
    of MySQL, where fields of type TEXT are reported as BLOBs (at least
    by cpdo).

    If the db returns a literal or SQL NULL for a value it is converted
    to a JSON null. If it somehow finds a column type it cannot handle,
    the value is also converted to a NULL in the output.

    Example
    
    @code
    cson_value * json = NULL;
    int rc = cson_cpdo_stmt_to_json( myStatement, &json, 1 );
    if( 0 != rc ) { ... error ... }
    else {
        cson_output_FILE( json, stdout, NULL );
        cson_value_free( json );
    }
    @endcode
*/
int cson_cpdo_stmt_to_json( cpdo_stmt * st, cson_value ** tgt, char fat );

/**
    A convenience wrapper around cson_cpdo_stmt_to_json(), which
    takes SQL instead of a cpdo_stmt object. It has the same
    return value and argument semantics as that function.
*/
int cson_cpdo_sql_to_json( cpdo_driver * db, cson_value ** tgt, char const * sql, char fat );

/**
   Converts statement column names to a JSON Array of strings.

   st must be a valid prepared statement with columns (only fetching
   queries have them). This function creates a JSON Array containing
   its column names, in order.
    
   Returns a new Array value on success, which the caller owns.
   On error NULL is returned.
    
   st is not traversed or freed by this function - only the column
   count and names are read.
*/
cson_value * cson_cpdo_stmt_col_names( cpdo_stmt * st );

    
/**
   Converts a single row from a SELECT-style query to a JSON
   object. Each column in the statement is added to the new object as
   a key/value pair, with the key being the value returned by
   st->api->get.column_name().

   st MUST be a just-stepped statement for which the step()
   return value was CPDO_STEP_OK, or results are undefined.

   The exact type of each value in the returned object will be
   determined by matching up the statement-reported type as closely as
   possible with cson's JSON data types. If this is not possible,
   creation of a value will fail and NULL will be returned.
   
   Please see cpdo_stmt_api::cpdo_stmt_api_get::column_name() for
   important details regarding how column names "might" not be what
   the db user expects, and information on how to ensure that they are
   what one expects.
   
   On success it returns a new object containing the key/value
   pairs from the row. On error NULL is returned.

   Ownership of st is not modified.

   @see cson_cpdo_row_to_array()
 */
cson_value * cson_cpdo_row_to_object( cpdo_stmt * st );

/**
   Functionally almost identical to cson_cpdo_row_to_object(), the
   only difference being how the result objects gets its column names.
   st must be a freshly-step()'d handle holding a result row.
   colNames must be an Array with at least the same number of columns
   as st. If it has fewer, NULL is returned and this function has
   no side-effects.

   For each column in the result set, the colNames entry at the same
   index is used for the column key. If a given entry is-not-a String
   then conversion will fail and NULL will be returned.

   The one reason to prefer this over cson_cpdo_row_to_object() is
   that this one can share the keys across multiple rows (or even
   other JSON containers), whereas the former makes fresh copies of
   the column names for each row.
*/
cson_value * cson_cpdo_row_to_object2( cpdo_stmt * st, cson_array * colNames );

    
/**
   Similar to cson_cpdo_row_to_object(), but this function
   creates an Array and stores each value from st's current
   row into that array. Thus a statement with 5 columns will
   result in a 5-element array.

   st MUST be a just-stepped statement for which the step()
   return value was CPDO_STEP_OK, or results are undefined.

   See cson_cpdo_row_to_object() for more details about the
   SQL-to-JSON type conversions.

   On success it returns a new Array value. On error it returns
   NULL. (There are any number of error cases.)

   Ownership of st is not modified.

   @see cson_cpdo_row_to_object()
*/
cson_value * cson_cpdo_row_to_array( cpdo_stmt * st );

/**
    Tries to fetch the contents of 0-based column ndx from st
    as either a blob or string and parse it as JSON. On success
    0 is returned and *tgt will be pointed to the new JSON value,
    which the caller owns. It will be of type Object or Array.
    
    On error it returns non-0 (a cson_rc error code) and tgt is not
    modified.

    If the statement contains no string/blob value in the given field,
    but otherwise encounters no error, 0 is returned but *tgt is not
    modified. Thus the caller must be sure to assign it to NULL before
    calling this so that he can check for NULL when this function
    returns 0. If the given field can be extracted as neither string
    nor blob then cson_rc.TypeError is returned.

    Returns cson_rc.ArgError if either tgt or st are NULL.
*/
int cson_cpdo_parse_json( cpdo_stmt * st, uint16_t ndx, cson_value ** tgt );

/**
   Given a valid cpdo_stmt object, created as the result of a SELECT
   (or db-specific pseudo-select) statement, it converts the value in
   the given 0-based response column index to a JSON value and returns
   it as a new cson_value instance.

   On success, non-NULL is returned and the ownership of the value is
   transfered to the caller.

   On error NULL is returned. Errors include:

   - stmt is NULL or ndx is out of bounds for the statement.

   - Allocation error when creating the new JSON value.

   The cpdo-type-to-JSON-type conversions are as follows:

   - CPDO_TYPE_NULL = JSON null (not a literal NULL).

   - CPDO_TYPE_INT8..64: JSON integer

   - CPDO_TYPE_DOUBLE: JSON double

   - CPDO_TYPE_STRING or CPDO_TYPE_BLOB: a JSON string. Results are undefined
   if the value is stored in any encoding other than ASCII or UTF8.

   - CPDO_TYPE_CUSTOM is ASSUMED to be convertible to a string
   value. This is primarily to accommodate MySQL
   TIME/DATE/DATETIME/TIMESTAMP fields.
   
   Any other result will cause NULL to be returned.
*/
cson_value * cson_cpdo_stmt_to_value( cpdo_stmt * st, uint16_t ndx );

/**
   If jv is NULL or is-not-a Array/Object then
   st->api->bind.null(st,ndx) is called (and its value returned),
   otherwise...

   jv is serialized to a buffer and bound as a blob (preferred) or string
   (fallback, if blob bind fails) in the 1-based ndx column.

   Returns 0 on success or a cpdo_rc value on error.

   FIXME: the return value can currently be either a cson_rc or
   cpdo_rc value, and they have some ambiguity/overlap.
   
   FIXME: add a variant of this function which takes a client-provided
   cson_buffer object to (re)use for serialization purposes. Why?
   Because i want this in the whiki code :).

*/
int cson_cpdo_bind_json( cpdo_stmt * st, uint16_t ndx, cson_value const * jv );

/**
   Binds a JSON value to a db column (1-based). v must be NULL or one
   of the types (integer, double, bool, string, null). To bind Objects
   and Arrays as JSON strings use cson_cpdo_bind_json().

   Integers are _naively assumed_ to be bindable as 64-bit parameters
   (via st->api->bind.i64()). Booleans are treated as
   integers. Strings are bound as STRING fields, but if that fails we
   attempt to bind them as BLOBs instead. NULL and the cson null value
   are bound as SQL NULL.

   This function _might_ be changed in the future to pass Objects and Arrays
   to cson_cpdo_bind_json().

   Returns 0 on success and a cpdo_rc code on error. Returns
   cpdo_rc.ArgError if !st or !ndx and cpdo_rc.TypeError if v is not
   one of the above-listed types/values.
*/       
int cson_cpdo_bind_value( cpdo_stmt * st, uint16_t ndx, cson_value const * v );
    
#if defined(__cplusplus)
} /*extern "C"*/
#endif
    
#endif /* CSON_ENABLE_CPDO */
#endif /* WANDERINGHORSE_NET_CSON_CPDO_H_INCLUDED */
/* end file include/wh/cson/cson_cpdo.h */
/* end file cson_amalgamation_session.h */
/* begin file cson_amalgamation_cgi.h */
/* auto-generated! Do not edit! */
/* begin file include/wh/cson/cson_cgi.h */
#if !defined(WANDERINGHORSE_NET_CSON_CGI_H_INCLUDED)
#define WANDERINGHORSE_NET_CSON_CGI_H_INCLUDED 1

/** @page page_cson_cgi cson CGI API

cson_cgi is a small framework encapsulating features usefor for
writing JSON-only applications (primarily CGI apps) in C. It is based
off of the cson JSON library:

    http://fossil.wanderinghorse.net/repos/cson/

In essence, it takes care of the basic CGI-related app setup, making
the data somewhat more accessible for client purposes.  Clients
create, as output, a single JSON object. The framework takes care of
outputing it, along with any necessary HTTP headers.


Primary features:

- Uses cson for its JSON handling, so it's pretty simple to use.

- Provides a simple-to-use mini-framework for writing CGI applications
which generate only JSON output.

- Various sources of system data are converted by the framework to
JSON for use by the client. This includes HTTP GET, OS environment,
command-line arguments, HTTP cookies, and (with some limitations) HTTP
POST.

- Can read unencoded JSON POST data (TODO: read in form-urlencoded as
a JSON object).

- Supports an optional JSON config file.

- Optional persistent sessions using files, sqlite3, or MySQL5
for the storage.


Primary misfeatures:

- Very young and not yet complete.

- Intended for writing apps which ONLY generate JSON data, not HTML.

- JSONP output support is currently incomplete.

- TODO: support reading of POSTed Array data (currently only Objects
work).

- We're missing a good number of convenience functions.

- Add client API for setting cookies. Currently they can be fetched
or removed but not explicitly set.


Other potential TODOs:

- Session support using session cookies for the IDs. For this to work
we also need to add a storage back-end or two (files, db (e.g. using
cpdo), etc.) and have a fast, good source of random numbers for
generating UUIDs. It _seems_ that using the address of (extern char **
environ) as a seed might be fairly random, but there are probably
environments where that won't suffice. i want to avoid
platform-specific bits, like /dev/urandom, if possible.

- Add client-definable i/o routines. We have this code in the whprintf
tree, but i was hoping to avoid having to import that here. This would
allow a client to add, e.g., gzip support.

*/

/** @page page_cson_cgi_session cson CGI Sessions

cson_cgi_init() will initialize a persistent session if it can figure
out everything it needs in order to do so. If it cannot it will simply
skip session initialization. A client can tell if a session was
established or not by calling cson_cgi_get_env_val(cx,'s',0).  If that
returns NULL then no session was created during initialization.
The API currently provides no way to initialize one after the fact.
That is, client code can use cson_cgi_get_env_val(cx,'s',1) to create a
session object, but it won't automatically be persistent across
application sessions.

Session management is really just the following:

- Load a JSON object from some persistent storage.
- Save that object at shutdown.

"Persistent storage" is the important phrase here. How the sessions
are saved is really unimportant. The library uses a generic interface
(cson_sessmgr) to handle the i/o, and can use any implementation we
care to provide. As of this writing (20110413) files, sqlite3, and
MySQL5 are supported.

The session object is a JSON object available via cson_cgi_getenv(),
using "s" as the environment name (the second parameter to
cson_cgi_getenv()).

At library shutdown (typically when main() exits, but also via
cson_cgi_cx_clean()), the session is saved using the current
session manager. If the session is not loaded during initialization,
but is created later, the library will assign a new session ID to it
before saving.

See cson_cgi_config_file() for examples of configuring the session
management.
*/


#if defined(__cplusplus)
extern "C" {
#endif

    /** @def CSON_CGI_ENABLE_POST_FORM_URLENCODED

    If CSON_CGI_ENABLE_POST_FORM_URLENCODED is set to a true value
    then the API will try to process form-urlencoded POST data,
    otherwise it will not.

    Reminder to self: this is basically a quick hack for fossil
    integration. We disable form encoding in that build because fossil
    handles that itself and we must not interfere with it.
    */
#if !defined(CSON_CGI_ENABLE_POST_FORM_URLENCODED)
#  define CSON_CGI_ENABLE_POST_FORM_URLENCODED 1
#endif    
    /** @def CSON_CGI_GETENV_DEFAULT

        The default environment name(s) to use for cson_cgi_getenv().
    */
#define CSON_CGI_GETENV_DEFAULT "gpce"
    /** @def CSON_CGI_KEY_JSONP

        TODO?: get rid of this and move cson_cgi_keys into the public
        API?

        The default environment key name to use for checking whether a
        response should used JSONP or not.
    */
#define CSON_CGI_KEY_JSONP "jspon"

#define CSON_CGI_KEY_SESSION "CSONSESSID"


    typedef struct cson_cgi_init_opt cson_cgi_init_opt;
    /** @struct cson_cgi_init_opt

        A type to hold core runtime configuration information for
        cson_cgi.
    */
    struct cson_cgi_init_opt
    {
        /**
           stdin stream. If NULL, stdin is used.
        */
        FILE * inStream;
        /**
           stdout stream. If NULL, stdout is used.
        */
        FILE * outStream;
        /**
           stderr stream. If NULL, stderr is used.
         */
        FILE * errStream;
        /**
           Path to a JSON config file.
        */
        char const * configFile;

        /**
           Alternately, configuration data can be supplied in the form
           of an Object (ownership is transfered to cson_cgi).
        */
        cson_object * configObj;

        /**
           If set then the session will be forced to use this
           ID, otherwise one will be generated.
        */
        char const * sessionID;

        /**
           Values are interpretted as:

           0 = do not output headers when cson_cgi_response_output_all()
           is called.

           (>0) = always output headers when cson_cgi_response_output_all()
           is called.

           (<0) = Try to determine, based on environment variables, if
           we are running in CGI mode. If so, output the headers when
           cson_cgi_response_output_all() is called, otherwise omit
           them.

           If the headers are omitted then so is the empty line
           which normally separates them from the response body!

           The intention of this flag is to allow non-CGI apps
           to disable output of the HTTP headers.
        */
        char httpHeadersMode;
        
        /**
           JSON output options.
        */
        cson_output_opt outOpt;

    };

    /** Empty-initialized cson_cgi_init_opt object. */
#define cson_cgi_init_opt_empty_m {                             \
        NULL/*inStream*/, NULL/*outStream*/, NULL/*errStream*/, \
            NULL /*configFile*/, NULL /*configObj*/, NULL /*sessionID*/, \
            -1/*httpHeadersMode*/,                              \
            cson_output_opt_empty_m /*outOpt*/                  \
    }

    /** Empty-initialized cson_cgi_init_opt object. */
    extern const cson_cgi_init_opt cson_cgi_init_opt_empty;


    /** @struct cson_cgi_env_map
       Holds a cson_object and its cson_value parent
       reference.
    */
    struct cson_cgi_env_map
    {
        /** Parent reference of jobj. */
        cson_value * jval;
        /** Object reference of jval. */
        cson_object * jobj;
    };
    typedef struct cson_cgi_env_map cson_cgi_env_map;

    /** Empty cson_cgi_env_map object. */
#define cson_cgi_env_map_empty_m { NULL, NULL }

    /** @struct cson_cgi_cx

       Internal state used by cson_cgi.

       Clients must not rely on its internal structure. It is in the
       public API so that it can be stack- or custom-allocated.  To
       properly initialize such an object, use cson_cgi_cx_empty_m
       cson_cgi_cx_empty, depending on the context.
    */
    struct cson_cgi_cx
    {
        /**
           Various key/value stores used by the framework. Each
           corresponds to some convention source of key/value
           pairs, e.g. QUERY_STRING or POST parameters.
        */
        struct {
            /**
               The system's environment variables.
            */
            cson_cgi_env_map env;
            /**
               Holds QUERY_STRING key/value pairs.
            */
            cson_cgi_env_map get;
            /**
               Holds POST form/JSON data key/value pairs.
            */
            cson_cgi_env_map post;
            /**
               Holds cookie key/value pairs.
            */
            cson_cgi_env_map cookie;
            /**
               Holds request headers.
            */
            cson_cgi_env_map headers;
        } request;
        /**
           Holds data related to the response JSON.
        */
        struct {
            /**
               HTTP error code to report.
            */
            int httpCode;

            /**
               Root JSON object. Must be an Object or Array (per the JSON
               spec).
            */
            cson_value * root;
            /**
               Holds HTTP response headers as an array of key/value
               pairs.
            */
            cson_cgi_env_map headers;
        } response;
        /**
           A place to store cson_value references
           for cleanup purposes.
        */
        cson_cgi_env_map gc;
        /**
           Holds client-defined key/value pairs.
        */
        cson_cgi_env_map clientEnv;
        cson_cgi_env_map config;
        struct {
            cson_cgi_env_map env;
            cson_sessmgr * mgr;
            char * id;
        } session;
        struct {
            cson_array * jarr;
            cson_value * jval;
        } argv;

        cson_cgi_init_opt opt;
        cson_buffer tmpBuf;
        struct {
            char isJSONP;
            void const * allocStamp;
        } misc;
    };
    typedef struct cson_cgi_cx cson_cgi_cx;
    /**
       Empty-initialized cson_cgi_cx object.
     */
    extern const cson_cgi_cx cson_cgi_cx_empty;
    /**
       Empty-initialized cson_cgi_cx object.
     */
#define cson_cgi_cx_empty_m \
    { \
    { /*maps*/ \
        cson_cgi_env_map_empty_m /*env*/, \
        cson_cgi_env_map_empty_m /*get*/, \
        cson_cgi_env_map_empty_m /*post*/, \
        cson_cgi_env_map_empty_m /*cookie*/, \
        cson_cgi_env_map_empty_m /*headers*/ \
    }, \
    {/*response*/ \
        0 /*httpCode*/, \
        NULL /*root*/, \
        cson_cgi_env_map_empty_m /*headers*/ \
    }, \
    cson_cgi_env_map_empty_m /*gc*/, \
    cson_cgi_env_map_empty_m /*clientEnv*/, \
    cson_cgi_env_map_empty_m /*config*/, \
    {/*session*/ \
        cson_cgi_env_map_empty_m /*env*/, \
        NULL /*mgr*/, \
        NULL /*id*/ \
    }, \
    {/*argv*/ \
        NULL /*jarr*/, \
        NULL /*jval*/ \
    }, \
    cson_cgi_init_opt_empty_m /*opt*/, \
    cson_buffer_empty_m /* tmpBuf */, \
    {/*misc*/ \
        -1 /*isJSONP*/,                         \
            NULL/*allocStamp*/ \
    } \
    }

    cson_cgi_cx * cson_cgi_cx_alloc();
    /**
       Cleans up all internal state of cx. IFF cx was allocated by
       cson_cgi_cx_alloc() then cx is also free()d, else it is assumed
       to have been allocated by the caller (possibly on the stack).

       Returns 1 if cx is not NULL and this function actually frees
       it.  If it returns 0 then either cx is NULL or this function
       cleaned up its internals but did not free(cx) (cx is assumed to
       have been allocated by the client).
    */
    char cson_cgi_cx_clean(cson_cgi_cx * cx);
    
    /**
       Initializes the internal cson_cgi environment and must be
       called one time, from main(), at application startup.

       cx must be either:

       - Created via cson_cgi_cx_alloc().

       - Alternately allocated (e.g. on the stack) and initialized
       by copying cson_cgi_cx_empty over it.

       Any other initialization leads to undefined behaviour.
       
       Returns 0 on success. If it returns non-0 then the rest of the
       API will "almost certainly" not work (and using the rest of the
       API invokes undefined behaviour unless documented otherwise for
       a specific function), so the application should exit
       immediately with an error code. The error codes returned by
       this function all come from the cson_rc object.

       The cx object must eventually be passed to cson_cgi_cx_clean(),
       regardless of success or failure, to clean up any resources
       allocated for the object.

       On success:

       - 0 is returned.

       - The cx object takes over ownership of any streams set in the
       opt object UNLESS they are the stdin/stdout/stderr streams (in
       which case ownership does not change).

       On error non-0 is returned and ownership of the opt.xxStream
       STILL transfers over to cx as described above (because this
       simpifies client-side error handling ).
       
       The 'opt' parameter can be used to tweak certain properties
       of the framework. It may be NULL, in which case defaults are
       used.
       
       This function currently performs the following initialization:

       - Parses QUERY_STRING environment variable into a JSON object.

       - Parses the HTTP_COOKIE environment variable into a JSON object.
       
       - Transforms the system environment to JSON.

       - Copies the list of arguments (argv, in the form conventional
       for main()) to JSON array form for downstream use by the client
       application. It does not interpret these arguments in any
       way. Clients may use cson_cgi_argv() and
       cson_cgi_argv_array() to fetch the list later on in the
       application's lifetime (presumably outside of main()). It is
       legal to pass (argv==NULL) only if argc is 0 or less.

       - If the CONTENT_TYPE env var is one of (application/json,
       application/javascript, or text/plain) and CONTENT_LENGTH is
       set then stdin is assumed to be JSON data coming in via POST.
       An error during that parsing is ignored for initialization purposes
       unless it is an allocation error, in which case it is propagated
       back to the caller of this function.

       - If session management is properly configured in the
       configuration file and if a variable named CSON_CGI_KEY_SESSION
       is found in the environment (cookies, GET, POST, or system env)
       then the previous session is loaded. If it cannot be loaded,
       the error is ignored. (Note that the cookie name can be
       changed via the configuration file.)

       Handling of configuration data:

       This routine tries to pull in configuration data from either a cson
       Object or a file...

       - If options.configObj is set then this function attempts to
       take over ownership of that object. If, upon returning,
       options.configObj is NULL then this function took over
       ownership, else ownership still lies with the caller.

       - If options.configObj is NULL then options.configFile is checked,
       and if it is not NULL then the given file is read in.
       
       - If still no configuration data has been set and the
       CSON_CGI_CONFIG env var is set then that file is read.

       Errors in loading the config from a file are silently ignored.

       TODOs:
       
       - Only read POST data when REQUEST_METHOD==POST?
       
       - Convert form-urlencoded POST data to a JSON object.

       - Potentially add an option to do automatic data type detection
       for numeric GET/POST/ENV/COOKIE data, such that fetching the
       cson_value for such a key would return a numeric value object
       as opposed to a string. Or we could add that option in a
       separate function which walks a JSON Object and performs that
       check/transformation on all of its entries. That currently
       can't be done properly with the cson_object_iterator API
       because changes to the object while looping invalidate the
       iterator. This option would also open up problems when clients
       pass huge strings which just happen to look like numbers.


       @see cson_cgi_config_file()
    */
    int cson_cgi_init( cson_cgi_cx * cx, int argc, char const * const * argv, cson_cgi_init_opt * options );

    /**
       Searches for a value from the CGI environment. The fromWhere
       parameter is a NUL-terminated string which specifies which
       environment(s) to check, and may be made up of any of the
       letters [gprecl], case-insensitive. If fromWhere is NULL or its
       first byte is NUL (i.e. it is empty) then the default value
       defined in CSON_CGI_GETENV_DEFAULT is used.

       The environments are searched in the order specified in
       fromWhere. The various letters mean:

       - g = GET: key/value pairs parsed from the QUERY_STRING
       environment variable.

       - p = POST: form-encoded key/value pairs parsed from stdin.

       - r = REQUEST, equivalent to "gpc", a superset of GET/POST/COOKIE.

       - e = ENV, e.g. via getenv(), but see cson_cgi_env_get_val()
       for more details.

       - c = COOKIE: request cookies (not response cookies) parsed
       from the HTTP_COOKIE environment variable.

       - a = APP: an environment namespace reserved for client app use.

       - f = CONFIG FILE.

       - Use key 's' for the SESSION.
       
       Invalid characters are ignored.

       The returned value is owned by the cson_cgi environment and
       must not be destroyed by the caller. NULL is returned if none
       of the requested environments contain the given key.

       Results are undefined if fromWhere is not NULL and is not
       NUL-terminated.

       TODOs:

       - Replace CSON_CGI_GETENV_DEFAULT with a runtime-configurable
       value (via a config file).

    */
    cson_value * cson_cgi_getenv( cson_cgi_cx * cx, char const * fromWhere, char const * key );

    /**
       A convenience form of cson_cgi_getenv() which returns the given
       key as a string. This will return NULL if the requested key
       is-not-a string value. It does not convert non-string values to
       strings.

       On success the string value is returned. Its bytes are owned by
       this API and are valid until the given key is removed/replaced
       from/in the environment object it was found in or that
       environment object is cleaned up.
    */
    char const * cson_cgi_getenv_cstr( cson_cgi_cx * cx, char const * where, char const * key );

    /**
       During initialization, if the PATH_INFO environment variable is set,
       it is split on '/' characters into array. That array is stored in the
       environment with the name PATH_INFO_SPLIT. This function returns the
       element of the PATH_INFO at the given index, or NULL if ndx is out
       of bounds or if no PATH_INFO is available.

       e.g. if PATH_INFO=/a/b/c, passing 0 to this function would return
       "a", passing 2 would return "c", and passing anything greater than 2
       would return NULL.
    */
    char const * cson_cgi_path_part_cstr( cson_cgi_cx * cx, unsigned short ndx );

    /**
       Functionally equivalent to cson_cgi_path_part_cstr(), but
       returns the underlying value as a cson value handle. That handle
       is owned by the underlying PATH_INFO_SPLIT array (which is
       owned by the "e" environment object).

       Unless the client has mucked with the PATH_INFO_SPLIT data, the
       returned value will (if it is not NULL) have a logical type of
       String.
    */
    cson_value * cson_cgi_path_part( cson_cgi_cx * cx, unsigned short ndx );
    
    /**
       Sets or unsets a key in the "user" environment/namespace. If v is NULL
       then the value is removed, otherwise it is set/replaced.

       Returns 0 on success. If key is NULL or has a length of 0 then
       cson_rc.ArgError is returned.

       The user namespace object can be fetched via
       cson_cgi_env_get_val('a',...).

       On success ownership of v is transfered to (or shared with) the
       cson_cgi API. On error ownership of v is not modified. Aside from
    */
    int cson_cgi_setenv( cson_cgi_cx * cx, char const * key, cson_value * v );

    /**
       This function is not implemented, but exists as a convenient
       place to document the cson_cgi config file format.
       
       cson_cgi_init() accepts the name of a configuration file
       (assumed to be in JSON format) to read during
       initialization. The library optionally uses the configuration
       to change certain aspects of its behaviour.

       The following commented JSON demonstrates the configuration
       file options:

       @code
       {
       "formatting": { // NOT YET HONORED. Will mimic cson_output_opt.
           "indentation": 1,
           "addNewline": true,
           "addSpaceAfterColon": true,
           "indentSingleMemberValues": true
       },
       "session": { // Options for session handling
           "manager": "file", // name of session manager impl. Should
                              // have a matching entry in "managers" (below)
           "cookieLifetimeMinutes": 10080, // cookie lifetime in minutes
           "cookieName": "cson_session_id", // cookie name for session ID
           "managers": {
               "file": {
                   "sessionDriver": "file", -- cson_cgi-internal session manager name
                   "dir": "./SESSIONS",
                   "prefix": "cson-session-",
                   "suffix": ".json"
               },
               "mysql5": {
                   "sessionDriver": "cpdo", -- cson_cgi-internal session manager name
                   "dsn": "mysql5:dbname=cpdo;host=localhost",
                   "user": "cpdo",
                   "password": "cpdo",
                   "table": "cson_session",
                   "fieldId": "id",
                   "fieldTimestamp": "last_saved",
                   "fieldSession": "json"
               },
               "sqlite3": {
                   "sessionDriver": "cpdo", -- cson_cgi-internal session manager name
                   "dsn": "sqlite3:sessions.sqlite3",
                   "user": null,
                   "password": null,
                   "table": "cson_session",
                   "fieldId": "id",
                   "fieldTimestamp": "last_saved",
                   "fieldSession": "json"
               }
           }
       }
       }
       @endcode

       TODO: allow initialization to take a JSON object, as opposed to
       a filename, so that we can embed the configuration inside client-side
       config data.
    */
    void cson_cgi_config_file();

    /**
       Sets or (if v is NULL) unsets a cookie value.

       v must either be of one of the types (string, integer, double,
       bool, null, NULL) or must be an object with the following
       structure:

       @code
       {
           value: (string, integer, double, bool, or null),
           OPTIONAL path: string,
           OPTIONAL domain: string,
           OPTIONAL expires: integer (Unix epoch timestamp),
           OPTIONAL secure: bool,
           OPTIONAL httponly: bool
       }
       @endcode

       For the object form, if the "value" property is missing or not of
       the correct type then the cookie will not be emitted in the
       HTTP response headers. The other properties are optional. A value
       of NULL or cson_value_null() will cause the expiry field (if set)
       to be ignored. Note, however, that removal will only work
       on the client side if all other cookie parameters match
       (e.g. domain and path).

       Returns 0 on success, non-0 on error.

       A duplicate cookie replaces any previous cookie with the same
       key.
       
       On success ownership of v is shared with the cson_cgi API (via
       reference counting). On error ownership of v is not modified.
    */
    int cson_cgi_cookie_set( cson_cgi_cx * cx, char const * key, cson_value * v );

    /**
       Sets or (if v is NULL) unsets an HTTP cookie value. key may not
       be NULL nor have a length of 0. v must be one of the types
       (string, integer, double, bool, null, NULL). Any other pointer
       arguments may be NULL, in which case they are not used.
       If v is NULL then the JSON null value is used as a placeholder
       value so that when the HTTP headers are generated, the cookie
       can be unset on the client side.

       This function creates an object with the structure documented
       in cson_cgi_cookie_set() and then passes that object to
       cson_cgi_cookie_set(). Any parameters which have NULL/0 values
       are not emitted in the generated object, with the exception of
       (v==NULL), which causes the expiry property to be ignored and a
       value from a time far in the past to be used (so that the
       client will expire it)..

       Returns 0 on success, non-0 on error.

       On success ownership of v is shared with the cson_cgi API (via
       reference counting). On error ownership of v is not modified.
    */
    int cson_cgi_cookie_set2( cson_cgi_cx * cx, char const * key, cson_value * v,
                              char const * domain, char const * path,
                              unsigned int expires, char secure, char httponly );
    
    /**
        Returns the internal "environment" JSON object corresponding
        to the given 'which' letter, which must be one of
        (case-insensitive):

        - g = GET
        - p = POST
        - c = COOKIE
        - e = ENV (i.e. system environment)
        - s = SESSION
        - a = APP (application-specific)

        TODO: s = SESSION

        See cson_cgi_getenv() for more details about each of those.

        Returns NULL if 'which' is not one of the above.

        Note that in the 'e' (system environment) case, making
        modifications to the returned object will NOT also modify the
        system environment.  Likewise, future updates to the system
        environment will not be automatically reflected in the
        returned object.

        The returned object is owned by the cson_cgi environment and
        must not be destroyed by the caller.

        If createIfNeeded is non-0 (true) then the requested
        environment object is created if it was formerly empty. In that
        case, a return value of NULL can indicate an invalid 'which'
        parameter or an allocation error.       

        To get the Object reference to this environment use
        cson_cgi_env_get_obj() or pass the result of this function
        to cson_value_get_object().
 
        The returned value is owned by the cson_cgi API.

        The public API does not provide a way for clients to modify
        several of the internal environment stores, e.g. HTTP GET
        parameters are set only by this framework. However, clients
        can (if needed) get around this by fetching the associated
        "environment object" via this function or
        cson_cgi_env_get_obj(), and modifying it directly. Clients are
        encouraged to use the other public APIs for dealing with the
        environment, however, and are encouraged to not directly modify
        "special" namespaces like the cookie/GET/POST data.        
    */
    cson_value * cson_cgi_env_get_val( cson_cgi_cx * cx, char which, char createIfNeeded );

    /**
       Equivalent to:

       @code
       cson_value_get_object( cson_cgi_env_get_val( which, createIfNeeded ) );
       @endcode

       Note, however, that it is at least theoretically possible that
       cson_cgi_env_get_val() return non-NULL but this function
       returns NULL. If that happens it means that the value returned
       by cson_cgi_env_get_val() is-not-a Object instance, but is
       something else (maybe an array?).
    */
    cson_object * cson_cgi_env_get_obj( cson_cgi_cx * cx, char which, char createIfNeeded );

    /**
       Adds the given key/value to the list of HTTP headers (replacing
       any existing entry with the same name).  If v is NULL then any
       header with the given key is removed from the pending response.

       Returns 0 on success. On success ownership of v is transfered
       to (or shared with) the internal header list. On error,
       ownership of v is not modified.

       If v is not of one of the types (string, integer, double, bool,
       undef, null) then the header will not be output when when
       cson_cgi_response_output_headers() is called. If it is one of
       those types then its stringified value will be its "natural"
       form (for strings and numbers), the integer 0 or 1 for
       booleans, and the number 0 for null.  Note that a literal
       (v==NULL) is treated differently from a JSON null - it UNSETS
       the given header.

       This function should not be used for setting cookies, as they
       require extra url-encoding and possibly additional
       parameters. Use cson_cgi_cookie_set() and
       cson_cgi_cookie_set2() to set cookie headers.
    */
    int cson_cgi_response_header_add( cson_cgi_cx * cx, char const * key, cson_value * v );

    /**
       Returns a cson array value containing the arguments passed
       to cson_cgi_init(). The returned value is owned by the cson_cgi
       API and must not be destroyed by the caller.

       Only returns NULL if initialization of cson_cgi_init() fails
       early on, and is almost certainly indicative of an allocation
       error. If cson_cgi_init() is given a 0-length argument list
       then this function will return an empty array (except in the
       NULL case mentioned above).
    */
    cson_value * cson_cgi_argv(cson_cgi_cx * cx);
    
    /**
       Equivalent to:

       @code
       cson_value_get_array( cson_cgi_argv() );
       @endcode
    */
    cson_array * cson_cgi_argv_array(cson_cgi_cx * cx);

    /**
       Flushes all response headers set via cson_cgi_response_header_add()
       to stdout. The client must output an empty line before the body
       part (if any), and may output custom headers before doing so.

       Do not call this more than once.
    */
    int cson_cgi_response_output_headers(cson_cgi_cx * cx);

    /**
       Outputs the response root object to stdout. If none has been
       set, non-0 is returned.

       Returns 0 on success. On error, partial output might be
       generated.

       Do not call this more than once.
    */
    int cson_cgi_response_output_root(cson_cgi_cx * cx);

    /**
       Outputs the whole response, including headers and the root JSON
       value.

       Returns 0 on success. Fails without side effects if
       no root is set.

       Do not call this more than once.
    */
    int cson_cgi_response_output_all(cson_cgi_cx * cx);
    
    /**
       Don't use this - i need to re-think the JSONP bits.
    
       Returns non-0 (true) if the GET/POST environment contains a key
       named CSON_CGI_KEY_JSONP. If this is the case, then when
       cson_cgi_response_output_headers() is called the Content-type
       is set to "application/javascript".

       If cson_cgi_enable_jsonp() is ever called to set this option
       explicitly, this function does not guess, but uses that value
       instead.

       When JSONP is desired, the generated page output must be
       wrapped in the appropriate JS code.
    */
    char cson_cgi_is_jsonp(cson_cgi_cx * cx);

    /**
       Don't use this - i need to re-think the JSONP bits.

       Sets or unsets JSONP mode. If b is 0 then JSONP guessing is
       explicitly disabled and output is assumed to be JSON. If it is
       non-0 then cson_cgi_guess_content_type() will never return
       "application/javascript".

       When JSONP is desired, the generated page output must be
       wrapped in the appropriate JS code.
    */
    void cson_cgi_enable_jsonp( cson_cgi_cx * cx, char b );

    /**
       Tries to guess the "best" Content-type header value for
       the current session, based on several factors:

       - If the GET/POST data contains a variable named
       CSON_CGI_KEY_JSONP then "application/javascript" is returned.

       - If the HTTP_ACCEPT environment variable is NOT set or
       contains "application/json" then "application/json [possibly
       charset info]" is returned.

       - If the HTTP_ACCEPT environment variable is set but does not
       contain "application/json" then "text/javascript" is returned.

       - If cson_cgi_enable_jsonp() is called and passed a true value,
       "application/javascript" is returned.

       
       If HTTP_ACCEPT_CHARSET is NOT set or contains "utf-8" then
       ";charset=utf-8" is included in the the returned string.
       
       The returned string is static and immutable and suitable for
       use as a Content-type header value. The string is guaranteed to
       be valid until the application exits. Multiple calls to this
       function, with different underlying environment data, can cause
       different results to be returned.

       Returns NULL if it absolutely cannot figure out what to do, but
       currently it has no such logic paths.
    */
    char const * cson_cgi_guess_content_type(cson_cgi_cx * cx);

    /**
       Sets the response content root, replacing any
       existing one (and possibly cleaning it up).

       Returns 0 on success. On success, ownership of v
       is transfered to (or shared with) the cson_cgi
       API. It will be cleaned up at app shutdown time
       or if it is subsequently replaced and has no
       other open references to it.

       On error ownership of v is not modified and any previous
       root is not removed.

       If v is-not-a Object or Array, nor NULL, then cson_rc.TypeError
       is returned. JSON requires either an object or array for the
       root node. Passing NULL will possibly free up any current root
       (depending on its reference count).
    */
    int cson_cgi_response_root_set( cson_cgi_cx * cx, cson_value * v );

    /**
       Fetches the current content root JSON value, as set via
       cson_cgi_response_root_set() or (if no root has been set), as
       defined by createMode, as described below.

       If a content root has been set (or previously initialized)
       then the value of createMode is ignored. If no root has been
       set then this function might try to create one, as described
       here:

       (createMode==0) means not to create a root element if none
       exists already.

       (createMode<0) means to create the root as an Array value.

       (createMode>0) means to create the root as an Object value.

       Returns NULL on allocation error or if no root has been set and
       (createMode==0). On success the returned value is guaranteed to
       be either an Array or Object (see cson_value_get_object() and
       cson_value_get_array()) and it is owned by the cson_cgi API (so
       it must not be destroyed by the caller). If the client needs to
       destroy it, pass NULL to cson_cgi_response_root_set().
    */
    cson_value * cson_cgi_response_root_get( cson_cgi_cx * cx, char createMode );
    
    /**
       Returns the current session ID. If session management is not
       enabled then NULL is returned.

       The returned bytes are owned by the cson_cgi API and are valid
       until the library is cleaned up (via cson_cgi_cleanup_lib() or
       via the normal shutdown process) or the session ID is
       re-generated for some reason. It is best not to hold a
       reference to this, but to copy it if it will be needed later.

       If the return value is not NULL, it is guaranteed to be
       NUL-terminated.
    */
    char const * cson_cgi_session_id(cson_cgi_cx *);

    /**
       Writes a 36-byte (plus one NUL byte) random UUID value to
       dest. dest must be at least 37 bytes long. If dest is NULL this
       function has no side effects.

       This function uses internal RNG state and is not thread-safe.
    */
    void cson_cgi_generate_uuid( cson_cgi_cx * cx, char * dest );

    /**
       Adds v to the API-internal cleanup mechanism. key must be a
       unique key for the given element. Adding another item with that
       key may free the previous one. If freeOnError is true then v is
       passed to cson_value_free() if the key cannot be inserted,
       otherweise ownership of v is not changed on error.

       Returns 0 on success.

       On success, ownership of v is transfered to (or shared with)
       cx, and v will be valid until cx is cleaned up or its key is
       replaced via another call to this function.
    */
    int cson_cgi_gc_add( cson_cgi_cx * cx, char const * key, cson_value * v, char freeOnError );

    /**

    Replaces %XX patterns in str with their equivalent character and
    '+' characters with a single whitespace. %XX patterns which are
    not hexidecimal values are not translated.

    str must be NULL or a NUL-terminated string. If it is NULL or the
    first byte is NUL then 0 is returned and this function has no
    side-effects.
   
    BUGS(?): URL-decoding might have a few bugs/corner cases.

    Returns 0 on success, non-0 on error.
    */
    int cson_cgi_urldecode_inline( char * str );

    /**
       Replaces the current POST data (if any) with the given value.
       Any previous POST data is cleaned up, possibly invalidating
       handles referencing data it holds.

       newPost must be-a Object or Array, or cson_rc.TypeError will be
       returned.
       
       On success 0 is returned and ownership of newPost is transfered
       to cx. On error ownership is not modified.
    */
    int cson_cgi_replace_POST( cson_cgi_cx * cx, cson_value * newPost );

    /**
       Replaces the current GET request data (if any) with the given
       value.  Any previous GET data is cleaned up, possibly
       invalidating handles referencing data it holds.

       On success 0 is returned and ownership of newPost is transfered
       to cx. On error ownership is not modified.
    */
    int cson_cgi_replace_GET( cson_cgi_cx * cx, cson_object * newGet );
    
    /* LICENSE

This software's source code, including accompanying documentation and
demonstration applications, are licensed under the following
conditions...

Certain files are imported from external projects and have their own
licensing terms. Namely, the JSON_parser.* files. See their files for
their official licenses, but the summary is "do what you want [with
them] but leave the license text and copyright in place."

The author (Stephan G. Beal [http://wanderinghorse.net/home/stephan/])
explicitly disclaims copyright in all jurisdictions which recognize
such a disclaimer. In such jurisdictions, this software is released
into the Public Domain.

In jurisdictions which do not recognize Public Domain property
(e.g. Germany as of 2011), this software is Copyright (c) 2011 by
Stephan G. Beal, and is released under the terms of the MIT License
(see below).

In jurisdictions which recognize Public Domain property, the user of
this software may choose to accept it either as 1) Public Domain, 2)
under the conditions of the MIT License (see below), or 3) under the
terms of dual Public Domain/MIT License conditions described here, as
they choose.

The MIT License is about as close to Public Domain as a license can
get, and is described in clear, concise terms at:

    http://en.wikipedia.org/wiki/MIT_License

The full text of the MIT License follows:

--
Copyright (c) 2011 Stephan G. Beal (http://wanderinghorse.net/home/stephan/)

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

--END OF MIT LICENSE--

For purposes of the above license, the term "Software" includes
documentation and demonstration source code which accompanies
this software. ("Accompanies" = is contained in the Software's
primary public source code repository.)

*/

#if defined(__cplusplus)
} /*extern "C"*/
#endif

#endif /* WANDERINGHORSE_NET_CSON_CGI_H_INCLUDED */
/* end file include/wh/cson/cson_cgi.h */
/* end file cson_amalgamation_cgi.h */