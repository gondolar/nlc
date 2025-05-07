#include "nlc_wifi.h"

#include "llc_frameinfo.h"
#include "llc_micro.h"
#include "llc_i2c.h"
#include "llc_spi.h"

#ifndef NLC_APP_H
#define NLC_APP_H

namespace llc
{
    struct SAppFolders {
        llc::vcs				Setup			= "/setup";
        llc::vcs				Front			= "/front";
    };

    struct SAppFilenames {
        llc::vcs				WiFi			= "wifi";
        llc::vcs				HTTP			= "http";
    };

    struct SAppExtensions {
        llc::vcs				Setup			= "json";
    };
	struct SBootStatus {
		::llc::ESP_RESET 		ResetCause	= {};
		::llc::ESP_AWAKE 		AwakeCause	= {};
		u3_t 					WakeupPins	= {};
	};

	using llc::SBusManager, llc::SI2CDevice, llc::SSPIDevice;

    struct SNLCApp : ::llc::SFrameTimer  {
 		u3_t					MACAddressEFuse	= 0LL;
		u3_t					MACAddressBase	= 0LL;
		SBootStatus				BootInfo		= {};

		::llc::vcst_t			PartitionBase	= "/spiffs";
		SAppFolders				Folders			= {};
		SAppFilenames			Filenames		= {};
		SAppExtensions			Extensions		= {};

		SBusManager<SI2CDevice>	I2CManager		= {};
		SBusManager<SSPIDevice>	SPIManager		= {};

		::llc::SWiFi			WiFi			= {};
		stxp	uint32_t		TickWiFi		= 10;
		float					TimeWiFi		= 0;
    };
    ::llc::err_t	evalResetCause		(SNLCApp & appInstance, ::llc::ESP_RESET & resetCause, ::llc::ESP_AWAKE & awakeCause, uint64_t & wakeupPins);
}

#endif // NLC_APP_H
