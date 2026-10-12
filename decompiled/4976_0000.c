
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_ui_temple(void)

{
  undefined2 unaff_SS;
  char *pcVar1;
  undefined4 uVar2;
  undefined2 uVar3;
  char local_54 [80];
  int local_4;
  
  music_cue(5);
  local_4 = FUN_828e_04b5(*DAT_451b_1f02,((undefined2 *)DAT_451b_1f02)[1]);
  FUN_22bf_07a5();
  FUN_54f6_0000();
  FUN_78a8_06ae(0x70002,0x10006);
  uVar2 = CONCAT22(0x3c04,local_4 * 0x1f + 0x815);
  pcVar1 = get_string(0x13,0);
  sprintf((char *)CONCAT22(unaff_SS,local_54),pcVar1,uVar2);
  FUN_7ecb_00d6(0x10e0140,local_54);
  pcVar1 = get_string(0x13,1);
  FUN_7ecb_00d6(0x1220140,(char *)pcVar1,(int)((ulong)pcVar1 >> 0x10));
  pcVar1 = get_string(0x13,2);
  FUN_7ecb_00d6(0x1360140,(char *)pcVar1,(int)((ulong)pcVar1 >> 0x10));
  auto_str_join_data_not_found();
  DAT_4125_166a = 1;
  FUN_18a9_066e(0x10148);
  if (((opt_quests == 0) || (*(int *)(current_player * 0xc + 0x1103) != 0)) || (_DAT_451b_1efe == 0)
     ) {
    uVar3 = 2;
  }
  else {
    uVar3 = 1;
  }
  FUN_18a9_066e(0x149,uVar3);
  FUN_22bf_0757();
  return;
}


