
void __cdecl16far auto_ui_fighting_order(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_SS;
  char *pcVar4;
  undefined2 uVar5;
  char local_52 [80];
  
  FUN_22bf_07a5();
  if (param_1 == 0) {
    FUN_78a8_06ae(1,0xf,0,3);
    pcVar4 = get_string(0x7f,0);
    FUN_7ecb_00d6(0x140,0x40,pcVar4);
    FUN_8611_0bf7(current_player,0,0x342a,0x4125,0x58,0x40);
    FUN_8611_0bf7(current_player,0,0x342a,0x4125,0x200,0x40);
    FUN_78a8_06ae(2,0xf,0,3);
    iVar3 = current_player * 0x14;
    uVar5 = 0x3c04;
    pcVar4 = get_string(0x7f,1);
    sprintf((char *)CONCAT22(unaff_SS,local_52),pcVar4,iVar3,uVar5);
    FUN_7ecb_00d6(0x140,0x68,local_52);
    pcVar4 = get_string(0x7f,2);
    FUN_7ecb_00d6(0x140,0x15c,pcVar4);
    pcVar4 = get_string(0x7f,3);
    FUN_7ecb_00d6(0x140,0x170,pcVar4);
    pcVar4 = get_string(0x7f,4);
    FUN_7ecb_00d6(0x140,0x184,pcVar4);
  }
  FUN_78a8_06ae(2,0xf,0,3);
  for (iVar3 = 0; iVar3 < 0x1b; iVar3 = iVar3 + 1) {
    iVar2 = 0;
    while ((iVar2 < 0x1d && (*(char *)(current_player * 0x1d + iVar2 + 0x60b) != iVar3))) {
      iVar2 = iVar2 + 1;
    }
    if (DAT_4125_0d56 == iVar3) {
      iVar1 = current_player + 2;
    }
    else {
      iVar1 = 1;
    }
    FUN_8611_08be(iVar2,current_player,0x342a,0x4125,(iVar3 % 4) * 0x78 + 0x58,
                  (iVar3 / 4) * 0x1f + 0x80,0,0,0,0,iVar1,0,0);
    if (param_1 == 0) {
      sprintf((char *)CONCAT22(unaff_SS,local_52),(char *)0x41250d5c,iVar3 + 1);
      FUN_7ecb_00bf((iVar3 % 4) * 0x78 + 0x80,(iVar3 / 4) * 0x1f + 0x86,local_52);
    }
  }
  FUN_22bf_0757();
  return;
}


