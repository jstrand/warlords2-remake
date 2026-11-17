
void __cdecl16far ui_vector_set_destination(int x,int y,int button)

{
  int iVar1;
  undefined1 local_e [8];
  int local_6;
  int iStack_4;
  
  if (((button == 1) && ((bRam00000417 & 3) == 0)) && (DAT_4125_0e94 == 0)) {
    iStack_4 = nearest_city(x,y,current_player,0);
    if (iStack_4 != DAT_4125_0e98) {
      city_dialog_show(2,x,y);
    }
  }
  else {
    if (*(char *)(DAT_4125_0e98 * 0x41 + 0x15aa) < '\x01') {
      return;
    }
    iVar1 = nearest_vector_target(x,y);
    cities_vectoring_to(iVar1,local_e);
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
    city_dialog_draw_map();
  }
LAB_7087_0407:
  city_dialog_buttons();
  return;
}


