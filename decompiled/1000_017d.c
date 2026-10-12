
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16near __save_vectors(void)

{
  code *pcVar1;
  undefined2 in_BX;
  undefined2 unaff_ES;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  unique0x10000025 = in_BX;
  _DAT_4125_005d = unaff_ES;
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  DAT_4125_005f = in_BX;
  DAT_4125_0061 = unaff_ES;
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  DAT_4125_0063 = in_BX;
  DAT_4125_0065 = unaff_ES;
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  DAT_4125_0067 = in_BX;
  DAT_4125_0069 = unaff_ES;
  (*pcVar1)();
  return;
}


