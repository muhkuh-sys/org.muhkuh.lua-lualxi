#include "wrapper.h"

#include "lxi.h"


#define MUHKUH_PLUGIN_ERROR(L,...) { lua_pushfstring(L,__VA_ARGS__); lua_error(L); }

#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM < 501
#       define WRAPPER_LUA_RAWLEN lua_strlen
#elif LUA_VERSION_NUM == 501
#       define WRAPPER_LUA_RAWLEN lua_objlen
#else
#       define WRAPPER_LUA_RAWLEN lua_rawlen
#endif



Lxi::Lxi(void)
 : m_iDevice(LXI_ERROR)
 , m_pcAddress(NULL)
 , m_iPort(0)
 , m_pcName(NULL)
 , m_tProtocol(__ENUM_VXI11)
 , m_tCallbackLuaFn({0, 0})
 , m_lCallbackUserData(0)
 , m_ptDiscoveredDevices(NULL)
 , m_sizDiscoveredDevicesCnt(0)
 , m_sizDiscoveredDevicesMax(0)
{
	/* Initialize the LXI stuff.
	 * NOTE: this function always returns LXI_OK.
	 */
	lxi_init();
}


Lxi::~Lxi(void)
{
	/* Close any running connection. */
	disconnect();

	free_discovered_devices();

	if( m_pcAddress!=NULL )
	{
		free(m_pcAddress);
	}
	if( m_pcName!=NULL )
	{
		free(m_pcName);
	}
}



void Lxi::free_discovered_devices(void)
{
	DISCOVER_RECORD_T *ptDevices;
	DISCOVER_RECORD_T *ptCnt;
	DISCOVER_RECORD_T *ptEnd;
	char *pcData;


	if( m_ptDiscoveredDevices!=NULL )
	{
		ptDevices = m_ptDiscoveredDevices;

		ptCnt = ptDevices;
		ptEnd = ptDevices + m_sizDiscoveredDevicesCnt;
		while( ptCnt<ptEnd )
		{
			/* Free the address. */
			pcData = ptCnt->pcAddress;
			if( pcData!=NULL )
			{
				free(pcData);
			}

			/* Free the ID. */
			pcData = ptCnt->pcId;
			if( pcData!=NULL )
			{
				free(pcData);
			}

			++ptCnt;
		}
		free(ptDevices);

		m_ptDiscoveredDevices = NULL;
	}
}



void Lxi::set_callback(SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	m_tCallbackLuaFn.L = tLuaFn.L;
	m_tCallbackLuaFn.ref = tLuaFn.ref;
	m_lCallbackUserData = lCallbackUserData;
}



RESULT_INT_NOTHING_OR_NIL_WITH_ERR Lxi::discover(lua_State *MUHKUH_SWIG_OUTPUT_CUSTOM_OBJECT, int timeout, enum LXI_DISCOVER_ENUM tDiscover)
{
	int iResult;
	lxi_discover_t tLxiDiscover;
	lxi_info_t tLxiInfo;
	void *pvUser;
	DISCOVER_RECORD_T *ptCnt;
	DISCOVER_RECORD_T *ptEnd;
	size_t sizCnt;
	int iCnt;


	iResult = CUSTOM_ERROR_INVALID_DISCOVER;
	switch(tDiscover)
	{
	case __ENUM_DISCOVER_VXI11:
		tLxiDiscover = DISCOVER_VXI11;
		iResult = LXI_OK;
		break;

	case __ENUM_DISCOVER_MDNS:
		tLxiDiscover = DISCOVER_MDNS;
		iResult = LXI_OK;
		break;
	}
	if( iResult==LXI_OK )
	{
		tLxiInfo.broadcast = s_discover_broadcast;
		tLxiInfo.device = s_discover_device;
		tLxiInfo.service = s_discover_service;
		pvUser = (void*)this;

		free_discovered_devices();

		m_sizDiscoveredDevicesCnt = 0;
		m_sizDiscoveredDevicesMax = 16;
		m_ptDiscoveredDevices = (DISCOVER_RECORD_T*)malloc(sizeof(DISCOVER_RECORD_T)*m_sizDiscoveredDevicesMax);
		if( m_ptDiscoveredDevices==NULL )
		{
			iResult = CUSTOM_ERROR_OUT_OF_MEMORY;
		}
		else
		{
			/* Start a new table. */
			iResult = lxi_discover(&tLxiInfo, timeout, tLxiDiscover, pvUser);
			if( iResult==LXI_OK )
			{
				if( m_ptDiscoveredDevices==NULL )
				{
					iResult = CUSTOM_ERROR_OUT_OF_MEMORY;
				}
				else
				{
					sizCnt = m_sizDiscoveredDevicesCnt;

					/* Create a new table for all results. */
					lua_createtable(MUHKUH_SWIG_OUTPUT_CUSTOM_OBJECT, 0, 7);

					/* Loop over all devices. */
					ptCnt = m_ptDiscoveredDevices;
					ptEnd = m_ptDiscoveredDevices + sizCnt;
					while( ptCnt<ptEnd )
					{
						lua_pushstring(MUHKUH_SWIG_OUTPUT_CUSTOM_OBJECT, ptCnt->pcAddress);
						lua_pushstring(MUHKUH_SWIG_OUTPUT_CUSTOM_OBJECT, ptCnt->pcId);
						lua_rawset(MUHKUH_SWIG_OUTPUT_CUSTOM_OBJECT, 4);

						++ptCnt;
					}
				}
			}

			free_discovered_devices();
		}
	}

	return iResult;
}



