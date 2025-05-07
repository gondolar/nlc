#include "nlc_wifi.h"

#include "llc_arduino_string.h"
#include "llc_std_cstring.h"
#include "llc_std_string.h"
#include "llc_parse.h"
#include "llc_append_tcpip.h"

#ifdef LLC_ESP8266
#	include <netif\ethernet.h>
#	include <netif\etharp.h>
#else // !LLC_ESP8266
#	include <esp_wifi.h>
#	include <esp_netif.h>
#	include "esp_wpa2.h"
#	include <WiFiGeneric.h>
#endif // LLC_ESP8266
// 20291938664
// l4c0nch4tuy44F1P!
// l4conch4tuy44F1P!

::llc::err_t		llc::wifiAddressLoad 	(const ::llc::SJSONReader & cfgDoc, int32_t iAddress, ::llc::SWiFiAddress & address) {
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	i_if_failed(::llc::jsonObjectGetAsString(cfgDoc, iAddress, "ssid"		, address.SSID		), "iAddress: %i.", iAddress) else address.SSID		= ::llc::label(address.SSID);
	i_if_failed(::llc::jsonObjectGetAsString(cfgDoc, iAddress, "password"	, address.Password	), "iAddress: %i.", iAddress) else address.Password	= ::llc::label(address.Password);
	i_if_failed(::llc::jsonObjectGetAsString(cfgDoc, iAddress, "hostname"	, address.Hostname	), "iAddress: %i.", iAddress) else address.Hostname	= ::llc::label(address.Hostname);
	i_if_failed(::llc::jsonObjectGetAsString(cfgDoc, iAddress, "mac"		, address.MAC		), "iAddress: %i.", iAddress) else address.MAC			= ::llc::label(address.MAC);
	{
		::llc::vcs				ipv4;
		if(succeeded(::llc::jsonObjectGetAsString(cfgDoc, iAddress, "ip", ipv4)))
			address.IPv4 = ipv4;
		else {
			i_if_failed(::llc::jsonObjectGetAsString(cfgDoc, iAddress, "ipv4", ipv4), "iAddress: %i.", iAddress)
			else
				address.IPv4 = ipv4;
		}
	}
	{ ::llc::vcs ipv4; i_if_failed(::llc::jsonObjectGetAsString(cfgDoc, iAddress, "gateway"	, ipv4), "iAddress: %i.", iAddress) else address.Gateway	= ipv4; }
	{ ::llc::vcs ipv4; i_if_failed(::llc::jsonObjectGetAsString(cfgDoc, iAddress, "netmask"	, ipv4), "iAddress: %i.", iAddress) else address.NetMask	= ipv4; }
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	rtrn 0;
}
::llc::err_t		llc::wifiAddressLoad 	(const ::llc::SJSONReader & cfgDoc, int32_t iNode, ::llc::vcs key, ::llc::SWiFiAddress & address) {
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	s2_t					iAddress;
	ree_if_failed(iAddress = ::llc::jsonObjectValueGet(cfgDoc, iNode, key), "iNode: %i, key: '%s'", iNode, ::llc::toString(key).begin());
	i_if_failed(::llc::wifiAddressLoad(cfgDoc, iAddress, address), "iAddress: %i, iNode: %i, key: '%s'", iAddress, iNode, ::llc::toString(key).begin());
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	rtrn 0;
}
::llc::err_t		llc::wifiSTAAddressLoad 	(const ::llc::SJSONReader & cfgDoc, int32_t iAddress, ::llc::SWiFiAddressSTA & address) {
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	::llc::err_t			iNode;
	llc_necs(iNode = ::llc::jsonObjectValueGet(cfgDoc, iAddress, "dns"));
	::llc::vcs				ipv4;
	if(cfgDoc[iNode]->Token->Type == llc::JSON_TYPE_STRING)
		i_if_failed(::llc::jsonObjectGetAsString(cfgDoc, iAddress, "dns", ipv4), "iAddress: %i.", iAddress) else address.DNS[0] = {cfgDoc.View[iNode]};
	else if(cfgDoc[iNode]->Token->Type == llc::JSON_TYPE_ARRAY) {
		u2_c listLen = ::llc::jsonArraySize(cfgDoc, iNode);
		if(listLen == 4)
			i_if_failed(::llc::jsonObjectGetAsString(cfgDoc, iAddress, "dns", ipv4), "iAddress: %i.", iAddress) else address.DNS[0] = ipv4;
		else {
			for(uint32_t iAddr = 0; iAddr < min(address.DNS.size(), listLen); ++iAddr) {
				u2_c iAddrNode = ::llc::jsonArrayValueGet(cfgDoc, iNode, iAddr);
				i_if_failed(::llc::jsonObjectGetAsString(cfgDoc, iAddress, "dns", ipv4), "iAddress: %i.", iAddress) else address.DNS[iAddr] = {cfgDoc.View[iAddrNode]};
			}
		}
	}
	info_printf("TODO: '%s'", ::llc::toString(ipv4).begin());
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	rtrn 0;
}
::llc::err_t		llc::wifiSTAAddressLoad 	(const ::llc::SJSONReader & cfgDoc, int32_t iNode, ::llc::vcs key, ::llc::SWiFiAddressSTA & address) {
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	s2_t					iAddress;
	ree_if_failed(iAddress = ::llc::jsonObjectValueGet(cfgDoc, iNode, key), "iNode: %i, key: '%s'", iNode, ::llc::toString(key).begin());
	i_if_failed(::llc::wifiAddressLoad(cfgDoc, iAddress, address), "iAddress: %i, iNode: %i, key: '%s'", iAddress, iNode, ::llc::toString(key).begin());
	i_if_failed(::llc::wifiSTAAddressLoad(cfgDoc, iAddress, address), "iAddress: %i, iNode: %i, key: '%s'", iAddress, iNode, ::llc::toString(key).begin());
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	rtrn 0;
}
::llc::err_t		llc::wifiSAPAddressLoad 	(const ::llc::SJSONReader & cfgDoc, int32_t iNode, ::llc::vcs key, ::llc::SWiFiAddressSAP & address) {
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	s2_t					iAddress;
	ree_if_failed(iAddress = ::llc::jsonObjectValueGet(cfgDoc, iNode, key), "iNode: %i, key: '%s'", iNode, ::llc::toString(key).begin());
	i_if_failed(::llc::wifiAddressLoad(cfgDoc, iAddress, address), "iAddress: %i, iNode: %i, key: '%s'", iAddress, iNode, ::llc::toString(key).begin());
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	rtrn 0;
}
::llc::err_t		llc::wifiConfigLoad		(const ::llc::SJSONReader & cfgDoc, int32_t iNode, ::llc::SWiFiState & wifiConfig) {
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	ws_if_failed(::llc::wifiSTAAddressLoad(cfgDoc, iNode, "sta", wifiConfig.STA.Address));
	ws_if_failed(::llc::wifiSAPAddressLoad(cfgDoc, iNode, "sap", wifiConfig.SAP.Address));
	{ bool disableSAP	= false; is_if_failed(::llc::jsonObjectGetAsBoolean(cfgDoc, iNode, "disable_sap"	, disableSAP	)) else wifiConfig.Mode.DisableSAP 	= disableSAP; }
	{ bool disableSTA	= false; is_if_failed(::llc::jsonObjectGetAsBoolean(cfgDoc, iNode, "disable_sta"	, disableSTA	)) else wifiConfig.Mode.DisableSTA	= disableSTA; }
	{ bool disableDHCP	= false; is_if_failed(::llc::jsonObjectGetAsBoolean(cfgDoc, iNode, "disable_dhcp"	, disableDHCP	)) else wifiConfig.Mode.DisableDHCP	= disableDHCP; }
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	rtrn 0;
}
::llc::err_t		llc::wifiConfigLoad		(const ::llc::vcsc_t & jsonString, ::llc::SWiFiState & wifiConfig) {
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	{
		::llc::SJSONReader		cfgDoc;
		llc_necs(::llc::jsonParse(cfgDoc, jsonString));
		llc_necs(::llc::wifiConfigLoad(cfgDoc, 0, wifiConfig));
	}
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	rtrn 0;
}
::llc::err_t		llc::wifiSAPSetup		(::llc::WiFiClass & device, ::llc::SWiFiStateSAP & state) {
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	info_printf("Starting AP. SSID: '%s'. Password: '%s'.", state.Address.SSID.begin(), state.Address.Password.begin());
	cnst SWiFiStateSAP		stateInit			= state;
	i_if(false == device.softAP(state.Address.SSID.begin(), state.Address.Password.begin()));
	if(state.Address.Hostname.size()) {
#ifndef LLC_ESP8266
		if_if(false == device.softAPsetHostname(state.Address.Hostname.begin()), "Hostname: '%s'.", state.Address.Hostname.begin());
#endif // LLC_ESP8266
	}
	if_fail_e(::llc::wifiSAPAddress(device, state.Address));
	SIPv4 local_ip	= state.Address.IPv4	;
	SIPv4 gateway	= state.Address.Gateway	;
	SIPv4 netmask	= state.Address.NetMask	;
	{ if(stateInit.Address.IPv4				) local_ip	= stateInit.Address.IPv4	; }
	{ if(stateInit.Address.NetMask			) netmask	= stateInit.Address.NetMask	; }
	gateway		= stateInit.Address.Gateway ? stateInit.Address.Gateway	: local_ip;
	e_if_zero(device.softAPConfig(local_ip.IP, gateway.IP, netmask.IP));

	if_fail_e(::llc::wifiLogSAP(state, device.softAPgetStationNum()));
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	rtrn 0;
}
::llc::err_t		llc::wifiSTAConnect		(::llc::WiFiClass & device, ::llc::vcs ssid, ::llc::vcs password) {
	rees_if(WLAN_STATUS_No_shield == (WLAN_STATUS)device.begin(ssid.begin(), password.begin()));
	info_printf("Connecting to '%s'.", ssid.begin());
	rtrn 0;
}
::llc::err_t		llc::wifiSTASetup		(::llc::WiFiClass & device, ::llc::SWiFiStateSTA & state, bool disableDHCP) {
	LLC_PLATFORM_CRT_CHECK_MEMORY();

	const ::llc::SWiFiStateSTA	stateInit			= state;
	llc_necall(::llc::wifiSTAConnect(device, state.Address.SSID, state.Address.Password), "Failed to connect using ssid: '%s', password: '%s'.", state.Address.SSID.begin(), state.Address.Password.begin());
	es_if_failed(::llc::wifiSTAAddress(device, state.Address));

	// obtain current values
	IPAddress local_ip	= state.Address.IPv4	.IP;
	IPAddress gateway	= state.Address.Gateway	.IP;
	IPAddress subnet	= state.Address.NetMask	.IP;
	IPAddress dns0		= state.Address.DNS[0]	.IP;
	IPAddress dns1		= state.Address.DNS[1]	.IP;

	// override with the ones specified by user
	{ if(stateInit.Address.DNS[0]) dns0 = stateInit.Address.DNS[0].IP; }
	{ if(stateInit.Address.DNS[1]) dns1 = stateInit.Address.DNS[1].IP; }

	if(false == disableDHCP) {
		is_if(false == device.config(local_ip, gateway, subnet, dns0, dns1));
	}
	else {
		info_printf("%s", "DHCP disabled.");
		// Override these too if DHCP is disabled.
		{ if(stateInit.Address.IPv4					) local_ip	= stateInit.Address.IPv4		.IP; }
		{ if(stateInit.Address.Gateway				) gateway	= stateInit.Address.Gateway		.IP; }
		{ if(stateInit.Address.NetMask			) subnet	= stateInit.Address.NetMask	.IP; }
		is_if(false == device.config(local_ip, gateway, subnet, dns0, dns1));
	}
	info_printf("WiFi configuration set:\nIPv4: '%s', \nGateway IP: '%s', \nSubnet mask: '%s', \nDNS 0: '%s', \nDNS 1: '%s'.", local_ip.toString().begin(), gateway.toString().begin(), subnet.toString().begin(), dns0.toString().begin(), dns1.toString().begin());

	if(stateInit.Address.Hostname.size()) {
		is_if(false == device.setHostname(stateInit.Address.Hostname.begin())) // define hostname
		else
			info_printf("Hostname set to: '%s'.", stateInit.Address.Hostname.begin());
	}
	es_if_failed(::llc::wifiSTAAddress(device, state.Address));
	es_if_failed(::llc::wifiLogSTA(state, (WLAN_STATUS)device.status()));
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	rtrn 0;
}
::llc::err_t		llc::wifiSetup			(::llc::WiFiClass & device, ::llc::SWiFiState & wifiConfig) {
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	::llc::WIFI_MODE	mode			= wifiConfig.Mode.DisableSTA
		? wifiConfig.Mode.DisableSAP ? ::llc::WIFI_MODE_Disabled : llc::WIFI_MODE_AP
		: wifiConfig.Mode.DisableSAP ? ::llc::WIFI_MODE_STA      : llc::WIFI_MODE_AP_STA
		;
#ifndef LLC_ESP8266
	device.mode(wifi_mode_t((mode == ::llc::WIFI_MODE_Disabled) ? ::llc::WIFI_MODE_Default : mode));
#else // !LLC_ESP8266
	device.mode(WiFiMode_t((mode == ::llc::WIFI_MODE_Disabled) ? ::llc::WIFI_MODE_Default : mode));
#endif // LLC_ESP8266
	rni_if(mode == ::llc::WIFI_MODE_Disabled, "%s", "WiFi disabled.");

	::llc::error_t 		softAPresult	= 0;
	if(wifiConfig.Mode.DisableSAP)
		info_printf("%s", "Soft AP disabled.");
	else {
		es_if_failed(softAPresult = ::llc::wifiSAPSetup(device, wifiConfig.SAP));
		wifiConfig.Mode.DisableSAP	= llc::failed(softAPresult) ? true : false;
		es_if_failed(::llc::wifiLogSAP(wifiConfig.SAP, device.softAPgetStationNum()));
	}
	if(wifiConfig.Mode.DisableSTA)
		info_printf("%s", "WiFi STA disabled.");
	else {
		if_zero_w(wifiConfig.STA.Address.SSID.size())
		else {
			llc_necs(::llc::wifiSTASetup(device, wifiConfig.STA, wifiConfig.Mode.DisableDHCP));
			es_if_failed(::llc::wifiLogSTA(wifiConfig.STA, (WLAN_STATUS)device.status()));
		}
	}
	rves_if_failed(softAPresult, softAPresult);
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	rtrn softAPresult;
}
::llc::err_t		llc::wifiSetup			(::llc::WiFiClass & device, ::llc::SWiFiState & deviceConfig, const ::llc::vcs & jsonString) {
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	if(jsonString.size()) {
		info_printf("Loading WiFi configuration from JSON:\n%s\n", ::llc::toString(jsonString).begin());

		::llc::SJSONReader	cfgDoc;
		llc_necs(::llc::jsonParse(cfgDoc, jsonString));
		llc_necs(::llc::wifiConfigLoad(cfgDoc, 0, deviceConfig));
	}
	es_if_failed(::llc::wifiSetup(device, deviceConfig));
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	rtrn 0;
}
::llc::err_t		llc::wifiSTADisconnect	(::llc::WiFiClass & device, ::llc::SWiFiState & config)	{
	rwws_if(config.Mode.DisableSTA);
    rews_if(0 == (device.getMode() & WIFI_MODE_STA));
#ifdef LLC_ESP8266
	rees_if(::wifi_station_disconnect());
#else // !LLC_ESP8266
	rees_if(::esp_wifi_disconnect());
#endif // LLC_ESP8266
    rtrn -1;
}
::llc::err_t		llc::wifiSTAUpdate		(::llc::WiFiClass & device, ::llc::SWiFiState & config)	{
	rnis_if(config.Mode.DisableSTA);
	if_zero_fw(config.STA.Address.SSID.size());
	if(WLAN_STATUS_Connected != (WLAN_STATUS)device.status()) {
		const int64_t		now			= millis() * .001f;
		if(now > config.STA.NextTime.Reconnect) {
			config.STA.NextTime.Reconnect	= now + config.STA.Timeouts.Reconnect;
			llc_necs(llc::wifiSTAConnect(device, config.STA.Address.SSID, config.STA.Address.Password));
		}
	}
	rtrn 0;
}

