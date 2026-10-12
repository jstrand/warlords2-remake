
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_7f77_0725(void)

{
  bool bVar1;
  ulong uVar2;
  undefined2 uVar3;
  undefined2 *puVar4;
  int iVar5;
  undefined2 uVar6;
  int iVar7;
  ulong in_EDX;
  undefined2 unaff_SS;
  char *pcVar8;
  char local_56 [84];
  
  FUN_54f6_0000();
  FUN_2012_01fe(0x2a0e,0x4125,0xd40100);
  uVar3 = FUN_1d28_01d5();
  DAT_451b_1e00 = CONCAT22((int)in_EDX,uVar3);
  DAT_451b_1d88._0_2_ = FUN_1d28_01d5();
  ram0x00046f3a = CONCAT22(_DAT_451b_1d8c,(int)in_EDX);
  for (iVar7 = 1; uVar2 = ram0x00046f3a, iVar7 < 0x1e; iVar7 = iVar7 + 1) {
    iVar5 = (int)DAT_451b_1d88 + iVar7 * 0x14;
    *(undefined2 *)((int)(undefined4 *)&DAT_451b_1d88 + iVar7 * 4 + 2) = (int)ram0x00046f3a;
    *(int *)((undefined4 *)&DAT_451b_1d88 + iVar7) = iVar5;
    in_EDX = uVar2;
  }
  puVar4 = (undefined2 *)FUN_1d28_01d5();
  DAT_451b_1d82 = (undefined2 *)CONCAT22((int)in_EDX,puVar4);
  iVar7 = 0;
  bVar1 = false;
  while (!bVar1) {
    iVar5 = auto_file_scenario_dat();
    if (iVar5 == 0) {
      bVar1 = true;
      DAT_451b_1d80 = iVar7;
    }
    else {
      *(int *)((int)DAT_451b_1e00 + iVar7 * 2) = iVar7;
      pcVar8 = strcpy((char *)((undefined4 *)&DAT_451b_1d88)[iVar7],
                      (char *)CONCAT22(unaff_SS,local_56));
      in_EDX = (ulong)pcVar8 >> 0x10;
      iVar7 = iVar7 + 1;
    }
  }
  DAT_451b_1d86 = DAT_4125_28ce;
  *DAT_451b_1d82 = 0;
  if (DAT_451b_1d80 < 2) {
    uVar3 = 0xffff;
  }
  else {
    uVar3 = 1;
  }
  ((undefined2 *)DAT_451b_1d82)[1] = uVar3;
  if (DAT_451b_1d80 < 3) {
    uVar3 = 0xffff;
  }
  else {
    uVar3 = 2;
  }
  ((undefined2 *)DAT_451b_1d82)[2] = uVar3;
  if (DAT_451b_1d80 < 4) {
    uVar3 = 0xffff;
  }
  else {
    uVar3 = 3;
  }
  ((undefined2 *)DAT_451b_1d82)[3] = uVar3;
  if (DAT_451b_1d80 < 5) {
    uVar3 = 0xffff;
  }
  else {
    uVar3 = 4;
  }
  ((undefined2 *)DAT_451b_1d82)[4] = uVar3;
  if (DAT_451b_1d80 < 6) {
    uVar3 = 0xffff;
  }
  else {
    uVar3 = 5;
  }
  ((undefined2 *)DAT_451b_1d82)[5] = uVar3;
  if (DAT_451b_1d80 < 7) {
    uVar3 = 0xffff;
  }
  else {
    uVar3 = 6;
  }
  ((undefined2 *)DAT_451b_1d82)[6] = uVar3;
  while (6 < DAT_451b_1d86) {
    DAT_451b_1d86 = DAT_451b_1d86 + -1;
    for (iVar7 = 0; iVar7 < 7; iVar7 = iVar7 + 1) {
      ((undefined2 *)DAT_451b_1d82)[iVar7] = ((undefined2 *)DAT_451b_1d82)[iVar7] + 1;
    }
  }
  DAT_4125_2a49 = DAT_4125_28ce;
  uVar3 = 0x79fa;
  auto_str_join_data_not_found();
  DAT_4125_166a = 1;
  if ((DAT_4125_2a18 != 0) && ((DAT_4125_2a43 & 10) == 0)) {
    for (iVar7 = 0; iVar7 < (int)(uint)DAT_4125_2a42; iVar7 = iVar7 + 1) {
      uVar3 = 0x1d28;
      uVar6 = mem_lock(((undefined4 *)&DAT_4125_2a18)[iVar7]);
      ((undefined2 *)&DAT_4125_2a2a)[iVar7 * 2] = (int)in_EDX;
      ((undefined2 *)&DAT_4125_2a28)[iVar7 * 2] = uVar6;
    }
  }
  (*DAT_451b_2a74)(uVar3,0x342a,0x4125,0x640120,0xd40100,0x2a0e,0x4125,0);
  if ((DAT_4125_2a18 != 0) && ((DAT_4125_2a43 & 10) == 0)) {
    for (iVar7 = 0; iVar7 < (int)(uint)DAT_4125_2a42; iVar7 = iVar7 + 1) {
      mem_unlock(((undefined4 *)&DAT_4125_2a18)[iVar7]);
    }
  }
  FUN_7f77_0996();
  FUN_7f77_0a86();
  return;
}


