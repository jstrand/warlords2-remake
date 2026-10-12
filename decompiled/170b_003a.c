
int __cdecl16far close(int fd)

{
  code *pcVar1;
  int iVar2;
  
  pcVar1 = (code *)swi(0x21);
  iVar2 = (*pcVar1)();
  return iVar2;
}


