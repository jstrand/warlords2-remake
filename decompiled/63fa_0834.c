
int __cdecl16far city_pillage_value(int city)

{
  int iVar1;
  undefined1 local_44 [30];
  uint local_26;
  
  iVar1 = city_production_count(city);
  if (iVar1 < 1) {
    iVar1 = 0;
  }
  else {
    armytype_lookup((int)*(char *)(city * 0x41 + iVar1 + 0x1592),local_44);
    iVar1 = (int)((local_26 ^ (int)local_26 >> 0xf) - ((int)local_26 >> 0xf)) / 2;
  }
  return iVar1;
}


