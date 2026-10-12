
int __cdecl16far bioskey(int cmd)

{
  code *pcVar1;
  int iVar2;
  bool bVar3;
  
  bVar3 = true;
  pcVar1 = (code *)swi(0x16);
  iVar2 = (*pcVar1)();
  if (bVar3) {
    if ((cmd & 1U) != 0) {
      iVar2 = 0;
    }
  }
  else if (((cmd & 1U) == 0) && (iVar2 == 0)) {
    iVar2 = -1;
  }
  return iVar2;
}


