
int __cdecl16far puts(char *s)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  if (s == (char *)0x0) {
    iVar1 = 0;
  }
  else {
    uVar2 = strlen(s);
    uVar3 = __fputn(0x3b80,0x4125,uVar2,s);
    if (uVar3 == uVar2) {
      iVar1 = fputc(10,(FILE *)0x3b80);
      if (iVar1 == 10) {
        iVar1 = 10;
      }
      else {
        iVar1 = -1;
      }
    }
    else {
      iVar1 = -1;
    }
  }
  return iVar1;
}