stxp uint32_t networkID(const llc::SIPv4 ip, const llc::SIPv4 subnet) {
	rtrn ::llc::byte_to<uint32_t>(subnet[0] & ip[0], 0)
	  |  ::llc::byte_to<uint32_t>(subnet[1] & ip[1], 1)
	  |  ::llc::byte_to<uint32_t>(subnet[2] & ip[2], 2)
	  |  ::llc::byte_to<uint32_t>(subnet[3] & ip[3], 3)
	  ;
}

stxp uint32_t broadcastIP(const llc::SIPv4 ip, const llc::SIPv4 subnet) {
	rtrn ::llc::byte_to<uint32_t>(~subnet[0] | ip[0], 0)
	  |  ::llc::byte_to<uint32_t>(~subnet[1] | ip[1], 1)
	  |  ::llc::byte_to<uint32_t>(~subnet[2] | ip[2], 2)
	  |  ::llc::byte_to<uint32_t>(~subnet[3] | ip[3], 3)
	  ;
}
#ifndef LLC_ESP8266
esp_netif_t* get_esp_interface_netif(esp_interface_t interface);
#endif // LLC_ESP8266

sttc	::llc::err_t	ipv4NodeAddresses	(llc::SIPv4Node & nodeAddresses, ::llc::NETWORK_ADAPTER adapter) {
#ifdef LLC_ESP8266
    struct ip_info ip;
    wifi_get_ip_info(STATION_IF, &ip);
#else // !LLC_ESP8266
	esp_netif_t					* netif;
    rees_if(0 == (netif = get_esp_interface_netif((esp_interface_t)adapter)));
	esp_netif_ip_info_t			ip			= {};
	rees_if(esp_netif_get_ip_info(netif, &ip));
#endif // LLC_ESP8266
	nodeAddresses.IPv4		= ip.ip.addr;
	nodeAddresses.Gateway	= ip.gw.addr;
	nodeAddresses.NetMask	= ip.netmask.addr;
	rtrn 0;
}

