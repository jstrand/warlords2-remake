
void __cdecl16far city_dialog_show(int mode,int x,int y)

{
  int iVar1;
  int iVar2;
  char *pcVar3;
  
  DAT_4125_0e94 = 0;
  if (mode == 4) {
    iVar2 = FUN_828e_06fd(x,y,1);
    iVar1 = FUN_8611_15ff(1,*(undefined2 *)(iVar2 * 0x1f + 0x811),
                          *(undefined2 *)(iVar2 * 0x1f + 0x813));
    if (iVar1 == 0) {
      return;
    }
    if (((LAB_4125_0e92 == 4) && (DAT_4125_0e98 == iVar2)) && (DAT_4125_166a != 0))
    goto code_r0x000720ad;
    DAT_4125_0e9a = *(undefined2 *)(iVar2 * 0x1f + 0x811);
    DAT_4125_0e9c = *(undefined2 *)(iVar2 * 0x1f + 0x813);
    DAT_4125_0e98 = iVar2;
LAB_7204_021d:
    if (-1 < DAT_4125_0e98) {
      LAB_4125_0e92 = mode;
      FUN_22bf_07a5();
      auto_ui_city_info();
      city_dialog_draw_map();
      auto_str_join_data_not_found();
      DAT_4125_166a = 1;
      city_dialog_buttons();
      FUN_22bf_0757();
      if (opt_tutorial != 0) {
        if ((DAT_3c04_0134 >> 1 & 1) == 0) {
          DAT_3c04_0134 = DAT_3c04_0134 | 2;
          pcVar3 = get_file_string(0x19,1);
          FUN_8065_168d((char *)pcVar3,(int)((ulong)pcVar3 >> 0x10));
        }
        if (((DAT_3c04_0134 & 0x20) == 0) &&
           (1 < (int)((undefined2 *)&side_city_count)[current_player])) {
          DAT_3c04_0134 = DAT_3c04_0134 | 0x20;
          pcVar3 = get_file_string(0x19,6);
          FUN_8065_168d((char *)pcVar3,(int)((ulong)pcVar3 >> 0x10));
        }
      }
    }
  }
  else {
    if (mode == 0) {
      iVar2 = nearest_city(x,y,0xe,1);
      iVar1 = FUN_8611_15ff(1,*(undefined2 *)(iVar2 * 0x41 + 0x157d),
                            *(undefined2 *)(iVar2 * 0x41 + 0x157f));
      if (((iVar1 == 0) &&
          (iVar1 = FUN_8611_15ff(1,*(int *)(iVar2 * 0x41 + 0x157d) + 1,
                                 *(undefined2 *)(iVar2 * 0x41 + 0x157f)), iVar1 == 0)) &&
         ((iVar1 = FUN_8611_15ff(1,*(undefined2 *)(iVar2 * 0x41 + 0x157d),
                                 *(int *)(iVar2 * 0x41 + 0x157f) + 1), iVar1 == 0 &&
          (iVar1 = FUN_8611_15ff(1,*(int *)(iVar2 * 0x41 + 0x157d) + 1,
                                 *(int *)(iVar2 * 0x41 + 0x157f) + 1), iVar1 == 0)))) {
        return;
      }
      if (((LAB_4125_0e92 != 0) || (DAT_4125_0e98 != iVar2)) || (DAT_4125_166a == 0))
      goto LAB_7204_01f8;
    }
    else {
      iVar2 = nearest_city(x,y,current_player,1);
      if ((((LAB_4125_0e92 != mode) || (DAT_4125_0e98 != iVar2)) || (DAT_4125_166a == 0)) ||
         (mode == 1)) {
        if (*(char *)(iVar2 * 0x41 + 0x15aa) < '\x01') {
          DAT_4125_0e9e = -1;
        }
        else {
          DAT_4125_0e9e = (int)*(char *)(iVar2 * 0x41 + 0x15a9);
        }
LAB_7204_01f8:
        DAT_4125_0e98 = iVar2;
        DAT_4125_0e9a = *(undefined2 *)(DAT_4125_0e98 * 0x41 + 0x157d);
        DAT_4125_0e9c = *(undefined2 *)(DAT_4125_0e98 * 0x41 + 0x157f);
        goto LAB_7204_021d;
      }
    }
code_r0x000720ad:
    city_dialog_buttons();
  }
  return;
}


