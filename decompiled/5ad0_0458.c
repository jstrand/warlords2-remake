
void __cdecl16far ai_phase_move_explore(void)

{
  uint *puVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 unaff_SS;
  int local_16 [8];
  int local_6;
  int local_4;
  
  iVar4 = army_count;
  do {
    do {
      iVar5 = iVar4 + -1;
      if (iVar4 == 0) {
        return;
      }
      iVar4 = iVar5;
    } while ((((*(char *)(iVar5 * 0x16 + -0x777b) != (char)current_player) ||
              (*(int *)(iVar5 * 0x16 + -0x7780) == -1)) ||
             ((*(uint *)(iVar5 * 0x16 + -0x7774) & 0x100) == 0)) ||
            (puVar1 = (uint *)(iVar5 * 0x16 + -0x7774), *puVar1 = *puVar1 & 0xfeff,
            *(char *)(iVar5 * 0x16 + -0x777c) != '\x1c'));
    bVar2 = true;
    do {
      if (((!bVar2) ||
          (iVar3 = ai_collect_stack(*(int *)(iVar5 * 0x16 + -0x7780),
                                    *(int *)(iVar5 * 0x16 + -0x777e),
                                    (int *)CONCAT22(unaff_SS,local_16),
                                    (*(byte *)(iVar5 * 0x16 + -0x7772) & 0x70) >> 4,
                                    *(byte *)(iVar5 * 0x16 + -0x7772) & 0xf,0), iVar3 == 0)) ||
         (*(char *)(iVar5 * 0x16 + -0x7779) < '\x03')) break;
      local_4 = *(int *)(iVar5 * 0x16 + -0x7780);
      local_6 = *(int *)(iVar5 * 0x16 + -0x777e);
      iVar3 = ai_send_hero_party((int *)CONCAT22(unaff_SS,local_16));
      if ((iVar3 == 0) || (iVar3 == 1)) {
        bVar2 = false;
      }
    } while (((*(int *)(iVar5 * 0x16 + -0x7780) != local_4) ||
             (*(int *)(iVar5 * 0x16 + -0x777e) != local_6)) ||
            (*(char *)(iVar5 * 0x16 + -0x777a) <= *(char *)(iVar5 * 0x16 + -0x7779)));
  } while( true );
}


