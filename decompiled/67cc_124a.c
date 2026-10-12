
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 * __cdecl16far auto_ui_being_attacked(int param_1,int param_2,int param_3)

{
  char *pcVar1;
  byte *pbVar2;
  bool bVar3;
  bool bVar4;
  bool bVar5;
  bool bVar6;
  bool bVar7;
  uint uVar8;
  int iVar9;
  int iVar10;
  undefined2 uVar11;
  char *pcVar12;
  undefined4 uVar13;
  
  uVar8 = *(byte *)(param_2 * 0xe0 + param_1 * 2 + 1) & 0xf;
  DAT_451b_01fe = param_1;
  DAT_451b_01fc = param_2;
  if ((DAT_3c04_0159 == 0) && ((7 < uVar8 || (*(int *)(uVar8 * 2 + 0xd0) != 0)))) {
    bVar5 = false;
  }
  else {
    bVar5 = true;
  }
  if (((DAT_3c04_0159 == 0) || (7 < uVar8)) && ((7 < uVar8 || (*(int *)(uVar8 * 2 + 0xd0) != 0)))) {
    bVar7 = false;
  }
  else {
    bVar7 = true;
  }
  if ((DAT_4125_3254 != 0) && (-1 < *(char *)(param_2 * 0x70 + param_1))) {
    bVar7 = false;
    bVar5 = false;
  }
  combat_setup(param_1,param_2);
  if (param_3 == 0) {
    DAT_451b_01cc = (int)combat_def_count;
    DAT_451b_01ce = 0;
    DAT_451b_01d0 = (int)combat_atk_count;
    DAT_451b_01d2 = 0;
    combat_resolve();
    iVar9 = (int)combat_atk_count;
    while (iVar10 = iVar9 + -1, iVar9 != 0) {
      pcVar1 = (char *)(iVar9 + 0x4223);
      iVar9 = iVar10;
      if (*pcVar1 != '\0') {
        DAT_451b_01d2 = DAT_451b_01d2 + 1;
      }
    }
    iVar9 = (int)combat_def_count;
    while (iVar10 = iVar9 + -1, iVar9 != 0) {
      pcVar1 = (char *)(iVar9 + 0x422b);
      iVar9 = iVar10;
      if (*pcVar1 != '\0') {
        DAT_451b_01ce = DAT_451b_01ce + 1;
      }
    }
    return (undefined2 *)&DAT_451b_01cc;
  }
  iVar9 = 0;
  for (iVar10 = 0; iVar10 < 8; iVar10 = iVar10 + 1) {
    if ((*(int *)(iVar10 * 2 + 0x137) != 0) && (*(int *)(iVar10 * 2 + 0xd0) == 0)) {
      iVar9 = iVar9 + 1;
    }
  }
  bVar3 = 1 < iVar9;
  bVar4 = bVar5;
  bVar6 = false;
  if ((bVar3) && (opt_hidden_map != 0)) {
    bVar7 = false;
    bVar4 = false;
    bVar6 = bVar5;
  }
  if (DAT_451b_027d != 0) {
    if (opt_diplomacy != 0) {
      uVar11 = (undefined2)((ulong)DAT_451b_027d >> 0x10);
      if ((*(char *)((int)DAT_451b_027d + 0x15) != '\x0f') &&
         ((*(byte *)(current_player * 8 + (int)*(char *)((int)DAT_451b_027d + 0x15) + 0x153b) & 3)
          != 2)) {
        auto_ui_diplomacy_events();
      }
    }
    pbVar2 = (byte *)((int)DAT_451b_027d + 0x30);
    *pbVar2 = *pbVar2 | *(byte *)(current_player * 2 + 0xcbe);
  }
  if ((bVar4) && ((!bVar3 || (opt_hidden_map == 0)))) {
    FUN_67cc_1836();
  }
  FUN_67cc_0522(param_1,param_2,0xffffffff);
  if (bVar4) {
    FUN_8961_0227(param_1,param_2);
  }
  else if (!bVar6) {
    DAT_4125_0cf8 = 0;
    goto LAB_67cc_14f1;
  }
  if (uVar8 < 8) {
    uVar13 = CONCAT22(0x3c04,uVar8 * 0x14);
    pcVar12 = get_string(0x94,0);
    sprintf((char *)0x41254162,pcVar12,uVar13);
  }
  else {
    pcVar12 = get_string(0x95,0);
    sprintf((char *)0x41254162,pcVar12);
  }
  FUN_8065_11d1();
  DAT_4125_0cf8 = 1;
LAB_67cc_14f1:
  iVar9 = combat_resolve();
  ai_record_battle(param_1,param_2,(undefined2 *)&combat_def_armies,0x4125,
                   (undefined2 *)&combat_atk_armies,0x4125);
  if ((bVar7) || (bVar6)) {
    FUN_22bf_07a5();
    FUN_6a35_0160();
    FUN_7ecb_0000();
    FUN_6a35_0094(0,iVar9);
    FUN_22bf_0757();
  }
  after_battle();
  if (iVar9 == 1) {
    pbVar2 = (byte *)(param_2 * 0xe0 + param_1 * 2 + 1);
    *pbVar2 = *pbVar2 | 0x10;
  }
  if (((bVar7) || (bVar6)) || (bVar4)) {
    if (iVar9 == 0) {
      uVar13 = CONCAT22(0x3c04,current_player * 0x14);
      iVar9 = 0x98;
    }
    else if (_DAT_451b_1efe == 0) {
      uVar13 = CONCAT22(0x3c04,current_player * 0x14);
      iVar9 = 0x96;
    }
    else {
      uVar13 = CONCAT22(0x3c04,*(char *)((int)_DAT_451b_1efe + 10) * 0x14 + 0x223);
      iVar9 = 0x8e;
    }
    pcVar12 = get_string(iVar9,0);
    sprintf((char *)0x41254162,pcVar12,uVar13);
    if ((bVar7) || (bVar6)) {
      FUN_67cc_16b3(0x4162,0x4125);
      FUN_6a35_04c5(0x4162,0x4125);
      FUN_7ecb_0000();
      FUN_6a35_04f6();
    }
    else {
      FUN_67cc_16b3(0x4162,0x4125);
    }
  }
  if ((bVar4) && ((!bVar3 || (opt_hidden_map == 0)))) {
    FUN_67cc_1836();
    FUN_828e_0af3();
    if ((DAT_4125_3254 != 0) && (opt_hidden_map != 0)) {
      FUN_8961_05ec();
    }
    FUN_834b_0000(0x10001,0);
  }
  auto_ui_medal_effect();
  refresh_map_owners();
  quest_check();
  FUN_5e97_038d(param_1,param_2);
  return (undefined2 *)0x0;
}


