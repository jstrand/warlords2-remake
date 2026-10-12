
int __cdecl16far getch(void)

{
  code *pcVar1;
  byte bVar2;
  
  if (DAT_4125_3ebe == '\0') {
    pcVar1 = (code *)swi(0x21);
    bVar2 = (*pcVar1)();
  }
  else {
    DAT_4125_3ebe = '\0';
    bVar2 = DAT_4125_3ebf;
  }
  return (uint)bVar2;
}


