
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_ui_save_map_2(int param_1)

{
  int iVar1;
  char *pcVar2;
  char *pcVar3;
  char *pcVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  
  if (param_1 == -1) {
    if (DAT_4125_28c6 == 0) {
      if (DAT_4125_2a86 == 0) {
        auto_ui_options_menu(0,0x10000);
      }
      else {
        game_screen_start(0);
      }
    }
    else {
      FUN_7f77_0000();
    }
  }
  else {
    _LAB_4125_128b_1 = param_1;
    _LAB_4125_128f_1 = param_1;
    uVar10 = 0xa0000f;
    uVar9 = 0x14014;
    uVar8 = 0x57;
    uVar7 = 0x407f;
    uVar6 = 0xa7;
    iVar1 = param_1 * 0xf + 0x467c;
    uVar5 = 0x4125;
    pcVar2 = get_string(0x30,2);
    pcVar3 = get_string(0x30,1);
    pcVar4 = get_string(0x30,0);
    FUN_7b4c_0000((char *)pcVar4,(int)((ulong)pcVar4 >> 0x10),pcVar3,pcVar2,iVar1,uVar5,uVar6,uVar7,
                  uVar8,uVar9,uVar10);
  }
  return;
}


