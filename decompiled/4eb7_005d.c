
void __cdecl16far FUN_4eb7_005d(int param_1,int param_2,int param_3)

{
  bool bVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  undefined2 uVar10;
  undefined4 local_16;
  int local_12;
  int local_10;
  undefined4 local_e;
  undefined4 local_a;
  undefined4 local_6;
  
  bVar1 = false;
  if (param_3 == 0) {
    for (iVar3 = 0; iVar3 < 200; iVar3 = iVar3 + 1) {
      iVar9 = dice(1,0x70,-1);
      local_6 = CONCAT22(local_6._2_2_,iVar9);
      iVar9 = dice(1,0x9c,-1);
      local_6 = CONCAT22(iVar9,(int)local_6);
      uVar10 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      if ((*(char *)((int)DAT_451b_002a + iVar9 * 0x70 + (int)local_6 + 0x6120) == '\x05') ||
         (*(char *)((int)DAT_451b_002a + iVar9 * 0x70 + (int)local_6 + 0x6120) == '\x06')) break;
    }
    for (iVar3 = 0; iVar3 < 200; iVar3 = iVar3 + 1) {
      iVar9 = dice(1,0x70,-1);
      local_a = CONCAT22(local_a._2_2_,iVar9);
      iVar9 = dice(1,0x9c,-1);
      local_a = CONCAT22(iVar9,(int)local_a);
      uVar10 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      if ((*(char *)((int)DAT_451b_002a + iVar9 * 0x70 + (int)local_a + 0x6120) == '\x02') ||
         (*(char *)((int)DAT_451b_002a + iVar9 * 0x70 + (int)local_a + 0x6120) == '\x03')) break;
    }
  }
  else {
    local_6 = local_6 & 0xffff0000;
    iVar3 = dice(1,0x9c,-1);
    local_6 = CONCAT22(iVar3,(int)local_6);
    local_a = CONCAT22(local_a._2_2_,0x6f);
    iVar3 = dice(1,0x9c,-1);
    local_a = CONCAT22(iVar3,(int)local_a);
  }
  local_e = local_6;
  if ((int)local_6 == (int)local_a) {
    if (local_6._2_2_ == local_a._2_2_) {
      bVar2 = true;
      goto LAB_4eb7_05ff;
    }
  }
  bVar2 = false;
LAB_4eb7_05ff:
  if (bVar2) {
    return;
  }
  FUN_4c49_0dd0(local_e,local_a);
  iVar9 = 0;
  iVar3 = -1;
  while( true ) {
    iVar7 = (int)DAT_451b_002a;
    uVar10 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    if (7 < iVar9) break;
    iVar7 = iVar7 + iVar9 * 4;
    if ((*(int *)(iVar7 + 0xbc) == local_12) && (*(int *)(iVar7 + 0xbe) == local_10)) {
      iVar3 = iVar9;
    }
    iVar9 = iVar9 + 1;
  }
  iVar4 = (iVar3 + 1) % 8;
  iVar5 = (iVar3 + 7) % 8;
  iVar9 = (iVar3 + 2) % 8;
  iVar6 = (iVar3 + 6) % 8;
  if (param_2 != 0) {
    iVar8 = iVar7 + iVar3 * 4;
    if ((*(char *)(iVar7 + (local_e._2_2_ + *(int *)(iVar8 + 0xbe)) * 0x70 + (int)local_e +
                   *(int *)(iVar8 + 0xbc) + 0x6120) == '\x02') && (bVar1)) {
      bVar2 = true;
    }
    iVar3 = iVar7 + iVar3 * 4;
    if ((*(char *)(iVar7 + (local_e._2_2_ + *(int *)(iVar3 + 0xbe)) * 0x70 + (int)local_e +
                   *(int *)(iVar3 + 0xbc) + 0x6120) == '\x02') && (!bVar1)) {
      bVar1 = true;
    }
  }
  if (param_1 == 0) {
    local_16 = local_e;
    FUN_4bed_0470(&local_16);
    *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 2;
    FUN_4bed_03aa(local_16);
    uVar10 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    iVar3 = (int)DAT_451b_002a + iVar4 * 4;
    local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar3 + 0xbe),(int)local_e + *(int *)(iVar3 + 0xbc)
                       );
    FUN_4bed_0470(&local_16);
    *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 2;
    FUN_4bed_03aa(local_16);
    uVar10 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    iVar3 = (int)DAT_451b_002a + iVar5 * 4;
    local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar3 + 0xbe),(int)local_e + *(int *)(iVar3 + 0xbc)
                       );
