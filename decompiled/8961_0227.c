
void __cdecl16far FUN_8961_0227(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  undefined2 in_DX;
  undefined2 extraout_DX;
  undefined2 extraout_DX_00;
  int iVar9;
  int iVar10;
  undefined2 uVar11;
  int local_c;
  
  if (DAT_4125_2f87 == 0) {
    auto_str_cannot_allocate_vbox_savebm();
  }
  iVar10 = DAT_451b_2872;
  iVar9 = DAT_451b_2870;
  if (param_1 < 5) {
    iVar1 = 0;
  }
  else {
    iVar1 = param_1 + -4;
  }
  if (param_2 < 5) {
    iVar2 = 0;
  }
  else {
    iVar2 = param_2 + -4;
  }
  if (0x66 < iVar1) {
    iVar1 = 0x67;
  }
  if (0x92 < iVar2) {
    iVar2 = 0x93;
  }
  uVar11 = 0x22bf;
  FUN_22bf_07a5();
  while ((iVar9 != iVar1 || (iVar10 != iVar2))) {
    if (iVar1 < iVar9) {
      if (iVar9 - iVar1 < 8) {
        iVar3 = iVar9 - iVar1;
      }
      else {
        iVar3 = 8;
      }
      iVar3 = -iVar3;
    }
    else if (iVar1 - iVar9 < 8) {
      iVar3 = iVar1 - iVar9;
    }
    else {
      iVar3 = 8;
    }
    iVar9 = iVar9 + iVar3;
    if (iVar2 < iVar10) {
      if (iVar10 - iVar2 < 8) {
        iVar3 = iVar10 - iVar2;
      }
      else {
        iVar3 = 8;
      }
      iVar3 = -iVar3;
    }
    else if (iVar2 - iVar10 < 8) {
      iVar3 = iVar2 - iVar10;
    }
    else {
      iVar3 = 8;
    }
    iVar10 = iVar10 + iVar3;
    if ((DAT_4125_2f54 != 0) && ((DAT_4125_2f7f & 10) == 0)) {
      for (local_c = 0; local_c < (int)(uint)DAT_4125_2f7e; local_c = local_c + 1) {
        uVar11 = 0x1d28;
        uVar4 = mem_lock(((undefined4 *)&DAT_4125_2f54)[local_c]);
        ((undefined2 *)&DAT_4125_2f66)[local_c * 2] = in_DX;
        ((undefined2 *)&DAT_4125_2f64)[local_c * 2] = uVar4;
      }
    }
    (*DAT_451b_2a74)(uVar11,0x2f4a,0x4125,0,0x130020,0x342a,0x4125,DAT_451b_2870 * 2 + 400U & 0xfff8
                     ,DAT_451b_2872 * 2 + 0x1e);
    uVar4 = extraout_DX;
    if ((DAT_4125_2f54 != 0) && ((DAT_4125_2f7f & 10) == 0)) {
      for (local_c = 0; local_c < (int)(uint)DAT_4125_2f7e; local_c = local_c + 1) {
        uVar11 = 0x1d28;
        mem_unlock(((undefined4 *)&DAT_4125_2f54)[local_c]);
      }
    }
    if ((DAT_4125_2f54 != 0) && ((DAT_4125_2f7f & 10) == 0)) {
      for (local_c = 0; local_c < (int)(uint)DAT_4125_2f7e; local_c = local_c + 1) {
        uVar11 = 0x1d28;
        uVar5 = mem_lock(((undefined4 *)&DAT_4125_2f54)[local_c]);
        ((undefined2 *)&DAT_4125_2f66)[local_c * 2] = uVar4;
        ((undefined2 *)&DAT_4125_2f64)[local_c * 2] = uVar5;
      }
    }
    (*DAT_451b_2a74)(uVar11,0x342a,0x4125,iVar9 * 2 + 400U & 0xfff8,iVar10 * 2 + 0x1e,0x130020,
                     0x2f4a,0x4125,0);
    in_DX = extraout_DX_00;
    if ((DAT_4125_2f54 != 0) && ((DAT_4125_2f7f & 10) == 0)) {
      for (local_c = 0; local_c < (int)(uint)DAT_4125_2f7e; local_c = local_c + 1) {
        mem_unlock(((undefined4 *)&DAT_4125_2f54)[local_c]);
      }
    }
    iVar3 = iVar9 * 2;
    iVar6 = iVar3 + 400;
    iVar7 = iVar10 * 2;
    iVar8 = iVar7 + 0x1e;
    DAT_451b_2870 = iVar9;
    DAT_451b_2872 = iVar10;
    FUN_24d0_0002();
    FUN_2133_02fe(iVar6,iVar8);
    FUN_2133_02fe(iVar6,iVar7 + 0x1f);
    FUN_2133_02fe(iVar6,iVar7 + 0x2e);
    FUN_2133_02fe(iVar6,iVar7 + 0x2f);
    FUN_2133_0344(iVar6,iVar8);
    FUN_2133_0344(iVar3 + 0x191,iVar8);
    FUN_2133_0344(iVar3 + 0x1a0,iVar8);
    FUN_2133_0344(iVar3 + 0x1a1,iVar8);
    FUN_1e68_000a();
    uVar11 = 0x1e68;
    FUN_1e68_000a();
  }
  DAT_3c04_017d = param_1;
  DAT_3c04_017f = param_2;
  if (param_2 < 4) {
    DAT_3c04_0175 = 0;
  }
  else {
    DAT_3c04_0175 = param_2 + -4;
  }
  if (0x93 < DAT_3c04_0175) {
    DAT_3c04_0175 = 0x93;
  }
  if (param_1 < 4) {
    DAT_3c04_0177 = 0;
  }
  else {
    DAT_3c04_0177 = param_1 + -4;
  }
  if (0x67 < DAT_3c04_0177) {
    DAT_3c04_0177 = 0x67;
  }
  DAT_3c04_017b = DAT_3c04_0177 + 8;
  DAT_3c04_0179 = DAT_3c04_0175 + 8;
  FUN_22bf_0757();
  return;
}


