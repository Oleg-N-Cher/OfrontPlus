MODULE libCurl [foreign];
IMPORT s := SYSTEM;

CONST
  (*CURLcode *)
  CURLE_OK               * =     0;

  (* CURLoption *)
  CURLOPT_FOLLOWLOCATION * =    52;
  CURLOPT_FILE           * = 10001;
  CURLOPT_URL            * = 10002;
  CURLOPT_WRITEFUNCTION  * = 20011;
  CURLOPT_WRITEDATA      * = CURLOPT_FILE;

TYPE
  PCURL* = POINTER [1] TO ARRAY [1] 1 OF SHORTCHAR; (* A notag pointer *)
  CURLcode   * = INTEGER;
  CURLoption * = INTEGER;

  Callback   * = PROCEDURE (ptr, size, nmemb, userdata: s.ADRINT): s.ADRINT;

PROCEDURE -AAIncludeCurlh0 '#include "libCurl.h0"';

PROCEDURE curl_easy_cleanup* (curl: PCURL);
PROCEDURE curl_easy_init* (): PCURL;
PROCEDURE curl_easy_perform* (curl: PCURL): CURLcode;
PROCEDURE -curl_easy_setopt_I* (
  handle: PCURL; option: CURLoption; parameter: INTEGER): CURLcode
  "curl_easy_setopt(handle, option, parameter)";
PROCEDURE -curl_easy_setopt_A* (
  handle: PCURL; option: CURLoption; parameter: s.ADRINT): CURLcode
  "curl_easy_setopt(handle, option, (CHAR*)(parameter))";
PROCEDURE -curl_easy_setopt_C* (
  handle: PCURL; option: CURLoption; callback: Callback): CURLcode
  "curl_easy_setopt(handle, option, callback)";
PROCEDURE curl_easy_strerror* (code: CURLcode): s.ADRINT;


END libCurl.
