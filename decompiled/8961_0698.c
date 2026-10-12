
void __cdecl16far FUN_8961_0698(void)

{
  undefined2 uVar1;
  undefined2 in_DX;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  
  uVar6 = 0x8961;
  if (DAT_4125_2f87 == 0) {
    auto_str_cannot_allocate_vbox_savebm();
  }
  if ((DAT_4125_2f54 != 0) && ((DAT_4125_2f7f & 10) == 0)) {
    for (iVar2 = 0; iVar2 < (int)(uint)DAT_4125_2f7e; iVar2 = iVar2 + 1) {
      uVar6 = 0x1d28;
      uVar1 = mem_lock(((undefined4 *)&DAT_4125_2f54)[iVar2]);
      ((undefined2 *)&DAT_4125_2f66)[iVar2 * 2] = in_DX;
      ((undefined2 *)&DAT_4125_2f64)[iVar2 * 2] = uVar1;
    }
  }
  (*DAT_451b_2a74)(uVar6,0x342a,0x4125,DAT_451b_2870 * 2 + 400U & 0xfff8,DAT_451b_2872 * 2 + 0x1e,
                   0x130020,0x2f4a,0x4125,0);
  if ((DAT_4125_2f54 != 0) && ((DAT_4125_2f7f & 10) == 0)) {
    for (iVar2 = 0; iVar2 < (int)(uint)DAT_4125_2f7e; iVar2 = iVar2 + 1) {
      mem_unlock(((undefined4 *)&DAT_4125_2f54)[iVar2]);
    }
  }
  iVar2 = DAT_451b_2870 * 2;
  iVar3 = iVar2 + 400;
  iVar4 = DAT_451b_2872 * 2;
  iVar5 = iVar4 + 0x1e;
  FUN_24d0_0002();
  FUN_2133_02fe(iVar3,iVar5,0x12);
  FUN_2133_02fe(iVar3,iVar4 + 0x1f,0x12);
  FUN_2133_02fe(iVar3,iVar4 + 0x2e,0x12);
  FUN_2133_02fe(iVar3,iVar4 + 0x2f,0x12);
  FUN_2133_0344(iVar3,iVar5,0x12);
  FUN_2133_0344(iVar2 + 0x191,iVar5,0x12);
  FUN_2133_0344(iVar2 + 0x1a0,iVar5,0x12);
  FUN_2133_0344(iVar2 + 0x1a1,iVar5,0x12);
  return;
}


