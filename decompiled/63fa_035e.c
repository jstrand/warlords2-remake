
void __cdecl16far city_sack(void)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = city_sack_value(DAT_4125_0a3c);
  city_apply_sack(DAT_4125_0a3c);
  piVar1 = (int *)(current_player * 0x14 + 0x185);
  *piVar1 = *piVar1 + iVar2;
  if (*(int *)(current_player * 2 + 0xd0) == 0) {
    auto_ui_pillage_sack_raze();
    FUN_8065_011f(0,0x10000);
    DAT_4125_0a3e = 0;
  }
  iVar3 = dice(1,10,0);
  piVar1 = (int *)(current_player * 2 + 0x10e3);
  *piVar1 = *piVar1 + iVar3 + 5;
  quest_check(1,iVar2,0);
  return;
}


