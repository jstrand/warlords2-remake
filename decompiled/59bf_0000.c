
void __cdecl16far ai_phase_evaluate(int param_1)

{
  int *piVar1;
  char *pcVar2;
  undefined1 uVar3;
  int iVar4;
  int *piVar5;
  uint unaff_DI;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  
  ai_mark_flier_cities();
  ai_clear_seen_flags();
  uVar8 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  piVar5 = (int *)DAT_451b_018e;
  piVar5[4] = 0;
  piVar5[3] = 0;
  piVar5[2] = 0;
  piVar5[1] = 0;
  iVar6 = city_count;
  while( true ) {
    iVar7 = iVar6;
    iVar6 = iVar7 + -1;
    piVar5 = (int *)DAT_451b_018e;
    uVar8 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    if (iVar7 == 0) break;
    if (*(char *)(*(byte *)(*(int *)(iVar6 * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(iVar6 * 0x41 + 0x157d) * 2) + 0x710) == '\n') {
      if (*(char *)(iVar6 * 0x41 + 0x1592) == param_1) {
        ((int *)DAT_451b_018e)[1] = ((int *)DAT_451b_018e)[1] + 1;
        pcVar2 = (char *)((int)piVar5 + iVar7 + 0xb9);
        *pcVar2 = *pcVar2 + '\x01';
        if (*(char *)((int)(int *)DAT_451b_018e + iVar7 + 0x55) == '\0') {
          *(undefined1 *)((int)(int *)DAT_451b_018e + iVar7 + 0x55) = 1;
        }
      }
      else {
        if ((*(byte *)((int)(int *)DAT_451b_018e + iVar7 + 0x11d) & 1) != 0) {
          ((int *)DAT_451b_018e)[4] = ((int *)DAT_451b_018e)[4] + 1;
        }
        uVar8 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
        *(undefined1 *)((int)(int *)DAT_451b_018e + iVar7 + 0x55) = 0;
        *(undefined1 *)((int)(int *)DAT_451b_018e + iVar7 + 0xb9) = 0;
        uVar8 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
        if (*(char *)(iVar6 * 0x41 + 0x1592) == '\x0f') {
          piVar1 = (int *)DAT_451b_018e + 3;
          *piVar1 = *piVar1 + 1;
        }
        else {
          piVar1 = (int *)DAT_451b_018e + 2;
          *piVar1 = *piVar1 + 1;
        }
      }
    }
    else {
      *(undefined1 *)((int)(int *)DAT_451b_018e + iVar7 + 0x55) = 0;
      *(undefined1 *)((int)(int *)DAT_451b_018e + iVar7 + 0xb9) = 0;
    }
  }
  iVar6 = city_count;
  if (((int *)DAT_451b_018e)[1] != 0) {
    while (iVar7 = iVar6, iVar6 = iVar7 + -1, iVar7 != 0) {
      uVar8 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      piVar5 = (int *)DAT_451b_018e;
      if ((((*(char *)((int)piVar5 + iVar7 + 0x55) == '\b') && (piVar5[4] != 0)) &&
          (piVar5[0x12] < 5)) && ((*(byte *)((int)piVar5 + iVar7 + 0x11d) & 2) != 0)) {
        *(undefined1 *)((int)piVar5 + iVar7 + 0x55) = 0xb;
      }
      if (*(char *)((int)(int *)DAT_451b_018e + iVar7 + 0x55) == '\x01') {
        iVar4 = ai_neutral_neighbours(iVar6,(char *)0x0,(char *)((ulong)unaff_DI << 0x10));
        if (iVar4 == 0) {
          uVar3 = 5;
        }
        else {
          uVar3 = 3;
        }
        *(undefined1 *)((int)(int *)DAT_451b_018e + iVar7 + 0x55) = uVar3;
      }
    }
    *DAT_451b_018e = *DAT_451b_018e + 1;
  }
  return;
}


