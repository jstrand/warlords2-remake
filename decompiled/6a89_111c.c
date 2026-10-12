
void __cdecl16far FUN_6a89_111c(void)

{
  int iVar1;
  
  for (iVar1 = 0; iVar1 < 0x1d; iVar1 = iVar1 + 1) {
    *(undefined1 *)(current_player * 0x1d + iVar1 + 0x60b) =
         *(undefined1 *)((int)DAT_4125_0d58 + iVar1);
  }
  FUN_1d28_050d(DAT_4125_0d58);
  FUN_54f6_069c();
  game_screen_start(0);
  return;
}


