
void __cdecl16far buy_production_type(int city,int slot,int record)

{
  int *piVar1;
  int iVar2;
  
  armytype_lock();
  *(undefined1 *)(city * 0x41 + slot + 0x1593) =
       *(undefined1 *)((int)armytype_table + record * 0x3e);
  *(undefined1 *)(city * 0x41 + slot + 0x1597) =
       *(undefined1 *)((int)armytype_table + record * 0x3e + 0x18);
  *(undefined1 *)(city * 0x41 + slot + 0x15a3) =
       *(undefined1 *)((int)armytype_table + record * 0x3e + 0x1a);
  *(undefined1 *)(city * 0x41 + slot + 0x159b) =
       *(undefined1 *)((int)armytype_table + record * 0x3e + 0x16);
  *(undefined1 *)(city * 0x41 + slot + 0x159f) =
       *(undefined1 *)((int)armytype_table + record * 0x3e + 0x1c);
  iVar2 = city_compute_defence(city);
  *(undefined1 *)(city * 0x41 + 0x1591) = (char)iVar2;
  piVar1 = (int *)(current_player * 0x14 + 0x185);
  *piVar1 = *piVar1 - *(int *)((int)armytype_table + record * 0x3e + 0x1e);
  armytype_unlock();
  return;
}


