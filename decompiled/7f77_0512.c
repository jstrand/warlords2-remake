
void __cdecl16far FUN_7f77_0512(int param_1)

{
  int local_6 [2];
  
  if (((undefined2 *)&DAT_4125_28d8)[param_1] == 0) {
    ((undefined2 *)&DAT_4125_28d8)[param_1] = 1;
  }
  else {
    FUN_22bf_0af4(local_6);
    ((undefined2 *)&DAT_4125_28d0)[param_1] =
         ((local_6[0] - *(int *)(param_1 * 8 + 0x29ac)) * 7) / *(int *)(param_1 * 8 + 0x29b0);
  }
  auto_ui_random_map_terrain(param_1);
  FUN_7f77_011e();
  return;
}


