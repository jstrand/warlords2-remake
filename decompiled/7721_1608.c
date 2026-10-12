
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_ui_resign_2(int param_1)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  char *pcVar4;
  char *pcVar5;
  
  FUN_54f6_069c();
  DAT_4125_166a = 0;
  if (param_1 == 0) {
    pcVar4 = get_string(0xa5,6);
    FUN_8065_10fb((char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    pcVar4 = get_string(0xa5,7);
    FUN_8065_10fb((char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    pcVar4 = get_string(0xa5,8);
    FUN_8065_10fb((char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    pcVar4 = get_string(0xa5,10);
    pcVar5 = get_string(0xa5,9);
    FUN_8065_1160((char *)pcVar5,(int)((ulong)pcVar5 >> 0x10),pcVar4);
    FUN_8611_006c();
  }
  for (iVar3 = 0; iVar2 = army_count, iVar3 < city_count; iVar3 = iVar3 + 1) {
    if (*(char *)(iVar3 * 0x41 + 0x1592) == current_player) {
      city_make_ruins(*(int *)(iVar3 * 0x41 + 0x157d),*(int *)(iVar3 * 0x41 + 0x157f));
    }
  }
  while (iVar3 = iVar2 + -1, iVar2 != 0) {
    iVar2 = iVar3;
    if (*(char *)(iVar3 * 0x16 + -0x777b) == current_player) {
      if (*(char *)(iVar3 * 0x16 + -0x777c) == '\x1c') {
        hero_drop_items(iVar3 * 0x16 + -0x7780,0x2e1d,*(undefined2 *)(iVar3 * 0x16 + -0x7780),
                        *(undefined2 *)(iVar3 * 0x16 + -0x777e));
      }
      FUN_1b62_03be();
    }
  }
  if (opt_hidden_map != 0) {
    for (iVar3 = 0; iVar3 < 0x70; iVar3 = iVar3 + 1) {
      for (iVar2 = 0; iVar2 < 0x9c; iVar2 = iVar2 + 1) {
        pbVar1 = (byte *)(iVar2 * 0x70 + iVar3);
        *pbVar1 = *pbVar1 | 0x20;
      }
    }
    FUN_834b_2200();
  }
  _DAT_451b_1efe = 0;
  DAT_451b_1f02 = 0;
  for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
    *(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar3 * 4) = 0;
    ((undefined2 *)&DAT_451b_1ede)[iVar3 * 2] = 0;
  }
  refresh_map_owners();
  FUN_828e_0af3();
  FUN_8065_011f();
  if (param_1 == 1) {
    pcVar4 = get_string(0xa5,5);
    pcVar5 = get_string(0xa5,4);
    FUN_8065_1160((char *)pcVar5,(int)((ulong)pcVar5 >> 0x10),pcVar4);
  }
  return;
}


