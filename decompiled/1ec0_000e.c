
undefined2 __cdecl16far FUN_1ec0_000e(void)

{
  code *pcVar1;
  int in_CX;
  undefined2 extraout_DX;
  undefined2 in_BX;
  
  pcVar1 = (code *)swi(0x33);
  (*pcVar1)();
  DAT_4125_3560 = in_CX >> ((byte)DAT_451b_2a56 & 0x1f);
  DAT_4125_3562 = extraout_DX;
  return in_BX;
}


