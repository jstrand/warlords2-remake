
void __cdecl16far FUN_1e8f_00c6(void)

{
  code *pcVar1;
  
  if (DAT_4125_3371 != '\0') {
    DAT_4125_3371 = '\0';
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
  }
  return;
}


