/* auto-generated! Do not edit! */
#include "cson_amalgamation_core.h"
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
