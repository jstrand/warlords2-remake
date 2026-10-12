
void __cdecl16far
FUN_2012_0f08(undefined4 param_1,uint param_2,int param_3,int param_4,byte param_5)

{
  byte bVar1;
  int iVar2;
  byte bVar3;
  byte bVar4;
  byte *pbVar5;
  byte *pbVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  undefined2 uVar12;
  undefined2 uVar13;
  int local_12;
  byte local_10;
  undefined4 local_8;
  byte local_3;
  
  iVar7 = (int)param_1;
  iVar11 = (int)((ulong)param_1 >> 0x10);
  if (DAT_4125_3384 == 0) {
    bVar1 = *(byte *)((param_2 & DAT_451b_2a44) + 0x3503);
    bVar3 = ~bVar1;
    bVar4 = FUN_2012_02c6();
    bVar4 = bVar4 & bVar1;
    iVar8 = param_3;
    if ((iVar11 == 0x4125) && (iVar7 == 0x342a)) {
      for (; iVar8 < param_3 + param_4; iVar8 = iVar8 + 1) {
        uVar12 = (undefined2)((ulong)DAT_4125_34e7 >> 0x10);
        local_8 = (byte *)CONCAT22(*(undefined2 *)((int)DAT_4125_34e7 + 0x18),
                                   (byte *)(*(int *)((int)DAT_4125_34e7 + 0x16) +
                                            ((undefined2 *)&DAT_451b_2a78)[iVar8] +
                                           ((int)param_2 >> (DAT_451b_2a3e & 0x1f))));
        *local_8 = *local_8 & bVar3 | bVar4;
      }
    }
    else {
      for (; iVar8 < param_3 + param_4; iVar8 = iVar8 + 1) {
        if (((*(uint *)(iVar7 + 0x35) & 1) == 0) || (iVar8 < *(int *)(iVar7 + 6))) {
          uVar12 = *(undefined2 *)(iVar7 + 0x1c);
          pbVar5 = (byte *)(*(int *)(iVar7 + 0x1a) + iVar8 * *(int *)(iVar7 + 8) * 2 +
                           ((int)param_2 >> (DAT_451b_2a3e & 0x1f)));
        }
        else {
          uVar13 = (undefined2)((ulong)*(undefined4 *)(iVar7 + 0x37) >> 0x10);
          iVar10 = (int)*(undefined4 *)(iVar7 + 0x37);
          uVar12 = *(undefined2 *)(iVar10 + 0x1c);
          pbVar5 = (byte *)(*(int *)(iVar10 + 0x1a) +
                            (iVar8 - *(int *)(iVar7 + 6)) * *(int *)(iVar10 + 8) * 2 +
                           ((int)param_2 >> (DAT_451b_2a3e & 0x1f)));
        }
        local_8 = (byte *)CONCAT22(uVar12,pbVar5);
        *local_8 = *local_8 & bVar3 | bVar4;
      }
    }
  }
  else {
    local_3 = 1;
    pbVar6 = (byte *)(7 - (param_2 & 7));
    pbVar5 = pbVar6;
    for (local_12 = 0; local_12 < (int)(uint)*(byte *)(iVar7 + 0x34); local_12 = local_12 + 1) {
      iVar8 = iVar7 + local_12 * 4;
      if ((*(int *)(iVar8 + 10) != 0 || *(int *)(iVar8 + 0xc) != 0) &&
         ((*(uint *)(iVar7 + 0x35) & 10) == 0)) {
        uVar12 = mem_lock(*(undefined4 *)(iVar7 + local_12 * 4 + 10));
        iVar8 = iVar7 + local_12 * 4;
        *(undefined2 *)(iVar8 + 0x1c) = pbVar5;
        *(undefined2 *)(iVar8 + 0x1a) = uVar12;
      }
      FUN_1e68_0018();
      local_10 = (byte)pbVar6;
      iVar8 = *(int *)(iVar7 + local_12 * 4 + 0x1a);
      iVar10 = *(int *)(iVar7 + 8);
      bVar1 = DAT_451b_2a3e & 0x1f;
      iVar2 = *(int *)(iVar7 + local_12 * 4 + 0x1a);
      bVar3 = DAT_451b_2a3e & 0x1f;
      iVar9 = iVar7 + local_12 * 4;
      pbVar5 = (byte *)(*(int *)(iVar9 + 0x1a) + param_3 * *(int *)(iVar7 + 8) * 2 +
                       ((int)param_2 >> (DAT_451b_2a3e & 0x1f)));
      local_8 = (byte *)CONCAT22(*(undefined2 *)(iVar9 + 0x1c),pbVar5);
      for (iVar9 = param_3; iVar9 < param_3 + param_4; iVar9 = iVar9 + 1) {
        bVar4 = ~('\x01' << (local_10 & 0x1f));
        pbVar5 = (byte *)CONCAT11((char)((uint)pbVar5 >> 8),bVar4);
        *local_8 = *local_8 & bVar4 | ((param_5 & local_3) != 0) << (local_10 & 0x1f);
        local_8 = (byte *)CONCAT22(local_8._2_2_,
                                   (byte *)local_8 +
                                   ((iVar8 + iVar10 * 2 + ((int)param_2 >> bVar1)) -
                                   (iVar2 + ((int)param_2 >> bVar3))));
      }
      local_3 = local_3 << 1;
      iVar8 = iVar7 + local_12 * 4;
      if ((*(int *)(iVar8 + 10) != 0 || *(int *)(iVar8 + 0xc) != 0) &&
         ((*(uint *)(iVar7 + 0x35) & 10) == 0)) {
        mem_unlock(*(undefined4 *)(iVar7 + local_12 * 4 + 10));
      }
    }
  }
  return;
}


