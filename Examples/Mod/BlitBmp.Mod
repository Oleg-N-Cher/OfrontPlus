MODULE BlitBmp;
IMPORT G := Graph, Out;
VAR s, b: G.Bitmap;
BEGIN
  s := G.Init();
  IF s = NIL THEN Out.String("Couldn't init graphic"); Out.Ln; HALT(1) END;
  b := G.LoadBitmap('Rocket.png');
  G.BlitWhole(b, s, 100, 60);
  G.Flip; G.Pause; G.Close
END BlitBmp.
