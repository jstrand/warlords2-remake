
void __cdecl16far auto_ui_diplomacy_state(int param_1)

{
  undefined2 uVar1;
  uint in_DX;
  int iVar2;
  int iVar3;
  uint unaff_SS;
  char *pcVar4;
  undefined4 local_e;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  if (DAT_4125_166a == 0) {
    FUN_54f6_0000();
  }
  if (param_1 == 0) {
    FUN_78a8_06ae(0xf0001,0x30000);
    pcVar4 = get_string(0x6c,0);
    FUN_7ecb_00d6(0x400140,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    FUN_8611_0bf7(current_player,0,0x342a,0x4125,0x6f0060);
    FUN_78a8_06ae(0xf0002,0x30000);
    FUN_7ecb_00bf(0x7a0090,current_player * 0x14,0x3c04);
    pcVar4 = get_string(0x6e,0);
    FUN_7ecb_00bf(0xa50068,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    pcVar4 = get_string(0x6e,1);
    FUN_7ecb_00bf(0xcd0068,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    pcVar4 = get_string(0x6e,2);
    FUN_7ecb_00bf(0xfb0068,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    pcVar4 = get_string(0x6e,3);
    FUN_7ecb_00bf(0x10f0068,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    pcVar4 = get_string(0x6e,4);
    FUN_7ecb_00bf(0x1230068,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    pcVar4 = get_string(0x6e,5);
    FUN_7ecb_00bf(0x1370068,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    pcVar4 = get_string(0x6e,6);
    in_DX = (uint)((ulong)pcVar4 >> 0x10);
    FUN_7ecb_00bf(0x14b0068,pcVar4);
  }
  iVar3 = 0;
  for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
    if (current_player != iVar2) {
      FUN_8611_0bf7(iVar2,0,0x342a,0x4125,iVar3 * 0x28 + 0x110,0x6f);
      if (*(int *)(iVar2 * 2 + 0x137) == 0) {
        FUN_24d0_0002();
        uVar1 = FUN_216d_002e(&local_a);
        FUN_216d_01fd(uVar1,in_DX);
        local_a = local_a + 1;
        local_8 = local_8 + 1;
        local_6 = local_6 + -2;
        local_4 = local_4 + -2;
        FUN_24d0_0002();
        FUN_216d_0288(&local_a);
      }
      else {
        FUN_216d_002e(&local_a);
        FUN_216d_000e(&local_e);
        in_DX = unaff_SS;
        __F_SPUSH(local_e);
        FUN_1997_0129();
      }
      if ((*(int *)(iVar2 * 2 + 0x137) == 0) ||
         (in_DX = (*(byte *)(iVar2 * 8 + current_player + 0x153b) & 0xc) >> 2,
         (*(byte *)(iVar2 * 8 + current_player + 0x153b) & 3) == in_DX)) {
        FUN_24d0_0002();
        uVar1 = FUN_216d_002e(&local_a);
        FUN_216d_01fd(uVar1,in_DX);
        local_a = local_a + 1;
        local_8 = local_8 + 1;
        local_6 = local_6 + -2;
        local_4 = local_4 + -2;
        FUN_24d0_0002();
        FUN_216d_0288(&local_a);
      }
      else {
        FUN_216d_002e(&local_a);
        FUN_216d_000e(&local_e);
        in_DX = unaff_SS;
        __F_SPUSH(local_e);
        FUN_1997_0129();
      }
      if (*(int *)(iVar2 * 2 + 0x137) == 0) {
        FUN_24d0_0002();
        uVar1 = FUN_216d_002e(&local_a);
        FUN_216d_01fd(uVar1,in_DX);
        local_a = local_a + 1;
        local_8 = local_8 + 1;
        local_6 = local_6 + -2;
        local_4 = local_4 + -2;
        FUN_24d0_0002();
        FUN_216d_0288(&local_a);
      }
      else {
        FUN_216d_002e(&local_a);
        FUN_216d_000e(&local_e);
        in_DX = unaff_SS;
        __F_SPUSH(local_e);
        FUN_1997_0129();
      }
      if (*(int *)(iVar2 * 2 + 0x137) == 0) {
        FUN_24d0_0002();
        uVar1 = FUN_216d_002e(&local_a);
        FUN_216d_01fd(uVar1,in_DX);
        local_a = local_a + 1;
        local_8 = local_8 + 1;
        local_6 = local_6 + -2;
        local_4 = local_4 + -2;
        FUN_24d0_0002();
        FUN_216d_0288(&local_a);
      }
      else {
        FUN_216d_002e(&local_a);
        FUN_216d_000e(&local_e);
        in_DX = unaff_SS;
        __F_SPUSH(local_e);
        FUN_1997_0129();
      }
      if (*(int *)(iVar2 * 2 + 0x137) == 0) {
        FUN_24d0_0002();
        uVar1 = FUN_216d_002e(&local_a);
        FUN_216d_01fd(uVar1);
        local_a = local_a + 1;
        local_8 = local_8 + 1;
        local_6 = local_6 + -2;
        local_4 = local_4 + -2;
        FUN_24d0_0002();
        FUN_216d_0288();
      }
      else {
        FUN_216d_002e(&local_a);
        FUN_216d_000e(&local_e);
        in_DX = unaff_SS;
        __F_SPUSH();
        FUN_1997_0129();
      }
      iVar3 = iVar3 + 1;
    }
  }
  return;
}