sttc	::llc::err_t	wifiSTASSID			(::llc::WiFiClass & device, ::llc::asc_t & ssid				)	{ rews_if(0 == (device.getMode() & ::llc::WIFI_MODE_STA));	rtrn ::llc::toachar(ssid		, device.SSID		()); }
sttc	::llc::err_t	wifiSTAMAC			(::llc::WiFiClass & device, ::llc::asc_t & mac				)	{ rews_if(0 == (device.getMode() & ::llc::WIFI_MODE_STA));	rtrn ::llc::toachar(mac		, device.macAddress	()); }
sttc	::llc::err_t	wifiSTAIP			(::llc::WiFiClass & device, ::llc::asc_t & ip				)	{ rews_if(0 == (device.getMode() & ::llc::WIFI_MODE_STA));	rtrn ::llc::toachar(ip		, device.localIP	().toString()); }
sttc	::llc::err_t	wifiSTAHostname		(::llc::WiFiClass & device, ::llc::asc_t & hostname			)	{ rews_if(0 == (device.getMode() & ::llc::WIFI_MODE_STA));	rtrn ::llc::toachar(hostname	, device.getHostname()); }
sttc	::llc::err_t	wifiSTAGatewayIP	(::llc::WiFiClass & device, ::llc::asc_t & gatewayIP		)	{ rews_if(0 == (device.getMode() & ::llc::WIFI_MODE_STA));	rtrn ::llc::toachar(gatewayIP	, device.gatewayIP	().toString()); }
sttc	::llc::err_t	wifiSTADNS			(::llc::WiFiClass & device, ::llc::asc_t & dns				)	{ rews_if(0 == (device.getMode() & ::llc::WIFI_MODE_STA));	rtrn ::llc::toachar(dns		, device.dnsIP		().toString()); }
sttc	::llc::err_t	wifiSTASubnetMask	(::llc::WiFiClass & device, ::llc::asc_t & subnetMask		)	{ rews_if(0 == (device.getMode() & ::llc::WIFI_MODE_STA));	rtrn ::llc::toachar(subnetMask, device.subnetMask	().toString()); }
::llc::err_t			llc::wifiSTAAddress		(::llc::WiFiClass & device, ::llc::SWiFiAddressSTA & address) {
	rews_if(0 == (device.getMode() & WIFI_MODE_STA));

	es_if_failed(ipv4NodeAddresses(address, NETWORK_ADAPTER_WiFi_STA));

	//::llc::tolabel(address.SSID			, device.SSID		());
	::llc::tolabel(address.MAC			, device.macAddress	());
	//::llc::tolabel(address.IPv4			, device.localIP	().toString());
#ifndef LLC_ESP8266
	if(memcmp(::llc::astu8<16>{}.begin(), (const uint8_t*)device.localIPv6(), 16))
		::llc::tolabel(address.IPv6, device.localIPv6().toString());
#endif // LLC_ESP8266
	if(device.getHostname()) {
		::llc::tolabel(address.Hostname, device.getHostname());
	}
	address.DNS[0].IP = device.dnsIP		();
	address.DNS[1].IP = device.dnsIP		(1);
	rtrn 0;
}

