#include "llc_error.h"

#ifndef NLC_OV2640_H
#define NLC_OV2640_H

namespace nlc
{
	LLC_USING_TYPEINT();
	using llc::err_t;
	struct SCameraPinout {
		s0_t			PWDN 			=  32;
		s0_t			RESET			=  -1;
		s0_t			XCLK 			=   0;
		s0_t			SIOD 			=  26;
		s0_t			SIOC 			=  27;
		s0_t			Y9   			=  35;
		s0_t			Y8   			=  34;
		s0_t			Y7   			=  39;
		s0_t			Y6   			=  36;
		s0_t			Y5   			=  21;
		s0_t			Y4   			=  19;
		s0_t			Y3   			=  18;
		s0_t			Y2   			=   5;
		s0_t			VSYNC			=  25;
		s0_t			HREF 			=  23;
		s0_t			PCLK 			=  22;
	};

	err_t 			cameraSetup		(SCameraPinout & pinoutCamera);
	err_t 			cameraShutdown	(SCameraPinout & pinoutCamera);
} // namespace

#endif // NLC_OV2640_H
