
int __cdecl16far FUN_623c_0749(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined2 uVar1;
  char *pcVar2;
  undefined2 in_DX;
  byte *pbVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined2 unaff_SS;
  byte local_7c [49];
  byte local_4b [51];
  char *local_18;
  int local_16;
  int local_14;
  int local_12;
  int local_10;
  int local_c;
  char *local_a;
  int local_8;
  undefined2 local_6;
  undefined2 uStack_4;
  
  uVar1 = FUN_1555_1b0a();
  _local_6 = CONCAT22(in_DX,uVar1);
  local_10 = 0;
  local_8 = 0x32;
  while (local_8 != 0) {
    pbVar3 = local_4b + local_8;
    local_8 = local_8 + -1;
    *pbVar3 = 0xff;
    local_7c[local_8] = 0;
  }
  local_8 = city_count;
  while (iVar7 = local_8 + -1, local_8 != 0) {
    local_8 = iVar7;
    if ((iVar7 != param_1) && (pcVar2 = (char *)FUN_59bf_0a85(iVar7,&local_c), local_c != -1)) {
      local_10 = local_10 + 1;
      iVar7 = -1;
      local_a = pcVar2;
      iVar5 = 0;
      while ((iVar6 = iVar7, iVar5 < 0x32 && (iVar6 = iVar5, local_4b[iVar5 + 1] != 0xff))) {
        if ((int)local_a < (int)(uint)local_7c[iVar5]) {
          local_a = (char *)(uint)local_7c[iVar5];
          iVar7 = iVar5;
        }
        iVar5 = iVar5 + 1;
      }
      if ((iVar6 != -1) &&
         ((local_4b[iVar6 + 1] == 0xff || ((int)pcVar2 < (int)(uint)local_7c[iVar6])))) {
        local_4b[iVar6 + 1] = (byte)local_8;
        local_7c[iVar6] = (byte)pcVar2;
      }
    }
  }
  iVar7 = 0;
  local_16 = 0;
  local_14 = 0;
  local_12 = 0;
  iVar5 = 0;
  do {
    local_18 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
    local_8 = 0x32;
    iVar6 = -1;
    while( true ) {
      iVar4 = local_8;
      local_8 = local_8 + -1;
      if (iVar4 == 0) break;
      if (local_4b[iVar4] != 0xff) {
        local_a = (char *)(uint)local_7c[iVar4 + -1];
        if (((((*(int *)((uint)local_4b[iVar4] * 0x41 + 0x157d) < *(int *)(param_1 * 0x41 + 0x157d))
              && (2 < local_12)) ||
             ((*(int *)(param_1 * 0x41 + 0x157d) < *(int *)((uint)local_4b[iVar4] * 0x41 + 0x157d)
              && (2 < iVar5)))) ||
            ((*(int *)((uint)local_4b[iVar4] * 0x41 + 0x157f) < *(int *)(param_1 * 0x41 + 0x157f) &&
             (2 < local_16)))) ||
           ((*(int *)(param_1 * 0x41 + 0x157f) < *(int *)((uint)local_4b[iVar4] * 0x41 + 0x157f) &&
            (2 < local_14)))) {
          local_a = (char *)(uint)local_7c[iVar4 + -1] + 0x32;
        }
        if (local_a < local_18) {
          local_18 = local_a;
          iVar6 = local_8;
        }
      }
    }
    if (iVar6 == -1) break;
    uVar1 = (undefined2)((ulong)param_2 >> 0x10);
    iVar4 = (int)param_2;
    *(byte *)(iVar4 + iVar7) = local_4b[iVar6 + 1];
    if (*(int *)((uint)*(byte *)(iVar4 + iVar7) * 0x41 + 0x157d) < *(int *)(param_1 * 0x41 + 0x157d)
       ) {
      local_12 = local_12 + 1;
    }
    if (*(int *)(param_1 * 0x41 + 0x157d) < *(int *)((uint)*(byte *)(iVar4 + iVar7) * 0x41 + 0x157d)
       ) {
      iVar5 = iVar5 + 1;
    }
    if (*(int *)((uint)*(byte *)(iVar4 + iVar7) * 0x41 + 0x157f) < *(int *)(param_1 * 0x41 + 0x157f)
       ) {
      local_16 = local_16 + 1;
    }
    if (*(int *)(param_1 * 0x41 + 0x157f) < *(int *)((uint)*(byte *)(iVar4 + iVar7) * 0x41 + 0x157f)
       ) {
      local_14 = local_14 + 1;
    }
    *(byte *)((int)param_3 + iVar7) = local_7c[iVar6];
    iVar7 = iVar7 + 1;
    local_4b[iVar6 + 1] = 0xff;
  } while (iVar7 != 6);
  FUN_1555_1b2e();
  return local_10;
}