sttc	::llc::err_t	wifiSAPSSID			(::llc::WiFiClass & device, ::llc::asc_t & ssid			)	{ rees_if(0 == (device.getMode() & ::llc::WIFI_MODE_AP)); rtrn ::llc::toachar(ssid		, device.softAPSSID			()); }
sttc	::llc::err_t	wifiSAPMAC			(::llc::WiFiClass & device, ::llc::asc_t & mac			)	{ rees_if(0 == (device.getMode() & ::llc::WIFI_MODE_AP)); rtrn ::llc::toachar(mac			, device.softAPmacAddress	()); }
sttc	::llc::err_t	wifiSAPIP			(::llc::WiFiClass & device, ::llc::asc_t & ip			)	{ rees_if(0 == (device.getMode() & ::llc::WIFI_MODE_AP)); rtrn ::llc::toachar(ip			, device.softAPIP			().toString()); }
#ifndef LLC_ESP8266
sttc	::llc::err_t	wifiSAPHostname		(::llc::WiFiClass & device, ::llc::asc_t & hostname		)	{ rees_if(0 == (device.getMode() & ::llc::WIFI_MODE_AP)); rtrn ::llc::toachar(hostname	, device.softAPgetHostname	()); }
sttc	::llc::err_t	wifiSAPBroadcastIP	(::llc::WiFiClass & device, ::llc::asc_t & broadcastIP	)	{ rees_if(0 == (device.getMode() & ::llc::WIFI_MODE_AP)); rtrn ::llc::toachar(broadcastIP	, device.softAPBroadcastIP	().toString()); }
sttc	::llc::err_t	wifiSAPNetworkID	(::llc::WiFiClass & device, ::llc::asc_t & networkID	)	{ rees_if(0 == (device.getMode() & ::llc::WIFI_MODE_AP)); rtrn ::llc::toachar(networkID	, device.softAPNetworkID	().toString()); }
#endif

