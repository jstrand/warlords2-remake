
undefined2 __screen_rowcol(uint param_1)

{
  char cVar1;
  
  cVar1 = (char)((param_1 >> 1) / (uint)DAT_4125_3e7a);
  return CONCAT11(cVar1,(char)(param_1 >> 1) - cVar1 * DAT_4125_3e7a);
}


