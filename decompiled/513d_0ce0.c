
void __cdecl16far random_map_cities(void)

{
  int iVar1;
  int city;
  
  for (city = 0; city < city_count; city = city + 1) {
    DAT_4125_02de = 0;
    DAT_4125_02dc = 0;
    DAT_4125_02da = 0;
    DAT_4125_02d8 = 0;
    DAT_4125_02d6 = 0;
    iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d),*(int *)(city * 0x41 + 0x157f),3);
    if ((((iVar1 != 0) ||
         (iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d) + 1,*(int *)(city * 0x41 + 0x157f),3),
         iVar1 != 0)) ||
        (iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d),*(int *)(city * 0x41 + 0x157f) + 1,3),
        iVar1 != 0)) ||
       (iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d) + 1,*(int *)(city * 0x41 + 0x157f) + 1,3
                            ), iVar1 != 0)) {
      DAT_4125_02d6 = 1;
    }
    iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d),*(int *)(city * 0x41 + 0x157f),4);
    if (((iVar1 != 0) ||
        (iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d) + 1,*(int *)(city * 0x41 + 0x157f),4),
        iVar1 != 0)) ||
       ((iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d),*(int *)(city * 0x41 + 0x157f) + 1,4),
        iVar1 != 0 ||
        (iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d) + 1,*(int *)(city * 0x41 + 0x157f) + 1,
                              4), iVar1 != 0)))) {
      DAT_4125_02d8 = 1;
    }
    iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d),*(int *)(city * 0x41 + 0x157f),0);
    if (((iVar1 != 0) ||
        (iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d) + 1,*(int *)(city * 0x41 + 0x157f),0),
        iVar1 != 0)) ||
       ((iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d),*(int *)(city * 0x41 + 0x157f) + 1,0),
        iVar1 != 0 ||
        (iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d) + 1,*(int *)(city * 0x41 + 0x157f) + 1,
                              0), iVar1 != 0)))) {
      DAT_4125_02da = 1;
    }
    iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d),*(int *)(city * 0x41 + 0x157f),5);
    if ((((iVar1 != 0) ||
         (iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d) + 1,*(int *)(city * 0x41 + 0x157f),5),
         iVar1 != 0)) ||
        (iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d),*(int *)(city * 0x41 + 0x157f) + 1,5),
        iVar1 != 0)) ||
       (iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d) + 1,*(int *)(city * 0x41 + 0x157f) + 1,5
                            ), iVar1 != 0)) {
      DAT_4125_02dc = 1;
    }
    iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d),*(int *)(city * 0x41 + 0x157f),8);
    if (((iVar1 != 0) ||
        (iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d) + 1,*(int *)(city * 0x41 + 0x157f),8),
        iVar1 != 0)) ||
       ((iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d),*(int *)(city * 0x41 + 0x157f) + 1,8),
        iVar1 != 0 ||
        (iVar1 = terrain_near(*(int *)(city * 0x41 + 0x157d) + 1,*(int *)(city * 0x41 + 0x157f) + 1,
                              8), iVar1 != 0)))) {
      DAT_4125_02de = 1;
    }
    iVar1 = random_city_value(city);
    random_map_city_economy(city,iVar1);
    random_map_city_production(city,iVar1);
  }
  if (DAT_4125_28cc != 0) {
    random_add_production();
  }
  FUN_513d_1c1d();
  return;
}


