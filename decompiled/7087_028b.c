
void __cdecl16far FUN_7087_028b(undefined2 param_1,undefined2 param_2,int param_3)

{
  int iVar1;
  undefined1 local_e [8];
  int local_6;
  int iStack_4;
  
  if (((param_3 == 1) && ((bRam00000417 & 3) == 0)) && (DAT_4125_0e94 == 0)) {
    iStack_4 = FUN_828e_04fa(param_1,param_2,current_player,0);
    if (iStack_4 != DAT_4125_0e98) {
      auto_tutorial_hook_3();
    }
  }
  else {
    if (*(char *)(DAT_4125_0e98 * 0x41 + 0x15aa) < '\x01') {
      return;
    }
    iVar1 = FUN_828e_0651(param_1,param_2);
    FUN_7087_0410(iVar1,local_e);
    if (iVar1 == DAT_4125_0e98) {
      *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + DAT_4125_0e98 * 0x41) = 0;
      *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + DAT_4125_0e98 * 0x41) = 0xffff;
      *(undefined2 *)(DAT_4125_0e98 * 0x41 + 0x15b2) = 0xffff;
    }
    else {
      if (3 < local_6) goto LAB_7087_0407;
      *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + DAT_4125_0e98 * 0x41) = 1;
      if (iVar1 < 100) {
        *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + DAT_4125_0e98 * 0x41) =
             *(undefined2 *)(iVar1 * 0x41 + 0x157d);
        *(undefined2 *)(DAT_4125_0e98 * 0x41 + 0x15b2) = *(undefined2 *)(iVar1 * 0x41 + 0x157f);
      }
      else {
        *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + DAT_4125_0e98 * 0x41) = 0xfffe;
        *(undefined2 *)(DAT_4125_0e98 * 0x41 + 0x15b2) = 0xfffe;
      }
    }
    FUN_54f6_07e4(0x280000,0x320000);
    auto_ui_city_info();
    auto_ui_city_info();
  }
LAB_7087_0407:
  FUN_7204_03a9();
  return;
}


