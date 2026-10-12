
void __cdecl16far ai_phase_assault(void)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  undefined2 uVar4;
  
  if (*DAT_451b_018e != 0) {
    FUN_59bf_01b3();
    iVar2 = city_count;
    while( true ) {
      iVar1 = iVar2;
      iVar2 = iVar1 + -1;
      piVar3 = (int *)DAT_451b_018e;
      uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      if (iVar1 == 0) break;
      if ((*(char *)(iVar2 * 0x41 + 0x1592) == current_player) &&
         (*(char *)((int)piVar3 + iVar1 + 0x55) == '\a')) {
        *(undefined1 *)((int)piVar3 + iVar1 + 0x55) = 8;
      }
    }
    if (piVar3[0x125] != 0) {
      iVar2 = 0;
      while( true ) {
        uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
        piVar3 = (int *)DAT_451b_018e;
        if (piVar3[0x125] <= iVar2) break;
        if (piVar3[iVar2 * 0x2e + 0x126] != 0) {
          *(undefined1 *)((int)piVar3 + piVar3[iVar2 * 0x2e + 0x128] + 0x56) = 7;
          iVar1 = ai_run_assault_group(iVar2);
          if (iVar1 != 0) {
            ((int *)DAT_451b_018e)[iVar2 * 0x2e + 0x126] =
                 ((int *)DAT_451b_018e)[iVar2 * 0x2e + 0x126] + 1;
          }
        }
        iVar2 = iVar2 + 1;
      }
    }
  }
  return;
}


