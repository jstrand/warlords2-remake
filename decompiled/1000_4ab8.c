
char * __cdecl16far strncpy(char *dst,char *src,uint n)

{
  char *pcVar1;
  char *pcVar2;
  uint uVar3;
  int iVar4;
  char *pcVar5;
  char *pcVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  
  uVar7 = (undefined2)((ulong)src >> 0x10);
  pcVar5 = (char *)src;
  uVar3 = n;
  pcVar6 = pcVar5;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar1 = pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (*pcVar1 != '\0');
  uVar8 = (undefined2)((ulong)dst >> 0x10);
  pcVar6 = (char *)dst;
  for (iVar4 = n - uVar3; iVar4 != 0; iVar4 = iVar4 + -1) {
    pcVar2 = pcVar6;
    pcVar6 = pcVar6 + 1;
    pcVar1 = pcVar5;
    pcVar5 = pcVar5 + 1;
    *pcVar2 = *pcVar1;
  }
  for (; uVar3 != 0; uVar3 = uVar3 - 1) {
    pcVar1 = pcVar6;
    pcVar6 = pcVar6 + 1;
    *pcVar1 = '\0';
  }
  return dst;
}


