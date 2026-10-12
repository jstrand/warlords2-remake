
void __cdecl16far
FUN_1f00_0884(undefined4 param_1,int param_2,int param_3,uint param_4,int param_5,undefined4 param_6
             ,uint param_7,int param_8)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  bool bVar5;
  byte bVar6;
  uint uVar7;
  uint uVar8;
  byte *pbVar9;
  undefined2 in_DX;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  undefined2 uVar16;
  undefined2 uVar17;
  undefined2 uVar18;
  char local_14;
  int local_12;
  byte local_10;
  undefined4 local_e;
  undefined4 local_a;
  byte local_3;
  
  uVar16 = (undefined2)((ulong)param_1 >> 0x10);
  iVar11 = (int)param_1;
  uVar17 = (undefined2)((ulong)DAT_4125_34e7 >> 0x10);
  if ((*(uint *)(iVar11 + 4) < *(uint *)((int)DAT_4125_34e7 + 4)) ||
     (*(uint *)(iVar11 + 6) < *(uint *)((int)DAT_4125_34e7 + 6))) {
    bVar5 = false;
  }
  else {
    bVar5 = true;
  }
  if (((!bVar5) && (*(long *)(iVar11 + 10) != 0)) && ((*(uint *)(iVar11 + 0x35) & 10) == 0)) {
    for (iVar14 = 0; iVar14 < (int)(uint)*(byte *)(iVar11 + 0x34); iVar14 = iVar14 + 1) {
      iVar15 = iVar11 + iVar14 * 4;
      uVar17 = mem_lock(*(undefined2 *)(iVar15 + 10),*(undefined2 *)(iVar15 + 0xc));
      iVar15 = iVar11 + iVar14 * 4;
      *(undefined2 *)(iVar15 + 0x1c) = in_DX;
      *(undefined2 *)(iVar15 + 0x1a) = uVar17;
    }
  }
  uVar7 = param_7 & DAT_451b_2a44;
  uVar8 = param_4 & DAT_451b_2a44;
  iVar14 = (int)param_4 >> (DAT_451b_2a3e & 0x1f);
  for (iVar15 = 0; iVar15 < (int)(uint)*(byte *)(iVar11 + 0x34); iVar15 = iVar15 + 1) {
    if (((bVar5) &&
        (iVar12 = iVar11 + iVar15 * 4, *(int *)(iVar12 + 10) != 0 || *(int *)(iVar12 + 0xc) != 0))
       && ((*(uint *)(iVar11 + 0x35) & 10) == 0)) {
      iVar12 = iVar11 + iVar15 * 4;
      uVar17 = mem_lock(*(undefined2 *)(iVar12 + 10),*(undefined2 *)(iVar12 + 0xc));
      iVar12 = iVar11 + iVar15 * 4;
      *(undefined2 *)(iVar12 + 0x1c) = in_DX;
      *(undefined2 *)(iVar12 + 0x1a) = uVar17;
    }
    FUN_1e68_0018(iVar15);
    if (((*(uint *)(iVar11 + 0x35) & 1) == 0) || (param_3 < *(int *)(iVar11 + 6))) {
      iVar12 = iVar11 + iVar15 * 4;
      uVar17 = *(undefined2 *)(iVar12 + 0x1c);
      pbVar9 = (byte *)(*(int *)(iVar12 + 0x1a) + param_3 * *(int *)(iVar11 + 8) * 2 +
                       (param_2 >> (DAT_451b_2a3e & 0x1f)));
    }
    else {
      uVar18 = (undefined2)((ulong)*(undefined4 *)(iVar11 + 0x37) >> 0x10);
      iVar12 = (int)*(undefined4 *)(iVar11 + 0x37) + iVar15 * 4;
      uVar17 = *(undefined2 *)(iVar12 + 0x1c);
      pbVar9 = (byte *)(*(int *)(iVar12 + 0x1a) +
                        (param_3 - *(int *)(iVar11 + 6)) *
                        *(int *)((int)*(undefined4 *)(iVar11 + 0x37) + 8) * 2 +
                       (param_2 >> (DAT_451b_2a3e & 0x1f)));
    }
    local_a = (byte *)CONCAT22(uVar17,pbVar9);
    uVar17 = (undefined2)((ulong)param_6 >> 0x10);
    iVar12 = (int)param_6;
    if (((*(uint *)(iVar12 + 0x35) & 1) == 0) || (param_8 < *(int *)(iVar12 + 6))) {
      iVar13 = iVar12 + iVar15 * 4;
      in_DX = *(undefined2 *)(iVar13 + 0x1c);
      pbVar9 = (byte *)(*(int *)(iVar13 + 0x1a) + param_8 * *(int *)(iVar12 + 8) * 2 +
                       ((int)param_7 >> (DAT_451b_2a3e & 0x1f)));
    }
    else {
      uVar18 = (undefined2)((ulong)*(undefined4 *)(iVar12 + 0x37) >> 0x10);
      iVar13 = (int)*(undefined4 *)(iVar12 + 0x37) + iVar15 * 4;
      in_DX = *(undefined2 *)(iVar13 + 0x1c);
      pbVar9 = (byte *)(*(int *)(iVar13 + 0x1a) +
                        (param_8 - *(int *)(iVar12 + 6)) *
                        *(int *)((int)*(undefined4 *)(iVar12 + 0x37) + 8) * 2 +
                       ((int)param_7 >> (DAT_451b_2a3e & 0x1f)));
    }
    local_e = (byte *)CONCAT22(in_DX,pbVar9);
    iVar13 = *(int *)(iVar11 + 8);
    iVar12 = *(int *)(iVar12 + 8);
    local_12 = param_5;
    while (local_12 != 0) {
      iVar10 = iVar14;
      local_3 = *local_e & *(byte *)(uVar7 + 0x3402);
      while (local_10 = (byte)uVar7, iVar10 != 0) {
        bVar6 = *local_a;
        bVar1 = *(byte *)(uVar7 + 0x33fa);
        bVar2 = *(byte *)(uVar7 + 0x3412);
        *local_e = (byte)((int)(char)*local_a >> (local_10 & 0x1f)) & *(byte *)(uVar7 + 0x340a) |
                   local_3;
        local_a = (byte *)CONCAT22(local_a._2_2_,(byte *)local_a + 1);
        local_e = (byte *)CONCAT22(local_e._2_2_,(byte *)local_e + 1);
        iVar10 = iVar10 + -1;
        local_3 = (bVar6 & bVar1) << (bVar2 & 0x1f);
      }
      local_14 = (char)uVar8;
      bVar6 = local_14 + local_10;
      if ((int)DAT_451b_2a44 < (int)(uint)bVar6) {
        bVar1 = *local_a;
        bVar2 = *(byte *)(uVar7 + 0x33fa);
        bVar3 = *(byte *)(uVar7 + 0x3412);
        bVar4 = *(byte *)(((uint)bVar6 - DAT_451b_2a48) + 0x3402);
        *local_e = (byte)((int)(char)*local_a >> (local_10 & 0x1f)) & *(byte *)(uVar7 + 0x340a) |
                   local_3;
        bVar6 = ((byte *)local_e)[1] & *(byte *)(((uint)bVar6 - DAT_451b_2a48) + 0x340a) |
                (bVar1 & bVar2) << (bVar3 & 0x1f) & bVar4;
        in_DX = CONCAT11(0xff,bVar6);
        ((byte *)local_e)[1] = bVar6;
      }
      else {
        in_DX = 0xffff;
        if (bVar6 != 0) {
          if (uVar8 != 0) {
            local_3 = local_3 | (byte)((int)(uint)(*local_a & *(byte *)(uVar8 + 0x3402)) >>
                                      (local_10 & 0x1f));
          }
          local_3 = *local_e & *(byte *)(bVar6 + 0x340a) | local_3;
          in_DX = CONCAT11(0xff,local_3);
          *local_e = local_3;
        }
      }
      local_e = (byte *)CONCAT22(local_e._2_2_,(byte *)local_e + (iVar12 * 2 - iVar14));
      local_a = (byte *)CONCAT22(local_a._2_2_,(byte *)local_a + (iVar13 * 2 - iVar14));
      local_12 = local_12 + -1;
    }
    iVar12 = iVar11 + iVar15 * 4;
    if ((*(int *)(iVar12 + 10) != 0 || *(int *)(iVar12 + 0xc) != 0) &&
       ((*(uint *)(iVar11 + 0x35) & 10) == 0)) {
      iVar12 = iVar11 + iVar15 * 4;
      mem_unlock(*(undefined2 *)(iVar12 + 10),*(undefined2 *)(iVar12 + 0xc));
    }
  }
  return;
}


