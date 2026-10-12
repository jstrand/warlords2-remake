
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_ui_save_game_2(int param_1)

{
  int iVar1;
  char *pcVar2;
  char *pcVar3;
  char *pcVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  undefined2 uVar9;
  undefined2 uVar10;
  undefined4 uVar11;
  
  if (param_1 < 0) {
    game_screen_start(0);
  }
  else {
    _LAB_4125_128b_1 = param_1;
    _LAB_4125_128d_1 = param_1;
    uVar11 = 0xa0000f;
    uVar10 = 1;
    uVar9 = 0x4014;
    uVar8 = 0x57;
    uVar7 = 0x407f;
    uVar6 = 0x84;
    iVar1 = param_1 * 0xf + 0x471c;
    uVar5 = 0x4125;
    pcVar2 = get_string(0x2e,2);
    pcVar3 = get_string(0x2e,1);
    pcVar4 = get_string(0x2e,0);
    FUN_7b4c_0000((char *)pcVar4,(int)((ulong)pcVar4 >> 0x10),pcVar3,pcVar2,iVar1,uVar5,uVar6,uVar7,
                  uVar8,uVar9,uVar10,uVar11);
  }
  return;
}


