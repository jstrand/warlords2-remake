
void __cdecl16far auto_tutorial_hook_6(int param_1)

{
  int iVar1;
  int iVar2;
  char *pcVar3;
  
  iVar1 = game_turn();
  if ((game_won == 0) || (DAT_3c04_0157 != 0)) {
    if ((surrender_offered == 0) || (DAT_3c04_015f != 0)) {
      if (param_1 < 1) {
        hero_check_promotions();
        iVar2 = hero_offer_check();
        if (iVar2 != 0) {
          if (game_won == 0) {
            music_cue(4);
          }
          auto_ui_hero_emerges();
          return;
        }
        if (game_won == 0) {
          music_cue(1);
        }
      }
      if ((param_1 < 2) && (iVar1 == 1)) {
        auto_tutorial_hook_3
                  (2,*(undefined2 *)(current_player * 0x14 + 0x189),
                   *(undefined2 *)(current_player * 0x14 + 0x18b));
      }
      else {
        if ((param_1 < 3) && (iVar2 = city_production_turn(), iVar2 != 0)) {
          FUN_6f8c_06bd();
        }
        FUN_89e0_0356();
        FUN_8065_0174();
        if (((opt_tutorial != 0) && (iVar1 == 2)) && ((DAT_3c04_0134 & 0x40) == 0)) {
          DAT_3c04_0134 = DAT_3c04_0134 | 0x40;
          pcVar3 = get_file_string(0x19,7);
          FUN_8065_168d((char *)pcVar3,(int)((ulong)pcVar3 >> 0x10));
        }
      }
    }
    else {
      FUN_8065_1f68();
    }
  }
  else {
    FUN_8065_1fbd();
  }
  return;
}


