
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_str_cannot_allocate_mouse_temp(int param_1)

{
  undefined2 in_DX;
  undefined2 extraout_DX;
  undefined2 extraout_DX_00;
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 local_a;
  undefined2 local_8;
  int local_6;
  int local_4;
  
  if (DAT_4125_357c != param_1) {
    if ((DAT_4125_366e != 0) && (DAT_4125_3570 == 0)) {
      (*DAT_451b_2a74)(0x22bf,0x357e,0x4125,0,0,DAT_4125_3670,DAT_4125_3674,0x342a,0x4125,
                       DAT_4125_3568 - DAT_451b_2fba & 0xfff8);
      in_DX = extraout_DX;
    }
    FUN_216d_002e(&local_a);
    DAT_4125_3670 = local_6 + 0xeU & 0xfff8;
    uVar3 = (undefined2)((ulong)DAT_451b_2fb4 >> 0x10);
    iVar1 = (int)DAT_451b_2fb4;
    DAT_4125_3672 = *(undefined2 *)((int)*(undefined4 *)(iVar1 + 2) + param_1 * 2);
    DAT_4125_3674 = *(undefined2 *)((int)*(undefined4 *)(iVar1 + 6) + param_1 * 2);
    DAT_4125_357c = param_1;
    DAT_451b_2fba = *(int *)((int)*(undefined4 *)(iVar1 + 10) + param_1 * 2);
    DAT_451b_2fb8 = *(int *)((int)*(undefined4 *)(iVar1 + 10) + param_1 * 2);
    FUN_22bf_0224(DAT_4125_3672,DAT_4125_3674,DAT_451b_2fba);
    iVar1 = FUN_2012_01fe(0x35f8,0x4125,(DAT_4125_35b9 + 8) * 8);
    if (iVar1 == 0) {
      printf(s_Cannot_allocate_mouse_temp_bitma_4125_36dd);
    }
    if ((DAT_4125_3602 != 0) && ((DAT_4125_362d & 10) == 0)) {
      for (iVar1 = 0; iVar1 < (int)(uint)DAT_4125_362c; iVar1 = iVar1 + 1) {
        uVar3 = mem_lock(*(undefined2 *)((undefined4 *)&DAT_4125_3602 + iVar1),
                         *(undefined2 *)((int)(undefined4 *)&DAT_4125_3602 + iVar1 * 4 + 2));
        ((undefined2 *)&DAT_4125_3614)[iVar1 * 2] = in_DX;
        ((undefined2 *)&DAT_4125_3612)[iVar1 * 2] = uVar3;
      }
    }
    uVar5 = 0x2012;
    FUN_2012_07cc(0x35f8,0x4125,DAT_4125_367e);
    uVar3 = (undefined2)((ulong)DAT_451b_2fb0 >> 0x10);
    if ((*(long *)((int)DAT_451b_2fb0 + 10) != 0) &&
       ((*(uint *)((int)DAT_451b_2fb0 + 0x35) & 10) == 0)) {
      iVar1 = 0;
      while( true ) {
        uVar3 = (undefined2)((ulong)DAT_451b_2fb0 >> 0x10);
        if ((int)(uint)*(byte *)((int)DAT_451b_2fb0 + 0x34) <= iVar1) break;
        iVar2 = (int)DAT_451b_2fb0 + iVar1 * 4;
        uVar5 = 0x1d28;
        uVar3 = mem_lock(*(undefined2 *)(iVar2 + 10),*(undefined2 *)(iVar2 + 0xc));
        uVar4 = (undefined2)((ulong)DAT_451b_2fb0 >> 0x10);
        iVar2 = (int)DAT_451b_2fb0 + iVar1 * 4;
        *(undefined2 *)(iVar2 + 0x1c) = in_DX;
        *(undefined2 *)(iVar2 + 0x1a) = uVar3;
        iVar1 = iVar1 + 1;
      }
    }
    iVar2 = 0;
    iVar1 = 8;
    while( true ) {
      if (iVar1 == 0) break;
      (*DAT_451b_2a74)(uVar5,(int)DAT_451b_2fb0,DAT_451b_2fb0._2_2_,local_a,local_8,local_6,local_4,
                       0x35f8,0x4125,(DAT_4125_35b9 + 8) * iVar2 + iVar2,0);
      iVar2 = iVar2 + 1;
      in_DX = extraout_DX_00;
      iVar1 = iVar1 + -1;
    }
    if ((*(long *)((int)DAT_451b_2fb0 + 10) != 0) &&
       ((*(uint *)((int)DAT_451b_2fb0 + 0x35) & 10) == 0)) {
      iVar1 = 0;
      while( true ) {
        uVar3 = (undefined2)((ulong)DAT_451b_2fb0 >> 0x10);
        if ((int)(uint)*(byte *)((int)DAT_451b_2fb0 + 0x34) <= iVar1) break;
        iVar2 = (int)DAT_451b_2fb0 + iVar1 * 4;
        mem_unlock(*(undefined2 *)(iVar2 + 10),*(undefined2 *)(iVar2 + 0xc));
        iVar1 = iVar1 + 1;
      }
    }
    uVar3 = 0x251d;
    auto_str_cannot_allocate_mask(0x3680,0x4125,0x35f8,0x4125,0x3633,0x4125,DAT_4125_367e);
    if ((_DAT_4125_35c7 != 0) && ((DAT_4125_35f2 & 10) == 0)) {
      for (iVar1 = 0; iVar1 < (int)(uint)DAT_4125_35f1; iVar1 = iVar1 + 1) {
        uVar3 = 0x1d28;
        uVar5 = mem_lock(((undefined2 *)&DAT_4125_35c7)[iVar1 * 2],
                         *(undefined2 *)((undefined *)&DAT_4125_35c9 + iVar1 * 4));
        ((undefined2 *)&DAT_4125_35d9)[iVar1 * 2] = in_DX;
        ((undefined2 *)&DAT_4125_35d7)[iVar1 * 2] = uVar5;
      }
    }
    if ((DAT_4125_3602 != 0) && ((DAT_4125_362d & 10) == 0)) {
      for (iVar1 = 0; iVar1 < (int)(uint)DAT_4125_362c; iVar1 = iVar1 + 1) {
        uVar3 = 0x1d28;
        uVar5 = mem_lock(*(undefined2 *)((undefined4 *)&DAT_4125_3602 + iVar1),
                         *(undefined2 *)((int)(undefined4 *)&DAT_4125_3602 + iVar1 * 4 + 2));
        ((undefined2 *)&DAT_4125_3614)[iVar1 * 2] = in_DX;
        ((undefined2 *)&DAT_4125_3612)[iVar1 * 2] = uVar5;
      }
    }
    (*DAT_451b_2a74)(uVar3,0x35f8,0x4125,0,0,(DAT_4125_35b9 + 8) * 8,DAT_4125_35bb,0x35bd,0x4125,0,0
                    );
    FUN_2012_0975(0x35f8,0x4125);
    DAT_4125_366e = 1;
    DAT_4125_3676 = local_a;
    DAT_4125_3678 = local_8;
    DAT_4125_367a = local_6;
    DAT_4125_367c = local_4;
    FUN_22bf_0abd((undefined2 *)&DAT_4125_3568,0x4125,(undefined2 *)&DAT_4125_356a,0x4125);
    uVar3 = (undefined2)((ulong)DAT_4125_34e7 >> 0x10);
    iVar1 = (int)DAT_4125_34e7;
    if (*(int *)(iVar1 + 4) < DAT_4125_3568 + local_6) {
      DAT_4125_3568 = (*(int *)(iVar1 + 4) - local_6) + DAT_451b_2fba;
    }
    else if (DAT_4125_3568 < DAT_451b_2fba) {
      DAT_4125_3568 = DAT_451b_2fba;
    }
    if (*(int *)(iVar1 + 6) < DAT_4125_356a + local_4) {
      DAT_4125_356a = (*(int *)(iVar1 + 6) - local_4) + DAT_451b_2fb8;
    }
    else if (DAT_4125_356a < DAT_451b_2fb8) {
      DAT_4125_356a = DAT_451b_2fb8;
    }
    FUN_1ec0_0046(DAT_4125_3568,DAT_4125_356a);
    if (DAT_4125_3570 == 0) {
      (*DAT_451b_2a74)(0x1ec0,0x342a,0x4125,DAT_4125_3568 - DAT_451b_2fba & 0xfff8,
                       DAT_4125_356a - DAT_451b_2fb8,DAT_4125_3670,DAT_4125_3674,0x357e,0x4125,0,0);
      FUN_22bf_098d(DAT_4125_3568,DAT_4125_356a);
    }
  }
  return;
}


