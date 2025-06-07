#include "nlc_http.h"

#include "llc_base64.h"
#include "llc_parse.h"
#include "llc_append_frontend.h"
#include <SPIFFS.h>

stxp	llc::vcs 	MAIN_FRAME_FORMS	[] 	= {LLC_CXS("system"), LLC_CXS("access"), LLC_CXS("server"), LLC_CXS("wifi"), LLC_CXS("bluetooth"), LLC_CXS("datalog"), LLC_CXS("history")};

::llc::err_t		llc::httpConfigLoad		(const ::llc::SJSONReader & jsonConfig, int32_t iNode, ::llc::SHTTPConfig & httpConfig) {
	ws_if_failed(::llc::jsonObjectGetAsInteger(jsonConfig, iNode, "http_port", httpConfig.HTTPPort));
	{ bool disableHTTP = false; ws_if_failed(::llc::jsonObjectGetAsBoolean(jsonConfig, iNode, "disable_http", disableHTTP)); httpConfig.DisableHTTP = disableHTTP; }
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	return 0;
}
::llc::err_t		llc::httpConfigLoad		(::llc::vcs jsonString, ::llc::SHTTPConfig & httpConfig) {
	::llc::SJSONReader	jsonConfig;
	llc_necs(::llc::jsonParse(jsonConfig, jsonString));
	llc_necs(::llc::httpConfigLoad(jsonConfig, 0, httpConfig));
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	return 0;
}
::llc::err_t		llc::httpSetup			(AsyncWebServer & httpServer, const ::llc::SHTTPConfig & httpConfig) {
	if(httpConfig.DisableHTTP)
		httpServer.end();
	else {
		httpServer.~AsyncWebServer();
		new (&httpServer) AsyncWebServer(httpConfig.HTTPPort);
		httpServer.begin();
	}
	LLC_PLATFORM_CRT_CHECK_MEMORY();
	return 0;
}
// Respond with a JSON object with a `code` key containing the httpCode and a `message` key containing the message.
::llc::err_t		llc::respond			(AsyncWebServerRequest & request, ::llc::vcs httpCode, ::llc::vcs messageString) {
	::llc::asc_t			response;
	llc_necs(llc::append_strings(response, "{\"code\":", httpCode, ",\"message\":\"", messageString, "\"}"));
	int32_t					httpCodeInt			= 0;
	llc_necall(::llc::parseIntegerDecimal(httpCode, httpCodeInt), "httpCode: '%s'.", ::llc::toString(httpCode).begin());
	info_printf("HTTP Response: '%s'.", response.begin());
	request.send(httpCodeInt, "application/json", &response[0]);
	return httpCodeInt * ((httpCodeInt >= 100 && httpCodeInt < 300) ? 1 : -1);
}
// Respond with a JSON containing the mid of the message being acknowledged
::llc::err_t		llc::respondEvent		(AsyncWebServerRequest & request, ::llc::vcs httpCode, ::llc::vcs mid) {
	::llc::asc_t			 response;
	llc_necs(llc::append_strings(response, "{\"code\":", httpCode, ",\"mid\":\"", mid, "\"}"));
	int32_t					httpCodeInt			= 0;
	llc_necall(::llc::parseIntegerDecimal(httpCode, httpCodeInt), "httpCode: '%s'.", ::llc::toString(httpCode).begin());
	info_printf("HTTP Response: '%s'.", response.begin());
	request.send(httpCodeInt, "application/json", &response[0]);
	return httpCodeInt * ((httpCodeInt >= 100 && httpCodeInt < 300) ? 1 : -1);
}
// Respond http code 400 with a JSON containing a message saying the key was not found.
::llc::err_t		llc::mandatoryKeyLoad	(AsyncWebServerRequest & request, const ::llc::SJSONReader & jsonBody, ::llc::vcs key, ::llc::vcsc_t & outputValue, uint32_t iObject) {
	::llc::err_t			iValue				= ::llc::jsonObjectGetAsString(jsonBody, iObject, key, outputValue);
	if_failed(iValue) {
		::llc::asc_t			text 				= ::llc::vcsc_t{1, "'"};
		llc_necs(text.append(key));
		llc_necs(text.append_string("' not found"));
		return respond(request, "400", text.cc());
	}
	return 0;
}
::llc::err_t 		llc::SHTTPEventServer::Initialize	(::llc::vcs configString) {
	if(configString.size()) {
		ws_if_failed(::llc::httpConfigLoad(configString, Config))
		else {
			info_printf("HTTP server configuration loaded from json:\n%s\n", configString.begin());
		}
	}
	llc_necs(::llc::httpSetup(Server, Config));
	info_printf("HTTP server started.");
	{
		::std::lock_guard 	lockEvents(Events.Lock);
		Events.Queue.clear();
		llc_necs(Events.Queue.push_back({}));
	}
	info_printf("Event queue initialized.");
	return SetupEndpoints();
}
struct SCSSNode {
	::llc::vcs BorderStyle		= LLC_CXS("border-style:solid;");
	::llc::vcs BorderColor		= LLC_CXS("border-color:red;");
	::llc::vcs BorderWidth		= LLC_CXS("border-width:1px;");
	::llc::vcs BackgroundColor	= LLC_CXS("background-color:#1e1e1e;");
	::llc::vcs Spacing			= LLC_CXS("padding:0px;margin:0px;");
	::llc::vcs Font				= LLC_CXS("font-family:Fira Mono,DejaVu Sans Mono,Menlo,Consolas,Liberation Mono,Monaco,Lucida Console,monospace;");
};
struct SCSSNodes {
	SCSSNode		html   = {LLC_CXS("border-style:dotted;" ),LLC_CXS("border-color:red;"     )};
	SCSSNode		body   = {LLC_CXS("border-style:solid;"  ),LLC_CXS("border-color:blue;"    )};
	SCSSNode		table  = {LLC_CXS("border-style:groove;" ),LLC_CXS("border-color:cyan;"    )};
	SCSSNode		thead  = {LLC_CXS("border-style:ridge;"  ),LLC_CXS("border-color:magenta;" )};
	SCSSNode		tbody  = {LLC_CXS("border-style:inset;"  ),LLC_CXS("border-color:yellow;"  )};
	SCSSNode		tfoot  = {LLC_CXS("border-style:inset;"  ),LLC_CXS("border-color:pink;"    )};
	SCSSNode		th     = {LLC_CXS("border-style:solid;"  ),LLC_CXS("border-color:olive;"   )};
	SCSSNode		tr     = {LLC_CXS("border-style:solid;"  ),LLC_CXS("border-color:mauve;"   )};
	SCSSNode		td     = {LLC_CXS("border-style:double;" ),LLC_CXS("border-color:purple;"  )};
	SCSSNode		form   = {LLC_CXS("border-style:outset;" ),LLC_CXS("border-color:grey;"    )};
	SCSSNode		iframe = {LLC_CXS("border-style:dotted;" ),LLC_CXS("border-color:black;"   )};
};

