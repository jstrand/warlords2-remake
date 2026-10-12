
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_ui_hero_emerges(void)

{
  int iVar1;
  undefined2 unaff_SS;
  char *pcVar2;
  undefined2 uVar3;
  undefined4 local_ea;
  char *pcStack_e6;
  char local_b8 [50];
  char local_86 [50];
  char local_54 [80];
  int local_4;
  
  FUN_22bf_07a5();
  local_4 = game_turn();
  DAT_4125_166a = 1;
  FUN_54f6_0000(2);
  FUN_834b_0000(1,1,0,0);
  FUN_834b_1f5f(*(undefined2 *)(DAT_451b_058c * 0x41 + 0x157d),
                *(undefined2 *)(DAT_451b_058c * 0x41 + 0x157f),1);
  FUN_834b_04d3(0,0);
  load_hero_name();
  if (local_4 == 1) {
    if (DAT_451b_0570 == 0) {
      pcVar2 = get_string(0x61,0);
      sprintf((char *)CONCAT22(unaff_SS,local_54),pcVar2);
      pcVar2 = get_string(0x61,1);
      sprintf((char *)CONCAT22(unaff_SS,local_86),pcVar2);
      pcVar2 = get_string(0x61,2);
      sprintf((char *)CONCAT22(unaff_SS,local_b8),pcVar2);
      iVar1 = DAT_451b_058c * 0x41 + 0x1581;
      uVar3 = 0x3c04;
      pcVar2 = get_string(0x61,3);
      sprintf((char *)CONCAT22(unaff_SS,&local_ea),pcVar2,iVar1,uVar3);
    }
    else {
      pcVar2 = get_string(0x61,8);
      sprintf((char *)CONCAT22(unaff_SS,local_54),pcVar2);
      pcVar2 = get_string(0x61,9);
      sprintf((char *)CONCAT22(unaff_SS,local_86),pcVar2);
      pcVar2 = get_string(0x61,10);
      sprintf((char *)CONCAT22(unaff_SS,local_b8),pcVar2);
      iVar1 = DAT_451b_058c * 0x41 + 0x1581;
      uVar3 = 0x3c04;
      pcVar2 = get_string(0x61,0xb);
      sprintf((char *)CONCAT22(unaff_SS,&local_ea),pcVar2,iVar1,uVar3);
    }
  }
  else {
    if (DAT_451b_0570 == 0) {
      iVar1 = DAT_451b_058c * 0x41 + 0x1581;
      uVar3 = 0x3c04;
      pcVar2 = get_string(0x61,4);
      sprintf((char *)CONCAT22(unaff_SS,local_54),pcVar2,iVar1,uVar3);
      uVar3 = _LAB_4125_110f_1;
      pcVar2 = get_string(0x61,5);
      sprintf((char *)CONCAT22(unaff_SS,local_86),pcVar2,uVar3);
      uVar3 = *(undefined2 *)(current_player * 0x14 + 0x185);
      pcVar2 = get_string(0x61,6);
      sprintf((char *)CONCAT22(unaff_SS,local_b8),pcVar2,uVar3);
      iVar1 = 7;
    }
    else {
      iVar1 = DAT_451b_058c * 0x41 + 0x1581;
      uVar3 = 0x3c04;
      pcVar2 = get_string(0x61,0xc);
      sprintf((char *)CONCAT22(unaff_SS,local_54),pcVar2,iVar1,uVar3);
      uVar3 = _LAB_4125_110f_1;
      pcVar2 = get_string(0x61,0xd);
      sprintf((char *)CONCAT22(unaff_SS,local_86),pcVar2,uVar3);
      uVar3 = *(undefined2 *)(current_player * 0x14 + 0x185);
      pcVar2 = get_string(0x61,0xe);
      sprintf((char *)CONCAT22(unaff_SS,local_b8),pcVar2,uVar3);
      iVar1 = 0xf;
    }
    pcVar2 = get_string(0x61,iVar1);
    sprintf((char *)CONCAT22(unaff_SS,&local_ea),pcVar2);
  }
  FUN_78a8_06ae(1,0xf,0,3);
  pcVar2 = get_string(0x5f,0);
  FUN_7ecb_00d6((int)_LAB_4125_1125_1,(int)((ulong)_LAB_4125_1125_1 >> 0x10),pcVar2);
  uVar3 = (undefined2)((ulong)_LAB_4125_1117_3 >> 0x10);
  if (DAT_451b_0570 == 0) {
    __F_SPUSH((int)_LAB_4125_1117_3,uVar3);
    uVar3 = 0x21;
  }
  else {
    __F_SPUSH((int)_LAB_4125_1117_3,uVar3);
    uVar3 = 0x22;
  }
  FUN_1997_0129(uVar3);
  _pcStack_e6 = (char *)0x1997;
  local_ea = CONCAT22(0x66b3,(undefined2)local_ea);
  FUN_24d0_0002();
  _pcStack_e6 = &LAB_4125_111e;
  local_ea = 0x24d066bd;
  FUN_216d_01fd();
  _pcStack_e6 = (char *)0x3000e;
  local_ea = 0xf0002;
  FUN_78a8_06ae();
  _pcStack_e6 = (char *)CONCAT22(unaff_SS,local_54);
  local_ea = _LAB_4125_1129_1;
  FUN_7ecb_00d6();
  _pcStack_e6 = (char *)CONCAT22(unaff_SS,local_86);
  local_ea = _LAB_4125_112d_1;
  FUN_7ecb_00d6();
  _pcStack_e6 = (char *)CONCAT22(unaff_SS,local_b8);
  local_ea = _LAB_4125_1131_1;
  FUN_7ecb_00d6();
  _pcStack_e6 = (char *)CONCAT22(unaff_SS,&local_ea);
  local_ea = _LAB_4125_1135_1;
  FUN_7ecb_00d6();
  _pcStack_e6 = (char *)0x30000;
  local_ea = 0xf0002;
  FUN_78a8_06ae();
  _pcStack_e6 = (char *)0x412544d2;
  local_ea = _LAB_4125_113d_1;
  FUN_7ecb_0058((int)_LAB_4125_1139_1,(int)((ulong)_LAB_4125_1139_1 >> 0x10));
  _pcStack_e6 = (char *)0x7ecb;
  local_ea = CONCAT22(0x6750,(undefined2)local_ea);
  FUN_24d0_0002();
  _pcStack_e6 = (char *)0x41251142;
  local_ea = 0x24d0675a;
  FUN_216d_01fd();
  _pcStack_e6 = (char *)_LAB_4125_1149_1;
  local_ea = 0x216d6780;
  __F_SPUSH();
  local_ea = 0x81000;
  FUN_1997_0129();
  __F_SPUSH();
  _pcStack_e6 = (char *)CONCAT22(0x67b4,pcStack_e6);
  FUN_1997_0129();
  get_string(0x62,0);
  FUN_7ecb_0103();
  get_string(0x62,1);
  FUN_7ecb_0103();
  auto_str_join_data_not_found();
  FUN_7563_120c();
  FUN_22bf_0757();
  if ((opt_tutorial != 0) && ((DAT_3c04_0134 & 1) == 0)) {
    DAT_3c04_0134 = DAT_3c04_0134 | 1;
    get_file_string(0x19,0);
    FUN_8065_168d();
  }
  return;
}


