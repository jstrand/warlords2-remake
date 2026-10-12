
int __cdecl16far dos_free(void *block)

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