#ifdef LLC_ESP8266
// see comments on wifi_station_hostname in LwipIntf.cpp
extern "C" char* wifi_station_hostname; // sdk's hostname location
#endif // LLC_ESP8266
::llc::err_t			llc::wifiSAPAddress		(::llc::WiFiClass & device, ::llc::SWiFiAddressSAP & address) {
	rees_if(0 == (device.getMode() & WIFI_MODE_AP));

	if_fail_fe(ipv4NodeAddresses(address, NETWORK_ADAPTER_WiFi_AP));

	::llc::tolabel(address.SSID			, device.softAPSSID			());
	::llc::tolabel(address.MAC			, device.softAPmacAddress	());
	const char * hostname = 0;
#ifdef LLC_ESP8266
	hostname = wifi_station_hostname;
#else // !LLC_ESP8266
	if(memcmp(::llc::astu8<16>{}.begin(), (u0_c*)device.softAPIPv6(), 16))
		::llc::tolabel(address.IPv6, device.softAPIPv6().toString());
	esp_netif_get_hostname(get_esp_interface_netif(ESP_IF_WIFI_AP), &hostname);
#endif // LLC_ESP8266
	::llc::tolabel(address.Hostname, hostname);
	rtrn 0;
}

sttc	::llc::err_t	wifiLogAddress		(const ::llc::SWiFiAddress & address) 		{
	info_printf(
		"\nSSID       : '%s'."
		"\nPassword   : '%s'."
		"\nHostname   : '%s'."
		"\nIPv4       : '%s'."
		"\nMAC        : '%s'."
		"\nGateway    : '%s'."
		"\nSubnetMask : '%s'."
		"\nBroadcastIP: '%s'."
		"\nNetworkID  : '%s'."
		"%s%s%s"
		, address.SSID		.begin()
		, address.Password	.begin()
		, address.Hostname	.begin()
		, address.IPv4		.toString<16>().begin()
		, address.MAC		.begin()
		, address.Gateway	.toString<16>().begin()
		, address.NetMask	.toString<16>().begin()
		, address.BroadcastIP().toString<16>().begin()
		, address.NetworkID  ().toString<16>().begin()
		, address.IPv6.size() ? "\nIPv6       : '"   : ""
		, address.IPv6.size() ? address.IPv6.begin() : ""
		, address.IPv6.size() ? "'."                 : ""
		);

	rtrn 0;
}
namespace llc
{
	sttc	::llc::err_t	jsonOpenKey				(::llc::asc_t & output, ::llc::vcs keyName, bool preComma = false) {
		u2_c						offset 					= output.size();
		if(preComma)
			llc_necs(output.push_back(','));
		llc_necs(::llc::appendQuoted(output, keyName));
		llc_necs(output.push_back(':'));
		rtrn output.size() - offset;
	}
	sttc	::llc::err_t	jsonOpenKeyForEnclosed	(::llc::asc_t & output, ::llc::vcs keyName, char openingCharacter, bool preComma = false) {
		u2_c						offset 					= output.size();
		llc_necs(::llc::jsonOpenKey(output, keyName, preComma));
		llc_necs(output.push_back(openingCharacter));
		rtrn output.size() - offset;
	}
	stin	::llc::err_t	jsonOpenKeyForNumber		(::llc::asc_t & output, ::llc::vcs keyName, bool preComma = false) 						{ rtrn ::llc::jsonOpenKey(output, keyName, preComma); }
	stin	::llc::err_t	jsonOpenKeyForBoolean		(::llc::asc_t & output, ::llc::vcs keyName, bool preComma = false) 						{ rtrn ::llc::jsonOpenKey(output, keyName, preComma); }
	stin	::llc::err_t	jsonOpenKeyForString		(::llc::asc_t & output, ::llc::vcs keyName, bool preComma = false) 						{ rtrn ::llc::jsonOpenKeyForEnclosed(output, keyName, '"', preComma); }
	stin	::llc::err_t	jsonOpenKeyForObject		(::llc::asc_t & output, ::llc::vcs keyName, bool preComma = false) 						{ rtrn ::llc::jsonOpenKeyForEnclosed(output, keyName, '{', preComma); }
	stin	::llc::err_t	jsonOpenKeyForList			(::llc::asc_t & output, ::llc::vcs keyName, bool preComma = false) 						{ rtrn ::llc::jsonOpenKeyForEnclosed(output, keyName, '[', preComma); }

