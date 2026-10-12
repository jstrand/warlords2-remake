
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_file_stand_pck(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  char *pcVar4;
  
  pcVar4 = get_file_string(0x18,0);
  uVar3 = (undefined2)((ulong)pcVar4 >> 0x10);
  iVar1 = auto_str_cannot_allocate_this_newbm(0x15e0,0x4125,0x1669,0x4125,pcVar4);
  if (iVar1 == 0) {
    pcVar4 = get_error_string(2,0);
    uVar3 = (undefined2)((ulong)pcVar4 >> 0x10);
    auto_str_fatal_error(pcVar4);
  }
  if ((_LAB_4125_15ea != 0) && ((DAT_4125_1615 & 10) == 0)) {
    for (iVar1 = 0; iVar1 < (int)(uint)DAT_4125_1614; iVar1 = iVar1 + 1) {
      mem_unlock(*(undefined4 *)((undefined1 *)&LAB_4125_15ea + iVar1 * 4));
    }
  }
  FUN_7ecb_0a1b();
  if ((_LAB_4125_15ea != 0) && ((DAT_4125_1615 & 10) == 0)) {
    for (iVar1 = 0; iVar1 < (int)(uint)DAT_4125_1614; iVar1 = iVar1 + 1) {
      uVar2 = mem_lock(*(undefined4 *)((undefined1 *)&LAB_4125_15ea + iVar1 * 4));
      ((undefined2 *)&DAT_4125_15fc)[iVar1 * 2] = uVar3;
      *(undefined2 *)((undefined1 *)&LAB_4125_15fa + iVar1 * 4) = uVar2;
    }
  }
  FUN_2012_09e3(0x15e0,0x4125,1);
  FUN_1709_0000(0x15e0,0x4125,0x1000a);
  if ((_LAB_4125_15ea != 0) && ((DAT_4125_1615 & 10) == 0)) {
    for (iVar1 = 0; iVar1 < (int)(uint)DAT_4125_1614; iVar1 = iVar1 + 1) {
      mem_unlock(*(undefined4 *)((undefined1 *)&LAB_4125_15ea + iVar1 * 4));
    }
  }
  FUN_1997_034a();
  FUN_1997_034a();
  FUN_2012_0975(0x5559,0x4125);
  auto_str_plane_handle_not_allocated(0x161b,0x4125,3,0x40028);
  FUN_2012_07cc(0x161b,0x4125,0);
  FUN_2012_0232(0x54e3,0x4125,0xe0,0x10138);
  return;
}


