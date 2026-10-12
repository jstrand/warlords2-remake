
void __cdecl16far ai_city_flag_check(int param_1)

{
  byte *pbVar1;
  int slot;
  int iVar2;
  int iVar3;
  bool bVar4;
  int local_a;
  undefined1 local_4 [2];
  
  slot = FUN_623c_1ae9(param_1);
  armytype_lock();
  iVar3 = 0;
  do {
    if (0x1c < iVar3) {
LAB_623c_12c0:
      armytype_unlock();
      return;
    }
    if ((((undefined1 *)&army_move_flags)[iVar3 * 6] != '\0') &&
       (((undefined1 *)&DAT_451b_0668)[iVar3 * 6] == '\0')) {
      iVar2 = 4;
      do {
        bVar4 = iVar2 != 0;
        iVar2 = iVar2 + -1;
      } while (bVar4);
      iVar2 = armytype_get_stats_price(iVar3,local_4);
      local_a = local_a + 0x1e;
      if (local_a <= *(int *)(current_player * 0x14 + 0x185)) {
        buy_production_type(param_1,slot,iVar2);
        pbVar1 = (byte *)((int)DAT_451b_018e + param_1 + 0x11e);
        *pbVar1 = *pbVar1 | 2;
        *(undefined1 *)(param_1 * 0x41 + 0x15aa) = 0;
        *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + param_1 * 0x41) = 0;
        goto LAB_623c_12c0;
      }
    }
    iVar3 = iVar3 + 1;
  } while( true );
}


