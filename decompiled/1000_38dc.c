
int __cdecl16far kbhit(void)

{
  code *pcVar1;
  char cVar2;
  int iVar3;
  
  if (DAT_4125_3ebe == '\0') {
    pcVar1 = (code *)swi(0x21);
    cVar2 = (*pcVar1)();
    iVar3 = (int)cVar2;
  }
  else {
    iVar3 = 1;
  }
  return iVar3;
}


