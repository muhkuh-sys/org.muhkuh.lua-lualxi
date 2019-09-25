#ifdef __cplusplus
extern "C" {
#endif
#include "lxi.h"
#include "lua.h"
#ifdef __cplusplus
}
#endif


#include <stdint.h>


#ifndef SWIGRUNTIME
#include <swigluarun.h>

/* swigluarun does not include the lua specific defines. Add them here. */
typedef struct
{
	lua_State* L; /* the state */
	int ref;      /* a ref in the lua global index */
} SWIGLUA_REF;
#endif  /* SWIGRUNTIME */



#ifndef __WRAPPER_H__
#define __WRAPPER_H__

#ifndef SWIG
	/* Some custom error messages. */
#       define CUSTOM_ERROR_NOT_CONNECTED -2
#       define CUSTOM_ERROR_INVALID_PROTOCOL -3
#       define CUSTOM_ERROR_INVALID_DISCOVER -4
#       define CUSTOM_ERROR_OUT_OF_MEMORY -5
#endif

enum LXI_RESULT_ENUM
{
	__ENUM_LXI_OK = LXI_OK,
	__ENUM_LXI_ERROR = LXI_ERROR,
	__ENUM_CUSTOM_ERROR_NOT_CONNECTED = CUSTOM_ERROR_NOT_CONNECTED,
	__ENUM_CUSTOM_ERROR_INVALID_PROTOCOL = CUSTOM_ERROR_INVALID_PROTOCOL,
	__ENUM_CUSTOM_ERROR_INVALID_DISCOVER = CUSTOM_ERROR_INVALID_DISCOVER,
	__ENUM_CUSTOM_ERROR_OUT_OF_MEMORY = CUSTOM_ERROR_OUT_OF_MEMORY
};


enum LXI_PROTOCOL_ENUM
{
	__ENUM_VXI11 = VXI11,
	__ENUM_RAW = RAW,
	__ENUM_HISLIP = HISLIP
};


enum LXI_DISCOVER_ENUM
{
	__ENUM_DISCOVER_VXI11 = DISCOVER_VXI11,
	__ENUM_DISCOVER_MDNS = DISCOVER_MDNS
};



typedef int RESULT_INT_TRUE_OR_NIL_WITH_ERR;
typedef int RESULT_INT_NOTHING_OR_NIL_WITH_ERR;
typedef int RESULT_INT_NOTHING_OR_NIL_WITH_ERR_FREE_ARG2;


class Lxi
{
public:
	Lxi(void);
	~Lxi(void);

	void set_callback(SWIGLUA_REF tLuaFn, long lCallbackUserData);

	RESULT_INT_NOTHING_OR_NIL_WITH_ERR discover(lua_State *MUHKUH_SWIG_OUTPUT_CUSTOM_OBJECT, int timeout, enum LXI_DISCOVER_ENUM tDiscover);
	RESULT_INT_TRUE_OR_NIL_WITH_ERR connect(const char *address, int port, const char *name, int timeout, enum LXI_PROTOCOL_ENUM tProtocol);
	RESULT_INT_TRUE_OR_NIL_WITH_ERR send(const char *pcBUFFER_IN, size_t sizBUFFER_IN, int timeout);
	RESULT_INT_NOTHING_OR_NIL_WITH_ERR_FREE_ARG2 receive(char *pcBUFFER_OUT, size_t sizBUFFER_MAX, int timeout);
	RESULT_INT_TRUE_OR_NIL_WITH_ERR disconnect(void);

#ifndef SWIG
	const char *get_error_string(void);

	static void s_discover_broadcast(const char *pcAddress, const char *pcInterface, void *pvUser);
	static void s_discover_device(const char *pcAddress, const char *pcId, void *pvUser);
	static void s_discover_service(const char *pcAddress, const char *pcId, const char *pcService, int iPort, void *pvUser);

	void discover_broadcast(const char *pcAddress, const char *pcInterface);
	void discover_device(const char *pcAddress, const char *pcId);
	void discover_service(const char *pcAddress, const char *pcId, const char *pcService, int iPort);


private:
	typedef struct DISCOVER_RECORD_STRUCT
	{
		char *pcAddress;
		char *pcId;
	} DISCOVER_RECORD_T;

	void free_discovered_devices(void);
	void log(const char *pcFmt, ...);
	void callback_long(SWIGLUA_REF *ptLuaFn, long lData, long lCallbackUserData);
	void callback_string(SWIGLUA_REF *ptLuaFn, const char *pcData, size_t sizData, long lCallbackUserData);
	void callback_common(SWIGLUA_REF *ptLuaFn, long lCallbackUserData, int iOldTopOfStack);

	SWIGLUA_REF m_tCallbackLuaFn;
	long m_lCallbackUserData;

	int m_iDevice;
	char *m_pcAddress;
	int m_iPort;
	char *m_pcName;
	enum LXI_PROTOCOL_ENUM m_tProtocol;

	DISCOVER_RECORD_T *m_ptDiscoveredDevices;
	size_t m_sizDiscoveredDevicesCnt;
	size_t m_sizDiscoveredDevicesMax;
#endif  /* SWIG */
};


#endif  /* __WRAPPER_H__ */
