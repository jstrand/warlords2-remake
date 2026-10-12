
undefined2 __cdecl16far set_city_production(int param_1,undefined2 param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_7087_0000(param_1,param_2);
  if ((*(int *)(current_player * 0x14 + 0x185) < *(char *)(param_1 * 0x41 + iVar1 + 0x15a3) + 0x1e)
     && (iVar2 = game_turn(), 5 < iVar2)) {
    return 0;
  }
  *(undefined1 *)(param_1 * 0x41 + 0x15a9) = (undefined1)param_2;
  *(undefined1 *)(param_1 * 0x41 + 0x15aa) = *(undefined1 *)(param_1 * 0x41 + iVar1 + 0x1597);
  if (param_3 == 0) {
    *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + param_1 * 0x41) = 0;
  }
  else {
    vector_city_to(param_1,param_4);
  }
  return 1;
}


