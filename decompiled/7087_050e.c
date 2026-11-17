
void __cdecl16far ui_vector_button(int n)

{
  int which;
  int local_c;
  undefined1 local_a [8];
  
  if (n == 2) {
    DAT_4125_0e96 = (uint)(DAT_4125_0e96 == 0);
    ui_city_dialog_map();
    goto LAB_7087_0631;
  }
  if (n == 0) {
    if (DAT_4125_0e94 == 0) {
      DAT_4125_0e94 = 1;
      draw_vector_map((int *)0x0,DAT_4125_0e98,-1);
      FUN_834b_04d3(0);
    }
    else {
      if (DAT_4125_0e94 != 1) {
        if (DAT_4125_0e94 != 2) goto LAB_7087_0631;
        DAT_4125_0e94 = 1;
        local_c = -1;
LAB_7087_0583:
        draw_vector_map((int *)0x0,DAT_4125_0e98,local_c);
        FUN_834b_04d3(0);
        ui_vector_help(0);
        goto LAB_7087_0629;
      }
      DAT_4125_0e94 = 0;
      ui_city_dialog_map();
    }
    which = 0;
  }
  else {
    if (n != 1) goto LAB_7087_0631;
    if (DAT_4125_0e94 == 0) {
      DAT_4125_0e94 = 2;
      cities_vectoring_to(DAT_4125_0e98,local_a);
      draw_vector_map((int *)0x0,DAT_4125_0e98,local_c);
      FUN_834b_04d3(0);
    }
    else {
      if (DAT_4125_0e94 == 1) {
        DAT_4125_0e94 = 2;
        cities_vectoring_to(DAT_4125_0e98,local_a);
        goto LAB_7087_0583;
      }
      if (DAT_4125_0e94 != 2) goto LAB_7087_0631;
      DAT_4125_0e94 = 0;
      ui_city_dialog_map();
    }
LAB_7087_0629:
    which = 1;
  }
  ui_vector_help(which);
LAB_7087_0631:
  city_dialog_buttons();
  return;
}


