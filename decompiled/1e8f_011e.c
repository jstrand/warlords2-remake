
void __cdecl16far FUN_1e8f_011e(void)

{
  code *pcVar1;
  
  if (DAT_4125_3372 != '\0') {
    DAT_4125_3372 = '\0';
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
  }
  return;
}


