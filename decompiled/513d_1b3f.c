
void __cdecl16far random_add_production(void)

{
  int iVar1;
  int iVar2;
  undefined1 local_46 [22];
  undefined1 local_30;
  undefined1 local_2e;
  undefined1 local_2c;
  undefined1 local_2a;
  undefined2 local_8;
  int local_6;
  int local_4;
  
  local_4 = dice(2,3,0);
  local_6 = 0;
  while ((0 < local_4 && (local_6 < 100))) {
    local_6 = local_6 + 1;
    iVar1 = dice(1,city_count,-1);
    iVar2 = city_production_count();
    if (iVar2 < 4) {
      local_8 = random_magical_type();
      armytype_lookup(local_8,local_46);
      *(undefined1 *)(iVar1 * 0x41 + iVar2 + 0x1593) = (undefined1)local_8;
      *(undefined1 *)(iVar1 * 0x41 + iVar2 + 0x1597) = local_2e;
      *(undefined1 *)(iVar1 * 0x41 + iVar2 + 0x15a3) = local_2c;
      *(undefined1 *)(iVar1 * 0x41 + iVar2 + 0x159b) = local_30;
      *(undefined1 *)(iVar1 * 0x41 + iVar2 + 0x159f) = local_2a;
      local_4 = local_4 + -1;
    }
  }
  return;
}


