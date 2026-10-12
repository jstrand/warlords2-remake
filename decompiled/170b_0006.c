
int __cdecl16far open(char *name,int mode)

{
  code *pcVar1;
  int iVar2;
  bool bVar3;
  
  if ((mode & 8U) != 0) {
    bVar3 = false;
    pcVar1 = (code *)swi(0x21);
    iVar2 = (*pcVar1)();
    if (!bVar3) {
      return iVar2;
    }
  }
  bVar3 = false;
  pcVar1 = (code *)swi(0x21);
  iVar2 = (*pcVar1)();
  if (bVar3) {
    iVar2 = -1;
  }
  return iVar2;
}


