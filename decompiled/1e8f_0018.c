
void __cdecl16far FUN_1e8f_0018(void)

{
  code *pcVar1;
  undefined2 in_BX;
  undefined2 *unaff_DI;
  undefined2 unaff_ES;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  *unaff_DI = in_BX;
  unaff_DI[1] = unaff_ES;
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}


