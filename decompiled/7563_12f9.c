
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_ui_hero_emerges_2(int param_1)

{
  int iVar1;
  undefined2 unaff_SS;
  char *pcVar2;
  undefined2 uVar3;
  char local_98 [50];
  char local_66 [50];
  char local_34 [50];
  
  DAT_451b_0570 = param_1;
  FUN_22bf_07a5();
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
  iVar1 = game_turn();
  if (iVar1 == 1) {
    if (DAT_451b_0570 == 0) {
      pcVar2 = get_string(0x61,0);
      sprintf((char *)CONCAT22(unaff_SS,local_34),pcVar2);
      pcVar2 = get_string(0x61,1);
      sprintf((char *)CONCAT22(unaff_SS,local_66),pcVar2);
      pcVar2 = get_string(0x61,2);
      sprintf((char *)CONCAT22(unaff_SS,local_98),pcVar2);
      pcVar2 = get_string(0x61,3);
      sprintf((char *)CONCAT22(unaff_SS,&stack0xff36),pcVar2);
    }
    else {
      pcVar2 = get_string(0x61,8);
      sprintf((char *)CONCAT22(unaff_SS,local_34),pcVar2);
      pcVar2 = get_string(0x61,9);
      sprintf((char *)CONCAT22(unaff_SS,local_66),pcVar2);
      pcVar2 = get_string(0x61,10);
      sprintf((char *)CONCAT22(unaff_SS,local_98),pcVar2);
      pcVar2 = get_string(0x61,0xb);
      sprintf((char *)CONCAT22(unaff_SS,&stack0xff36),pcVar2);
    }
  }
  else {
    if (DAT_451b_0570 == 0) {
      pcVar2 = get_string(0x61,4);
      sprintf((char *)CONCAT22(unaff_SS,local_34),pcVar2);
      pcVar2 = get_string(0x61,5);
      sprintf((char *)CONCAT22(unaff_SS,local_66),pcVar2);
      pcVar2 = get_string(0x61,6);
      sprintf((char *)CONCAT22(unaff_SS,local_98),pcVar2);
      iVar1 = 7;
    }
    else {
      pcVar2 = get_string(0x61,0xc);
      sprintf((char *)CONCAT22(unaff_SS,local_34),pcVar2);
      pcVar2 = get_string(0x61,0xd);
      sprintf((char *)CONCAT22(unaff_SS,local_66),pcVar2);
      pcVar2 = get_string(0x61,0xe);
      sprintf((char *)CONCAT22(unaff_SS,local_98),pcVar2);
      iVar1 = 0xf;
    }
    pcVar2 = get_string(0x61,iVar1);
    sprintf((char *)CONCAT22(unaff_SS,&stack0xff36),pcVar2);
  }
  FUN_78a8_06ae();
  FUN_7ecb_00d6();
  FUN_7ecb_00d6();
  FUN_7ecb_00d6();
  FUN_7ecb_00d6();
  __F_SPUSH();
  FUN_1997_0129();
  __F_SPUSH();
  FUN_1997_0129();
  FUN_7563_120c();
  FUN_22bf_0757();
  return;
}


