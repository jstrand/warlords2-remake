
void __cdecl16far FUN_4c49_02f9(int param_1)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  local_a = 0;
  local_c = 0;
  uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
  *(undefined4 *)((int)DAT_451b_002a + param_1 * 0x50 + 0x104) =
       *(undefined4 *)((int)DAT_451b_002a + param_1 * 4 + 0x244);
  uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
  iVar2 = (int)DAT_451b_002a + param_1 * 2;
  *(undefined2 *)(iVar2 + 0xfc) = 1;
  iVar3 = (int)DAT_451b_002a;
  uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
  if ((*(int *)(iVar2 + 0x2c) < 3) ||
     (((((iVar2 = iVar3 + param_1 * 4, *(int *)(iVar2 + 0x244) == 0 ||
         (*(int *)(iVar2 + 0x246) == 0)) || (*(int *)(iVar2 + 0x244) == 0x6f)) ||
       (*(int *)(iVar2 + 0x246) == 0x9b)) &&
      (((iVar2 = iVar3 + param_1 * 4, *(int *)(iVar2 + 0x254) == 0 || (*(int *)(iVar2 + 0x256) == 0)
        ) || ((*(int *)(iVar2 + 0x254) == 0x6f || (*(int *)(iVar2 + 0x256) == 0x9b)))))))) {
    piVar1 = (int *)(iVar3 + param_1 * 2 + 0xfc);
    *piVar1 = *piVar1 + 1;
    *(undefined4 *)((int)DAT_451b_002a + param_1 * 0x50 + 0x108) =
         *(undefined4 *)((int)DAT_451b_002a + param_1 * 4 + 0x254);
  }
  else {
    local_8 = FUN_4c49_0fed(*(undefined4 *)(iVar3 + param_1 * 4 + 0x254));
    local_8 = local_8 / (*(int *)((int)DAT_451b_002a + param_1 * 2 + 0x2c) + -1);
    while( true ) {
      uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      if (local_c != 0) break;
      FUN_4c49_0dd0(*(undefined4 *)((int)DAT_451b_002a + param_1 * 0x50 + local_a * 4 + 0x104),
                    *(undefined4 *)((int)DAT_451b_002a + param_1 * 4 + 0x254));
      uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      local_e = *(int *)((int)DAT_451b_002a + param_1 * 0x50 + local_a * 4 + 0x104) +
                local_6 * local_8;
      iVar2 = *(int *)((int)DAT_451b_002a + param_1 * 0x50 + local_a * 4 + 0x106) +
              local_4 * local_8;
      FUN_4bed_04ae(&local_e);
      local_a = local_a + 1;
      *(undefined1 *)((int)DAT_451b_002a + iVar2 * 0x70 + local_e + 0x6120) = 7;
      FUN_4bed_03aa(local_e,iVar2);
      FUN_216d_000e((int)DAT_451b_002a + param_1 * 0x50 + local_a * 4 + 0x104,
                    (int)((ulong)DAT_451b_002a >> 0x10),local_e,iVar2);
      uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      piVar1 = (int *)((int)DAT_451b_002a + param_1 * 2 + 0xfc);
      *piVar1 = *piVar1 + 1;
      iVar2 = FUN_4c49_0fed(*(undefined4 *)((int)DAT_451b_002a + param_1 * 4 + 0x254));
      if ((iVar2 < local_8) ||
         (uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10),
         *(int *)((int)DAT_451b_002a + param_1 * 2 + 0x2c) + -1 <=
         *(int *)((int)DAT_451b_002a + param_1 * 2 + 0xfc))) {
        local_c = 1;
      }
    }
    *(undefined4 *)((int)DAT_451b_002a + param_1 * 0x50 + (local_a + 1) * 4 + 0x104) =
         *(undefined4 *)((int)DAT_451b_002a + param_1 * 4 + 0x254);
    piVar1 = (int *)((int)DAT_451b_002a + param_1 * 2 + 0xfc);
    *piVar1 = *piVar1 + 1;
  }
  return;
}


