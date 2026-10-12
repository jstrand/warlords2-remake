
void __cdecl16far FUN_4c49_0087(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined4 local_6;
  
  uVar3 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
  iVar2 = (int)DAT_451b_002a;
  if (*(int *)(iVar2 + param_1 * 2 + 0x2c) < 3) {
    *(undefined4 *)(iVar2 + param_1 * 4 + 0x244) = *(undefined4 *)(iVar2 + param_1 * 8 + 0x8c);
    local_6 = *(undefined4 *)((int)DAT_451b_002a + param_1 * 8 + 0x90);
    iVar2 = (int)DAT_451b_002a + param_1 * 4;
  }
  else {
    uVar1 = *(undefined4 *)(iVar2 + param_1 * 8 + 0x8c);
    iVar2 = iVar2 + param_1 * 4;
    local_6._0_2_ = (int)uVar1;
    local_6._2_2_ = (int)((ulong)uVar1 >> 0x10);
    local_6 = CONCAT22(local_6._2_2_ + *(int *)(iVar2 + 0xae) * -10,
                       (int)local_6 + *(int *)(iVar2 + 0xac) * -10);
    FUN_4bed_04ae(&local_6);
    *(undefined1 *)((int)DAT_451b_002a + local_6._2_2_ * 0x70 + (int)local_6 + 0x6120) = 7;
    FUN_4bed_03aa(local_6);
    uVar3 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    *(undefined4 *)((int)DAT_451b_002a + param_1 * 4 + 0x244) = local_6;
    uVar1 = *(undefined4 *)((int)DAT_451b_002a + param_1 * 8 + 0x90);
    iVar2 = (int)DAT_451b_002a + ((param_1 + 1) % 4) * 4;
    local_6._0_2_ = (int)uVar1;
    local_6._2_2_ = (int)((ulong)uVar1 >> 0x10);
    local_6 = CONCAT22(local_6._2_2_ + *(int *)(iVar2 + 0xae) * -10,
                       (int)local_6 + *(int *)(iVar2 + 0xac) * -10);
    FUN_4bed_04ae(&local_6);
    *(undefined1 *)((int)DAT_451b_002a + local_6._2_2_ * 0x70 + (int)local_6 + 0x6120) = 7;
    FUN_4bed_03aa(local_6);
    uVar3 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    iVar2 = (int)DAT_451b_002a + param_1 * 4;
  }
  *(undefined4 *)(iVar2 + 0x254) = local_6;
  return;
}


