
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far ui_side_setup_list(int param_1)

{
  long lVar1;
  int iVar2;
  int iVar3;
  int iStack0004;
  long in_stack_00000002;
  undefined4 local_a;
  int local_6;
  int local_4;
  
  lVar1 = in_stack_00000002;
  iStack0004 = (int)((ulong)in_stack_00000002 >> 0x10);
  if (lVar1 < 0) {
    local_4 = 8;
    iVar3 = 0;
  }
  else {
    local_4 = iStack0004 + 1;
    iVar3 = iStack0004;
  }
  if (iStack0004 == -2) {
    iVar3 = local_4;
  }
  FUN_22bf_07a5();
  if (lVar1 < 0) {
    FUN_78a8_06ae(2,*(undefined2 *)(current_player * 2 + 0xa0),
                  *(undefined2 *)(current_player * 2 + 0xb0),3);
    FUN_7ecb_0103(DAT_4125_0ae4,0xc0c,0x4125);
    FUN_7ecb_00d6(DAT_4125_0ae8,0xc11,0x4125);
    FUN_7ecb_00d6(DAT_4125_0aec,0xc17,0x4125);
    FUN_7ecb_00d6(DAT_4125_0af0,0xc20,0x4125);
  }
  for (; iVar3 < local_4; iVar3 = iVar3 + 1) {
    FUN_216d_000e(&local_a);
    if ((-1 < lVar1) && (iVar2 = *(int *)(iVar3 * 8 + 0xb96) + -0x32, param_1 != 0)) {
      FUN_54f6_07e4(0,iVar2,0,0x14b - iVar2);
    }
    FUN_78a8_06ae(2,*(undefined2 *)(iVar3 * 2 + 0xa0),*(undefined2 *)(iVar3 * 2 + 0xb0),3);
    FUN_7ecb_0103(*(undefined2 *)(iVar3 * 4 + 0xaf4),*(undefined2 *)(iVar3 * 4 + 0xaf6),iVar3 * 0x14
                  ,0x3c04);
    FUN_78a8_06ae(0xf0002,0x30000);
    if ((*(int *)(iVar3 * 2 + 0x137) == 0) &&
       (iVar2 = strcmp((char *)CONCAT22(0x3c04,(char *)(iVar3 * 0x14)),s_Not_used_4125_0c28),
       iVar2 != 0)) {
      FUN_7ecb_00bf(*(undefined2 *)(iVar3 * 4 + 0xb34),*(undefined2 *)(iVar3 * 4 + 0xb36),0xc31);
    }
    else if (*(int *)(iVar3 * 2 + 0x137) != 0) {
      __F_SPUSH(*(undefined4 *)(iVar3 * 4 + 0xb14));
      FUN_1997_0129();
      if (*(int *)(iVar3 * 2 + 0xd0) == 0) {
        local_6 = 3;
      }
      else {
        local_6 = *(int *)(iVar3 * 2 + 0xc0);
      }
      local_a = *(undefined4 *)(local_6 * 4 + 0xac4);
      FUN_7ecb_00bf(*(undefined2 *)(iVar3 * 4 + 0xb34),*(undefined2 *)(iVar3 * 4 + 0xb36));
      local_a = *(undefined4 *)(iVar3 * 4 + 0xb54);
      __F_SPUSH();
      FUN_1997_0129(8);
      if (*(int *)(iVar3 * 2 + 0xd0) != 0) {
        local_4 = 0x1997;
        local_6 = 0x5119;
        __F_SPUSH();
        local_4 = 8;
        local_6 = 0x1000;
        local_a = CONCAT22(0x5120,(undefined2)local_a);
        FUN_1997_0129();
      }
    }
  }
  FUN_78a8_06ae(0xf0002,0x30000);
  __F_SPUSH(_DAT_4125_0bd4);
  FUN_1997_0129();
  local_a = DAT_4125_0bd8;
  __F_SPUSH();
  FUN_1997_0129(8);
  local_4 = 0x1997;
  local_6 = 0x51a6;
  __F_SPUSH();
  local_4 = 8;
  local_6 = 0x1000;
  local_a = CONCAT22(0x51ad,(undefined2)local_a);
  FUN_1997_0129();
  FUN_7ecb_00bf();
  FUN_7ecb_00bf();
  FUN_7ecb_00bf();
  FUN_22bf_0757();
  return;
}


