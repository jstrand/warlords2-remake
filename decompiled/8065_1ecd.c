
void __cdecl16far FUN_8065_1ecd(void)

{
  int iVar1;
  int iVar2;
  
  FUN_22bf_07a5();
  FUN_54f6_069c();
  music_cue(10);
  FUN_54f6_0000();
  auto_str_join_data_not_found();
  FUN_18a9_066e(0x101e4);
  FUN_22bf_0757();
  for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
    if (*(int *)(iVar1 * 2 + 0x137) != 0) {
      for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
        if ((*(int *)(iVar2 * 2 + 0x137) != 0) && (*(int *)(iVar2 * 2 + 0xd0) == 0)) {
          *(undefined2 *)(iVar1 * 2 + 0x10f3) = current_player;
        }
      }
    }
  }
  return;
}


