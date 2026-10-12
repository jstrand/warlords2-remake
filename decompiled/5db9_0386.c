
void __cdecl16far ai_turn_setup(void)

{
  byte *pbVar1;
  undefined1 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  
  *(undefined2 *)(current_player * 2 + 0x40f2) = *(undefined2 *)((int)DAT_451b_018e + 0x38);
  iVar4 = city_count;
  while (iVar5 = iVar4 + -1, iVar4 != 0) {
    iVar4 = iVar5;
    if (*(char *)(iVar5 * 0x41 + 0x1592) == current_player) {
      uVar6 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      if ((*(char *)((int)DAT_451b_018e + iVar5 + 0x56) == '\r') ||
         (*(char *)((int)DAT_451b_018e + iVar5 + 0x56) == '\v')) {
        iVar3 = game_turn();
        if (iVar3 < 5) {
          uVar2 = 4;
        }
        else {
          uVar2 = 8;
        }
        *(undefined1 *)((int)DAT_451b_018e + iVar5 + 0x56) = uVar2;
      }
      if ((opt_quick_start != 0) && (opt_hidden_map != 0)) {
        iVar3 = game_turn();
        if (iVar3 == 1) {
          *(undefined1 *)((int)DAT_451b_018e + iVar5 + 0x56) = 0xd;
        }
        else {
          iVar3 = game_turn();
          if (iVar3 < 3) {
            *(undefined1 *)((int)DAT_451b_018e + iVar5 + 0x56) = 0xb;
          }
        }
      }
      pbVar1 = (byte *)((int)DAT_451b_018e + iVar5 + 0x11e);
      *pbVar1 = *pbVar1 & 0xfb;
    }
  }
  return;
}


