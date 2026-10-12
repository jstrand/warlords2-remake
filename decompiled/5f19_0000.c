
void __cdecl16far ai_phase_assault_xx(void)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  undefined2 uVar6;
  int local_6;
  
  iVar5 = 0;
  local_6 = 0;
  iVar2 = city_count;
  if (*DAT_451b_018e != 0) {
    while( true ) {
      iVar3 = iVar2;
      iVar2 = iVar3 + -1;
      piVar4 = (int *)DAT_451b_018e;
      uVar6 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      if (iVar3 == 0) break;
      if ((*(char *)(iVar2 * 0x41 + 0x1592) == current_player) &&
         ((*(char *)((int)piVar4 + iVar3 + 0x55) == '\x05' ||
          (*(char *)((int)piVar4 + iVar3 + 0x55) == '\b')))) {
        local_6 = local_6 + 1;
      }
    }
    iVar2 = 4;
    while (iVar3 = iVar2 + -1, iVar2 != 0) {
      iVar2 = iVar3;
      if (piVar4[iVar3 * 0x2e + 0x126] != 0) {
        iVar5 = iVar5 + 1;
      }
    }
    if (((iVar5 < piVar4[0x125]) && (local_6 != 0)) && ((iVar5 == 0 || (2 < local_6)))) {
      iVar2 = piVar4[0x125];
      iVar5 = -1;
      while (iVar3 = iVar2 + -1, iVar2 != 0) {
        iVar2 = iVar3;
        if (piVar4[iVar3 * 0x2e + 0x126] == 0) {
          iVar5 = iVar3;
        }
      }
      if (iVar5 != -1) {
        FUN_5f19_08ba(iVar5);
        iVar2 = ai_pick_enemy();
        if (iVar2 != -1) {
          iVar3 = FUN_5f19_01bb(iVar5,iVar2,(int *)DAT_451b_018e + iVar5 * 0x2e + 0x12d,
                                DAT_451b_018e._2_2_,(int *)DAT_451b_018e + iVar5 * 0x2e + 0x139,
                                DAT_451b_018e._2_2_);
          if (iVar3 == -1) {
            FUN_5f19_08ba(iVar5);
          }
          else {
            uVar6 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
            ((int *)DAT_451b_018e)[iVar5 * 0x2e + 0x126] = 1;
            ((int *)DAT_451b_018e)[iVar5 * 0x2e + 0x127] = iVar2;
            ((int *)DAT_451b_018e)[iVar5 * 0x2e + 0x128] = iVar3;
            *(undefined1 *)((int)(int *)DAT_451b_018e + iVar3 + 0x56) = 7;
            FUN_5f19_06a0(iVar5);
            iVar2 = current_player * 8 + iVar2;
            pbVar1 = (byte *)(iVar2 + 0x153b);
            *pbVar1 = *pbVar1 & 0xf3;
            pbVar1 = (byte *)(iVar2 + 0x153b);
            *pbVar1 = *pbVar1 | 8;
            FUN_5f19_083e(iVar3);
            FUN_5f19_09ca(iVar5);
            FUN_59bf_01b3();
          }
        }
      }
    }
  }
  return;
}


