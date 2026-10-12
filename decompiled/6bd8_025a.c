
void __cdecl16far neutral_production_turn(void)

{
  char *pcVar1;
  int iVar2;
  int iVar3;
  int city;
  undefined2 unaff_SS;
  int local_4;
  
  iVar3 = city_count;
  if (1 < opt_neutral_cities) {
    while (city = iVar3 + -1, iVar3 != 0) {
      iVar3 = city;
      if (((*(char *)(city * 0x41 + 0x1592) == '\x0f') &&
          (*(char *)(*(byte *)(*(int *)(city * 0x41 + 0x157f) * 0xe0 +
                              *(int *)(city * 0x41 + 0x157d) * 2) + 0x710) == '\n')) &&
         (*(char *)(city * 0x41 + 0x15ad) != '\0')) {
        if ((*(char *)(city * 0x41 + 0x15aa) != '\0') &&
           (pcVar1 = (char *)(city * 0x41 + 0x15aa), *pcVar1 = *pcVar1 + -1,
           *(char *)(city * 0x41 + 0x15aa) == '\0')) {
          produce_garrison(city);
        }
        iVar2 = city_is_building();
        if ((iVar2 == 0) &&
           (armies_at_tile(*(int *)(city * 0x41 + 0x157d),*(int *)(city * 0x41 + 0x157f),
                           (int *)CONCAT22(unaff_SS,&local_4)), local_4 < 4)) {
          best_production_for(city,2,-1,0);
        }
      }
    }
  }
  return;
}