RESULT_INT_TRUE_OR_NIL_WITH_ERR Lxi::connect(const char *address, int port, const char *name, int timeout, enum LXI_PROTOCOL_ENUM tProtocol)
{
	int iResult;
	lxi_protocol_t tLxiProtocol;


	/* Be optimistic. */
	iResult = LXI_OK;

	/* Close an existing connection. */
	if( m_iDevice!=LXI_ERROR )
	{
		iResult = disconnect();
	}
	if( iResult==LXI_OK )
	{
		iResult = CUSTOM_ERROR_INVALID_PROTOCOL;
		switch(tProtocol)
		{
		case __ENUM_VXI11:
			tLxiProtocol = VXI11;
			iResult = LXI_OK;
			break;

		case __ENUM_RAW:
			tLxiProtocol = RAW;
			iResult = LXI_OK;
			break;

		case __ENUM_HISLIP:
			tLxiProtocol = HISLIP;
			iResult = LXI_OK;
			break;
		}
		if( iResult==LXI_OK )
		{
			iResult = lxi_connect(address, port, name, timeout, tLxiProtocol);
			if( iResult!=LXI_ERROR )
			{
				/* The result is the new handle. */
				m_iDevice = iResult;

				if( m_pcAddress!=NULL )
				{
					free(m_pcAddress);
				}
				if( m_pcName!=NULL )
				{
					free(m_pcName);
				}

				m_pcAddress = strdup(address);
				m_iPort = port;
				m_pcName = strdup(name);
				m_tProtocol = tProtocol;

				iResult = LXI_OK;
			}
		}
	}

	return iResult;
}



RESULT_INT_TRUE_OR_NIL_WITH_ERR Lxi::disconnect(void)
{
	int iResult;


	/* Be optimistic. */
	iResult = LXI_OK;

	/* Is an LXI connection open? */
	if( m_iDevice!=LXI_ERROR )
	{
		/* Close the connection. */
		iResult = lxi_disconnect(m_iDevice);
		m_iDevice = LXI_ERROR;
	}

	return iResult;
}



RESULT_INT_TRUE_OR_NIL_WITH_ERR Lxi::send(const char *pcBUFFER_IN, size_t sizBUFFER_IN, int timeout)
{
	int iResult;


	/* Already connected? */
	if( m_iDevice==LXI_ERROR )
	{
		/* Error: not connected. */
		iResult = CUSTOM_ERROR_NOT_CONNECTED;
	}
	else
	{
		iResult = lxi_send(m_iDevice, pcBUFFER_IN, sizBUFFER_IN, timeout);
	}

	return iResult;
}



RESULT_INT_NOTHING_OR_NIL_WITH_ERR_FREE_ARG2 Lxi::receive(char *pcBUFFER_OUT, size_t sizBUFFER_MAX, int timeout)
{
	int iResult;


	/* Already connected? */
	if( m_iDevice==LXI_ERROR )
	{
		/* Error: not connected. */
		iResult = CUSTOM_ERROR_NOT_CONNECTED;
	}
	else
	{
		iResult = lxi_receive(m_iDevice, pcBUFFER_OUT, sizBUFFER_MAX, timeout);
	}

	return iResult;
}



void Lxi::s_discover_broadcast(const char *pcAddress, const char *pcInterface, void *pvUser)
{
	Lxi *ptSelf;


	/* Get the pointer to the context. */
	ptSelf = (Lxi*)pvUser;
	ptSelf->discover_broadcast(pcAddress, pcInterface);
}



void Lxi::discover_broadcast(const char *pcAddress, const char *pcInterface)
{
	log("discover: scanning interface %s (%s)", pcInterface, pcAddress);
}



void Lxi::s_discover_device(const char *pcAddress, const char *pcId, void *pvUser)
{
	Lxi *ptSelf;


	/* Get the pointer to the context. */
	ptSelf = (Lxi*)pvUser;
	ptSelf->discover_device(pcAddress, pcId);
}



