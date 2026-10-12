
void __cdecl16far auto_str_cannot_allocate_vbox_savebm(void)

{
  int iVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 in_DX;
  int iVar4;
  undefined2 uVar5;
  
  FUN_2012_09cf(0x2f4a,0x4125);
  uVar5 = 0x2012;
  iVar1 = FUN_2012_01fe(0x2f4a,0x4125,0x130020);
  if (iVar1 == 0) {
    uVar5 = 0x79de;
    auto_str_fatal_error((char *)s_Cannot_allocate_VBOX_saveBM_4125_2f89,0x4125);
  }
  DAT_451b_2870 = DAT_3c04_0177;
  DAT_451b_2872 = DAT_3c04_0175;
  DAT_451b_2874 = 0x12;
  DAT_451b_2876 = 0x13;
  if (DAT_4125_2f85 == 0) {
    iVar1 = *(int *)(DAT_3c04_0163 * 0x14 + 0x189);
    iVar3 = *(int *)(DAT_3c04_0163 * 0x14 + 0x18b);
    for (iVar4 = 0; iVar4 < 8; iVar4 = iVar4 + 1) {
      if ((*(int *)(*(int *)(iVar4 * 2 + 0x163) * 2 + 0x137) != 0) &&
         (*(int *)(*(int *)(iVar4 * 2 + 0x163) * 2 + 0xd0) == 0)) {
        iVar1 = *(int *)(*(int *)(iVar4 * 2 + 0x163) * 0x14 + 0x189);
        iVar3 = *(int *)(*(int *)(iVar4 * 2 + 0x163) * 0x14 + 0x18b);
        break;
      }
    }
    if (iVar1 < 5) {
      DAT_451b_2870 = 0;
    }
    else {
      DAT_451b_2870 = iVar1 + -4;
    }
    if (iVar3 < 5) {
      DAT_451b_2872 = 0;
    }
    else {
      DAT_451b_2872 = iVar3 + -4;
    }
    if (0x66 < DAT_451b_2870) {
      DAT_451b_2870 = 0x67;
    }
    if (0x92 < DAT_451b_2872) {
      DAT_451b_2872 = 0x93;
    }
    DAT_4125_2f85 = 1;
  }
  else {
    DAT_4125_2f87 = 1;
    if ((DAT_4125_2f54 != 0) && ((DAT_4125_2f7f & 10) == 0)) {
      for (iVar1 = 0; iVar1 < (int)(uint)DAT_4125_2f7e; iVar1 = iVar1 + 1) {
        uVar5 = 0x1d28;
        uVar2 = mem_lock(((undefined4 *)&DAT_4125_2f54)[iVar1]);
        ((undefined2 *)&DAT_4125_2f66)[iVar1 * 2] = in_DX;
        ((undefined2 *)&DAT_4125_2f64)[iVar1 * 2] = uVar2;
      }
    }
    (*DAT_451b_2a74)(uVar5,0x342a,0x4125,DAT_451b_2870 * 2 + 400U & 0xfff8,DAT_451b_2872 * 2 + 0x1e,
                     0x130020,0x2f4a,0x4125,0);
    if ((DAT_4125_2f54 != 0) && ((DAT_4125_2f7f & 10) == 0)) {
      for (iVar1 = 0; iVar1 < (int)(uint)DAT_4125_2f7e; iVar1 = iVar1 + 1) {
        mem_unlock(((undefined4 *)&DAT_4125_2f54)[iVar1]);
      }
    }
  }
  return;
}


