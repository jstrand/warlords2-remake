
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_6a89_1475(int param_1,undefined2 param_2,undefined2 param_3)

{
  int iVar1;
  undefined2 local_2a;
  undefined2 local_28;
  undefined4 local_26;
  
  if (_LAB_4125_1174 == 0) {
    iVar1 = 0;
    while ((iVar1 < 0x1d && (*(char *)(current_player * 0x1d + iVar1 + 0x60b) != param_1))) {
      iVar1 = iVar1 + 1;
    }
    armytype_lookup();
    ui_army_type_info(iVar1,local_28,local_2a,local_26,param_2,param_3);
  }
  return;
}


