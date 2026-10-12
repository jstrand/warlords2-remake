
void __cdecl16far auto_ui_diplomacy_events(void)

{
  int *piVar1;
  byte *pbVar2;
  uint uVar3;
  int iVar4;
  char *fmt;
  undefined4 uVar5;
  
  uVar3 = *(byte *)(DAT_451b_01fc * 0xe0 + DAT_451b_01fe * 2 + 1) & 0xf;
  if (uVar3 == 0xf) {
    return;
  }
  if ((*(byte *)(current_player * 8 + uVar3 + 0x153b) & 3) == 0) {
    iVar4 = dice(1,100,0);
    iVar4 = iVar4 + 100;
  }
  else {
    if ((*(byte *)(current_player * 8 + uVar3 + 0x153b) & 3) != 1) goto LAB_67cc_214b;
    iVar4 = dice(1,0xf,0);
    iVar4 = iVar4 + 10;
  }
  piVar1 = (int *)(current_player * 2 + 0x10e3);
  *piVar1 = *piVar1 + iVar4;
LAB_67cc_214b:
  iVar4 = current_player * 8 + uVar3;
  pbVar2 = (byte *)(iVar4 + 0x153b);
  *pbVar2 = *pbVar2 & 0xfc;
  pbVar2 = (byte *)(iVar4 + 0x153b);
  *pbVar2 = *pbVar2 | 2;
  iVar4 = uVar3 * 8 + current_player;
  pbVar2 = (byte *)(iVar4 + 0x153b);
  *pbVar2 = *pbVar2 & 0xfc;
  pbVar2 = (byte *)(iVar4 + 0x153b);
  *pbVar2 = *pbVar2 | 2;
  iVar4 = current_player * 8 + uVar3;
  pbVar2 = (byte *)(iVar4 + 0x153b);
  *pbVar2 = *pbVar2 & 0xf3;
  pbVar2 = (byte *)(iVar4 + 0x153b);
  *pbVar2 = *pbVar2 | 8;
  iVar4 = uVar3 * 8 + current_player;
  pbVar2 = (byte *)(iVar4 + 0x153b);
  *pbVar2 = *pbVar2 & 0xf3;
  pbVar2 = (byte *)(iVar4 + 0x153b);
  *pbVar2 = *pbVar2 | 8;
  iVar4 = is_computer_turn();
  if (iVar4 == 0) {
    FUN_8065_011f();
    attack_tile(DAT_451b_01fe,DAT_451b_01fc);
  }
  iVar4 = is_computer_turn();
  if ((iVar4 != 0) && (DAT_3c04_0159 != 0)) {
    uVar5 = CONCAT22(0x3c04,uVar3 * 0x14);
    fmt = get_string(0x6f,2);
    sprintf(&DAT_451b_1d28,fmt,uVar5);
    FUN_8065_11d1();
  }
  FUN_6d51_1244(current_player,8,current_player,uVar3,0);
  return;
}


