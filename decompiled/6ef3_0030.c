
void __cdecl16far auto_ui_reports_menu(int param_1)

{
  int index;
  undefined2 unaff_SS;
  char *pcVar1;
  uint index_00;
  int iVar2;
  char local_52 [80];
  
  if (param_1 != 0) {
    if (DAT_4125_166a == 0) {
      DAT_4125_166a = 1;
      FUN_54f6_0000(2);
    }
    else {
      FUN_54f6_07e4(0,0,0,0x110);
      FUN_54f6_07e4(0,0x58,0,0x1e);
    }
  }
  FUN_78a8_06ae(1,0xf,0,3);
  pcVar1 = get_string(0x49,DAT_4125_0df6);
  FUN_7ecb_00d6((int)DAT_4125_0df8,(int)((ulong)DAT_4125_0df8 >> 0x10),pcVar1);
  FUN_78a8_06ae(2,0xf,0,3);
  pcVar1 = get_string(0x4a,DAT_4125_0df6);
  FUN_7ecb_00d6((int)DAT_4125_0dfc,(int)((ulong)DAT_4125_0dfc >> 0x10),pcVar1);
  index = FUN_6ef3_02fb();
  FUN_6ef3_05ff();
  FUN_78a8_06ae(2,*(undefined2 *)(current_player * 2 + 0xa0),
                *(undefined2 *)(current_player * 2 + 0xb0),3);
  strcpy((char *)CONCAT22(unaff_SS,local_52),(char *)0x41250e42);
  if (4 < DAT_4125_0df6) goto LAB_6ef3_024d;
                    /* WARNING: Switch is manually overridden */
  switch((undefined4)*(undefined2 *)(DAT_4125_0df6 * 2 + 0x262)) {
  case 0x6f073:
    FUN_834b_0000(1,1,0,0);
    FUN_834b_158d(0,0,0xffff,0xffff,0,1);
    FUN_834b_04d3(0,0);
    if (index != 1) {
      iVar2 = 0x4b;
      index_00 = 1;
      goto LAB_6ef3_0185;
    }
    iVar2 = 0x4b;
    index = 0;
    break;
  case 0x6f0cf:
    FUN_834b_0000(1,1,0,0);
    if (index != 1) {
      iVar2 = 0x4c;
      index_00 = 1;
      goto LAB_6ef3_0185;
    }
    iVar2 = 0x4c;
    index = 0;
    break;
  case 0x6f0f3:
    FUN_834b_0000(1,1,0,0);
    compute_income();
    compute_upkeep();
    iVar2 = 0x4d;
    index_00 = 0;
    goto LAB_6ef3_0185;
  case 0x6f114:
    FUN_834b_0000(0,1,0,0);
    FUN_834b_1817(0,0,0xffff);
    FUN_834b_04d3(0,0);
    index_00 = (uint)(index == 1);
    iVar2 = 0x4e;
LAB_6ef3_0185:
    pcVar1 = get_string(iVar2,index_00);
    sprintf((char *)CONCAT22(unaff_SS,local_52),pcVar1,index);
    goto LAB_6ef3_024d;
  case 0x6f152:
    FUN_834b_0000(1,1,0,0);
    iVar2 = 0x4f;
  }
  pcVar1 = get_string(iVar2,index);
  sprintf((char *)CONCAT22(unaff_SS,local_52),pcVar1);
LAB_6ef3_024d:
  FUN_7ecb_00d6((int)DAT_4125_0e00,(int)((ulong)DAT_4125_0e00 >> 0x10),local_52);
  return;
}


