
void __cdecl16far FUN_7f77_081d(void)

{
  int *piVar1;
  undefined2 in_AX;
  undefined2 uVar2;
  undefined2 uVar3;
  int in_CX;
  undefined2 in_DX;
  int in_BX;
  int iVar4;
  undefined2 unaff_ES;
  
  if (in_CX == 1) {
    *(undefined2 *)(in_BX + 4) = in_AX;
    if (DAT_451b_1d80 < 4) {
      uVar2 = 0xffff;
    }
    else {
      uVar2 = 3;
    }
    *(undefined2 *)((int)DAT_451b_1d82 + 6) = uVar2;
    if (DAT_451b_1d80 < 5) {
      uVar2 = 0xffff;
    }
    else {
      uVar2 = 4;
    }
    *(undefined2 *)((int)DAT_451b_1d82 + 8) = uVar2;
    if (DAT_451b_1d80 < 6) {
      uVar2 = 0xffff;
    }
    else {
      uVar2 = 5;
    }
    *(undefined2 *)((int)DAT_451b_1d82 + 10) = uVar2;
    if (DAT_451b_1d80 < 7) {
      in_AX = 0xffff;
    }
    else {
      in_AX = 6;
    }
    unaff_ES = (undefined2)((ulong)DAT_451b_1d82 >> 0x10);
    in_BX = (int)DAT_451b_1d82;
  }
  *(undefined2 *)(in_BX + 0xc) = in_AX;
  while (6 < DAT_451b_1d86) {
    DAT_451b_1d86 = DAT_451b_1d86 + -1;
    for (iVar4 = 0; iVar4 < 7; iVar4 = iVar4 + 1) {
      piVar1 = (int *)((int)DAT_451b_1d82 + iVar4 * 2);
      *piVar1 = *piVar1 + 1;
    }
  }
  DAT_4125_2a49 = DAT_4125_28ce;
  uVar2 = 0x79fa;
  auto_str_join_data_not_found();
  DAT_4125_166a = 1;
  if ((DAT_4125_2a18 != 0) && ((DAT_4125_2a43 & 10) == 0)) {
    for (iVar4 = 0; iVar4 < (int)(uint)DAT_4125_2a42; iVar4 = iVar4 + 1) {
      uVar2 = 0x1d28;
      uVar3 = mem_lock(((undefined4 *)&DAT_4125_2a18)[iVar4]);
      ((undefined2 *)&DAT_4125_2a2a)[iVar4 * 2] = in_DX;
      ((undefined2 *)&DAT_4125_2a28)[iVar4 * 2] = uVar3;
    }
  }
  (*DAT_451b_2a74)(uVar2,0x342a,0x4125,0x640120,0xd40100,0x2a0e,0x4125,0);
  if ((DAT_4125_2a18 != 0) && ((DAT_4125_2a43 & 10) == 0)) {
    for (iVar4 = 0; iVar4 < (int)(uint)DAT_4125_2a42; iVar4 = iVar4 + 1) {
      mem_unlock(((undefined4 *)&DAT_4125_2a18)[iVar4]);
    }
  }
  FUN_7f77_0996();
  FUN_7f77_0a86();
  return;
}


