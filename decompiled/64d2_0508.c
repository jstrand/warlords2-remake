
void __cdecl16far toggle_side_enhanced(int param_1)

{
  *(uint *)(param_1 * 2 + 0xf0) = (uint)(*(int *)(param_1 * 2 + 0xf0) == 0);
  ui_side_setup_list(param_1,0);
  FUN_64d2_0067();
  return;
}


