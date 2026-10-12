
int __cdecl16far change_dir(char *path)

{
  code *pcVar1;
  byte bVar2;
  bool bVar3;
  bool bVar4;
  
  bVar3 = (byte)((char *)path)[1] < 0x3a;
  if (((char *)path)[1] == 0x3a) {
    bVar2 = *path;
    if ((0x60 < bVar2) && (bVar2 < 0x7b)) {
      bVar2 = bVar2 - 0x20;
    }
    if (bVar2 < 0x41) {
      return 0;
    }
    if (0x5a < bVar2) {
      return 0;
    }
    bVar4 = false;
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    bVar3 = false;
    if (bVar4) {
      return 0;
    }
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if (bVar3) {
    return 0;
  }
  return 1;
}


