MODULE Sound;
IMPORT
  SYSTEM, SDL2, mix := SDL2mixer;

TYPE
  Rsrc* = mix.PMix_Chunk;

VAR
  initOk-: BOOLEAN;

PROCEDURE Load* (VAR sound: Rsrc; fileName: ARRAY OF CHAR);
BEGIN
  IF initOk THEN
    sound := mix.Mix_LoadWAV(SYSTEM.ADR(fileName));
  ELSE
    sound := NIL;
  END;
END Load;

PROCEDURE Play* (sound: Rsrc);
BEGIN
  IF initOk & (sound # NIL) THEN
    IF mix.Mix_PlayChannel(-1, sound, 0) = 0 THEN END;
  END;
END Play;

PROCEDURE Free* (VAR sound: Rsrc);
BEGIN
  IF initOk & (sound # NIL) THEN mix.Mix_FreeChunk(sound) END;
END Free;

PROCEDURE Quit* ;
BEGIN
  mix.Mix_CloseAudio;
  SDL2.QuitSubSystem({SDL2.initAudio});
END Quit;

BEGIN
  initOk := SDL2.InitSubSystem({SDL2.initAudio}) >= 0;
  IF initOk THEN
    initOk := (0 <=
      mix.Mix_OpenAudio(22050, mix.MIX_DEFAULT_FORMAT-10000H, 2, 4096)
    );
  END;
END Sound.
