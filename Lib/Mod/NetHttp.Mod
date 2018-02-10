(*
http://www.cyberforum.ru/blogs/131347/blog1151.html#a_6
 https://curl.haxx.se/libcurl/c/getinmemory.html
https://stackoverflow.com/questions/2329571/c-libcurl-get-output-into-a-string
 *)
MODULE NetHttp;
IMPORT SYSTEM, Types, c := libCurl (*, io := Console*);

CONST
  DEFAULT_BUFFER_SIZE = 32768;

TYPE
  Buffer = POINTER TO ARRAY OF CHAR;
  CString = Types.ADRINT;

TYPE
  Socket* = RECORD
    hCurl: c.PCURL;
    ok-: BOOLEAN;
    err-: ARRAY 128 OF CHAR;
    data*: Buffer;
    datasize-: INTEGER;
  END;

PROCEDURE -AAIncludeCurlh0 '#include "libCurl.h0"';

PROCEDURE Length (s: CString): INTEGER;
VAR
  len: INTEGER; ch: CHAR;
BEGIN
  len := 0;
  SYSTEM.GET(s, ch);
  WHILE ch # 0X DO INC(len); SYSTEM.GET(s + len, ch) END;
  RETURN len
END Length;

PROCEDURE write_callback (
  ptr, size, nmemb, userdata: Types.ADRINT): Types.ADRINT;
TYPE
  Self = POINTER [1] TO Socket;
VAR
  self: Self; cur_size, n: INTEGER; newbuf: Buffer;
BEGIN
  self := SYSTEM.VAL(Self, userdata);
  cur_size := SYSTEM.VAL(INTEGER, size) * SYSTEM.VAL(INTEGER, nmemb);
  IF self.datasize + cur_size >= LEN(self.data^) THEN
    (* Reserve 3/2 for future *)
    NEW(newbuf, (self.datasize + cur_size + 1) * 3 DIV 2);
    (*io.String("Old: "); io.Int(LEN(self.data^), 0);
    io.String(" New: "); io.Int(LEN(newbuf^), 0); io.Ln;*)
    (* Copy data from old buffer to the new buffer *)
    SYSTEM.MOVE(
      SYSTEM.ADR(self.data[0]), SYSTEM.ADR(newbuf[0]), self.datasize);
    (*FOR n := 0 TO self.datasize - 1 DO newbuf^[n] := self.data^[n] END;*)
    self.data := newbuf;
  END;
  (* Copy the received data to the buffer *)
  FOR n := 0 TO cur_size - 1 DO
    SYSTEM.GET(ptr + n, self.data[self.datasize + n]);
  END;
  INC(self.datasize, cur_size);
  self.data[self.datasize] := 0X;
  RETURN cur_size
END write_callback;

PROCEDURE (VAR self: Socket) Get* (url: ARRAY [1] OF CHAR);
VAR
  msglen: INTEGER; ch: CHAR; res: c.CURLcode; syserr: Types.ADRINT;
BEGIN
  self.hCurl := c.curl_easy_init();
  self.ok := (self.hCurl # NIL);
  IF self.ok THEN
    IF c.curl_easy_setopt_A(self.hCurl, c.CURLOPT_URL, SYSTEM.ADR(url)) # c.CURLE_OK
    THEN
      self.ok := FALSE; self.err := "CURLOPT_URL"; RETURN
    END;
    IF c.curl_easy_setopt_I(self.hCurl, c.CURLOPT_FOLLOWLOCATION, 1) # c.CURLE_OK
    THEN
      self.ok := FALSE; self.err := "CURLOPT_FOLLOWLOCATION"; RETURN
    END;
    IF c.curl_easy_setopt_A(self.hCurl, c.CURLOPT_WRITEDATA, SYSTEM.ADR(self)) # c.CURLE_OK
    THEN
      self.ok := FALSE; self.err := "CURLOPT_WRITEDATA"; RETURN
    END;
    IF c.curl_easy_setopt_C(self.hCurl, c.CURLOPT_WRITEFUNCTION, write_callback) # c.CURLE_OK
    THEN
      self.ok := FALSE; self.err := "CURLOPT_WRITEFUNCTION"; RETURN
    END;
    self.datasize := 0;
    IF self.data = NIL THEN NEW(self.data, DEFAULT_BUFFER_SIZE) END;
    res := c.curl_easy_perform(self.hCurl);
    (* Check for errors *)
    IF res # c.CURLE_OK THEN
      self.ok := FALSE;
      syserr := c.curl_easy_strerror(res);
      msglen := Length(syserr);
      IF msglen >= LEN(self.err) THEN msglen := LEN(self.err) - 1 END;
      SYSTEM.MOVE(syserr, SYSTEM.ADR(self.err[0]), msglen);
      self.err[msglen] := 0X;
    END;
    (* always cleanup *)
    c.curl_easy_cleanup(self.hCurl);
  END;
END Get;

END NetHttp.
