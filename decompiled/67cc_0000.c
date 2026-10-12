
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far attack_tile(int x,int y)

{
  byte *pbVar1;
  int iVar2;
  char *pcVar3;
  undefined2 *puVar4;
  uint uVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  char *pcVar7;
  char *pcVar8;
  char *pcVar9;
  char *pcVar10;
  undefined4 uVar11;
  int local_e;
  int local_c;
  int local_a;
  uint local_8;
  undefined2 local_6;
  undefined2 local_4;
  
  DAT_451b_01fe = x;
  DAT_451b_01fc = y;
  if (DAT_451b_1f02 == (undefined2 *)0x0) {
    return;
  }
  local_8 = (uint)*(byte *)(y * 0xe0 + x * 2);
  if (((DAT_451b_1e9c != 2) &&
      (*(int *)((undefined1 *)&terrain_move_cost + *(char *)(local_8 + 0x710) * 2) < 1)) &&
     ((*(byte *)(y * 0x70 + x) & 0x1f) == 0)) {
    sound_chord();
    return;
  }
  iVar2 = FUN_828e_044a(x);
  uVar5 = *(byte *)(y * 0xe0 + x * 2 + 1) & 0xf;
  if (((opt_diplomacy != 0) && (uVar5 != 0xf)) &&
     (((*(byte *)(current_player * 8 + uVar5 + 0x153b) & 3) == 0 ||
      ((0 < iVar2 && ((*(byte *)(current_player * 8 + uVar5 + 0x153b) & 3) == 1)))))) {
    get_string(0x8c,4);
    pcVar10 = get_string(0x8c,3);
    pcVar7 = get_string(0x8c,2);
    pcVar8 = get_string(0x8c,1);
    pcVar9 = get_string(0x8c,0);
    FUN_7b4c_0088((char *)pcVar9,(int)((ulong)pcVar9 >> 0x10),pcVar8,pcVar7,pcVar10);
    return;
  }
  FUN_828e_0af3();
  uVar6 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
  local_4 = *DAT_451b_1f02;
  local_6 = ((undefined2 *)DAT_451b_1f02)[1];
  if (((DAT_4125_2bda != 0) && (DAT_451b_1ea0 < 1)) ||
     ((DAT_4125_2bda == 0 && (*(char *)((int)(undefined2 *)DAT_451b_1f02 + 7) < '\x01')))) {
    FUN_1b62_08b3();
    FUN_89e0_0356();
    return;
  }
  local_8 = (uint)*(byte *)(y * 0xe0 + x * 2);
  if (*(int *)((undefined1 *)&terrain_move_cost + *(char *)(local_8 + 0x710) * 2) < 2) {
    DAT_451b_0200 = 2;
  }
  else {
    DAT_451b_0200 =
         *(undefined2 *)((undefined1 *)&terrain_move_cost + *(char *)(local_8 + 0x710) * 2);
  }
  if ((*(byte *)(y * 0xe0 + x * 2 + 1) & 0xf) == current_player) {
    return;
  }
  if ((*(char *)(local_8 + 0x710) != '\n') && ((*(byte *)(y * 0xe0 + x * 2 + 1) & 0x10) == 0)) {
    return;
  }
  FUN_67cc_1836();
  FUN_67cc_0522(x,y,local_4);
  combat_setup(x,y);
  if (DAT_451b_027d != 0) {
    pbVar1 = (byte *)((int)DAT_451b_027d + 0x30);
    *pbVar1 = *pbVar1 | *(byte *)(current_player * 2 + 0xcbe);
  }
  local_a = combat_resolve();
  ai_record_battle(x,y,(undefined2 *)&combat_def_armies,0x4125,(undefined2 *)&combat_atk_armies);
  FUN_22bf_07a5();
  FUN_6a35_0160();
  FUN_6a35_0094(0);
  FUN_22bf_0757();
  refresh_map_owners();
  if (local_a == 1) {
    if (*(char *)(local_8 + 0x710) == '\n') {
      if (DAT_451b_0252 != 0) {
        pcVar10 = get_string(0x8d,-1);
        FUN_6a35_04c5((char *)pcVar10);
      }
      if (_DAT_451b_1efe == 0) {
        pcVar3 = (char *)(DAT_451b_029a * 0x14);
      }
      else {
        pcVar3 = (char *)(*(char *)((int)_DAT_451b_1efe + 10) * 0x14 + 0x223);
      }
      strcpy((char *)0x41254148,(char *)CONCAT22(0x3c04,pcVar3));
      iVar2 = FUN_828e_044a(x);
      strcpy((char *)0x41254134,(char *)CONCAT22(0x3c04,(char *)(iVar2 * 0x41 + 0x1581)));
      if (_DAT_451b_1efe != 0) {
        uVar11 = CONCAT22(0x3c04,*(char *)((int)_DAT_451b_1efe + 10) * 0x14 + 0x223);
        pcVar10 = get_string(0x8e,0);
        uVar6 = (undefined2)((ulong)pcVar10 >> 0x10);
        sprintf((char *)0x41254162,pcVar10,uVar11);
        pcVar10 = (char *)0x41254162;
        goto LAB_67cc_042d;
      }
      uVar11 = 0x8f;
    }
    else if (_DAT_451b_1efe == 0) {
      uVar11 = 0x91;
    }
    else {
      uVar11 = CONCAT22(0x3c04,*(char *)((int)_DAT_451b_1efe + 10) * 0x14 + 0x223);
      pcVar10 = get_string(0x90,0);
      sprintf((char *)0x41254162,pcVar10,uVar11);
      FUN_6a35_04c5(0x4162,0x4125);
      uVar11 = 0x10090;
    }
  }
  else {
    uVar11 = 0x92;
  }
  pcVar10 = get_string((int)uVar11,(int)((ulong)uVar11 >> 0x10));
  uVar6 = (undefined2)((ulong)pcVar10 >> 0x10);
LAB_67cc_042d:
  FUN_6a35_04c5(pcVar10);
  local_c = after_battle();
  FUN_6a35_04f6();
  FUN_834b_0000(0x10001,0);
  if (local_a == 1) {
    pbVar1 = (byte *)(y * 0xe0 + x * 2 + 1);
    *pbVar1 = *pbVar1 | 0x10;
    puVar4 = (undefined2 *)armies_at_tile(x,y,(int *)CONCAT22(unaff_SS,&local_e));
    DAT_451b_1f02 = (undefined2 *)CONCAT22(uVar6,puVar4);
    FUN_8c07_06eb(puVar4,uVar6);
  }
  auto_ui_medal_effect();
  FUN_8065_011f();
  if (local_c == 0x21) {
    iVar2 = quest_check();
    if (iVar2 == 0) {
      FUN_67cc_04f7();
    }
  }
  else {
    iVar2 = quest_check();
    if (iVar2 == 0) {
      FUN_89e0_0356();
      FUN_8065_0174();
    }
  }
  return;
}


