
void __cdecl16far FUN_649c_0061(void)

{
  int *piVar1;
  int iVar2;
  
  iVar2 = dice(1,0x19,0);
  piVar1 = (int *)(current_player * 2 + 0x10e3);
  *piVar1 = *piVar1 + iVar2 + 0x19;
  FUN_649c_00fe(0x7ecb,DAT_451b_01c2,DAT_451b_01c0,10);
  FUN_828e_03f9();
  compute_upkeep();
  compute_income();
  if (DAT_451b_01c4 == 0) {
    auto_tutorial_hook_3(0,DAT_4125_0e9a,DAT_4125_0e9c);
  }
  else {
    game_screen_start(0);
  }
  return;
}


