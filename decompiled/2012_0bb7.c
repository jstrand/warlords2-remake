
void __cdecl16far
FUN_2012_0bb7(undefined4 param_1,uint param_2,int param_3,int param_4,uint param_5)

{
  byte bVar1;
  bool bVar2;
  byte bVar3;
  byte bVar4;
  uint uVar5;
  int iVar6;
  undefined2 uVar7;
  byte *pbVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  undefined2 uVar14;
  uint local_14;
  uint local_e;
  int local_8;
  undefined4 local_6;
  
  bVar2 = false;
  local_e = 0;
  uVar5 = param_2 & DAT_451b_2a44;
  if (uVar5 != 0) {
    param_2 = param_2 & ~DAT_451b_2a44;
    param_4 = param_4 + uVar5;
    local_e = uVar5;
  }
  if (param_4 >> (DAT_451b_2a3e & 0x1f) < 1) {
    iVar6 = 0;
  }
  else {
    iVar6 = (param_4 >> (DAT_451b_2a3e & 0x1f)) + -1;
  }
  uVar5 = param_4 + param_2 & DAT_451b_2a44;
  iVar13 = (int)((ulong)param_1 >> 0x10);
  iVar9 = (int)param_1;
  if (*(byte *)(iVar9 + 0x34) < 2) {
    bVar1 = *(byte *)(local_e + 0x34fb);
    bVar3 = *(byte *)(uVar5 + 0x34ff);
    bVar4 = FUN_2012_02c6();
    if ((iVar13 == 0x4125) && (iVar9 == 0x342a)) {
      uVar14 = (undefined2)((ulong)DAT_4125_34e7 >> 0x10);
      uVar7 = *(undefined2 *)((int)DAT_4125_34e7 + 0x18);
      pbVar8 = (byte *)(*(int *)((int)DAT_4125_34e7 + 0x16) +
                        ((undefined2 *)&DAT_451b_2a78)[param_3] +
                       ((int)param_2 >> (DAT_451b_2a3e & 0x1f)));
    }
    else if (((*(uint *)(iVar9 + 0x35) & 1) == 0) || (param_3 < *(int *)(iVar9 + 6))) {
      uVar7 = *(undefined2 *)(iVar9 + 0x1c);
      pbVar8 = (byte *)(*(int *)(iVar9 + 0x1a) + param_3 * *(int *)(iVar9 + 8) * 2 +
                       ((int)param_2 >> (DAT_451b_2a3e & 0x1f)));
    }
    else {
      uVar14 = (undefined2)((ulong)*(undefined4 *)(iVar9 + 0x37) >> 0x10);
      iVar11 = (int)*(undefined4 *)(iVar9 + 0x37);
      uVar7 = *(undefined2 *)(iVar11 + 0x1c);
      pbVar8 = (byte *)(*(int *)(iVar11 + 0x1a) +
                        (param_3 - *(int *)(iVar9 + 6)) * *(int *)(iVar11 + 8) * 2 +
                       ((int)param_2 >> (DAT_451b_2a3e & 0x1f)));
    }
    local_6 = (byte *)CONCAT22(uVar7,pbVar8);
    *local_6 = *local_6 & ~bVar1 | bVar4 & bVar1;
    local_6 = (byte *)CONCAT22(uVar7,pbVar8 + 1);
    local_8 = iVar6;
    while (local_8 != 0) {
      *local_6 = bVar4;
      local_6 = (byte *)CONCAT22(local_6._2_2_,(byte *)local_6 + 1);
      local_8 = local_8 + -1;
    }
    *local_6 = *local_6 & ~bVar3 | bVar4 & bVar3;
  }
  else {
    local_e._0_1_ = *(byte *)(local_e + 0x34eb);
    bVar1 = *(byte *)(uVar5 + 0x34f3);
    iVar11 = (int)(param_2 + param_4) >> 3;
    if (((int)param_2 >> 3 == iVar11) && (iVar6 == 0)) {
      local_e._0_1_ = (byte)local_e & bVar1;
      bVar2 = true;
    }
    local_14 = 1;
    for (iVar12 = 0; iVar12 < (int)(uint)*(byte *)(iVar9 + 0x34); iVar12 = iVar12 + 1) {
      iVar10 = iVar9 + iVar12 * 4;
      if ((*(int *)(iVar10 + 10) != 0 || *(int *)(iVar10 + 0xc) != 0) &&
         ((*(uint *)(iVar9 + 0x35) & 10) == 0)) {
        uVar7 = mem_lock(*(undefined4 *)(iVar9 + iVar12 * 4 + 10));
        iVar10 = iVar9 + iVar12 * 4;
        *(int *)(iVar10 + 0x1c) = iVar11;
        *(undefined2 *)(iVar10 + 0x1a) = uVar7;
      }
      FUN_1e68_0018();
      if ((param_5 & local_14) == 0) {
        bVar3 = 0;
      }
      else {
        bVar3 = 0xff;
      }
      if (((*(uint *)(iVar9 + 0x35) & 1) == 0) || (param_3 < *(int *)(iVar9 + 6))) {
        iVar11 = iVar9 + iVar12 * 4;
        uVar7 = *(undefined2 *)(iVar11 + 0x1c);
        pbVar8 = (byte *)(*(int *)(iVar11 + 0x1a) + param_3 * *(int *)(iVar9 + 8) * 2 +
                         ((int)param_2 >> (DAT_451b_2a3e & 0x1f)));
      }
      else {
        uVar14 = (undefined2)((ulong)*(undefined4 *)(iVar9 + 0x37) >> 0x10);
        iVar11 = (int)*(undefined4 *)(iVar9 + 0x37) + iVar12 * 4;
        uVar7 = *(undefined2 *)(iVar11 + 0x1c);
        pbVar8 = (byte *)(*(int *)(iVar11 + 0x1a) +
                          (param_3 - *(int *)(iVar9 + 6)) *
                          *(int *)((int)*(undefined4 *)(iVar9 + 0x37) + 8) * 2 +
                         ((int)param_2 >> (DAT_451b_2a3e & 0x1f)));
      }
      local_6 = (byte *)CONCAT22(uVar7,pbVar8);
      bVar4 = bVar3 & (byte)local_e;
      *local_6 = *local_6 & ~(byte)local_e | bVar4;
      local_6 = (byte *)CONCAT22(uVar7,pbVar8 + 1);
      local_8 = iVar6;
      while( true ) {
        if (local_8 == 0) break;
        *local_6 = bVar3;
        local_6 = (byte *)CONCAT22(local_6._2_2_,(byte *)local_6 + 1);
        local_8 = local_8 + -1;
      }
      if (!bVar2) {
        bVar4 = bVar3 & bVar1;
        *local_6 = *local_6 & ~bVar1 | bVar4;
      }
      iVar11 = CONCAT11((char)((uint)uVar7 >> 8),bVar4);
      iVar10 = iVar9 + iVar12 * 4;
      if ((*(int *)(iVar10 + 10) != 0 || *(int *)(iVar10 + 0xc) != 0) &&
         ((*(uint *)(iVar9 + 0x35) & 10) == 0)) {
        mem_unlock(*(undefined4 *)(iVar9 + iVar12 * 4 + 10));
      }
      local_14 = local_14 << 1;
    }
  }
  return;
}


