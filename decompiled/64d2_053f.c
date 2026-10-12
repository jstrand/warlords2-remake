
void __cdecl16far toggle_side_observe(int param_1)

{
  *(uint *)(param_1 * 2 + 0x147) = (uint)(*(int *)(param_1 * 2 + 0x147) == 0);
  ui_side_setup_list(param_1,0);
  FUN_64d2_0067();
  return;
}


