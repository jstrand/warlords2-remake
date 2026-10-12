
void __cdecl16far FUN_4e47_00b5(void)

{
  int *piVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  int local_34 [8];
  int local_24;
  int local_22;
  int local_20;
  int local_1e;
  int local_1c;
  int local_1a;
  int local_18;
  int local_16;
  int local_14;
  int local_12;
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  bVar2 = false;
  while (!bVar2) {
    local_4 = dice(1,0x70,-1);
    local_6 = dice(1,0x9c,-1);
    if (*(char *)((int)DAT_451b_002a + local_6 * 0x70 + local_4 + 0x6120) == '\a') {
      bVar2 = true;
    }
  }
  iVar3 = dice(1,100,0);
  if (iVar3 < 0x41) {
    for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
      iVar4 = dice(1,8,2);
      local_34[iVar3] = iVar4;
      local_20 = 4;
      local_22 = 2;
    }
  }
  else {
    for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
      iVar4 = dice(1,10,5);
      local_34[iVar3] = iVar4;
      local_20 = 6;
      local_22 = 4;
    }
  }
  *(undefined1 *)((int)DAT_451b_002a + local_6 * 0x70 + local_4 + 0x6120) = 4;
  FUN_4bed_03aa(local_4);
  piVar1 = (int *)((int)DAT_451b_002a + 0x1f52);
  *piVar1 = *piVar1 + 1;
  for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
    local_e = local_6;
    local_c = local_4;
    local_a = iVar3;
    local_24 = 0;
    for (local_8 = 0; local_8 < local_34[local_a]; local_8 = local_8 + 1) {
      uVar6 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar4 = (int)DAT_451b_002a + local_a * 4;
      local_c = local_c + *(int *)(iVar4 + 0xbc);
      local_e = local_e + *(int *)(iVar4 + 0xbe);
      FUN_4bed_0470(&local_c);
      uVar6 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      if (*(char *)((int)DAT_451b_002a + local_e * 0x70 + local_c + 0x6120) == '\a') {
        *(undefined1 *)((int)DAT_451b_002a + local_e * 0x70 + local_c + 0x6120) = 4;
        FUN_4bed_03aa(local_c);
        piVar1 = (int *)((int)DAT_451b_002a + 0x1f52);
        *piVar1 = *piVar1 + 1;
      }
      local_18 = (local_a + 2) % 8;
      local_1a = (local_a + 6) % 8;
      local_24 = local_24 + 1;
      local_1c = dice(1,local_20 - local_24 / 2,local_22 - local_24 / 4);
      local_1e = dice(1,local_20 - local_24 / 2,local_22 - local_24 / 3);
      local_10 = local_c;
      local_12 = local_e;
      for (iVar4 = 0; iVar4 < local_1c; iVar4 = iVar4 + 1) {
        uVar6 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar5 = (int)DAT_451b_002a + local_18 * 4;
        local_10 = local_10 + *(int *)(iVar5 + 0xbc);
        local_12 = local_12 + *(int *)(iVar5 + 0xbe);
        FUN_4bed_0470(&local_10);
        uVar6 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        if (*(char *)((int)DAT_451b_002a + local_12 * 0x70 + local_10 + 0x6120) == '\a') {
          *(undefined1 *)((int)DAT_451b_002a + local_12 * 0x70 + local_10 + 0x6120) = 4;
          FUN_4bed_03aa(local_10);
          piVar1 = (int *)((int)DAT_451b_002a + 0x1f52);
          *piVar1 = *piVar1 + 1;
        }
      }
      local_14 = local_c;
      local_16 = local_e;
      for (iVar4 = 0; iVar4 < local_1e; iVar4 = iVar4 + 1) {
        uVar6 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar5 = (int)DAT_451b_002a + local_1a * 4;
        local_14 = local_14 + *(int *)(iVar5 + 0xbc);
        local_16 = local_16 + *(int *)(iVar5 + 0xbe);
        FUN_4bed_0470(&local_14);
        uVar6 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        if (*(char *)((int)DAT_451b_002a + local_16 * 0x70 + local_14 + 0x6120) == '\a') {
          *(undefined1 *)((int)DAT_451b_002a + local_16 * 0x70 + local_14 + 0x6120) = 4;
          FUN_4bed_03aa(local_14);
          piVar1 = (int *)((int)DAT_451b_002a + 0x1f52);
          *piVar1 = *piVar1 + 1;
        }
      }
    }
  }
  return;
}


