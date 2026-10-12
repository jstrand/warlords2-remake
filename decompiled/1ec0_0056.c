
undefined2 __cdecl16far FUN_1ec0_0056(void)

{
  code *pcVar1;
  undefined2 in_CX;
  undefined2 extraout_DX;
  
  pcVar1 = (code *)swi(0x33);
  (*pcVar1)();
  DAT_4125_3564 = in_CX;
  DAT_4125_3566 = extraout_DX;
  return 0;
}


