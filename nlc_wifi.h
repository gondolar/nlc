#include "llc_json.h"
#include "llc_tcpip.h"
#include "llc_append_json.h"

#ifdef LLC_ESP8266
#	include <ESP8266WiFi.h>
#else
#	include <WiFi.h>
#endif

#ifndef NLC_WIFI_H
#define NLC_WIFI_H

namespace llc
{
#pragma pack(push, 1)
	struct SWiFiMode {
		uint32_t	DisableSAP		: 1;
		uint32_t	DisableSTA		: 1;
		uint32_t	DisableDHCP		: 1;
	};
#pragma pack(pop)
	struct SWiFiAddress : llc::SIPv4Node {
		::llc::vcs		SSID			= {};//::llc::vcsc_t{8, "firewall"};
		::llc::vcs		Password		= {};//::llc::vcsc_t{8, "miau1234"};
		::llc::vcs		Hostname		= {};//::llc::vcsc_t{5, "mlcap"};
		::llc::vcs		IPv6			= {};
		::llc::vcs		MAC				= {};

		::llc::error_t	KeyValues		(::llc::asc_t & output, bool preComma = false) {
			if(SSID		.size()	)	{ llc_necs(llc::appendKeyString(output, "ssid"		, str(SSID		), preComma)); 	preComma = true; }
			if(Password	.size()	)	{ llc_necs(llc::appendKeyString(output, "password"	, str(Password	), preComma));	preComma = true; }
			if(Hostname	.size()	)	{ llc_necs(llc::appendKeyString(output, "hostname"	, str(Hostname	), preComma));	preComma = true; }
			if(IPv4				)	{ llc_necs(llc::appendKeyString(output, "ipv4"		, str(IPv4		), preComma));	preComma = true; }
			if(IPv6		.size()	)	{ llc_necs(llc::appendKeyString(output, "ipv6"		, str(IPv6		), preComma));	preComma = true; }
			if(MAC		.size()	)	{ llc_necs(llc::appendKeyString(output, "mac"		, str(MAC		), preComma));	preComma = true; }
			if(Gateway			)	{ llc_necs(llc::appendKeyString(output, "gateway"	, str(Gateway	), preComma));	preComma = true; }
			if(NetMask			)	{ llc_necs(llc::appendKeyString(output, "netmask"	, str(NetMask	), preComma));	preComma = true; }
			return 0;
		}
	};

	struct SWiFiAddressSAP : SWiFiAddress {};
	struct SWiFiAddressSTA : SWiFiAddress {
		llc::astipv4<3>	DNS				= {}; // {::llc::vcs{"208.67.222.222"}, ::llc::vcs{"208.67.220.220"}};
	};

	template<typename TTime>
	struct SWiFiTimeoutSTA {
		TTime	 		Connect		= TTime(10);	// Seconds
		TTime	 		Reconnect	= TTime(60);	// Seconds
		TTime	 		Refresh		= TTime(10);	// Seconds
	};

	template<typename TTime>
	struct SWiFiTimeoutSAP {
		TTime	 		Disconnect	= 0;		// Seconds
		TTime	 		Purge		= TTime(3600 * 24);	// Seconds
		TTime	 		Refresh		= TTime(10);		// Seconds
	};

	struct SWiFiStateSTA { SWiFiAddressSTA Address = {}; SWiFiTimeoutSTA<uint32_t> Timeouts = {}; SWiFiTimeoutSTA<float> NextTime = {}; };
	struct SWiFiStateSAP { SWiFiAddressSAP Address = {}; SWiFiTimeoutSAP<uint32_t> Timeouts = {}; SWiFiTimeoutSAP<float> NextTime = {}; };

	struct SWiFiState {
		SWiFiStateSTA	STA			= {};
		SWiFiStateSAP	SAP			= {};
		SWiFiMode		Mode		= {};
	};

	GDEFINE_ENUM_TYPE(WLAN_STATUS, u0_t);
	GDEFINE_ENUM_VALUE(WLAN_STATUS, No_shield       , 255);   // for compatibility with WiFi Shield library
	GDEFINE_ENUM_VALUE(WLAN_STATUS, Idle_status     ,   0);
	GDEFINE_ENUM_VALUE(WLAN_STATUS, No_SSID_found   ,   1);
	GDEFINE_ENUM_VALUE(WLAN_STATUS, Scan_completed  ,   2);
	GDEFINE_ENUM_VALUE(WLAN_STATUS, Connected       ,   3);
	GDEFINE_ENUM_VALUE(WLAN_STATUS, Connect_failed  ,   4);
	GDEFINE_ENUM_VALUE(WLAN_STATUS, Connection_lost ,   5);
	GDEFINE_ENUM_VALUE(WLAN_STATUS, Disconnected    ,   6);

	::llc::error_t	wifiLogSAP	(const ::llc::SWiFiStateSAP & state, uint32_t clients);
	::llc::error_t	wifiLogSTA	(const ::llc::SWiFiStateSTA & state, WLAN_STATUS wifiStatus);

	GDEFINE_ENUM_TYPE(NETWORK_ADAPTER, uint8_t);
	GDEFINE_ENUM_VALUE(NETWORK_ADAPTER, WiFi_STA   , 0);
	GDEFINE_ENUM_VALUE(NETWORK_ADAPTER, WiFi_AP    , 1);
	GDEFINE_ENUM_VALUE(NETWORK_ADAPTER, Ethernet   , 2);