stxp	SCSSNodes	CSS_DEFAULTS		= {};
static ::llc::err_t	appendJS			(::llc::asc_t & output) { return llc::append_strings(output, "let a = 0;"); }
static ::llc::err_t	appendCSS			(::llc::asc_t & output) {
	llc::appendCSSTag(output, LLC_CXS("html"	), CSS_DEFAULTS.html   .Spacing, CSS_DEFAULTS.html   .BorderStyle, CSS_DEFAULTS.html   .BackgroundColor, CSS_DEFAULTS.html   .BorderColor, [](auto & output0) { return llc::appendCSSSizeWithBorderWidth(output0, {100, 100}, 1); }, CSS_DEFAULTS.html   .Font);
	llc::appendCSSTag(output, LLC_CXS("body"	), CSS_DEFAULTS.body   .Spacing, CSS_DEFAULTS.body   .BorderStyle, CSS_DEFAULTS.body   .BackgroundColor, CSS_DEFAULTS.body   .BorderColor, [](auto & output0) { return llc::appendCSSSizeWithBorderWidth(output0, {100, 100}, 1); }, CSS_DEFAULTS.body   .Font);
	llc::appendCSSTag(output, LLC_CXS("table"	), CSS_DEFAULTS.table  .Spacing, CSS_DEFAULTS.table  .BorderStyle, CSS_DEFAULTS.table  .BackgroundColor, CSS_DEFAULTS.table  .BorderColor, [](auto & output0) { return llc::appendCSSSizeWithBorderWidth(output0, {100, 100}, 1); }, CSS_DEFAULTS.table  .Font);
	llc::appendCSSTag(output, LLC_CXS("thead"	), CSS_DEFAULTS.thead  .Spacing, CSS_DEFAULTS.thead  .BorderStyle, CSS_DEFAULTS.thead  .BackgroundColor, CSS_DEFAULTS.thead  .BorderColor, [](auto & output0) { return llc::appendCSSSizeWithBorderWidth(output0, {100, 100}, 1); }, CSS_DEFAULTS.thead  .Font);
	llc::appendCSSTag(output, LLC_CXS("tbody"	), CSS_DEFAULTS.tbody  .Spacing, CSS_DEFAULTS.tbody  .BorderStyle, CSS_DEFAULTS.tbody  .BackgroundColor, CSS_DEFAULTS.tbody  .BorderColor, [](auto & output0) { return llc::appendCSSSizeWithBorderWidth(output0, {100, 100}, 1); }, CSS_DEFAULTS.tbody  .Font);
	llc::appendCSSTag(output, LLC_CXS("tfoot"	), CSS_DEFAULTS.tfoot  .Spacing, CSS_DEFAULTS.tfoot  .BorderStyle, CSS_DEFAULTS.tfoot  .BackgroundColor, CSS_DEFAULTS.tfoot  .BorderColor, [](auto & output0) { return llc::appendCSSSizeWithBorderWidth(output0, {100, 100}, 1); }, CSS_DEFAULTS.tfoot  .Font);
	llc::appendCSSTag(output, LLC_CXS("th"		), CSS_DEFAULTS.th     .Spacing, CSS_DEFAULTS.th     .BorderStyle, CSS_DEFAULTS.th     .BackgroundColor, CSS_DEFAULTS.th     .BorderColor, [](auto & output0) { return llc::appendCSSSizeWithBorderWidth(output0, {100, 100}, 1); }, CSS_DEFAULTS.th     .Font);
	llc::appendCSSTag(output, LLC_CXS("tr"		), CSS_DEFAULTS.tr     .Spacing, CSS_DEFAULTS.tr     .BorderStyle, CSS_DEFAULTS.tr     .BackgroundColor, CSS_DEFAULTS.tr     .BorderColor, [](auto & output0) { return llc::appendCSSSizeWithBorderWidth(output0, {100, 100}, 1); }, CSS_DEFAULTS.tr     .Font);
	llc::appendCSSTag(output, LLC_CXS("td"		), CSS_DEFAULTS.td     .Spacing, CSS_DEFAULTS.td     .BorderStyle, CSS_DEFAULTS.td     .BackgroundColor, CSS_DEFAULTS.td     .BorderColor, [](auto & output0) { return llc::appendCSSSizeWithBorderWidth(output0, {100, 100}, 1); }, CSS_DEFAULTS.td     .Font);
	llc::appendCSSTag(output, LLC_CXS("form"	), CSS_DEFAULTS.form   .Spacing, CSS_DEFAULTS.form   .BorderStyle, CSS_DEFAULTS.form   .BackgroundColor, CSS_DEFAULTS.form   .BorderColor, [](auto & output0) { return llc::appendCSSSizeWithBorderWidth(output0, {100, 100}, 1); }, CSS_DEFAULTS.form   .Font);
	llc::appendCSSTag(output, LLC_CXS("iframe"	), CSS_DEFAULTS.iframe .Spacing, CSS_DEFAULTS.iframe .BorderStyle, CSS_DEFAULTS.iframe .BackgroundColor, CSS_DEFAULTS.iframe .BorderColor, [](auto & output0) { return llc::appendCSSSizeWithBorderWidth(output0, {100, 100}, 1); }, CSS_DEFAULTS.iframe .Font);
	return 0;
}

