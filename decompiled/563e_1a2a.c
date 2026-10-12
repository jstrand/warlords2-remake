
void __cdecl16far auto_str_is_being_pillaged(int param_1)

{
  int *piVar1;
  int iVar2;
  
  iVar2 = city_pillage_value(param_1);
  if (iVar2 != 0) {
    sprintf(&DAT_451b_1d28,s__s_is_being_pillaged__4125_07af,param_1 * 0x41 + 0x1581);
    FUN_67cc_16b3((undefined1 *)&DAT_451b_1d28);
    piVar1 = (int *)(current_player * 0x14 + 0x185);
    *piVar1 = *piVar1 + iVar2;
    city_apply_pillage(param_1);
    iVar2 = dice(1,5,0);
    piVar1 = (int *)(current_player * 2 + 0x10e3);
    *piVar1 = *piVar1 + iVar2;
  }
  return;
}


