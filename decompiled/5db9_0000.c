
void __cdecl16far ai_turn(void)

{
  byte *pbVar1;
  undefined1 *puVar2;
  undefined4 uVar3;
  bool bVar4;
  int iVar5;
  undefined2 uVar6;
  
  bVar4 = false;
  if (DAT_4125_0832 == 0) {
    auto_str_no_room_ai_memory(1);
  }
  pbVar1 = (byte *)(current_player * 9 + 0x153b);
  *pbVar1 = *pbVar1 & 0xf3;
  iVar5 = current_player * 9;
  uVar6 = 0x3c04;
  pbVar1 = (byte *)(iVar5 + 0x153b);
  *pbVar1 = *pbVar1 & 0xfc;
  puVar2 = (undefined1 *)(iVar5 + 0x153b);
  *puVar2 = *puVar2;
  draw_turn_strip(0,0);
  auto_str_no_room_ai_data();
  DAT_4125_0836 = 1;
  uVar3 = *(undefined4 *)(current_player * 4 + 0x40ca);
  DAT_451b_018e._0_2_ = mem_lock((int)uVar3,(int)((ulong)uVar3 >> 0x10));
  DAT_451b_018e._2_2_ = uVar6;
  sprintf(&DAT_451b_1d28,s_Begin_player__d_4125_0838,current_player);
  ai_debug_log((undefined1 *)&DAT_451b_1d28,0x4125,0);
  ai_turn_setup();
  FUN_8065_131b(10);
  ai_debug_log(0x848,0x4125,0);
  ai_phase_diplomacy();
  ai_debug_log(0x852,0x4125,0);
  ai_phase_move_hero();
  FUN_8065_131b(0xf);
  ai_debug_log(0x85c,0x4125,0);
  if (((bRam00000417 & 3) == 0) && (*(int *)(current_player * 2 + 0xd0) == 0)) {
    *(undefined2 *)(current_player * 2 + 0xd0) = 1;
    bVar4 = true;
  }
  if (opt_hidden_map != 0) {
    ai_phase_move_search();
  }
  if (bVar4) {
    *(undefined2 *)(current_player * 2 + 0xd0) = 0;
  }
  FUN_8065_131b(0x14);
  ai_debug_log(0x868,0x4125,0);
  ai_phase_move_explore();
  FUN_8065_131b(0x19);
  ai_debug_log(0x875,0x4125,0);
  ai_phase_assault();
  FUN_8065_131b(0x1e);
  ai_debug_log(0x87d,0x4125,0);
  ai_phase_move();
  FUN_8065_131b(0x28);
  ai_debug_log(0x885,0x4125,0);
  ai_phase_rescue();
  FUN_8065_131b(0x2d);
  ai_debug_log(0x88c,0x4125,0);
  ai_phase_evaluate(current_player);
  FUN_8065_131b(0x32);
  ai_debug_log(0x895,0x4125,0);
  ai_phase_clean_city();
  FUN_8065_131b(0x37);
  ai_debug_log(0x8a0,0x4125,0);
  ai_phase_neutral();
  FUN_8065_131b(0x3c);
  ai_debug_log(0x8a8,0x4125,0);
  ai_phase_move();
  FUN_8065_131b(0x41);
  ai_debug_log(0x8b0,0x4125,0);
  ai_phase_quick_attack();
  ai_debug_log(0x8bd,0x4125,0);
  if (opt_hidden_map != 0) {
    ai_phase_update_hide();
  }
  ai_debug_log(0x8c9,0x4125,0);
  ai_phase_assault_xx();
  FUN_8065_131b(0x46);
  ai_debug_log(0x8d4,0x4125,0);
  ai_phase_specials();
  FUN_8065_131b(0x4b);
  ai_debug_log(0x8dd,0x4125,0);
  ai_rebuilding();
  FUN_8065_131b(0x50);
  ai_debug_log(0x8e8,0x4125,0);
  ai_phase_last_rescue();
  FUN_8065_131b(0x55);
  ai_debug_log(0x8f4,0x4125,0);
  ai_production();
  FUN_8065_131b(0x5a);
  ai_debug_log(0x8ff,0x4125,0);
  ai_vectoring();
  FUN_8065_131b(0x5f);
  auto_str_conv_lu_maxblock_lu_2();
  ai_debug_log((undefined1 *)&DAT_451b_1d28,0x4125,1);
  FUN_8065_131b(100);
  DAT_4125_0836 = 0;
  FUN_5db9_063d();
  uVar3 = *(undefined4 *)(current_player * 4 + 0x40ca);
  mem_unlock((int)uVar3,(int)((ulong)uVar3 >> 0x10));
  diplomacy_score_update();
  FUN_5db9_045e();
  return;
}


