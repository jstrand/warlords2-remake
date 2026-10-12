
int __cdecl16far read(int fd,void *buf,uint len)

{
  code *pcVar1;
  int iVar2;
  undefined1 in_CF;
  
  pcVar1 = (code *)swi(0x21);
  iVar2 = (*pcVar1)();
  if ((bool)in_CF) {
    iVar2 = -1;
  }
  return iVar2;
}