stxp ::llc::vcs FRONT_FILES_WEBGL_JS		[]	= {LLC_CXS("front/gl-matrix-min"), LLC_CXS("front/nlc_scene")};
stxp ::llc::vcs FRONT_FILES_WEBGL_CSS		[]	= {LLC_CXS("front/nlc_default")};
stxp ::llc::vcs FRONT_FILES_FRAME_JS		[]	= {LLC_CXS("front/nlc_time"), LLC_CXS("front/nlc_menuitem"), LLC_CXS("front/nlc_main_frame")};
stxp ::llc::vcs FRONT_FILES_FRAME_CSS		[]	= {LLC_CXS("front/nlc_default")};
stxp ::llc::vcs FRONT_FILES_JS				[]	= {LLC_CXS("front/nlc_time"), LLC_CXS("front/nlc_menuitem"), LLC_CXS("front/nlc_main_frame")};
stxp ::llc::vcs FRONT_FILES_CSS				[]	= {LLC_CXS("front/nlc_default")};
stxp ::llc::vcs FRONT_BODY_WEBGL				= LLC_CXS(
"\n<table style=\"border-spacing: 0px;\">"
"\n	<tr style=\"height: 100%; vertical-align: center; text-align: center;\" >"
"\n		<td style=\"width:100%; border-width: 0px; border-style: none; padding: 0px; margin: 0px;\" >"
"\n			<canvas id=\"webgl-canvas\"></canvas>"
"\n			<img id=\"no-webgl-canvas\" src=\"../front/noise_canvas.png\" />"
"\n		</td>"
"\n	</tr>"
"\n</table>"
);
stxp ::llc::vcs FRONT_BODY_FRAME				= LLC_CXS(
"\n<table>"
"\n	<tr style=\"height:114px; vertical-align: center; text-align: center;\">"
"\n		<td style=\"width:10%;\"></td>"
"\n		<td style=\"text-align: center;\"></td>"
"\n		<td style=\"width:10%;\"><iframe name=\"iframe_clock\" id=\"iframe_clock\" src=\"/clock\"></iframe></td>"
"\n	</tr>"
"\n	<tr style=\"height: 100%; vertical-align: top;\">"
"\n		<td><table id=\"menu_table\" style=\"height:10%;\"></table></td>"
"\n		<td><iframe name=\"iframe_main\" id=\"iframe_main\" src=\"/frame/scene\"></iframe></td>"
"\n		<td></td>"
"\n	</tr>"
"\n	<tr><td></td><td></td><td></td></tr>"
"\n</table>"
);
stin llc::err_t	appendBodyMain		(::llc::asc_t & output) { return llc::append_strings(output, "\nHello mc\n"); }
stin llc::err_t	appendHtmlMain		(::llc::asc_t & output) { return llc::appendHtmlPage(output, "Hello MC!", ::FRONT_FILES_CSS, ::FRONT_FILES_JS, ::appendBodyMain); }
stin llc::err_t	appendBodyScene		(::llc::asc_t & output) { return llc::append_strings(output, FRONT_BODY_WEBGL); }
stin llc::err_t	appendHtmlScene		(::llc::asc_t & output) { return llc::appendHtmlPage(output, "WebGL Scene", ::FRONT_FILES_WEBGL_CSS, ::FRONT_FILES_WEBGL_JS, ::appendBodyScene, "initWebGLCanvas('webgl-canvas');"); }
stin llc::err_t	appendBodyMainframe	(::llc::asc_t & output) { return llc::appendXmlTag(output, "iframe", "src=\"/frame/scene\""); }
stin llc::err_t	appendHtmlMainframe	(::llc::asc_t & output) { return llc::appendHtmlPage(output, "Hello MC!", ::FRONT_FILES_FRAME_CSS, ::FRONT_FILES_FRAME_JS, ::appendBodyMainframe, "initPage();"); }

