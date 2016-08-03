/* Ofront+ 1.0 -xtspkae */

#ifndef Oberon__h
#define Oberon__h

#include "SYSTEM.h"
#include "Texts.h"

typedef
	struct Oberon_ParRec *Oberon_ParList;

typedef
	struct Oberon_ParRec {
		Texts_Text text;
		INTEGER pos;
	} Oberon_ParRec;


import Texts_Text Oberon_Log;
import Oberon_ParList Oberon_Par;
import CHAR Oberon_OptionChar;

import LONGINT *Oberon_ParRec__typ;

import void Oberon_GetClock (INTEGER *t, INTEGER *d);
import void Oberon_GetSelection (Texts_Text *text, INTEGER *beg, INTEGER *end, INTEGER *time);
import INTEGER Oberon_Time (void);
import void *Oberon__init(void);


#endif
