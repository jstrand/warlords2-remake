
void __cdecl16far auto_ui_triumphs(int param_1)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_SS;
  char *pcVar3;
  undefined2 uVar4;
  undefined1 local_e [4];
  undefined1 local_a [8];
  
  FUN_22bf_07a5();
  if (param_1 != 0) {
    if (DAT_4125_166a == 0) {
      DAT_4125_166a = 1;
      FUN_54f6_0000(0);
      FUN_78a8_06ae(1,0xf,0,3);
      pcVar3 = get_string(0x50,0);
      FUN_7ecb_00d6(0x140,0x40,pcVar3);
    }
    else {
      FUN_54f6_070b(0x40,0x75,0x170,0xa8);
    }
  }
  for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
    FUN_216d_002e(local_a);
    FUN_216d_000e(local_e);
    __F_SPUSH(local_e._0_2_,local_e._2_2_);
    FUN_1997_0129(4);
    FUN_216d_002e(local_a);
    FUN_216d_000e(local_e);
    __F_SPUSH();
    FUN_1997_027e();
  }
  FUN_8611_08be(4,DAT_451b_04c2,0x342a,0x4125,0x68,0xa8,0,0,0,1,0,0);
  FUN_8611_08be(0x19,DAT_451b_04c2,0x342a,0x4125,0x68,0xcb,0,0,0,1,0,0);
  FUN_8611_08be(0x1c,DAT_451b_04c2,0x342a,0x4125,0x68,0xee,0,0,0,1,0,0);
  FUN_8611_08be(5,DAT_451b_04c2,0x342a,0x4125,0x68,0x111,0,0,0,1,0,0);
  FUN_8611_08be(0x1d,DAT_451b_04c2,0x342a,0x4125,0x68,0x134,0,0,0,1,0,0);
  FUN_78a8_06ae(2,0xf,0,3);
  if (current_player == DAT_451b_04c2) {
    iVar1 = 0x51;
  }
  else {
    iVar1 = 0x56;
  }
  for (iVar2 = 0; iVar2 < 5; iVar2 = iVar2 + 1) {
    if (0 < *(int *)(current_player * 0x50 + DAT_451b_04c2 * 10 + iVar2 * 2 + 0x1163)) {
      uVar4 = *(undefined2 *)(current_player * 0x50 + DAT_451b_04c2 * 10 + iVar2 * 2 + 0x1163);
      pcVar3 = get_string(iVar1 + iVar2,
                          (uint)(*(int *)(current_player * 0x50 + DAT_451b_04c2 * 10 + iVar2 * 2 +
                                         0x1163) != 1));
      sprintf((char *)CONCAT22(unaff_SS,&stack0xff9e),pcVar3,uVar4);
      FUN_7ecb_00bf(0x90,iVar2 * 0x23 + 0xb1,&stack0xff9e);
    }
  }
  FUN_22bf_0757();
  return;
}


