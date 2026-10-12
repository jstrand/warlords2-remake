
long __cdecl16far lseek(int handle,long offset,int whence)

{
  uint *puVar1;
  code *pcVar2;
  int iVar3;
  bool bVar4;
  long lVar5;
  
  puVar1 = (uint *)(handle * 2 + 0x3cfe);
  bVar4 = false;
  *puVar1 = *puVar1 & 0xfdff;
  pcVar2 = (code *)swi(0x21);
  lVar5 = (*pcVar2)();
  if (bVar4) {
    iVar3 = __IOerror();
    lVar5 = (long)iVar3;
  }
  return lVar5;
}


