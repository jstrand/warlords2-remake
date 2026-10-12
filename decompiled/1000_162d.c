
void __cdecl16near __scanrslt(double *param_1,uint param_2)

{
  unkbyte10 in_ST0;
  longdouble in_ST1;
  
  if ((param_2 & 4) == 0) {
    if ((param_2 & 8) == 0) {
      __fpu_limit(DAT_4125_391e,in_ST0,0);
      *(float *)param_1 = (float)in_ST1;
    }
    else {
      *(unkbyte10 *)param_1 = in_ST0;
    }
  }
  else {
    __fpu_limit(DAT_4125_38aa,in_ST0,1);
    *param_1 = (double)in_ST1;
  }
  return;
}


