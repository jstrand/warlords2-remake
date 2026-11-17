
void __cdecl16far setup_capitals(void)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_SS;
  int local_26 [8];
  int local_16 [8];
  int local_6;
  int local_4;
  
  local_4 = -1;
  local_6 = 8;
  while( true ) {
    iVar1 = local_6;
    local_6 = local_6 + -1;
    if (iVar1 == 0) break;
    if (*(int *)(local_6 * 2 + 0x137) == 0) {
      *(undefined2 *)(local_6 * 0x14 + 0x193) = 0xffff;
      *(undefined2 *)(local_6 * 0x14 + 0x195) = 0xffff;
    }
    else {
      *(undefined2 *)(local_6 * 0x14 + 0x193) = *(undefined2 *)(local_6 * 0x14 + 0x189);
      *(undefined2 *)(local_6 * 0x14 + 0x195) = *(undefined2 *)(local_6 * 0x14 + 0x18b);
    }
    local_16[local_6] = *(int *)(local_6 * 0x14 + 0x189);
    local_26[local_6] = *(int *)(local_6 * 0x14 + 0x18b);
    iVar1 = city_at(local_16[local_6],local_26[local_6]);
    if (-1 < iVar1) {
      if (*(int *)(local_6 * 2 + 0x137) == 0) {
        set_city_tiles((int *)CONCAT22(0x3c04,(int *)(iVar1 * 0x41 + 0x157d)),0xf);
        *(undefined1 *)(iVar1 * 0x41 + 0x1592) = 0xf;
      }
      else {
        set_city_tiles((int *)CONCAT22(0x3c04,(int *)(iVar1 * 0x41 + 0x157d)),local_6);
        *(undefined1 *)(iVar1 * 0x41 + 0x1592) = (undefined1)local_6;
        local_4 = local_6;
      }
    }
  }
  if ((local_4 != -1) && (opt_quick_start != 0)) {
    while (iVar1 = nearest_city(local_16[local_4],local_26[local_4],0xf,0), -1 < iVar1) {
      set_city_tiles((int *)CONCAT22(0x3c04,(int *)(iVar1 * 0x41 + 0x157d)),local_4);
      *(undefined1 *)(iVar1 * 0x41 + 0x1592) = (undefined1)local_4;
      iVar2 = dice(1,10,-1);
      if (iVar2 < 5) {
        local_16[local_4] = *(int *)(local_4 * 0x14 + 0x189);
        iVar1 = *(int *)(local_4 * 0x14 + 0x18b);
      }
      else {
        local_16[local_4] = *(int *)(iVar1 * 0x41 + 0x157d);
        iVar1 = *(int *)(iVar1 * 0x41 + 0x157f);
      }
      local_26[local_4] = iVar1;
      do {
        local_4 = (local_4 + 1) % 8;
      } while (*(int *)(local_4 * 2 + 0x137) == 0);
    }
  }
  setup_city_production();
  for (local_6 = 0; local_6 < city_count; local_6 = local_6 + 1) {
    iVar1 = city_compute_defence(local_6);
    *(undefined1 *)(local_6 * 0x41 + 0x1591) = (char)iVar1;
  }
  return;
}