void Lxi::discover_device(const char *pcAddress, const char *pcId)
{
	size_t sizNew;
	DISCOVER_RECORD_T *ptNew;


	log("discover: found device %s at %s", pcId, pcAddress);

	if( m_ptDiscoveredDevices!=NULL )
	{
		/* Does the entry fit into the list? */
		if( m_sizDiscoveredDevicesCnt>=m_sizDiscoveredDevicesMax )
		{
			/* No -> allocate more space. */
			sizNew = m_sizDiscoveredDevicesMax * 2U;
			/* Detect overflow. */
			if( sizNew<m_sizDiscoveredDevicesMax )
			{
				free_discovered_devices();
			}
			else
			{
				ptNew = (DISCOVER_RECORD_T*)realloc(m_ptDiscoveredDevices, sizeof(DISCOVER_RECORD_T)*sizNew);
				if( ptNew==NULL )
				{
					free_discovered_devices();
				}
				else
				{
					m_ptDiscoveredDevices = ptNew;
					m_sizDiscoveredDevicesMax = sizNew;
				}
			}
		}

		if( m_ptDiscoveredDevices!=NULL )
		{
			/* Get a pointer to the next free entry. */
			ptNew = m_ptDiscoveredDevices + m_sizDiscoveredDevicesCnt;
			ptNew->pcAddress = strdup(pcAddress);
			ptNew->pcId = strdup(pcId);

			++m_sizDiscoveredDevicesCnt;
		}
	}
}



void Lxi::s_discover_service(const char *pcAddress, const char *pcId, const char *pcService, int iPort, void *pvUser)
{
	Lxi *ptSelf;


	/* Get the pointer to the context. */
	ptSelf = (Lxi*)pvUser;
	ptSelf->discover_service(pcAddress, pcId, pcService, iPort);
}



void Lxi::discover_service(const char *pcAddress, const char *pcId, const char *pcService, int iPort)
{
	log("discover: service %s / %s / %s / %d", pcAddress, pcId, pcService, iPort);
}



const char *Lxi::get_error_string(void)
{
	const char *pcErrorString;


	pcErrorString = "Error";

	return pcErrorString;
}



void Lxi::log(const char *pcFmt, ...)
{
	va_list ptArguments;
	int iMessageLength;
	char acBuffer[1024];


	/* Print the message to the buffer. */
	va_start(ptArguments, pcFmt);
	iMessageLength = vsnprintf(acBuffer, sizeof(acBuffer), pcFmt, ptArguments);
	va_end(ptArguments);

	callback_string(&m_tCallbackLuaFn, acBuffer, iMessageLength, m_lCallbackUserData);
}



void Lxi::callback_long(SWIGLUA_REF *ptLuaFn, long lData, long lCallbackUserData)
{
	int iOldTopOfStack;


	/* Check the LUA state and callback tag. */
	if( ptLuaFn->L!=NULL && ptLuaFn->ref!=LUA_NOREF && ptLuaFn->ref!=LUA_REFNIL )
	{
		/* Get the current stack position. */
		iOldTopOfStack = lua_gettop(ptLuaFn->L);
		lua_rawgeti(ptLuaFn->L, LUA_REGISTRYINDEX, ptLuaFn->ref);
		/* Push the arguments on the stack. */
		lua_pushnumber(ptLuaFn->L, lData);
		callback_common(ptLuaFn, lCallbackUserData, iOldTopOfStack);
	}
}



void Lxi::callback_string(SWIGLUA_REF *ptLuaFn, const char *pcData, size_t sizData, long lCallbackUserData)
{
	int iOldTopOfStack;


	/* Check the LUA state and callback tag. */
	if( ptLuaFn->L!=NULL && ptLuaFn->ref!=LUA_NOREF && ptLuaFn->ref!=LUA_REFNIL )
	{
		/* Get the current stack position. */
		iOldTopOfStack = lua_gettop(ptLuaFn->L);
		lua_rawgeti(ptLuaFn->L, LUA_REGISTRYINDEX, ptLuaFn->ref);
		/* Push the arguments on the stack. */
		lua_pushlstring(ptLuaFn->L, pcData, sizData);
		callback_common(ptLuaFn, lCallbackUserData, iOldTopOfStack);
	}
}



void Lxi::callback_common(SWIGLUA_REF *ptLuaFn, long lCallbackUserData, int iOldTopOfStack)
{
	int iResult;
	const char *pcErrMsg;
	const char *pcErrDetails;


	/* Check the LUA state and callback tag. */
	if( ptLuaFn->L!=NULL && ptLuaFn->ref!=LUA_NOREF && ptLuaFn->ref!=LUA_REFNIL )
	{
		lua_pushnumber(ptLuaFn->L, lCallbackUserData);
		/* Call the function. */
		iResult = lua_pcall(ptLuaFn->L, 2, 0, 0);
		if( iResult!=0 )
		{
			switch( iResult )
			{
			case LUA_ERRRUN:
				pcErrMsg = "runtime error";
				break;
			case LUA_ERRMEM:
				pcErrMsg = "memory allocation error";
				break;
			default:
				pcErrMsg = "unknown errorcode";
				break;
			}
			pcErrDetails = lua_tostring(ptLuaFn->L, -1);
			MUHKUH_PLUGIN_ERROR(ptLuaFn->L, "callback function failed: %s (%d): %s", pcErrMsg, iResult, pcErrDetails);
		}
		/* Restore the old stack top. */
		lua_settop(ptLuaFn->L, iOldTopOfStack);
	}
}
