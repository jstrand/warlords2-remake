
void __cdecl16far FUN_78a8_0839(undefined4 param_1,uint param_2,uint param_3)

{
  int iVar1;
  long lVar2;
  uint *puVar3;
  uint *puVar4;
  uint *puVar5;
  uint *puVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  undefined2 uVar18;
  undefined2 uVar19;
  undefined2 uVar20;
  undefined2 unaff_SS;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  uint *local_28;
  uint *local_24;
  uint *local_20;
  uint local_1c;
  uint local_1a;
  uint local_18;
  uint local_16;
  int local_14;
  int local_12;
  int local_10;
  uint local_e;
  uint local_c;
  uint local_a;
  uint local_8;
  undefined4 local_6;
  
  uVar18 = (undefined2)((ulong)param_1 >> 0x10);
  iVar15 = (int)param_1;
  local_6 = *(undefined4 *)(iVar15 + 4);
  local_10 = 0;
  local_12 = *(int *)(iVar15 + 0xd);
  lVar2 = (long)(*(int *)(iVar15 + 0xf) + -1) * (long)*(int *)(iVar15 + 0xd);
  uVar18 = (undefined2)((ulong)lVar2 >> 0x10);
  local_14 = (int)lVar2;
  local_16 = param_2 & 1;
  local_18 = param_2 & 2;
  local_1a = param_2 & 4;
  local_1c = param_2 & 8;
  if ((param_3 & 1) == 0) {
    local_e = 0;
  }
  else {
    local_e = 0xffff;
  }
  if ((param_3 & 2) == 0) {
    local_c = 0;
  }
  else {
    local_c = 0xffff;
  }
  if ((param_3 & 4) == 0) {
    local_a = 0;
  }
  else {
    local_a = 0xffff;
  }
  if ((param_3 & 8) == 0) {
    local_8 = 0;
  }
  else {
    local_8 = 0xffff;
  }
  uVar19 = (undefined2)((ulong)local_6 >> 0x10);
  if ((*(long *)((int)local_6 + 10) != 0) && ((*(uint *)((int)local_6 + 0x35) & 10) == 0)) {
    for (iVar15 = 0; iVar15 < (int)(uint)*(byte *)((int)local_6 + 0x34); iVar15 = iVar15 + 1) {
      uVar19 = mem_lock();
      uVar20 = (undefined2)((ulong)local_6 >> 0x10);
      iVar16 = (int)local_6 + iVar15 * 4;
      *(undefined2 *)(iVar16 + 0x1c) = uVar18;
      *(undefined2 *)(iVar16 + 0x1a) = uVar19;
    }
  }
  for (iVar15 = 0; uVar18 = (undefined2)((ulong)local_6 >> 0x10), iVar16 = (int)local_6,
      iVar15 < (int)(uint)*(byte *)(iVar16 + 0x34); iVar15 = iVar15 + 1) {
    iVar17 = iVar16 + iVar15 * 4;
    iVar1 = *(int *)(iVar17 + 0x1a);
    iVar16 = *(int *)(iVar16 + 8) * local_10;
    *(undefined2 *)((int)&local_2c + iVar15 * 4 + 2) = *(undefined2 *)(iVar17 + 0x1c);
    *(int *)(&local_2c + iVar15) = iVar1 + iVar16 * 2;
  }
  iVar15 = 0;
  while( true ) {
    uVar18 = (undefined2)((ulong)local_6 >> 0x10);
    iVar16 = (int)local_6;
    if ((int)(uint)*(byte *)(iVar16 + 0x34) <= iVar15) break;
    iVar17 = iVar16 + iVar15 * 4;
    iVar1 = *(int *)(iVar17 + 0x1a);
    iVar16 = *(int *)(iVar16 + 8) * local_14;
    *(undefined2 *)((int)&local_3c + iVar15 * 4 + 2) = *(undefined2 *)(iVar17 + 0x1c);
    *(int *)(&local_3c + iVar15) = iVar1 + iVar16 * 2;
    iVar15 = iVar15 + 1;
  }
  iVar15 = (local_12 - local_10) * *(int *)(iVar16 + 8);
  while (puVar6 = local_30, puVar5 = local_34, puVar4 = local_38, puVar3 = local_3c, iVar15 != 0) {
    if (local_16 == 0) {
      uVar7 = *local_2c;
    }
    else {
      uVar7 = 0;
    }
    if (local_18 == 0) {
      uVar8 = *local_28;
    }
    else {
      uVar8 = 0;
    }
    if (local_1a == 0) {
      uVar9 = *local_24;
    }
    else {
      uVar9 = 0;
    }
    if (local_1c == 0) {
      uVar10 = *local_20;
    }
    else {
      uVar10 = 0;
    }
    if (local_16 == 0) {
      uVar11 = 0xffff;
    }
    else {
      uVar11 = *local_2c;
    }
    if (local_18 == 0) {
      uVar12 = 0xffff;
    }
    else {
      uVar12 = *local_28;
    }
    if (local_1a == 0) {
      uVar13 = 0xffff;
    }
    else {
      uVar13 = *local_24;
    }
    if (local_1c == 0) {
      uVar14 = 0xffff;
    }
    else {
      uVar14 = *local_20;
    }
    uVar14 = ((uVar7 | uVar8 | uVar9 | uVar10) ^ 0xffff) & uVar11 & uVar12 & uVar13 & uVar14;
    uVar8 = uVar14 ^ 0xffff;
    uVar7 = *local_3c;
    local_3c = (uint *)CONCAT22((int)((ulong)local_3c >> 0x10),(uint *)local_3c + 1);
    *puVar3 = local_e & uVar14 | uVar7 & uVar8;
    uVar7 = *local_38;
    local_38 = (uint *)CONCAT22((int)((ulong)local_38 >> 0x10),(uint *)local_38 + 1);
    *puVar4 = local_c & uVar14 | uVar7 & uVar8;
    uVar7 = *local_34;
    local_34 = (uint *)CONCAT22((int)((ulong)local_34 >> 0x10),(uint *)local_34 + 1);
    *puVar5 = local_a & uVar14 | uVar7 & uVar8;
    uVar7 = *local_30;
    local_30 = (uint *)CONCAT22((int)((ulong)local_30 >> 0x10),(uint *)local_30 + 1);
    *puVar6 = local_8 & uVar14 | uVar7 & uVar8;
    local_2c = (uint *)CONCAT22(local_2c._2_2_,(uint *)local_2c + 1);
    local_28 = (uint *)CONCAT22(local_28._2_2_,(uint *)local_28 + 1);
    local_24 = (uint *)CONCAT22(local_24._2_2_,(uint *)local_24 + 1);
    local_20 = (uint *)CONCAT22(local_20._2_2_,(uint *)local_20 + 1);
    iVar15 = iVar15 + -1;
  }
  if ((*(long *)(iVar16 + 10) != 0) && ((*(uint *)(iVar16 + 0x35) & 10) == 0)) {
    for (iVar15 = 0; uVar18 = (undefined2)((ulong)local_6 >> 0x10),
        iVar15 < (int)(uint)*(byte *)((int)local_6 + 0x34); iVar15 = iVar15 + 1) {
      mem_unlock(*(undefined4 *)((int)local_6 + iVar15 * 4 + 10));
    }
  }
  return;
}