	sttc	::llc::err_t	jsonAppendBooleanValue		(::llc::asc_t & output, ::llc::vcs keyName, ::llc::vcs stnumber, bool preComma = false)	{ u2_c offset = output.size(); llc_necs(llc::jsonOpenKey(output, keyName, preComma)); llc_necs(output.append(stnumber)); rtrn output.size() - offset; }
	sttc	::llc::err_t	jsonAppendNumberValue 		(::llc::asc_t & output, ::llc::vcs keyName, ::llc::vcs stnumber, bool preComma = false)	{ u2_c offset = output.size(); llc_necs(llc::jsonOpenKey(output, keyName, preComma)); llc_necs(output.append(stnumber)); rtrn output.size() - offset; }
	sttc	::llc::err_t	jsonAppendStringValue 		(::llc::asc_t & output, ::llc::vcs keyName, ::llc::vcs strvalue, bool preComma = false)	{ u2_c offset = output.size(); llc_necs(llc::jsonOpenKey(output, keyName, preComma)); llc_necs(::llc::appendQuoted(output, strvalue)); rtrn output.size() - offset; }
	sttc	::llc::err_t	jsonAppendListContents 		(::llc::asc_t & output, ::llc::vcs keyName, ::llc::vcs contents, bool preComma = false)	{ u2_c offset = output.size(); llc_necs(llc::jsonOpenKey(output, keyName, preComma)); llc_necs(::llc::appendBraced(output, contents)); rtrn output.size() - offset; }
	sttc	::llc::err_t	jsonAppendObjectValues		(::llc::asc_t & output, ::llc::vcs keyName, ::llc::vcs stvalues, bool preComma = false) { u2_c offset = output.size(); llc_necs(llc::jsonOpenKeyForObject  (output, keyName, preComma)); llc_necs(output.append(stvalues)); llc_necs(output.push_back('}')); rtrn output.size() - offset; }
	sttc	::llc::err_t	jsonAppendIPValueAsList		(::llc::asc_t & output, ::llc::vcs keyName, ::llc::SIPv4 ip, bool preComma = false) 	{ u2_c offset = output.size(); llc_necs(llc::jsonOpenKey(output, keyName, preComma)); llc_necs(::llc::appendBraced(output, ip)); rtrn output.size() - offset; }
	sttc	::llc::err_t	jsonAppendIPValueAsString	(::llc::asc_t & output, ::llc::vcs keyName, ::llc::SIPv4 ip, bool preComma = false) 	{ u2_c offset = output.size(); llc_necs(llc::jsonOpenKey(output, keyName, preComma)); llc_necs(::llc::appendQuoted(output, ip)); rtrn output.size() - offset; }
	stin	::llc::err_t	jsonAppendIPAsList			(::llc::asc_t & output, ::llc::SIPv4 ip, bool preComma = false) 						{ rtrn ::llc::appendBracedPrefixed(output, ip, preComma, ',', ','); }
	stin	::llc::err_t	jsonAppendIPAsString		(::llc::asc_t & output, ::llc::SIPv4 ip, bool preComma = false) 						{ rtrn ::llc::appendQuotedPrefixed(output, ip, preComma, ',', '.'); }
} // namespace

