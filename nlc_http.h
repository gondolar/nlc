#include "llc_queue_event.h"

#include "llc_system_event.h"

#include "llc_json.h"

#ifdef LLC_ESP32

#include <ESPAsyncWebServer.h>
#include <Udp.h>

#ifndef MLC_HTTP_H
#define MLC_HTTP_H

namespace llc
{
#pragma pack(push, 1)
	struct SHTTPConfig {
		uint16_t				HTTPPort		= 80;
		bool					DisableHTTP		: 1;
	};
#pragma pack(pop)

	//struct SHTTPServer {
	//};

	::llc::error_t			httpConfigLoad		(const ::llc::SJSONReader & jsonConfig, int32_t iNode, ::llc::SHTTPConfig & httpConfig);
	::llc::error_t			httpConfigLoad		(::llc::vcs jsonString, ::llc::SHTTPConfig & httpConfig);
	::llc::error_t			httpSetup			(AsyncWebServer & httpServer, const ::llc::SHTTPConfig & httpConfig);

	stxp	char			HTTP_CONFIG	[]		=
		"{\"http_port\""		":80"
		",\"disable_http\"" 	":false"
		"}";
} // namespace 

namespace llc
{
	::llc::error_t			respond				(AsyncWebServerRequest & request, ::llc::vcs httpCode, ::llc::vcs messageString);
	::llc::error_t			respondEvent		(AsyncWebServerRequest & request, ::llc::vcs httpCode, ::llc::vcs mid);
	::llc::error_t			mandatoryKeyLoad	(AsyncWebServerRequest & request, const ::llc::SJSONReader & jsonBody, ::llc::vcs key, ::llc::vcsc_t & outputValue, uint32_t iObject = 0);

	typedef ::llc::qpevt<::llc::SYSTEM_EVENT> TEventQueueAsync;

	struct SEventSystemBox {
		::llc::TEventQueueAsync	Inbox	= {};
		::llc::TEventQueueAsync	Outbox	= {};
	};
	struct SHTTPEventServer {
		::llc::SHTTPConfig						Config		= {};
		AsyncWebServer							Server			{80};
		::llc::qpobj<::llc::SEventSystemBox>	Events;
	
		::llc::error_t 			Initialize		(::llc::vcs configString = ::llc::HTTP_CONFIG); 
		::llc::error_t 			SetupEndpoints	();
	};
} // namespace

#endif // MLC_HTTP_H

#endif // LLC_ESP32
