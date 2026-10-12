
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far ui_buy_production_type(int param_1)

{
  int *piVar1;
  char cVar2;
  bool bVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  
  armytype_lock();
  iVar5 = 0;
  iVar4 = 0;
  while( true ) {
    uVar7 = (undefined2)((ulong)armytype_table >> 0x10);
    iVar6 = param_1;
    if (0x1b < iVar5) break;
    if (-1 < *(int *)((int)armytype_table + iVar5 * 0x3e + 0x1e)) {
      iVar6 = iVar5;
      if (iVar4 == param_1) break;
      iVar4 = iVar4 + 1;
    }
    iVar5 = iVar5 + 1;
  }
  cVar2 = *(char *)(DAT_4125_0e98 * 0x41 + 0x15a9);
  *(undefined1 *)(DAT_4125_0e98 * 0x41 + _LAB_4125_0e86 + 0x1593) =
       *(undefined1 *)((int)armytype_table + iVar6 * 0x3e);
  *(undefined1 *)(DAT_4125_0e98 * 0x41 + _LAB_4125_0e86 + 0x1597) =
       *(undefined1 *)((int)armytype_table + iVar6 * 0x3e + 0x18);
  *(undefined1 *)(DAT_4125_0e98 * 0x41 + _LAB_4125_0e86 + 0x15a3) =
       *(undefined1 *)((int)armytype_table + iVar6 * 0x3e + 0x1a);
  *(undefined1 *)(DAT_4125_0e98 * 0x41 + _LAB_4125_0e86 + 0x159b) =
       *(undefined1 *)((int)armytype_table + iVar6 * 0x3e + 0x16);
  *(undefined1 *)(DAT_4125_0e98 * 0x41 + _LAB_4125_0e86 + 0x159f) =
       *(undefined1 *)((int)armytype_table + iVar6 * 0x3e + 0x1c);
  iVar4 = city_compute_defence(DAT_4125_0e98);
  *(undefined1 *)(DAT_4125_0e98 * 0x41 + 0x1591) = (char)iVar4;
  piVar1 = (int *)(current_player * 0x14 + 0x185);
  *piVar1 = *piVar1 - *(int *)((int)armytype_table + iVar6 * 0x3e + 0x1e);
  armytype_unlock();
  if ('\0' < *(char *)(DAT_4125_0e98 * 0x41 + 0x15aa)) {
    bVar3 = false;
    for (iVar4 = 0; iVar4 < 4; iVar4 = iVar4 + 1) {
      if (*(char *)(DAT_4125_0e98 * 0x41 + iVar4 + 0x1593) == cVar2) {
        bVar3 = true;
        break;
      }
    }
    if (!bVar3) {
      *(undefined1 *)(DAT_4125_0e98 * 0x41 + 0x15aa) = 0;
      *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + DAT_4125_0e98 * 0x41) = 0;
      *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + DAT_4125_0e98 * 0x41) = 0xffff;
      *(undefined2 *)(DAT_4125_0e98 * 0x41 + 0x15b2) = 0xffff;
      *(undefined1 *)(DAT_4125_0e98 * 0x41 + 0x15a9) = 0xff;
    }
  }
  if ((_LAB_4125_0e86 < 3) && (*(char *)(DAT_4125_0e98 * 0x41 + _LAB_4125_0e86 + 0x1594) < '\0')) {
    _LAB_4125_0e86 = _LAB_4125_0e86 + 1;
  }
  if (*(int *)(current_player * 2 + 0xd0) == 0) {
    auto_ui_build_production(0);
    FUN_7087_0dae();
  }
  return;
}


