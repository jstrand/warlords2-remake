
void __cdecl16far city_apply_sack(int city)

{
  int iVar1;
  undefined1 local_42 [30];
  uint local_24;
  int local_4;
  
  DAT_451b_01b8 = 0;
  DAT_451b_01b4 = 0;
  DAT_451b_01b6 = 1;
  local_4 = city_production_count(city);
  if (1 < local_4) {
    for (iVar1 = 1; iVar1 < local_4; iVar1 = iVar1 + 1) {
      armytype_lookup((int)*(char *)(city * 0x41 + iVar1 + 0x1593),local_42);
      ((undefined2 *)&DAT_451b_01ba)[DAT_451b_01b8] = (int)*(char *)(city * 0x41 + iVar1 + 0x1593);
      ((undefined2 *)&DAT_451b_01ae)[DAT_451b_01b8] =
           (int)((local_24 ^ (int)local_24 >> 0xf) - ((int)local_24 >> 0xf)) / 2;
      DAT_451b_01b4 = DAT_451b_01b4 + ((undefined2 *)&DAT_451b_01ae)[DAT_451b_01b8];
      DAT_451b_01b8 = DAT_451b_01b8 + 1;
      *(undefined1 *)(city * 0x41 + iVar1 + 0x1593) = 0xff;
    }
    iVar1 = city_compute_defence(city);
    *(undefined1 *)(city * 0x41 + 0x1591) = (char)iVar1;
  }
  return;
}


