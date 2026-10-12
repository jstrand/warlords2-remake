
void __cdecl16far
auto_str_cannot_allocate_mask(int *param_1,undefined4 param_2,undefined4 param_3,uint param_4)

{
  int iVar1;
  undefined2 uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  undefined2 in_DX;
  undefined4 *puVar11;
  int iVar12;
  int iVar13;
  undefined2 uVar14;
  undefined2 uVar15;
  undefined2 unaff_SS;
  undefined4 local_1c;
  uint *local_18;
  uint *local_14;
  uint *local_10;
  int local_c;
  uint *local_a;
  uint local_6;
  int local_4;
  
  if (DAT_4125_3384 != 0) {
    iVar13 = (int)param_2;
    uVar14 = (undefined2)((ulong)param_2 >> 0x10);
    if (*param_1 == 0) {
      iVar1 = FUN_2012_0232(param_3,*(undefined2 *)(iVar13 + 4));
      if (iVar1 == 0) {
        printf(s_cannot_allocate_mask_4125_3787);
        return;
      }
      *param_1 = 1;
    }
    if ((*(long *)(iVar13 + 10) != 0) && ((*(uint *)(iVar13 + 0x35) & 10) == 0)) {
      for (local_c = 0; local_c < (int)(uint)*(byte *)(iVar13 + 0x34); local_c = local_c + 1) {
        uVar15 = mem_lock();
        iVar1 = iVar13 + local_c * 4;
        *(undefined2 *)(iVar1 + 0x1c) = in_DX;
        *(undefined2 *)(iVar1 + 0x1a) = uVar15;
      }
    }
    uVar15 = (undefined2)((ulong)param_3 >> 0x10);
    iVar1 = (int)param_3;
    if ((*(long *)(iVar1 + 10) != 0) && ((*(uint *)(iVar1 + 0x35) & 10) == 0)) {
      for (local_c = 0; local_c < (int)(uint)*(byte *)(iVar1 + 0x34); local_c = local_c + 1) {
        uVar2 = mem_lock();
        iVar12 = iVar1 + local_c * 4;
        *(undefined2 *)(iVar12 + 0x1c) = in_DX;
        *(undefined2 *)(iVar12 + 0x1a) = uVar2;
      }
    }
    for (local_6 = 0; local_6 < *(byte *)(iVar13 + 0x34); local_6 = local_6 + 1) {
      puVar11 = &local_1c + local_6;
      iVar12 = iVar13 + local_6 * 4;
      uVar2 = *(undefined2 *)(iVar12 + 0x1a);
      *(undefined2 *)((int)&local_1c + local_6 * 4 + 2) = *(undefined2 *)(iVar12 + 0x1c);
      *(undefined2 *)puVar11 = uVar2;
    }
    local_a = (uint *)*(undefined4 *)(iVar1 + 0x1a);
    local_4 = *(int *)(iVar13 + 8) * *(int *)(iVar13 + 6);
    while (local_4 != 0) {
      if ((param_4 & 1) == 0) {
        uVar3 = *local_1c;
      }
      else {
        uVar3 = 0;
      }
      if ((param_4 & 2) == 0) {
        uVar4 = *local_18;
      }
      else {
        uVar4 = 0;
      }
      if ((param_4 & 4) == 0) {
        uVar5 = *local_14;
      }
      else {
        uVar5 = 0;
      }
      if ((param_4 & 8) == 0) {
        uVar6 = *local_10;
      }
      else {
        uVar6 = 0;
      }
      if ((param_4 & 1) == 0) {
        uVar7 = 0xffff;
      }
      else {
        uVar7 = *local_1c;
      }
      if ((param_4 & 2) == 0) {
        uVar8 = 0xffff;
      }
      else {
        uVar8 = *local_18;
      }
      if ((param_4 & 4) == 0) {
        uVar9 = 0xffff;
      }
      else {
        uVar9 = *local_14;
      }
      if ((param_4 & 8) == 0) {
        uVar10 = 0xffff;
      }
      else {
        uVar10 = *local_10;
      }
      *local_a = ((uVar3 | uVar4 | uVar5 | uVar6) ^ 0xffff) & uVar7 & uVar8 & uVar9 & uVar10;
      local_1c = (uint *)CONCAT22(local_1c._2_2_,(uint *)local_1c + 1);
      local_18 = (uint *)CONCAT22(local_18._2_2_,(uint *)local_18 + 1);
      local_14 = (uint *)CONCAT22(local_14._2_2_,(uint *)local_14 + 1);
      local_10 = (uint *)CONCAT22(local_10._2_2_,(uint *)local_10 + 1);
      local_a = (uint *)CONCAT22(local_a._2_2_,(uint *)local_a + 1);
      local_4 = local_4 + -1;
    }
    local_4 = 0xffff;
    if ((*(long *)(iVar13 + 10) != 0) && ((*(uint *)(iVar13 + 0x35) & 10) == 0)) {
      for (iVar12 = 0; iVar12 < (int)(uint)*(byte *)(iVar13 + 0x34); iVar12 = iVar12 + 1) {
        mem_unlock(*(undefined4 *)(iVar13 + iVar12 * 4 + 10));
      }
    }
    if ((*(long *)(iVar1 + 10) != 0) && ((*(uint *)(iVar1 + 0x35) & 10) == 0)) {
      for (iVar13 = 0; iVar13 < (int)(uint)*(byte *)(iVar1 + 0x34); iVar13 = iVar13 + 1) {
        mem_unlock(*(undefined4 *)(iVar1 + iVar13 * 4 + 10));
      }
    }
  }
  return;
}


