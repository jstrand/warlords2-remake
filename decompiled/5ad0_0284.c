
void __cdecl16far ai_phase_move_search(void)

{
  byte *pbVar1;
  uint *puVar2;
  int *piVar3;
  bool bVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  
  bVar4 = false;
  iVar7 = city_count;
  while( true ) {
    iVar5 = iVar7 + -1;
    iVar6 = (int)DAT_451b_018e;
    uVar8 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    if (iVar7 == 0) break;
    iVar7 = iVar5;
    if ((*(char *)(iVar5 * 0x41 + 0x1592) == current_player) &&
       (*(char *)(iVar6 + iVar5 + 0x56) == '\a')) {
      bVar4 = true;
    }
  }
  *(undefined2 *)(iVar6 + 0x22) = 0;
  *(undefined2 *)(iVar6 + 0x24) = 0;
  for (iVar7 = 0; iVar5 = army_count, iVar7 < 2; iVar7 = iVar7 + 1) {
    while (iVar6 = iVar5 + -1, iVar5 != 0) {
      iVar5 = iVar6;
      if (((((*(char *)(iVar6 * 0x16 + -0x777b) == (char)current_player) &&
            (*(int *)(iVar6 * 0x16 + -0x7780) != -1)) &&
           ((*(uint *)(iVar6 * 0x16 + -0x7774) & 0x20) != 0)) &&
          ((*(char *)(iVar6 * 0x16 + -0x777c) != '\x1c' || (iVar7 == 0)))) &&
         ((*(char *)(iVar6 * 0x16 + -0x777c) == '\x1c' || (iVar7 != 0)))) {
        if (((*(char *)(iVar6 * 0x16 + -0x777c) == '\x1c') && (bVar4)) ||
           ((((undefined1 *)&army_move_flags)[*(char *)(iVar6 * 0x16 + -0x777c) * 6] != '\0' &&
            ((7 < *(int *)((int)DAT_451b_018e + 0x24) &&
             (*(char *)(iVar6 * 0x16 + -0x7778) < '\x04')))))) {
          pbVar1 = (byte *)(iVar6 * 0x16 + -0x7772);
          *pbVar1 = *pbVar1 & 0xf0;
          pbVar1 = (byte *)(iVar6 * 0x16 + -0x7771);
          *pbVar1 = *pbVar1 & 1;
          puVar2 = (uint *)(iVar6 * 0x16 + -0x7774);
          *puVar2 = *puVar2 & 0xffdf;
        }
        else {
          FUN_57ea_0e4b(iVar6,0xffff);
          if (*(char *)(iVar6 * 0x16 + -0x777b) == current_player) {
            uVar8 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
            if (((undefined1 *)&army_move_flags)[*(char *)(iVar6 * 0x16 + -0x777c) * 6] == '\0') {
              piVar3 = (int *)((int)DAT_451b_018e + 0x22);
              *piVar3 = *piVar3 + 1;
            }
            else {
              piVar3 = (int *)((int)DAT_451b_018e + 0x24);
              *piVar3 = *piVar3 + 1;
            }
          }
        }
      }
    }
  }
  return;
}


