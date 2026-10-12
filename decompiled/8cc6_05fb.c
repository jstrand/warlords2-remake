
void __cdecl16far reset_movement(void)

{
  char *pcVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  int local_4;
  
  iVar3 = army_count;
  while (iVar4 = iVar3 + -1, iVar5 = army_count, iVar3 != 0) {
    iVar3 = iVar4;
    if ((*(char *)(iVar4 * 0x16 + -0x777b) == current_player) &&
       (-1 < *(int *)(iVar4 * 0x16 + -0x7780))) {
      if (*(char *)(iVar4 * 0x16 + -0x7779) < '\x03') {
        cVar2 = *(char *)(iVar4 * 0x16 + -0x7779);
      }
      else {
        cVar2 = '\x02';
      }
      if ((*(uint *)(iVar4 * 0x16 + -0x7774) & 0x1000) == 0) {
        *(char *)(iVar4 * 0x16 + -0x7779) = *(char *)(iVar4 * 0x16 + -0x777a) + cVar2;
      }
      else {
        *(char *)(iVar4 * 0x16 + -0x7779) = cVar2 + '\x14';
      }
      if ((*(int *)(iVar4 * 0x16 + -0x776e) == *(int *)(iVar4 * 0x16 + -0x7780)) &&
         (*(int *)(iVar4 * 0x16 + -0x776c) == *(int *)(iVar4 * 0x16 + -0x777e))) {
        *(undefined2 *)(iVar4 * 0x16 + -0x776e) = 0xffff;
        *(undefined2 *)(iVar4 * 0x16 + -0x776c) = 0xffff;
      }
    }
  }
  while (iVar4 = iVar5 + -1, iVar3 = army_count, iVar5 != 0) {
    iVar5 = iVar4;
    if (((*(char *)(iVar4 * 0x16 + -0x777b) == current_player) &&
        (*(char *)(iVar4 * 0x16 + -0x777c) == '\x1c')) &&
       (iVar3 = hero_double_move_items(iVar4 * 0x16 + -0x7780), iVar3 != 0)) {
      armies_at_tile(*(int *)(iVar4 * 0x16 + -0x7780),*(int *)(iVar4 * 0x16 + -0x777e),
                     (int *)CONCAT22(unaff_SS,&local_4));
      for (iVar3 = 0; iVar3 < local_4; iVar3 = iVar3 + 1) {
        uVar6 = (undefined2)
                ((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_1ebe + iVar3 * 2) >> 0x10);
        pcVar1 = (char *)(((undefined2 *)&DAT_451b_1ebe)[iVar3 * 2] + 7);
        *pcVar1 = *pcVar1 + *(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ebe + iVar3 * 2)
                                     + 6);
      }
    }
  }
  while (iVar5 = iVar3 + -1, iVar3 != 0) {
    iVar3 = iVar5;
    if ((*(char *)(iVar5 * 0x16 + -0x777b) == current_player) &&
       ('c' < *(char *)(iVar5 * 0x16 + -0x7779))) {
      *(undefined1 *)(iVar5 * 0x16 + -0x7779) = 99;
    }
  }
  return;
}


