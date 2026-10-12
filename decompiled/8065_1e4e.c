
void __cdecl16far FUN_8065_1e4e(void)

{
  int iVar1;
  
  for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
    if ((*(int *)(iVar1 * 2 + 0x137) != 0) && (*(int *)(iVar1 * 2 + 0xd0) == 1)) {
      *(undefined2 *)(iVar1 * 2 + 0x137) = 0;
    }
  }
  game_won = 1;
  FUN_6d51_1244(current_player,7,current_player,0,0x2b03,0x4125);
  FUN_22bf_07a5();
  FUN_54f6_069c();
  FUN_8065_1fbd();
  FUN_22bf_0757();
  return;
}


