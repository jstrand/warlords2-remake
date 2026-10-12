
void __cdecl16far city_apply_pillage(int city)

{
  int iVar1;
  undefined1 local_40 [30];
  uint local_22;
  
  DAT_451b_01b8 = 0;
  DAT_451b_01b4 = 0;
  iVar1 = city_production_count(city);
  if (0 < iVar1) {
    DAT_451b_01b6 = iVar1 + -1;
    armytype_lookup((int)*(char *)(city * 0x41 + iVar1 + 0x1592),local_40);
    ((undefined2 *)&DAT_451b_01ba)[DAT_451b_01b8] = (int)*(char *)(city * 0x41 + iVar1 + 0x1592);
    ((undefined2 *)&DAT_451b_01ae)[DAT_451b_01b8] =
         (int)((local_22 ^ (int)local_22 >> 0xf) - ((int)local_22 >> 0xf)) / 2;
    DAT_451b_01b4 = DAT_451b_01b4 + ((undefined2 *)&DAT_451b_01ae)[DAT_451b_01b8];
    DAT_451b_01b8 = DAT_451b_01b8 + 1;
    *(undefined1 *)(city * 0x41 + iVar1 + 0x1592) = 0xff;
    iVar1 = city_compute_defence(city);
    *(undefined1 *)(city * 0x41 + 0x1591) = (char)iVar1;
  }
  return;
}


