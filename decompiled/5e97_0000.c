
undefined2 __cdecl16far FUN_5e97_0000(int param_1,int param_2)

{
  int iVar1;
  
  if ((((*(int *)(current_player * 2 + 0xd0) != 0) && (*(int *)(param_2 * 2 + 0xd0) == 0)) &&
      (*(int *)((int)DAT_451b_018e + 0x42) != 0)) &&
     ((iVar1 = game_turn(), iVar1 < 10 && (*(int *)((int)DAT_451b_018e + 0x46) != param_1)))) {
    iVar1 = city_sack_value(param_1);
    if (iVar1 < 200) {
      auto_str_is_being_razed(param_1,0);
    }
    else {
      auto_str_is_being_sacked(param_1);
    }
    return 1;
  }
  return 0;
}


