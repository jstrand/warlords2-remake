
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far city_capture_dialog(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  int iVar1;
  undefined2 unaff_SS;
  char *pcVar2;
  undefined2 uVar3;
  char local_a2 [80];
  char local_52 [80];
  
  iVar1 = current_player;
  DAT_4125_0a3e = 1;
  DAT_4125_0a3a = param_3;
  if (param_3 == 0xf) {
    DAT_4125_0a3a = current_player;
  }
  DAT_4125_0a3c = param_4;
  pcVar2 = get_string(0x42,-1);
  sprintf((char *)CONCAT22(unaff_SS,local_52),pcVar2,param_1);
  pcVar2 = get_string(0x43,-1);
  sprintf((char *)CONCAT22(unaff_SS,local_a2),pcVar2,param_2);
  FUN_22bf_07a5();
  FUN_54f6_0000();
  FUN_78a8_06ae(0xf0001,0x30000);
  pcVar2 = get_string(0x41,0);
  FUN_7ecb_00d6(DAT_4125_0a40,(char *)pcVar2,(int)((ulong)pcVar2 >> 0x10));
  FUN_78a8_06ae(0xf0002,0x30000);
  FUN_7ecb_00d6(DAT_4125_0a44,local_52);
  FUN_7ecb_00d6(DAT_4125_0a48,local_a2);
  pcVar2 = get_string(0x41,1);
  FUN_7ecb_00d6(DAT_4125_0a4c,(char *)pcVar2,(int)((ulong)pcVar2 >> 0x10));
  pcVar2 = get_string(0x41,2);
  FUN_7ecb_00d6(DAT_4125_0a50,(char *)pcVar2,(int)((ulong)pcVar2 >> 0x10));
  auto_str_join_data_not_found();
  DAT_4125_166a = 1;
  FUN_17be_0001(0x11d011c);
  FUN_17be_0001(0x11d011b);
  FUN_17be_0001(0x11d011e);
  if ((((opt_quests != 0) && (*(int *)(iVar1 * 0xc + 0x1103) != 0)) && (_DAT_451b_1f00 == 0x2e1d))
     && (*(int *)(iVar1 * 0xc + 0x1107) * 0x16 + -0x7780 == DAT_451b_1efe)) {
    if ((*(int *)(iVar1 * 0xc + 0x1105) == 5) && (*(int *)(iVar1 * 0xc + 0x1109) == param_4)) {
      uVar3 = 0x11c;
    }
    else {
      if (*(int *)(iVar1 * 0xc + 0x1105) != 6) goto LAB_63fa_0210;
      iVar1 = city_sack_value(param_4);
      if (iVar1 == 0) {
        iVar1 = city_pillage_value(param_4);
        if (iVar1 == 0) goto LAB_63fa_0210;
        uVar3 = 0x11b;
      }
      else {
        uVar3 = 0x11e;
      }
    }
    FUN_17be_0001(0x11d,uVar3);
  }
LAB_63fa_0210:
  FUN_18a9_066e(0x1011d);
  city_pillage_value(param_4);
  FUN_18a9_066e(0x11b);
  iVar1 = city_sack_value(param_4);
  if (iVar1 == 0) {
    uVar3 = 2;
  }
  else {
    uVar3 = 1;
  }
  FUN_18a9_066e(0x11e,uVar3);
  FUN_18a9_066e(0x1011c);
  FUN_22bf_0757();
  if ((opt_tutorial != 0) && (DAT_4125_0a54 == 0)) {
    DAT_4125_0a54 = 1;
    pcVar2 = get_file_string(0x19,10);
    FUN_8065_168d((char *)pcVar2,(int)((ulong)pcVar2 >> 0x10));
  }
  return;
}


