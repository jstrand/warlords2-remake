
void __cdecl16far ui_vector_map_click(int x,int y,int button)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_SS;
  int local_16 [4];
  int local_e;
  undefined1 local_c [8];
  int local_4;
  
  if ((button == 1) && ((bRam00000417 & 3) != 0)) {
    ui_vector_set_destination(x,y,1);
    DAT_4125_0e94 = 0;
  }
  else {
    iVar1 = nearest_vector_target(x,y);
    cities_vectoring_to(iVar1,local_c);
    cities_vectoring_to(DAT_4125_0e98,local_16);
    if (((DAT_4125_0e94 == 2) &&
        ((((0 < local_e && (local_e + local_4 < 5)) && (local_16[0] != iVar1)) &&
         ((local_16[1] != iVar1 && (local_16[2] != iVar1)))))) && (local_16[3] != iVar1)) {
      for (iVar2 = 0; iVar2 < local_e; iVar2 = iVar2 + 1) {
        if (iVar1 < 100) {
          *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + local_16[iVar2] * 0x41) =
               *(undefined2 *)(iVar1 * 0x41 + 0x157d);
          *(undefined2 *)(local_16[iVar2] * 0x41 + 0x15b2) = *(undefined2 *)(iVar1 * 0x41 + 0x157f);
        }
        else {
          *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + local_16[iVar2] * 0x41) = 0xfffe;
          *(undefined2 *)(local_16[iVar2] * 0x41 + 0x15b2) = 0xfffe;
        }
      }
    }
    if (DAT_4125_0e94 == 1) {
      ui_vector_set_destination(x,y,button);
    }
    iVar2 = DAT_4125_0e98;
    if ((DAT_4125_0e94 != 1) && (iVar1 < 100)) {
      iVar2 = iVar1;
    }
    DAT_4125_0e94 = 0;
    city_dialog_show(3,*(int *)(iVar2 * 0x41 + 0x157d),*(int *)(iVar2 * 0x41 + 0x157f));
    ui_city_dialog_map();
    ui_vector_help(0);
    ui_vector_help(1);
    city_dialog_buttons();
  }
  return;
}


