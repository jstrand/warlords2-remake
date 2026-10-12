
void __cdecl16far FUN_8c07_0000(void)

{
  byte *pbVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar4 = army_count;
  while (iVar5 = iVar4 + -1, iVar4 != 0) {
    iVar4 = iVar5;
    if (((*(char *)(iVar5 * 0x16 + -0x777b) == (char)current_player) &&
        ((~*(uint *)(iVar5 * 0x16 + -0x7774) & 1) != 0)) &&
       (*(char *)(iVar5 * 0x16 + -0x777a) <= *(char *)(iVar5 * 0x16 + -0x7779))) {
      iVar3 = *(int *)(iVar5 * 0x16 + -0x7780);
      iVar5 = *(int *)(iVar5 * 0x16 + -0x777e);
      cVar2 = *(char *)(*(byte *)(iVar5 * 0xe0 + iVar3 * 2) + 0x710);
      if (((((cVar2 == '\a') || (cVar2 == '\x04')) ||
           ((cVar2 == '\x05' || ((cVar2 == '\x01' || (cVar2 == '\b')))))) || (cVar2 == '\t')) ||
         ((*(byte *)(iVar5 * 0x70 + iVar3) & 0x1f) != 0)) {
        pbVar1 = (byte *)(iVar5 * 0xe0 + iVar3 * 2 + 1);
        *pbVar1 = *pbVar1 | 0x20;
      }
    }
  }
  return;
}


