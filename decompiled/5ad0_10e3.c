
void __cdecl16far ai_phase_specials(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_SS;
  int local_12 [8];
  
  iVar2 = city_count;
  while (iVar3 = iVar2 + -1, iVar2 != 0) {
    iVar2 = iVar3;
    if (*(char *)(iVar3 * 0x41 + 0x1592) == (char)current_player) {
      uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      if (((((*(byte *)((int)DAT_451b_018e + iVar3 + 0x11e) & 0x20) != 0) &&
           (*(char *)((int)DAT_451b_018e + iVar3 + 0x56) != '\a')) &&
          (iVar1 = ai_collect_stack(*(int *)(iVar3 * 0x41 + 0x157d) + 1,
                                    *(int *)(iVar3 * 0x41 + 0x157f),
                                    (int *)CONCAT22(unaff_SS,local_12),0,0,0), iVar1 != 0)) &&
         ((iVar1 = ai_send_hero_party((int *)CONCAT22(unaff_SS,local_12)), iVar1 == 1 &&
          (iVar1 = city_is_building(), iVar1 == 0)))) {
        *(undefined1 *)((int)DAT_451b_018e + iVar3 + 0x56) = 0xd;
      }
    }
  }
  return;
}


