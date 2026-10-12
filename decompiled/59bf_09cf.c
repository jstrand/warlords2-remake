
void __cdecl16far ai_mark_flier_cities(void)

{
  byte *pbVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  iVar6 = city_count;
  while (iVar7 = iVar6 + -1, iVar6 != 0) {
    iVar6 = iVar7;
    if (*(char *)(*(byte *)(*(int *)(iVar7 * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(iVar7 * 0x41 + 0x157d) * 2) + 0x710) == '\n') {
      bVar2 = false;
      pbVar1 = (byte *)((int)DAT_451b_018e + iVar7 + 0x11e);
      *pbVar1 = *pbVar1 & 0xfd;
      iVar4 = 4;
      while (iVar5 = iVar4 + -1, iVar4 != 0) {
        iVar3 = (int)*(char *)(iVar7 * 0x41 + iVar5 + 0x1593);
        iVar4 = iVar5;
        if ((iVar3 != -1) && (((undefined1 *)&army_move_flags)[iVar3 * 6] != '\0')) {
          bVar2 = true;
        }
      }
      if (bVar2) {
        pbVar1 = (byte *)((int)DAT_451b_018e + iVar7 + 0x11e);
        *pbVar1 = *pbVar1 | 2;
      }
    }
  }
  return;
}


