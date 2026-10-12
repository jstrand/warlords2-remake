
void __cdecl16far auto_str_is_being_sacked(int param_1)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = city_sack_value(param_1);
  if (iVar2 == 0) {
    auto_str_is_being_pillaged();
  }
  else {
    sprintf(&DAT_451b_1d28,s__s_is_being_sacked__4125_079b,param_1 * 0x41 + 0x1581);
    FUN_67cc_16b3((undefined1 *)&DAT_451b_1d28);
    city_apply_sack(param_1);
    iVar3 = dice(1,10,0);
    piVar1 = (int *)(current_player * 2 + 0x10e3);
    *piVar1 = *piVar1 + iVar3 + 5;
    piVar1 = (int *)(current_player * 0x14 + 0x185);
    *piVar1 = *piVar1 + iVar2;
  }
  return;
}


