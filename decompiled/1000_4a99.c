
uint __cdecl16far strlen(char *s)

{
  char *pcVar1;
  uint uVar2;
  char *pcVar3;
  
  pcVar3 = (char *)s;
  uVar2 = 0;
  if (s != (char *)0x0) {
    uVar2 = 0xffff;
    do {
      if (uVar2 == 0) break;
      uVar2 = uVar2 - 1;
      pcVar1 = pcVar3;
      pcVar3 = pcVar3 + 1;
    } while (*pcVar1 != '\0');
    uVar2 = ~uVar2 - 1;
  }
  return uVar2;
}


