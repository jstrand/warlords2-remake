
void FUN_400f_001f(void)

{
  code *pcVar1;
  byte in_AL;
  byte in_CL;
  int in_BX;
  int unaff_SI;
  byte *unaff_DI;
  
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + in_AL;
  *(char *)(unaff_SI + -0x16) = *(char *)(unaff_SI + -0x16) << (in_CL & 0x1f);
  *(char *)(unaff_SI + -0x16) = *(char *)(unaff_SI + -0x16) << (in_CL & 0x1f);
  *unaff_DI = *unaff_DI | in_AL;
  *(char *)(unaff_SI + -0x16) = *(char *)(unaff_SI + -0x16) << (in_CL & 0x1f);
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}


