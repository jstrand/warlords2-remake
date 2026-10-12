
void __cdecl16far FUN_7087_050e(int param_1)

{
  undefined2 local_c;
  undefined1 local_a [8];
  
  if (param_1 == 2) {
    DAT_4125_0e96 = (uint)(DAT_4125_0e96 == 0);
    FUN_7087_08e4();
    goto LAB_7087_0631;
  }
  if (param_1 == 0) {
    if (DAT_4125_0e94 == 0) {
      DAT_4125_0e94 = 1;
      FUN_834b_08df(0,DAT_4125_0e98,0xffff);
      FUN_834b_04d3(0);
    }
    else if (DAT_4125_0e94 == 1) {
      DAT_4125_0e94 = 0;
      FUN_7087_08e4();
    }
    else {
      if (DAT_4125_0e94 != 2) goto LAB_7087_0631;
      DAT_4125_0e94 = 1;
LAB_7087_0583:
      FUN_834b_08df(0,DAT_4125_0e98);
      FUN_834b_04d3(0);
      auto_ui_vector_help();
    }
  }
  else {
    if (param_1 != 1) goto LAB_7087_0631;
    if (DAT_4125_0e94 == 0) {
      DAT_4125_0e94 = 2;
      FUN_7087_0410(DAT_4125_0e98,local_a);
      FUN_834b_08df(0,DAT_4125_0e98,local_c);
      FUN_834b_04d3(0);
    }
    else {
      if (DAT_4125_0e94 == 1) {
        DAT_4125_0e94 = 2;
        FUN_7087_0410(DAT_4125_0e98,local_a);
        goto LAB_7087_0583;
      }
      if (DAT_4125_0e94 != 2) goto LAB_7087_0631;
      DAT_4125_0e94 = 0;
      FUN_7087_08e4();
    }
  }
  auto_ui_vector_help();
LAB_7087_0631:
  FUN_7204_03a9();
  return;
}


