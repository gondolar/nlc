#include "nlc_app.h"

#ifdef LLC_ESP8266
#	include <user_interface.h>
#else
#	include <esp_system.h>
#	include <esp_sleep.h>
#endif

#include <cmath>
#include <LittleFS.h>


#ifndef LLC_ESP8266
RTC_DATA_ATTR	int		bootCount       = 0;
#else // LLC_ESP8266
sttc	int		RTC_BOOTCOUNT_ADDR 	= 64;  // Just pick an offset; 64 is safe
sttc	int		bootCount 			= 0;
sttc	void	loadBootCount	()	{ system_rtc_mem_read(RTC_BOOTCOUNT_ADDR, &bootCount, sizeof(bootCount)); }
sttc	void	saveBootCount	()	{ system_rtc_mem_write(RTC_BOOTCOUNT_ADDR, &bootCount, sizeof(bootCount)); }
#endif // LLC_ESP8266

HardwareSerial	& LLCLogStream	= Serial;
sttc	::llc::err_t	serial_write	(const char * text, uint32_t textLen)	{ return LLCLogStream.write(text, textLen); }
sttc	::llc::err_t	serial_print	(const char * text)						{ return LLCLogStream.print(text); }

::llc::err_t	llc::evalResetCause		(SNLCApp & appInstance, ::llc::ESP_RESET & resetCause, ::llc::ESP_AWAKE & awakeCause, uint64_t & wakeupPins) {
#ifdef LLC_ESP8266
	loadBootCount();
#endif // LLC_ESP8266
	++bootCount;
#ifdef LLC_ESP8266
	saveBootCount();
#endif // LLC_ESP8266

	stxp	uint32_t		BAUD_MONITOR		= 115200;
	LLCLogStream.begin(BAUD_MONITOR); // Baud rate for serial communication
	llc::setupLogCallbacks(serial_print, serial_write);

	LLC_PLATFORM_CRT_CHECK_MEMORY();

#ifdef LLC_ESP8266
	wifi_get_macaddr(STATION_IF, (uint8_t*)&appInstance.MACAddressEFuse);
	wifi_get_macaddr(SOFTAP_IF, (uint8_t*)&appInstance.MACAddressBase);
#else // !LLC_ESP8266
	esp_efuse_mac_get_default	((uint8_t*)&appInstance.MACAddressEFuse);
	esp_base_mac_addr_get		((uint8_t*)&appInstance.MACAddressBase);
#endif // LLC_ESP8266

	info_printf("CPU frequency:%u.",
#ifdef LLC_ESP8266
		system_get_cpu_freq()
#else // !LLC_ESP8266
		getCpuFrequencyMhz()
#endif // LLC_ESP8266
	);

	info_printf("Boot number: %i.", bootCount);
	info_printf("MAC Address (EFuse/STA):%llx(%llu).", appInstance.MACAddressEFuse, appInstance.MACAddressEFuse);
	info_printf("MAC Address (Base/SAP) :%llx(%llu).", appInstance.MACAddressBase, appInstance.MACAddressBase);

#ifdef LLC_ESP8266
	resetCause = (::llc::ESP_RESET)system_get_rst_info()->reason;
	awakeCause = (::llc::ESP_RESET_DEEPSLEEP == resetCause) ? ::llc::ESP_AWAKE_TIMER : ::llc::ESP_AWAKE_UNDEFINED;
#else // !LLC_ESP8266
	llc_enum_valued_info(resetCause = (::llc::ESP_RESET)esp_reset_reason());
	if((awakeCause = (::llc::ESP_AWAKE)esp_sleep_get_wakeup_cause())) {
		llc_enum_valued_info(awakeCause);
		wakeupPins	= esp_sleep_get_ext1_wakeup_status();// Determine which GPIO caused the wake-up
		info_printf("Wake up caused because of GPIO %u (0x%llX).", log(wakeupPins)/log(2), wakeupPins);
	}
#endif // LLC_ESP8266

	bool			fs_mounted;
	if_zero_e((fs_mounted = LittleFS.begin()));
	return bootCount; // Increment boot number on every reboot
}


::llc::err_t	llc::setupNetwork		(llc::SNLCApp & app, llc::vcs filenameWiFi, llc::vcs filenameHTTP, llc::vcs pathConfig) {
 	LLC_PLATFORM_CRT_CHECK_MEMORY();
	::llc::WiFiClass 			& wifiDevice 		= app.WiFi.WiFi;
	::llc::SWiFiState 			& wifiState 		= app.WiFi.Setup;
	::llc::SHTTPEventServer 	& eventServer 		= app.EventServer;
	::llc::error_t				error				= 0;
	const int64_t				timeZone			= -180;
	const ::llc::vcs			addressOfNTP		= {};
	{
		info_printf("Initializing '%s'.", "WiFi");
		llc::asc_t 	filepath;// = app.Folders.Partition_Base;
		llc::pathNameCompose(pathConfig, filenameWiFi, filepath);
		llc::append_strings(filepath, '.', app.Extensions.Setup);
		filenameWiFi = llc::label({filepath});
		::llc::au0_t	configWiFi;
		es_if_failed(::llc::fileToMemory(filenameWiFi, configWiFi));
		info_printf("WiFi configuration filename: \"%s\".", filenameWiFi.begin());
		info_printf("Current configuration:\n\"%s\".", configWiFi.begin());
		es_if_failed(error = ::llc::wifiSetup(wifiDevice, wifiState, configWiFi.size() ? configWiFi.cc() : ::llc::vcsc_t{::llc::WIFI_CONFIG}));
	}
	if(false == (wifiState.Mode.DisableSAP && wifiState.Mode.DisableSTA)) {
		configTime(timeZone * 60, 0, addressOfNTP, "south-america.pool.ntp.org");

		info_printf("Initializing '%s'.", "HTTP Event Server");
		llc::asc_t 	filepath;// = app.Folders.Partition_Base;
		llc::pathNameCompose(pathConfig, filenameHTTP, filepath);
		llc::append_strings(filepath, '.', app.Extensions.Setup);
		filenameHTTP = llc::label({filepath});
		info_printf("'%s'", filenameHTTP.begin());
		::llc::au0_t	configHTTP;
		es_if_failed(::llc::fileToMemory(filenameHTTP, configHTTP));
		rees_if_failed(eventServer.Initialize(configHTTP.size() ? configHTTP.cc() : ::llc::vcsc_t{::llc::HTTP_CONFIG}));

	}
	////ArduinoOTA.begin();
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	return 0;
}