::llc::err_t			llc::wifiAddressInfo	(const ::llc::SWiFiAddress & address, ::llc::asc_t & output, bool ownBrackets)			{
	uint32_t					offset 					= output.size();
	if(ownBrackets) {
		llc_necs(output.push_back('{'));
		++offset;
	}
	if(address.SSID		.size()	) { llc_necs(llc::jsonAppendStringValue		(output, "ssid"		, address.SSID      , output.size() - offset)); }
	if(address.Password	.size()	) { llc_necs(llc::jsonAppendStringValue		(output, "password"	, address.Password  , output.size() - offset)); }
	if(address.Hostname	.size()	) { llc_necs(llc::jsonAppendStringValue		(output, "hostname"	, address.Hostname  , output.size() - offset)); }
	if(address.IPv6		.size()	) { llc_necs(llc::jsonAppendStringValue		(output, "ipv6"		, address.IPv6      , output.size() - offset)); }
	if(address.MAC		.size()	) { llc_necs(llc::jsonAppendStringValue		(output, "mac"		, address.MAC       , output.size() - offset)); }
	if(address.IPv4				) { llc_necs(llc::jsonAppendIPValueAsString	(output, "ipv4"		, address.IPv4      , output.size() - offset)); }
	if(address.Gateway			) { llc_necs(llc::jsonAppendIPValueAsString	(output, "gateway"	, address.Gateway   , output.size() - offset)); }
	if(address.NetMask			) { llc_necs(llc::jsonAppendIPValueAsString	(output, "netmask"	, address.NetMask	, output.size() - offset)); }
	if(address.NetMask && address.IPv4) {
		llc_necs(llc::jsonAppendIPValueAsString(output, "broadcast_ip", address.BroadcastIP(), output.size() - offset));
		llc_necs(llc::jsonAppendIPValueAsString(output, "network_id"  , address.NetworkID(), output.size() - offset));
	}
	if(ownBrackets) {
		llc_necs(output.push_back('}'));
		--offset;
	}
	rtrn output.size() - offset;
}
::llc::err_t			llc::wifiSAPAddressInfo	(const ::llc::SWiFiAddressSAP & address, ::llc::asc_t & output, bool ownBrackets) 		{
	uint32_t					offset 					= output.size();
	if(ownBrackets) {
		llc_necs(output.push_back('{'));
		++offset;
	}
	uint32_t					lenAddress;
	llc_necs(lenAddress = ::llc::wifiAddressInfo(address, output));

	if(ownBrackets) {
		llc_necs(output.push_back('}'));
		--offset;
	}
	rtrn output.size() - offset;
}
::llc::err_t			llc::wifiSTAAddressInfo	(const ::llc::SWiFiAddressSTA & address, ::llc::asc_t & output, bool ownBrackets) 		{
	uint32_t					offset 					= output.size();
	if(ownBrackets) {
		llc_necs(output.push_back('{'));
		++offset;
	}
	uint32_t					lenAddress;
	llc_necs(lenAddress = ::llc::wifiAddressInfo(address, output));
	llc_necs(llc::jsonOpenKeyForList(output, "dns", lenAddress)); {
		if(address.DNS[0])
			llc_necs(::llc::appendQuoted(output, address.DNS[0]));
		if(address.DNS[1])
			llc_necs(::llc::appendBracedPrefixed(output, address.DNS[1], address.DNS[0]));
	} llc_necs(output.push_back(']'));

	if(ownBrackets) {
		llc_necs(output.push_back('}'));
		--offset;
	}
	rtrn output.size() - offset;
}
::llc::err_t			llc::wifiAllAddressInfo	(const ::llc::SWiFiState & state, ::llc::asc_t & output, bool ownBrackets) 		{
	uint32_t					offset 					= output.size();
	if(ownBrackets) {
		llc_necs(output.push_back('{'));
		++offset;
	}
	llc_necs(output.append_string("\"sta\":")); llc_necs(::llc::wifiSTAAddressInfo(state.STA.Address, output, true));
	llc_necs(output.append_string(",\"sap\":")); llc_necs(::llc::wifiSAPAddressInfo(state.SAP.Address, output, true));

	if(ownBrackets) {
		llc_necs(output.push_back('}'));
		--offset;
	}
	rtrn output.size() - offset;
}
sttc ::llc::err_t	wifiLogSTAAddress	(const ::llc::SWiFiAddressSTA & address)	{
	::wifiLogAddress(address);
	info_printf(
		"\nDNS[0]     : '%s'."
		"\nDNS[1]     : '%s'."
		, address.DNS[0].toString<16>().begin()
		, address.DNS[1].toString<16>().begin()
		);
	rtrn 0;
}
::llc::err_t		llc::wifiLogSAP			(const ::llc::SWiFiStateSAP & state, u2_t clients)				{
	::wifiLogAddress(state.Address);
	info_printf("Soft AP clients: %i.", clients);
	rtrn 0;
}
::llc::err_t		llc::wifiLogSTA			(const ::llc::SWiFiStateSTA & state, WLAN_STATUS wifiStatus)	{
	info_printf("STA Status: %i - '%s'.", wifiStatus, get_value_namep(wifiStatus));
	::wifiLogSTAAddress(state.Address);
	if_true_if(wifiStatus != WLAN_STATUS_Connected, "WiFi STA not connected(%s). Reconnection in %f seconds.", get_value_namep(wifiStatus), state.NextTime.Reconnect - .001f * millis());
	rtrn wifiStatus;
}
::llc::err_t		llc::wifiSAPEnable		(::llc::WiFiClass & device, ::llc::SWiFiMode & activeMode)	{ rees_if(activeMode.DisableSAP); rwws_if(0 != (device.getMode() & WIFI_MODE_AP)); rees_if(false == device.enableAP(true)); rtrn 0; }
::llc::err_t		llc::wifiSAPDisable		(::llc::WiFiClass & device, ::llc::SWiFiMode & activeMode)	{ rees_if(activeMode.DisableSAP); rees_if(0 == (device.getMode() & WIFI_MODE_AP)); rees_if(false == device.enableAP(false)); rtrn 0; }
::llc::err_t		llc::wifiUpdate			(::llc::WiFiClass & device, ::llc::SWiFiState & state) {
	if_if(state.Mode.DisableSAP, "%s", "Soft AP disabled.")
	else wf_if(0 == (device.getMode() & WIFI_MODE_AP), "(%u='%s') not in Soft AP mode.", device.getMode(), ::llc::get_value_namep(llc::WIFI_MODE_AP))
	else {
		es_if_failed(::llc::wifiSAPAddress(device, state.SAP.Address))
		else
			es_if_failed(::llc::wifiLogSAP(state.SAP, device.softAPgetStationNum()));
	}
	if_if(state.Mode.DisableSTA, "%s", "WiFi STA disabled.")
	else wf_if(0 == (device.getMode() & WIFI_MODE_STA), "(%u='%s') not in STA mode.", device.getMode(), ::llc::get_value_namep(llc::WIFI_MODE_STA))
	else {
		es_if_failed(::llc::wifiSTAAddress(device, state.STA.Address))
		else
			es_if_failed(::llc::wifiLogSTA(state.STA, (WLAN_STATUS)device.status()));
	}
	rees_if_failed(::llc::wifiSTAUpdate(device, state));
	rtrn 0;
}
//stin	::llc::err_t	splitIPv4	(::llc::vcs ipv4, ::llc::avcc & outputNumbers, const char separator = '.')				{ rtrn ::llc::split(ipv4, separator, outputNumbers); }
//stin	::llc::err_t	joinIPv4	(::llc::asc_t & ipv4, const ::llc::vcvcc & inputNumbers, const char separator = '.')	{ rtrn ::llc::join(ipv4, separator, inputNumbers); }
//sttc	::llc::err_t	ipNodeValues(const ::llc::SIPv4Node & nodeAddress) {
// 	struct NamedAddress : ::llc::SIPv4 { const char * Name; };
// 	const NamedAddress addresses[] = {{{nodeAddress.IPv4}, "ipv4"}, {{nodeAddress.NetMask}, "netmask"}, {{nodeAddress.Gateway}, "gateway"}};
// 	for(const NamedAddress & address : addresses) {
// 	    for(const int i : {0, 1, 2, 3})
// 	        address[i];
// 	}
// 	rtrn 0;
// }
