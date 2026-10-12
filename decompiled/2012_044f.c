
void __cdecl16far FUN_2012_044f(undefined4 param_1,void *param_2,uint param_3)

{
  byte bVar1;
  byte bVar2;
  undefined4 uVar3;
  byte bVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  byte *pbVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  undefined2 uVar13;
  undefined2 unaff_SS;
  uint local_1e;
  int local_12;
  int local_10;
  undefined4 local_e;
  uint local_a;
  int local_8;
  int local_6;
  int local_4;
  
  memcpy((uint *)CONCAT22(unaff_SS,&local_a),param_2,8);
  bVar1 = 0xff;
  uVar5 = local_a & DAT_451b_2a44;
  if (uVar5 != 0) {
    local_a = local_a & ~DAT_451b_2a44;
    local_6 = local_6 + uVar5;
    if (DAT_4125_3384 == 0) {
      iVar6 = 0x34fb;
    }
    else {
      iVar6 = 0x34eb;
    }
    bVar1 = *(byte *)(iVar6 + uVar5);
  }
  iVar6 = (local_6 >> (DAT_451b_2a3e & 0x1f)) + -1;
  if (DAT_4125_3384 == 0) {
    iVar7 = 0x34ff;
  }
  else {
    iVar7 = 0x34f3;
  }
  uVar8 = 0x4125;
  bVar2 = *(byte *)(iVar7 + (local_6 + local_a & DAT_451b_2a44));
  iVar7 = (int)param_1;
  iVar12 = (int)((ulong)param_1 >> 0x10);
  if (DAT_4125_3384 == 0) {
    bVar4 = FUN_2012_02c6(param_3);
    iVar10 = local_8;
    if ((iVar12 == 0x4125) && (iVar7 == 0x342a)) {
      for (; iVar10 < local_8 + local_4; iVar10 = iVar10 + 1) {
        uVar13 = (undefined2)((ulong)DAT_4125_34e7 >> 0x10);
        uVar8 = *(undefined2 *)((int)DAT_4125_34e7 + 0x18);
        pbVar9 = (byte *)(*(int *)((int)DAT_4125_34e7 + 0x16) +
                          ((undefined2 *)&DAT_451b_2a78)[iVar10] +
                         ((int)local_a >> (DAT_451b_2a3e & 0x1f)));
        local_e = (byte *)CONCAT22(uVar8,pbVar9);
        *local_e = *local_e ^ bVar4 & bVar1;
        local_e = (byte *)CONCAT22(uVar8,pbVar9 + 1);
        local_12 = iVar6;
        while (local_12 != 0) {
          *local_e = *local_e ^ bVar4;
          local_e = (byte *)CONCAT22(local_e._2_2_,(byte *)local_e + 1);
          local_12 = local_12 + -1;
        }
        *local_e = *local_e ^ bVar4 & bVar2;
      }
    }
    else {
      for (; iVar10 < local_8 + local_4; iVar10 = iVar10 + 1) {
        if (((*(uint *)(iVar7 + 0x35) & 1) == 0) || (iVar10 < *(int *)(iVar7 + 6))) {
          uVar8 = *(undefined2 *)(iVar7 + 0x1c);
          pbVar9 = (byte *)(*(int *)(iVar7 + 0x1a) + iVar10 * *(int *)(iVar7 + 8) * 2 +
                           ((int)local_a >> (DAT_451b_2a3e & 0x1f)));
        }
        else {
          uVar13 = (undefined2)((ulong)*(undefined4 *)(iVar7 + 0x37) >> 0x10);
          iVar11 = (int)*(undefined4 *)(iVar7 + 0x37);
          uVar8 = *(undefined2 *)(iVar11 + 0x1c);
          pbVar9 = (byte *)(*(int *)(iVar11 + 0x1a) +
                            (iVar10 - *(int *)(iVar7 + 6)) * *(int *)(iVar11 + 8) * 2 +
                           ((int)local_a >> (DAT_451b_2a3e & 0x1f)));
        }
        local_e = (byte *)CONCAT22(uVar8,pbVar9);
        *local_e = *local_e ^ bVar4 & bVar1;
        local_e = (byte *)CONCAT22(uVar8,pbVar9 + 1);
        local_12 = iVar6;
        while (local_12 != 0) {
          *local_e = *local_e ^ bVar4;
          local_e = (byte *)CONCAT22(local_e._2_2_,(byte *)local_e + 1);
          local_12 = local_12 + -1;
        }
        *local_e = *local_e ^ bVar4 & bVar2;
      }
    }
  }
  else {
    local_1e = 1;
    for (local_10 = 0; local_10 < (int)(uint)*(byte *)(iVar7 + 0x34); local_10 = local_10 + 1) {
      iVar10 = iVar7 + local_10 * 4;
      if ((*(int *)(iVar10 + 10) != 0 || *(int *)(iVar10 + 0xc) != 0) &&
         ((*(uint *)(iVar7 + 0x35) & 10) == 0)) {
        uVar3 = *(undefined4 *)(iVar7 + local_10 * 4 + 10);
        uVar13 = mem_lock((int)uVar3,(int)((ulong)uVar3 >> 0x10));
        iVar10 = iVar7 + local_10 * 4;
        *(undefined2 *)(iVar10 + 0x1c) = uVar8;
        *(undefined2 *)(iVar10 + 0x1a) = uVar13;
      }
      FUN_1e68_0018(local_10);
      iVar10 = local_8;
      if ((param_3 & local_1e) == 0) {
        bVar4 = 0;
      }
      else {
        bVar4 = 0xff;
      }
      for (; iVar10 < local_8 + local_4; iVar10 = iVar10 + 1) {
        if (((*(uint *)(iVar7 + 0x35) & 1) == 0) || (iVar10 < *(int *)(iVar7 + 6))) {
          iVar11 = iVar7 + local_10 * 4;
          uVar8 = *(undefined2 *)(iVar11 + 0x1c);
          pbVar9 = (byte *)(*(int *)(iVar11 + 0x1a) + iVar10 * *(int *)(iVar7 + 8) * 2 +
                           ((int)local_a >> (DAT_451b_2a3e & 0x1f)));
        }
        else {
          uVar13 = (undefined2)((ulong)*(undefined4 *)(iVar7 + 0x37) >> 0x10);
          iVar11 = (int)*(undefined4 *)(iVar7 + 0x37) + local_10 * 4;
          uVar8 = *(undefined2 *)(iVar11 + 0x1c);
          pbVar9 = (byte *)(*(int *)(iVar11 + 0x1a) +
                            (iVar10 - *(int *)(iVar7 + 6)) *
                            *(int *)((int)*(undefined4 *)(iVar7 + 0x37) + 8) * 2 +
                           ((int)local_a >> (DAT_451b_2a3e & 0x1f)));
        }
        local_e = (byte *)CONCAT22(uVar8,pbVar9);
        *local_e = *local_e ^ bVar4 & bVar1;
        local_e = (byte *)CONCAT22(uVar8,pbVar9 + 1);
        local_12 = iVar6;
        while (local_12 != 0) {
          *local_e = *local_e ^ bVar4;
          local_e = (byte *)CONCAT22(local_e._2_2_,(byte *)local_e + 1);
          local_12 = local_12 + -1;
        }
        *local_e = *local_e ^ bVar4 & bVar2;
      }
      iVar10 = iVar7 + local_10 * 4;
      if ((*(int *)(iVar10 + 10) != 0 || *(int *)(iVar10 + 0xc) != 0) &&
         ((*(uint *)(iVar7 + 0x35) & 10) == 0)) {
        uVar3 = *(undefined4 *)(iVar7 + local_10 * 4 + 10);
        mem_unlock((int)uVar3,(int)((ulong)uVar3 >> 0x10));
      }
      local_1e = local_1e << 1;
    }
  }
  return;
}


