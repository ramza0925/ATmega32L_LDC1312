#ifndef _MENU_H_
#define _MENU_H_

#include <stdio.h>
#include <stdint.h>

#include "..\GLCD\SPLC501C.h"
#include "..\GLCD\graphic.h"
#include "..\LDC\LDC1612.h"

#include "..\Variable.h"

enum UNIT{
  MM,
  MIL,
  PERCENT,
  MA,
};

void Dsp_Distance();
void Dsp_Offset();
void Dsp_Contrast();
void Dsp_Setting();
void Dsp_Debugging();
void Dsp_4mA_Setting();
void Dsp_20mA_Setting();
    
#endif