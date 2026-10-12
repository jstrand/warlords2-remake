
void __cdecl16far FUN_6d51_16ba(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 in_DX;
  int *piVar3;
  int iVar4;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined1 local_14 [8];
  int *local_c;
  undefined2 uStack_a;
  undefined2 local_8;
  undefined2 local_6;
  int local_4;
  
  local_4 = 0x95;
  local_6 = 0x108;
  local_8 = 0x96;
  FUN_54f6_070b(0x540000,0xaf0160);
  FUN_78a8_06ae(0xf0002,0x30000);
  local_c = (int *)FUN_6d51_11d7();
  local_20 = (int *)CONCAT22(in_DX,local_c);
  local_c = local_c + 1;
  iVar1 = *local_20;
  uStack_a = in_DX;
  for (iVar4 = 0; iVar4 < iVar1; iVar4 = iVar4 + 1) {
    FUN_216d_002e(local_14);
    FUN_216d_000e(&local_18);
    __F_SPUSH(local_18);
    FUN_1997_027e();
    piVar3 = local_c + iVar4 * 0xb + 3;
    uVar2 = auto_ui_history_events();
    local_1c = CONCAT22(piVar3,uVar2);
    FUN_7ecb_00bf(0x118,local_4 + iVar4 * 0x11,uVar2,piVar3);
    if (((*(char *)((int)local_c + iVar4 * 0x16 + 1) == '\t') ||
        (*(char *)((int)local_c + iVar4 * 0x16 + 1) == '\n')) ||
       (*(char *)((int)local_c + iVar4 * 0x16 + 1) == '\b')) {
      uVar2 = FUN_21e2_0a8e(local_1c);
      FUN_216d_002e(local_14);
      FUN_216d_000e(&local_18);
      __F_SPUSH(local_18,uVar2);
      FUN_1997_027e(9);
    }
  }
  return;
}


