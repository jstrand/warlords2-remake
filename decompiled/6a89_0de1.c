
void __cdecl16far FUN_6a89_0de1(void)

{
  undefined2 in_DX;
  int iVar1;
  
  FUN_54f6_0000(0xb);
  DAT_4125_0d56 = 0xffff;
  DAT_4125_0d58._0_2_ = FUN_1d28_01d5(0x1d);
  DAT_4125_0d58._2_2_ = in_DX;
  for (iVar1 = 0; iVar1 < 0x1d; iVar1 = iVar1 + 1) {
    *(undefined1 *)((int)DAT_4125_0d58 + iVar1) =
         *(undefined1 *)(current_player * 0x1d + iVar1 + 0x60b);
  }
  DAT_4125_166a = 1;
  auto_ui_fighting_order(0);
  auto_str_join_data_not_found(0x1a);
  FUN_6a89_1067();
  return;
}


