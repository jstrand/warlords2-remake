
void __cdecl16far FUN_6a89_116a(void)

{
  int iVar1;
  
  for (iVar1 = 0; iVar1 < 0x1d; iVar1 = iVar1 + 1) {
    *(undefined1 *)(current_player * 0x1d + iVar1 + 0x60b) = *(undefined1 *)(iVar1 + 0x6f3);
  }
  DAT_4125_0d56 = 0xffff;
  auto_ui_fighting_order(1);
  FUN_6a89_1067();
  return;
}


