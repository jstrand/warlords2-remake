
void __cdecl16far auto_ui_army_info_labels(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  undefined2 unaff_SS;
  char *pcVar5;
  char local_62 [80];
  int local_12;
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  FUN_22bf_07a5();
  uVar2 = 0x8880;
  iVar3 = *(int *)(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7780);
  iVar4 = *(int *)(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x777e);
  armies_at_tile(iVar3,iVar4,(int *)CONCAT22(unaff_SS,&local_4));
  local_8 = FUN_828e_044a(iVar3,iVar4);
  local_6 = FUN_828e_04fa(iVar3,iVar4,0xe,1);
  if (DAT_4125_166a == 0) {
    DAT_4125_166a = 1;
    FUN_54f6_0000(2);
    FUN_24d0_0002(0);
    uVar1 = FUN_216d_002e(&local_12);
    FUN_216d_01fd(uVar1,uVar2);
    local_12 = local_12 + -1;
    local_10 = local_10 + -1;
    local_e = local_e + 2;
    local_c = local_c + 2;
    FUN_24d0_02e5(&local_12);
  }
  else {
    FUN_54f6_070b(0,0,0x100,0x91);
    FUN_54f6_070b(0,0x11e,0x48,0x15);
  }
  FUN_78a8_06ae(1,0xf,0,3);
  FUN_7ecb_00d6(0x1b0,0x3e,
                *(char *)(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7776) * 0x14 +
                0x223,0x3c04);
  FUN_8611_08be(0x1c,current_player,0x342a,0x4125,0x130,0x6e,0,0,0,1,0,0);
  iVar4 = 1;
  for (iVar3 = 0; iVar3 < local_4; iVar3 = iVar3 + 1) {
    if ((((undefined2 *)&DAT_451b_1ec0)[iVar3 * 2] != 0x2e1d) ||
       (((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7780 !=
        ((undefined2 *)&DAT_451b_1ebe)[iVar3 * 2])) {
      FUN_8611_08be((int)*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ebe + iVar3 * 2) +
                                  4),current_player,0x342a,0x4125,iVar4 * 0x20 + 0x130,0x6e,0,0,0,1,
                    0,0);
      iVar4 = iVar4 + 1;
    }
  }
  for (; iVar4 < 8; iVar4 = iVar4 + 1) {
    FUN_8611_08be(0xffff,current_player,0x342a,0x4125,iVar4 * 0x20 + 0x130,0x6e,0,0,0,1,0,0);
  }
  FUN_78a8_06ae(2,0xf,0,3);
  pcVar5 = get_string(0x83,(uint)(-1 < local_8));
  FUN_7ecb_0103(0x180,0x91,pcVar5);
  pcVar5 = get_string(0x83,2);
  FUN_7ecb_0103(0x180,0xa5,pcVar5);
  pcVar5 = get_string(0x83,3);
  FUN_7ecb_0103(0x180,0xb9,pcVar5);
  pcVar5 = get_string(0x83,4);
  FUN_7ecb_0103(0x210,0xa5,pcVar5);
  pcVar5 = get_string(0x83,5);
  FUN_7ecb_0103(0x210,0xb9,pcVar5);
  iVar3 = DAT_4125_0d6a + 1;
  uVar2 = DAT_4125_0d68;
  pcVar5 = get_string(0x83,6);
  sprintf((char *)CONCAT22(unaff_SS,local_62),pcVar5,iVar3,uVar2);
  FUN_7ecb_00bf(0x138,0x15b,local_62);
  iVar3 = (int)*(char *)(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7778);
  iVar4 = hero_battle_items(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7780,0x2e1d);
  iVar3 = iVar3 + iVar4;
  if (9 < iVar3) {
    iVar3 = 9;
  }
  local_a = hero_command_items(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7780,0x2e1d
                              );
  local_a = *(int *)(iVar3 * 2 + 0xce4) + local_a;
  FUN_7ecb_00bf(0x188,0x91,local_6 * 0x41 + 0x1581,0x3c04);
  uVar2 = hero_battle_items(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7780,0x2e1d);
  sprintf((char *)CONCAT22(unaff_SS,local_62),(char *)0x41250d6e,uVar2);
  FUN_7ecb_00bf(0x188,0xa5,local_62);
  sprintf((char *)CONCAT22(unaff_SS,local_62),(char *)0x41250d72,local_a);
  FUN_7ecb_00bf(0x188,0xb9,local_62);
  sprintf((char *)CONCAT22(unaff_SS,local_62),(char *)0x41250d76,
          (*(byte *)(*(char *)(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7776) +
                    0x5e3) & 3) + 1);
  FUN_7ecb_00bf(0x218,0xa5,local_62);
  sprintf((char *)CONCAT22(unaff_SS,local_62),(char *)0x41250d79,
          (uint)(*(byte *)(*(char *)(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7776)
                          + 0x5e3) >> 2));
  FUN_7ecb_00bf(0x218,0xb9,local_62);
  list_carried_items(6,0,0,((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7780,0x2e1d);
  if (DAT_451b_08ce == 0) {
    DAT_4125_0d6c = 0;
  }
  else {
    DAT_4125_0d6c = (uint)(*(char *)(DAT_451b_0c9c * 0x1d + 0xcff) == '\x01');
  }
  DAT_451b_04a8 = 0xffff;
  if (DAT_451b_08ce < 1) {
    DAT_451b_04aa = 0xffff;
  }
  else {
    DAT_451b_04aa = 0;
  }
  if (DAT_451b_08ce < 2) {
    DAT_451b_04ac = 0xffff;
  }
  else {
    DAT_451b_04ac = 1;
  }
  auto_ui_army_info_labels_2();
  FUN_22bf_0757();
  FUN_18a9_0345();
  return;
}


