
void __cdecl16far FUN_2372_049b(int param_1)

{
  int iVar1;
  char *pcVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined2 *puVar5;
  int *piVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  undefined2 uVar10;
  undefined2 uVar11;
  int local_16;
  undefined4 local_14;
  int local_10;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  uVar10 = 0x2372;
  local_4 = 1;
  local_10 = 0;
  iVar1 = ((int *)DAT_451b_2fc8)[1];
  puVar4 = (undefined4 *)(*DAT_451b_2fc8 + param_1 * 0x2e);
  local_14 = (undefined4 *)CONCAT22(iVar1,puVar4);
  local_16 = 0xc;
  iVar3 = 0x1f;
  while (iVar3 != 0) {
    *(undefined2 *)((iVar3 + -1) * 2 + 0x6f7d) = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
    iVar3 = iVar3 + -1;
  }
  (*DAT_451b_2fc4)(0x2372,0);
  DAT_451b_3011 = param_1;
  *(undefined2 *)(puVar4 + 10) = 0;
  *(undefined2 *)(puVar4 + 9) = 0;
  *(undefined2 *)((int)puVar4 + 0x26) = 2;
  iVar3 = 0;
  while (uVar11 = (undefined2)((ulong)*local_14 >> 0x10),
        piVar6 = (int *)((int)*local_14 + iVar3 * 4), *piVar6 != 0 || piVar6[1] != 0) {
    if (*(char *)*(undefined4 *)((int)*local_14 + iVar3 * 4) == '%') {
      iVar7 = iVar3 + 1;
      local_6 = 0;
    }
    else {
      iVar8 = local_16;
      if (puVar4[8] == -3) {
        iVar8 = 0;
      }
      if ((*(ulong *)((int)puVar4 + 0x1a) >> ((byte)local_4 & 0x1f) & 1) == 0) {
        pcVar2 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
      }
      else {
        pcVar2 = (char *)(*(int *)((int)puVar4 + 0x26) + -1);
      }
      *(undefined2 *)(local_4 * 2 + 0x6f7d) = pcVar2;
      local_4 = local_4 + 1;
      uVar10 = (undefined2)((ulong)*local_14 >> 0x10);
      iVar7 = iVar3 + 1;
      puVar5 = (undefined2 *)((int)*local_14 + iVar3 * 4);
      FUN_21e2_0ab8(*puVar5,puVar5[1],&local_8);
      iVar8 = iVar8 + local_8;
      local_6 = local_a;
      local_c = local_a;
      if (*(char *)*(undefined4 *)((int)*local_14 + iVar7 * 4) == '&') {
        if (*(uint *)(puVar4 + 10) < iVar8 + 5U) {
          *(int *)(puVar4 + 10) = iVar8 + 5;
        }
        uVar9 = iVar8 + 5;
        if (*(uint *)(puVar4 + 10) < uVar9) {
          *(uint *)(puVar4 + 10) = uVar9;
        }
        else {
          uVar9 = *(uint *)(puVar4 + 10);
        }
        uVar10 = (undefined2)((ulong)*local_14 >> 0x10);
        iVar7 = iVar3 + 3;
        puVar5 = (undefined2 *)((int)*local_14 + (iVar3 + 2) * 4);
        FUN_21e2_0ab8(*puVar5,puVar5[1],&local_8);
        iVar3 = local_8;
        if (local_8 < local_10) {
          iVar3 = local_10;
        }
        iVar8 = uVar9 + iVar3;
        local_10 = iVar3;
        if (local_6 < local_a) {
          local_6 = local_a;
        }
      }
      uVar10 = 0x21e2;
      if (*(uint *)(puVar4 + 9) < iVar8 + 5U) {
        *(uint *)(puVar4 + 9) = iVar8 + 5U;
      }
    }
    *(int *)((int)puVar4 + 0x26) = *(int *)((int)puVar4 + 0x26) + local_6 + 2;
    iVar3 = iVar7;
  }
  *(int *)((int)puVar4 + 0x26) = *(int *)((int)puVar4 + 0x26) + 1;
  iVar3 = FUN_2372_0803(param_1);
  if (iVar3 != 0) {
    local_6 = 2;
    local_4 = 1;
    iVar3 = 0;
    while (uVar11 = (undefined2)((ulong)*local_14 >> 0x10),
          piVar6 = (int *)((int)*local_14 + iVar3 * 4), *piVar6 != 0 || piVar6[1] != 0) {
      if (*(char *)*(undefined4 *)((int)*local_14 + iVar3 * 4) == '%') {
        uVar10 = 0x2012;
        FUN_2012_0bb7(0x342a,0x4125,*(int *)((int)puVar4 + 0x16) + 1,
                      *(int *)(puVar4 + 6) + local_6 + -1,*(int *)(puVar4 + 9) + -2,0);
        local_6 = local_6 + 2;
        iVar3 = iVar3 + 1;
      }
      else {
        iVar8 = 3;
        if ((*(ulong *)((int)puVar4 + 0x1a) >> ((byte)local_4 & 0x1f) & 1) == 0) {
          uVar11 = 2;
        }
        else {
          uVar11 = 1;
        }
        (*DAT_451b_2fc4)(uVar10,uVar11);
        if (puVar4[8] != 0) {
          if (((ulong)puVar4[8] >> ((byte)local_4 & 0x1f) & 1) != 0) {
            FUN_21e2_0401(*(int *)((int)puVar4 + 0x16) + 3,*(int *)(puVar4 + 6) + local_6,0x3702,
                          0x4125);
          }
          iVar8 = 0xf;
        }
        local_4 = local_4 + 1;
        uVar10 = (undefined2)((ulong)*local_14 >> 0x10);
        iVar7 = iVar3 + 1;
        puVar5 = (undefined2 *)((int)*local_14 + iVar3 * 4);
        FUN_21e2_0401(*(int *)((int)puVar4 + 0x16) + iVar8,*(int *)(puVar4 + 6) + local_6,*puVar5,
                      puVar5[1]);
        if (*(char *)*(undefined4 *)((int)*local_14 + iVar7 * 4) == '&') {
          uVar10 = (undefined2)((ulong)*local_14 >> 0x10);
          iVar7 = iVar3 + 3;
          puVar5 = (undefined2 *)((int)*local_14 + (iVar3 + 2) * 4);
          FUN_21e2_0401(*(int *)((int)puVar4 + 0x16) + *(int *)(puVar4 + 10),
                        *(int *)(puVar4 + 6) + local_6,*puVar5,puVar5[1]);
        }
        uVar10 = 0x21e2;
        local_6 = local_6 + local_c + 2;
        iVar3 = iVar7;
      }
    }
  }
  return;
}