	GDEFINE_ENUM_TYPE(WIFI_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(WIFI_MODE, Default        , 0);
	GDEFINE_ENUM_VALUE(WIFI_MODE, STA            , 1);
	GDEFINE_ENUM_VALUE(WIFI_MODE, AP             , 2);
	GDEFINE_ENUM_VALUE(WIFI_MODE, AP_STA         , 3);
	GDEFINE_ENUM_VALUE(WIFI_MODE, Disabled       , 4);
	::llc::error_t	wifiAddressLoad		(const ::llc::SJSONReader & config, int32_t iJSONValue, ::llc::SWiFiAddress & address);
	::llc::error_t	wifiAddressLoad		(const ::llc::SJSONReader & config, int32_t iJSONValue, ::llc::vcs key, ::llc::SWiFiAddress & address);
	::llc::error_t	wifiSTAAddressLoad	(const ::llc::SJSONReader & config, int32_t iJSONValue, ::llc::SWiFiAddressSTA & address);
	::llc::error_t	wifiSTAAddressLoad	(const ::llc::SJSONReader & config, int32_t iJSONValue, ::llc::vcs key, ::llc::SWiFiAddressSTA & address);
	//::llc::error_t	wifiSAPAddressLoad	(const ::llc::SJSONReader & config, int32_t iJSONValue, ::llc::SWiFiAddressSAP & address);
	::llc::error_t	wifiSAPAddressLoad	(const ::llc::SJSONReader & config, int32_t iJSONValue, ::llc::vcs key, ::llc::SWiFiAddressSAP & address);

#ifdef	ESP8266
	struct WiFiClass : public ::ESP8266WiFiClass {};
#else
	struct WiFiClass : public ::WiFiClass {};
#endif
	struct SWiFi {
		WiFiClass		WiFi				= {};
		//SWiFiState		State			= {};	// Current state
		SWiFiState		Setup				= {};	// Desired state
	};

	::llc::error_t		wifiConfigLoad		(const ::llc::SJSONReader & jsonConfig, int32_t iNode, ::llc::SWiFiState & wifiConfig);
	::llc::error_t		wifiConfigLoad		(const ::llc::vcsc_t & jsonString, ::llc::SWiFiState & wifiConfig);

	::llc::error_t		wifiSAPSetup		(::llc::WiFiClass & device, ::llc::SWiFiStateSAP & state);
	::llc::error_t		wifiSAPEnable		(::llc::WiFiClass & device, ::llc::SWiFiMode & activeMode);
	::llc::error_t		wifiSAPDisable		(::llc::WiFiClass & device, ::llc::SWiFiMode & activeMode);
	::llc::error_t		wifiSTASetup		(::llc::WiFiClass & device, ::llc::SWiFiStateSTA & state, bool disableDHCP = false);
	::llc::error_t		wifiSTAConnect		(::llc::WiFiClass & device, ::llc::vcs ssid, ::llc::vcs password);
	::llc::error_t		wifiSTADisconnect	(::llc::WiFiClass & device, ::llc::SWiFiState & config);
	::llc::error_t		wifiSTAUpdate		(::llc::WiFiClass & device, ::llc::SWiFiState & config);
	::llc::error_t		wifiSTAAddress		(::llc::WiFiClass & device, ::llc::SWiFiAddressSTA & address);
	::llc::error_t		wifiSAPAddress		(::llc::WiFiClass & device, ::llc::SWiFiAddressSAP & address);
	::llc::error_t		wifiAddressInfo		(const ::llc::SWiFiAddress & address, ::llc::asc_t & infoString, bool ownBrackets = false);
	::llc::error_t		wifiSAPAddressInfo	(const ::llc::SWiFiAddressSAP & address, ::llc::asc_t & infoString, bool ownBrackets = false);
	::llc::error_t		wifiSTAAddressInfo	(const ::llc::SWiFiAddressSTA & address, ::llc::asc_t & infoString, bool ownBrackets = false);
	::llc::error_t		wifiAllAddressInfo	(const ::llc::SWiFiState & state, ::llc::asc_t & infoString, bool ownBrackets = false);
	::llc::error_t		wifiSetup			(::llc::WiFiClass & device, ::llc::SWiFiState & config);
	::llc::error_t		wifiUpdate			(::llc::WiFiClass & device, ::llc::SWiFiState & config);

	::llc::error_t		wifiSetup			(::llc::WiFiClass & device, ::llc::SWiFiState & wifiConfig, const ::llc::vcs & configJson);

	stxp	sc_c		WIFI_CONFIG[]
		= "{ \"sta\":"
		"\n\t{ \"ssid\"     : \"\"" // \"Fibertel WiFi314 2.4GHz\""
		"\n\t, \"password\" : \"\"" // \"00927959079590\""
		"\n\t, \"hostname\" : \"mlcsta\""
		"\n\t, \"dns\"      : [[208,67,222,222], \"208.67.220.220\"]"
		"\n\t}"
		"\n, \"sap\":"
		"\n\t{ \"ssid\"     : \"firewall\""
		"\n\t, \"password\" : \"miau1234\""
		"\n\t, \"hostname\" : \"mlcsap\""
		"\n\t, \"ipv4\"     : [10,0,0,1]"
		"\n\t, \"netmask\"  : [255,0,0,0]"
		"\n\t, \"gateway\"  : [10,0,0,1]"
		"\n\t}"
		"\n, \"disable_sap\"  : false" //true"
		"\n, \"disable_sta\"  : false" //true"
		"\n, \"disable_dhcp\" : false"
		"\n}";

} // namespace

#endif // NLC_WIFI_H
