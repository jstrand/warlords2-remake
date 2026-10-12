
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far start_of_turn(int side)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_SS;
  char local_54 [80];
  int local_4;
  
  iVar2 = 0;
  bVar1 = true;
  local_4 = 0;
  DAT_3c04_0159 = *(int *)(side * 2 + 0x147);
  for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
    if ((*(int *)(iVar3 * 2 + 0x137) != 0) && (*(int *)(iVar3 * 2 + 0xd0) == 0)) {
      iVar2 = iVar2 + 1;
      bVar1 = false;
    }
  }
  if ((iVar2 == 1) && (opt_hidden_map != 0)) {
    DAT_4125_3254 = 1;
  }
  else {
    DAT_4125_3254 = 0;
  }
  if (bVar1) {
    DAT_3c04_0159 = 1;
  }
  if (((1 < iVar2) && (opt_hidden_map != 0)) && (*(int *)(side * 2 + 0x147) == 0)) {
    DAT_3c04_0159 = 0;
  }
  if (opt_hidden_map != 0) {
    local_4 = 1;
    FUN_8611_0d39();
    FUN_8611_1558();
    if ((bVar1) || ((DAT_3c04_0159 != 0 && (DAT_4125_3254 == 0)))) {
      FUN_834b_2200();
    }
  }
  if ((bVar1) || ((DAT_3c04_0159 != 0 && (DAT_4125_3254 == 0)))) {
    FUN_834b_0000(0x10001,0);
  }
  sprintf((char *)CONCAT22(unaff_SS,local_54),(char *)0x41253260,side * 0x14,0x3c04);
  FUN_8065_11d1();
  FUN_8065_1291();
  FUN_8065_131b();
  diplomacy_apply();
  FUN_828e_03f9();
  if ((int)((undefined2 *)&side_city_count)[side] < 1) {
    draw_turn_strip(0);
    FUN_8065_1291();
    FUN_8065_131b();
    FUN_8065_131b();
    FUN_8065_131b();
    FUN_8065_131b();
  }
  else {
    iVar2 = hero_offer_check();
    if (iVar2 != 0) {
      FUN_7563_0301();
    }
    hero_check_promotions();
    apply_income();
    city_production_turn();
    FUN_1b62_0d85();
    reset_movement();
    FUN_8c07_0000();
    FUN_828e_03f9();
    FUN_8c07_0113();
    quest_check();
    ai_turn();
    iVar2 = 8;
    while (iVar3 = iVar2 + -1, iVar2 != 0) {
      *(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar3 * 4) = 0;
      ((undefined2 *)&DAT_451b_1ede)[iVar3 * 2] = 0;
      iVar2 = iVar3;
    }
    DAT_451b_1f02 = 0;
    _DAT_451b_1efe = 0;
    FUN_828e_0af3();
    if (local_4 != 0) {
      FUN_8611_0e68();
    }
  }
  return;
}


