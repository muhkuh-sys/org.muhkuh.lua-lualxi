%module lualxi

%include "stdint.i"
%include "typemaps.i"
#ifdef SWIGLUA
%include "lua_fnptr.i"
#endif

%{
	#include "wrapper.h"
%}


/* This typemap adds "SWIGTYPE_" to the name of the input parameter to
 * construct the swig typename. The parameter name must match the definition
 * in the wrapper.
 */
%typemap(in, numinputs=0) swig_type_info *
%{
	$1 = SWIGTYPE_$1_name;
%}


/* This typemap passes Lua state to the function. The function must create one
 * lua object on the stack. This is passed as the return value to lua.
 * No further checks are done!
 */
%typemap(in, numinputs=0) lua_State *MUHKUH_SWIG_OUTPUT_CUSTOM_OBJECT
%{
	$1 = L;
%}
%typemap(argout) (lua_State *MUHKUH_SWIG_OUTPUT_CUSTOM_OBJECT)
%{
	++SWIG_arg;
%}


/* This typemap passes Lua state to the function. The function must create 0
 * or more lua object on the stack. The function returns the number of objects
 * created. They are passed as the return value to lua.
 * No further checks are done!
 */
%typemap(in, numinputs=0) (lua_State *MUHKUH_SWIG_OUTPUT_CUSTOM_OBJECT_LIST, unsigned int *uiNUMBER_OF_CREATED_OBJECTS)
%{
	unsigned int uiObjects;
	$1 = L;
	$2 = &uiObjects;
%}
%typemap(argout) (lua_State *MUHKUH_SWIG_OUTPUT_CUSTOM_OBJECT_LIST, unsigned int *uiNUMBER_OF_CREATED_OBJECTS)
%{
	SWIG_arg += uiObjects;
%}


%typemap(in) (const char *pcBUFFER_IN, size_t sizBUFFER_IN)
{
        size_t sizBuffer;
        $1 = (char*)lua_tolstring(L, $argnum, &sizBuffer);
        $2 = sizBuffer;
}


%typemap(in, numinputs=1) (char *pcBUFFER_OUT, size_t sizBUFFER_MAX)
%{
	if(!lua_isnumber(L,$argnum)) SWIG_fail_arg("LxiContext::receive",$argnum,"size_t");
	$2 = (size_t)lua_tonumber(L, $argnum);
	if( $2!=0 )
	{
		$1 = (char*)malloc($2);
	}
%}
%typemap(argout) (char *pcBUFFER_OUT, size_t sizBUFFER_MAX)
%{
	if( $1!=NULL )
	{
		lua_pushlstring(L, $1, result);
	}
	else
	{
		lua_pushnil(L);
	}
	++SWIG_arg;
%}
%typemap(freearg) (char *pcBUFFER_OUT, size_t sizBUFFER_MAX)
%{
	if( $1!=NULL )
	{
		free($1);
	}
%}



%typemap(in, numinputs=0) (unsigned char *pucARGUMENT_OUT)
%{
	unsigned char ucArgument;
	$1 = &ucArgument;
%}
%typemap(argout) (unsigned char *pucARGUMENT_OUT)
%{
	lua_pushnumber(L, ucArgument);
	++SWIG_arg;
%}



%typemap(in, numinputs=0) (unsigned short *pusARGUMENT_OUT)
%{
	unsigned short usArgument;
	$1 = &usArgument;
%}
%typemap(argout) (unsigned short *pusARGUMENT_OUT)
%{
	lua_pushnumber(L, usArgument);
	++SWIG_arg;
%}



%typemap(in, numinputs=0) (int *piARGUMENT_OUT)
%{
	int iArgument;
	$1 = &iArgument;
%}
%typemap(argout) (int *piARGUMENT_OUT)
%{
	lua_pushnumber(L, iArgument);
	++SWIG_arg;
%}



%typemap(in, numinputs=0) (unsigned int *puiARGUMENT_OUT)
%{
	unsigned int uiArgument;
	$1 = &uiArgument;
%}
%typemap(argout) (unsigned int *puiARGUMENT_OUT)
%{
	lua_pushnumber(L, uiArgument);
	++SWIG_arg;
%}



%typemap(out) RESULT_INT_TRUE_OR_NIL_WITH_ERR
%{
	if( $1>=0 )
	{
		lua_pushboolean(L, 1);
		SWIG_arg = 1;
	}
	else
	{
		lua_pushnil(L);
		lua_pushstring(L, arg1->get_error_string());
		SWIG_arg = 2;
	}
%}


%typemap(out) RESULT_INT_NOTHING_OR_NIL_WITH_ERR
%{
	if( $1<0 )
	{
		lua_pushnil(L);
		lua_pushstring(L, arg1->get_error_string());
		SWIG_arg = 2;
		return SWIG_arg;
	}
%}


%typemap(out) RESULT_INT_NOTHING_OR_NIL_WITH_ERR_FREE_ARG2
%{
	if( $1<0 )
	{
		lua_pushnil(L);
		lua_pushstring(L, arg1->get_error_string());
		SWIG_arg = 2;

		if( arg2!=NULL )
		{
			free(arg2);
		}

		return SWIG_arg;
	}
%}


%typemap(out) RESULT_INT_INT_OR_NIL_WITH_ERR
%{
	if( $1>=0 )
	{
		lua_pushnumber(L, $1);
		SWIG_arg = 1;
	}
	else
	{
		lua_pushnil(L);
		lua_pushstring(L, arg1->get_error_string());
		SWIG_arg = 2;
	}
%}



/* The "usb_find_all" method of the "Context" object returns a new "List" object. It must be freed by the LUA interpreter. */
%newobject Context::usb_find_all;

/* The "eeprom" method of the "Context" object returns a new "Eeprom" object. It must be freed by the LUA interpreter. */
%newobject Context::eeprom;

/* Do not wrap the "next" method of the "List" object. */
%ignore List::next;

/* Do not wrap the "iterator_next" method. It makes only sense in the closure function and shoud not be called directly. */
%ignore List::iterator_next;

/* Do not wrap the "get_usb_device" method. It is used only internally to communicate between the ListEntry and the Context class. */
%ignore ListEntry::get_usb_device;


%rename("%(regex:/__ENUM_(.*)/\\1/)s", %$isenumitem) "";


%include "wrapper.h"
