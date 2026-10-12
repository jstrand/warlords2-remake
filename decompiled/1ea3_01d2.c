
void __cdecl16far FUN_1ea3_01d2(void)

{
  code *pcVar1;
  
  DAT_4125_338e = 0;
  DAT_4125_3390 = 0;
  pcVar1 = (code *)swi(0x33);
  (*pcVar1)();
  return;
}


