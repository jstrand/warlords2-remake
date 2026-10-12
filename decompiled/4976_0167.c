
void __cdecl16far auto_ui_no_quest(void)

{
  bool bVar1;
  char *pcVar2;
  
  bVar1 = *(int *)(current_player * 0xc + 0x1103) != 0;
  if (((*(int *)(*(int *)(current_player * 0xc + 0x1107) * 0x16 + -0x7780) < 0) ||
      (*(int *)(*(int *)(current_player * 0xc + 0x1107) * 0x16 + -0x777e) < 0)) ||
     (*(char *)(*(int *)(current_player * 0xc + 0x1107) * 0x16 + -0x777c) != '\x1c')) {
    bVar1 = false;
  }
  FUN_22bf_07a5();
  FUN_54f6_0000();
  if (bVar1) {
    __F_SPUSH(DAT_4125_009c);
    FUN_1997_027e();
  }
  else {
    FUN_78a8_06ae(0xf0002,0x30000);
    pcVar2 = get_string(0x14,-1);
    FUN_7ecb_00d6(0x8601b0,(char *)pcVar2,(int)((ulong)pcVar2 >> 0x10));
  }
  FUN_78a8_06ae(0xf0001,0x30000);
  FUN_7ecb_00d6(0x3e01b0,0xb4,0x4125);
  auto_str_join_data_not_found();
  DAT_4125_166a = 1;
  FUN_18a9_066e(0x1014a);
  FUN_834b_0000(0,0);
  if (bVar1) {
    auto_ui_quest_pillage_gold();
    FUN_834b_1f5f();
    FUN_834b_04d3(0);
  }
  FUN_22bf_0757();
  return;
}


