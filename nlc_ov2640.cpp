#include "nlc_ov2640.h"
#include "llc_micro.h"

#ifdef LLC_ESP32
#	include <esp_camera.h>
#	include <img_converters.h>
#	include <fb_gfx.h>

using namespace llc;

namespace nlc
{
	GDEFINE_ENUM_TYPE(CAMERA_PIXEL_FORMAT, u0_t);
	GDEFINE_ENUM_VALUE(CAMERA_PIXEL_FORMAT, RGB565      , PIXFORMAT_RGB565    ); // 2BPP/RGB565
	GDEFINE_ENUM_VALUE(CAMERA_PIXEL_FORMAT, YUV422      , PIXFORMAT_YUV422    ); // 2BPP/YUV422
	GDEFINE_ENUM_VALUE(CAMERA_PIXEL_FORMAT, YUV420      , PIXFORMAT_YUV420    ); // 1.5BPP/YUV420
	GDEFINE_ENUM_VALUE(CAMERA_PIXEL_FORMAT, GRAYSCALE   , PIXFORMAT_GRAYSCALE ); // 1BPP/GRAYSCALE
	GDEFINE_ENUM_VALUE(CAMERA_PIXEL_FORMAT, JPEG        , PIXFORMAT_JPEG      ); // JPEG/COMPRESSED
	GDEFINE_ENUM_VALUE(CAMERA_PIXEL_FORMAT, RGB888      , PIXFORMAT_RGB888    ); // 3BPP/RGB888
	GDEFINE_ENUM_VALUE(CAMERA_PIXEL_FORMAT, RAW         , PIXFORMAT_RAW       ); // RAW
	GDEFINE_ENUM_VALUE(CAMERA_PIXEL_FORMAT, RGB444      , PIXFORMAT_RGB444    ); // 3BP2P/RGB444
	GDEFINE_ENUM_VALUE(CAMERA_PIXEL_FORMAT, RGB555      , PIXFORMAT_RGB555    ); // 3BP2P/RGB555
} // namespace

llc::err_t nlc::cameraShutdown(nlc::SCameraPinout & pinoutCamera) {
	llc::ESP_ERROR	err;
	if_true_fef(err = (llc::ESP_ERROR)esp_camera_deinit(), "Camera init failed with error 0x%x:'%s'", err, llc::get_value_namep(err));
	return err;
}

llc::err_t nlc::cameraSetup(nlc::SCameraPinout & pinoutCamera) {
	camera_config_t config  = {};
	config.ledc_channel = LEDC_CHANNEL_0;
	config.ledc_timer   = LEDC_TIMER_0;
	config.pin_d0       = pinoutCamera.Y2;
	config.pin_d1       = pinoutCamera.Y3;
	config.pin_d2       = pinoutCamera.Y4;
	config.pin_d3       = pinoutCamera.Y5;
	config.pin_d4       = pinoutCamera.Y6;
	config.pin_d5       = pinoutCamera.Y7;
	config.pin_d6       = pinoutCamera.Y8;
	config.pin_d7       = pinoutCamera.Y9;
	config.pin_xclk     = pinoutCamera.XCLK;
	config.pin_pclk     = pinoutCamera.PCLK;
	config.pin_vsync    = pinoutCamera.VSYNC;
	config.pin_href     = pinoutCamera.HREF;
	config.pin_sccb_sda = pinoutCamera.SIOD;
	config.pin_sccb_scl = pinoutCamera.SIOC;
	config.pin_pwdn     = pinoutCamera.PWDN;
	config.pin_reset   	= pinoutCamera.RESET;
	config.xclk_freq_hz = 20000000;
	config.frame_size  	= FRAMESIZE_SVGA;//FRAMESIZE_UXGA;
	config.pixel_format = PIXFORMAT_JPEG;  // for streaming
	//config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
	config.grab_mode   	= CAMERA_GRAB_WHEN_EMPTY;
	config.fb_location 	= CAMERA_FB_IN_PSRAM;
	config.jpeg_quality = 10;
	config.fb_count   	= psramFound() ? 2 : 1;

	// if PSRAM IC present, init with UXGA resolution and higher JPEG quality for larger pre-allocated frame buffer.
	
	info_printf("config.pixel_format: '%s'.", llc::get_value_namep((CAMERA_PIXEL_FORMAT)config.pixel_format));
	if (config.pixel_format != PIXFORMAT_JPEG) {
		//config.frame_size = FRAMESIZE_240X240;	 // Best option for face detection/recognition
#if CONFIG_IDF_TARGET_ESP32S3
		config.fb_count = 2;
#endif
	}
	else {
		info_printf("psramFound(): %s.", llc::bool2char(psramFound()));
		if (false == psramFound()) { // Limit the frame size when PSRAM is not available
			config.frame_size = FRAMESIZE_SVGA;
			config.fb_location = CAMERA_FB_IN_DRAM;
		}
		else{
			//config.jpeg_quality = 10;
			config.fb_count = 2;
			config.grab_mode = CAMERA_GRAB_LATEST;
		}
	} 
	llc::ESP_ERROR	err;
	if_true_fef(err = (llc::ESP_ERROR)esp_camera_init(&config), "Camera init failed with error 0x%x:'%s'", err, llc::get_value_namep(err));
	return err;
}

#endif // LLC_ESP32