stxp	char		INTERNAL_SERVER_ERROR[]	= "internal server error";

static	llc::err_t 	contentTypeResolve	(llc::vcs filename, llc::vcs & type, AsyncWebServerRequest & request) {
    if (request.hasArg("download"))
		return (type = "application/octet-stream").size();
	static const
	struct ContentType {
		llc::vcs Extension;
		llc::vcs ContentType;
	} CONTENT_TYPES[] =
		{ {".htm"						, "text/html"}
		, {".html"						, "text/html"}
		, {".css"						, "text/css"}
		, {".csv"						, "text/csv"}
		, {".js"						, "text/javascript"}
		, {".json"						, "application/json"}
		, {".svg"						, "image/svg+xml"}
		, {".png"						, "image/png"}
		, {".gif"						, "image/gif"}
		, {".jpg"						, "image/jpeg"}
		, {".ico"						, "image/x-icon"}
		, {".xml"						, "text/xml"}
		, {".pdf"						, "application/x-pdf"}
		, {".zip"						, "application/x-zip"}
		, {".gz"						, "application/x-gzip"}
		};

	for(const ContentType & ct : CONTENT_TYPES) {
		error_t found;
		if_failed(found = llc::find_string(filename, ct.Extension))
			continue;
		return (type = ct.ContentType).size();
	}
	return (type = "application/octet-stream").size();
}
static	llc::err_t	serveFile 			(::llc::vcs path, AsyncWebServerRequest & request) {
	info_printf("Serving path: '%s'.", path.begin());
	llc::vcs			contentType;
	llc_necs(contentTypeResolve(path, contentType, request));
	if (SPIFFS.exists(path)) {
		request.send(request.beginResponse(SPIFFS, path.begin(), contentType.begin()));
		return 200;
	}
	error_printf("'%s' not found.", path.begin());
	request.send(404);
	return 404;
}
static	llc::err_t	respondString		(AsyncWebServerRequest & request, ::llc::vcs mimeType, const ::llc::FAppend & funcAppend) {
	::llc::asc_t		response			= {};
	if_not_fail(funcAppend ? funcAppend(response) : 0)
		request.send(200, mimeType.begin(), response.begin());
	else {
		llc_necs(::llc::respond(request, "500", INTERNAL_SERVER_ERROR));
		return -1;
	}
	return 0;
}
sttc	llc::err_t	sf					(AsyncWebServerRequest * request, Stream & mySource) {
	AsyncWebServerResponse & response = *request->beginChunkedResponse("text/plain", [&mySource](uint8_t * buffer, size_t maxLen, size_t index) -> size_t {
		// Write up to "maxLen" bytes into "buffer" and return the amount written. index equals the amount of bytes that have been already sent.
		// You will be asked for more data until 0 is returned. Keep in mind that you can not delay or yield waiting for more data!
		return mySource.available() ? mySource.available() - mySource.readBytes(buffer, ::llc::min((uint32_t)mySource.available(), (uint32_t)maxLen)) : 0;
	});
	response.addHeader("Server","ESP Async Web Server");
	request->send(&response);
	return 0;
}

