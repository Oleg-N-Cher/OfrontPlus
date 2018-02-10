MODULE TestHttp; (*$MAIN*)
IMPORT Http := NetHttp, io := Console;

VAR
  http: Http.Socket;

BEGIN
  NEW(http.data, 2000); (* Reserve the buffer of the required size *)
  http.Get("http://example.com");
  IF http.ok THEN
    io.String("Length = "); io.Int(http.datasize, 0); io.Ln; io.Ln;
    io.String(http.data^);
  ELSE
    io.String("Error: "); io.String(http.err);
  END;
  io.Ln;
END TestHttp.