LAB_4eb7_0551:
    FUN_4bed_0470(&local_16);
    *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 2;
    FUN_4bed_03aa(local_16);
  }
  else {
    local_16 = local_e;
    FUN_4bed_0470(&local_16);
    *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 2;
    FUN_4bed_03aa(local_16);
    uVar10 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    iVar3 = (int)DAT_451b_002a + iVar4 * 4;
    local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar3 + 0xbe),(int)local_e + *(int *)(iVar3 + 0xbc)
                       );
    FUN_4bed_0470(&local_16);
    *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 2;
    FUN_4bed_03aa(local_16);
    uVar10 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    iVar3 = (int)DAT_451b_002a + iVar5 * 4;
    local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar3 + 0xbe),(int)local_e + *(int *)(iVar3 + 0xbc)
                       );
    FUN_4bed_0470(&local_16);
    *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 2;
    FUN_4bed_03aa(local_16);
    uVar10 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    iVar3 = (int)DAT_451b_002a + iVar4 * 4;
    local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar3 + 0xbe) * 2,
                        (int)local_e + *(int *)(iVar3 + 0xbc) * 2);
    FUN_4bed_0470(&local_16);
    *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 2;
    FUN_4bed_03aa(local_16);
    uVar10 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    iVar3 = (int)DAT_451b_002a + iVar5 * 4;
    local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar3 + 0xbe) * 2,
                        (int)local_e + *(int *)(iVar3 + 0xbc) * 2);
    FUN_4bed_0470(&local_16);
    *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 2;
    FUN_4bed_03aa(local_16);
    if (param_3 != 0) {
      uVar10 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar3 = (int)DAT_451b_002a + iVar4 * 4;
      local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar3 + 0xbe) * 3,
                          (int)local_e + *(int *)(iVar3 + 0xbc) * 3);
      FUN_4bed_0470(&local_16);
      *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 2;
      FUN_4bed_03aa(local_16);
      uVar10 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar3 = (int)DAT_451b_002a + iVar5 * 4;
      local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar3 + 0xbe) * 3,
                          (int)local_e + *(int *)(iVar3 + 0xbc) * 3);
      goto LAB_4eb7_0551;
    }
  }
  iVar3 = (int)local_e + local_12;
  local_e._2_2_ = local_e._2_2_ + local_10;
  local_e = CONCAT22(local_e._2_2_,iVar3);
  if ((iVar3 == (int)local_a) && (local_e._2_2_ == local_a._2_2_)) {
    bVar2 = true;
  }
  iVar3 = dice(1,4,0);
  iVar7 = (int)DAT_451b_002a;
  uVar10 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
  if (iVar3 != 2) goto code_r0x0004f127;
  iVar3 = *(int *)(iVar7 + iVar9 * 4 + 0xbc);
  goto LAB_4eb7_05ee;
code_r0x0004f127:
  if (iVar3 == 3) {
    iVar3 = *(int *)(iVar7 + iVar6 * 4 + 0xbc);
    iVar9 = iVar6;
LAB_4eb7_05ee:
    local_e = CONCAT22(local_e._2_2_ + *(int *)(iVar7 + iVar9 * 4 + 0xbe),(int)local_e + iVar3);
  }
  goto LAB_4eb7_05ff;
}


