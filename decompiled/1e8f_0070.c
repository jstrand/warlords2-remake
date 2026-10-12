
void __cdecl16far FUN_1e8f_0070(void)

{
  code *pcVar1;
  
  if (DAT_4125_3370 != '\0') {
    DAT_4125_3370 = '\0';
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)(0x4125);
  }
  return;
}


