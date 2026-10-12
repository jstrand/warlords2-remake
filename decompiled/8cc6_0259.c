
void __cdecl16far FUN_8cc6_0259(int param_1)

{
  bool bVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  undefined2 unaff_SS;
  char local_34 [50];
  
  bVar1 = true;
  DAT_4125_3254 = 0;
  if (DAT_4125_16a6 != 0) {
    DAT_4125_16a6 = 0;
    for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
      if (*(int *)(iVar3 * 2 + 0x137) != 0) {
        *(undefined2 *)(iVar3 * 2 + 0x147) = 1;
      }
    }
  }
  iVar3 = 0;
  for (iVar4 = 0; iVar4 < 8; iVar4 = iVar4 + 1) {
    if ((*(int *)(iVar4 * 2 + 0x137) != 0) && (*(int *)(iVar4 * 2 + 0xd0) == 0)) {
      iVar3 = iVar3 + 1;
    }
  }
  if ((1 < iVar3) && (opt_hidden_map != 0)) {
    bVar1 = false;
  }
  DAT_3c04_0159 = 1;
  diplomacy_apply();
  FUN_8611_0d39();
  FUN_828e_03f9();
  compute_income();
  compute_upkeep();
  if (opt_hidden_map != 0) {
    FUN_8611_1558();
  }
  if (opt_hidden_map != 0) {
    FUN_834b_2200(param_1);
  }
  DAT_4125_325e = param_1;
  if (bVar1) {
    FUN_834b_0000(1,1,0,0);
  }
  FUN_8065_0aeb();
  FUN_1b62_0d85();
  reset_movement();
  FUN_8c07_0000();
  apply_income();
  FUN_8c07_0113();
  FUN_828e_0af3();
  draw_turn_strip(0,0);
  if (bVar1) {
    FUN_8611_0629(*(undefined2 *)(param_1 * 0x14 + 0x189),*(undefined2 *)(param_1 * 0x14 + 0x18b));
    FUN_89e0_0356();
  }
  FUN_22bf_07a5();
  FUN_54f6_0000(6);
  FUN_78a8_06ae(1,0xf,0,3);
  FUN_7ecb_00d6(0x140,0x55,current_player * 0x14,0x3c04);
  uVar2 = game_turn();
  sprintf((char *)CONCAT22(unaff_SS,local_34),s_Turn__d_4125_3263,uVar2);
  FUN_7ecb_00d6(0x140,0x82,local_34);
  sound_turn();
  FUN_7ecb_01f1();
  FUN_22bf_0757();
  DAT_4125_166a = 1;
  FUN_7ecb_0142();
  DAT_4125_166a = 0;
  FUN_22bf_07a5();
  FUN_54f6_069c();
  sample_wait();
  FUN_7ecb_0a1b();
  if (!bVar1) {
    FUN_834b_0000(1,1,0,0);
    FUN_8611_0629(*(undefined2 *)(param_1 * 0x14 + 0x189),*(undefined2 *)(param_1 * 0x14 + 0x18b));
  }
  FUN_8611_006c();
  FUN_22bf_0757();
  auto_ui_armies_sunk(0xffff,0xffff);
  quest_check(0xffff,0,0,0,0);
  advisor_speak(5);
  auto_tutorial_hook_6(0);
  return;
}


