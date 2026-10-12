
bool __cdecl16far ai_buy_production_type(undefined2 param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  int record;
  int slot;
  int local_a;
  undefined1 local_4 [2];
  
  armytype_lock();
  record = armytype_get_stats_price(param_1,local_4);
  iVar2 = *(int *)(current_player * 0x14 + 0x185);
  if (local_a + 0x1e < iVar2) {
    slot = FUN_623c_1ae9(param_2);
    buy_production_type(param_2,slot,record);
    piVar1 = (int *)((int)DAT_451b_018e + 0x16);
    *piVar1 = *piVar1 + 1;
    *(undefined1 *)(param_2 * 0x41 + 0x15aa) = 0;
    *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + param_2 * 0x41) = 0;
    *(undefined1 *)((int)DAT_451b_018e + param_2 + 0x56) = 5;
  }
  armytype_unlock();
  return local_a + 0x1e < iVar2;
}