// Returns true if the browser sending the request is from a mobile device
sttc	llc::error_t	userAgentIsMobile	(llc::vcst_c userAgent) {
	stxp	llc::vcst_c		tokens	[]		= {LLC_CXS("Mobile"), LLC_CXS("Android"), LLC_CXS("iPhone"), LLC_CXS("iPad")};
	for(auto token : tokens)	// Check for keywords that indicate a mobile device.
		if_true_vi(1, llc::find_sequence_pod(userAgent, token));

	return 0;
}

::llc::err_t 		llc::SHTTPEventServer::SetupEndpoints	() {
	Server.onNotFound([](AsyncWebServerRequest * request) { 
		if(llc::failed(::serveFile({request->url().begin(), request->url().length()}, *request))) {
			error_printf("%s", request->url().begin());
			res_if_failed(::llc::respond(*request, "404", "not found"));
		}
	});
	Server.on("/front/main.js"		, HTTP_GET, [](AsyncWebServerRequest *request) { es_if_failed(respondString(*request, "text/javascript"	, appendJS)); });
	Server.on("/front/default.css"	, HTTP_GET, [](AsyncWebServerRequest *request) { es_if_failed(respondString(*request, "text/css"		, appendCSS)); });
	Server.on("/frame/main"			, HTTP_GET, [](AsyncWebServerRequest *request) { es_if_failed(respondString(*request, "text/html"		, appendHtmlMain)); });
	Server.on("/frame/scene"		, HTTP_GET, [](AsyncWebServerRequest *request) { es_if_failed(respondString(*request, "text/html"		, appendHtmlScene)); });
	Server.on("/"					, HTTP_GET, [](AsyncWebServerRequest *request) { es_if_failed(respondString(*request, "text/html"		, appendHtmlMainframe)); });
	Server.on("/llc"				, HTTP_POST
	, [](AsyncWebServerRequest * request)																					{ es_if_failed(::llc::respond(*request, "403", "")); }
	, [](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t * data, size_t len, bool final)	{ es_if_failed(::llc::respond(*request, "403", "")); }
	, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)							{
		const ::llc::vcsc_t	body		= {(const char*)data, len};
		info_printf("Received POST request. body:\n%s", ::llc::toString(body).begin());

		::llc::SJSONReader	jsonBody;
		if_failed(::llc::jsonParse(jsonBody, body)) {
			res_if_failed(::llc::respond(*request, "400", "failed to parse body as json"));
			return;
		}
		::llc::vcsc_t			mid			= ::llc::vcs{}; res_if_failed(::llc::mandatoryKeyLoad(*request, jsonBody, "mid", mid));
		::llc::vcsc_t			b64 		= ::llc::vcs{}; res_if_failed(::llc::mandatoryKeyLoad(*request, jsonBody, "b64", b64));
		::llc::au0_t			payload		= {};
		if_failed(::llc::base64Decode(b64, payload)) {
			stxp	char		strError[]	= "failed to decode base64 string";
			error_printf("%s", strError);
			res_if_failed(::llc::respond(*request, "400", strError));
			return;
		}
		llc::vcu0_t			vpayload	= payload;
		::llc::PEvent<::llc::SYSTEM_EVENT>	receivedEvent;
		receivedEvent->Load(vpayload);
		{
			::std::lock_guard 	lockEvents	(Events.Lock);
			if(0 == Events.Queue.size()) {
				i_if(0 <= Events.Queue.push_back({}))
				else {
					error_printf("Events.Queue.push_back({}): '%s'.", INTERNAL_SERVER_ERROR);
					res_if_failed(::llc::respond(*request, "500", INTERNAL_SERVER_ERROR));
					return;
				}
			}

			if_if(0 <= Events.Queue[0]->Inbox.Post(receivedEvent), "Received event [%u](%s).", receivedEvent->Type, ::llc::get_value_namep(receivedEvent->Type))
			else {
				error_printf("Events.Queue[0]->Inbox.Post(loadedEvent): '%s'.", INTERNAL_SERVER_ERROR);
				es_if_failed(::llc::respond(*request, "500", INTERNAL_SERVER_ERROR));
				return;
			}
		}
		re_if_failed(::llc::respondEvent(*request, "200", mid), "mid: '%s'.", mid.begin());
	});
	return 0;
}