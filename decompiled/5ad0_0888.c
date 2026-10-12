
void __cdecl16far ai_phase_rescue(void)

{
  byte *pbVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar5 = army_count;
  while (iVar6 = iVar5 + -1, iVar5 != 0) {
    iVar5 = iVar6;
    if (((*(char *)(iVar6 * 0x16 + -0x777b) == (char)current_player) &&
        (*(int *)(iVar6 * 0x16 + -0x7780) != -1)) &&
       (*(char *)(iVar6 * 0x16 + -0x777a) <= *(char *)(iVar6 * 0x16 + -0x7779))) {
      if ((((int)*(char *)(iVar6 * 0x16 + -0x7779) == *(char *)(iVar6 * 0x16 + -0x777a) + 2) &&
          (*(char *)(*(byte *)(*(int *)(iVar6 * 0x16 + -0x777e) * 0xe0 +
                              *(int *)(iVar6 * 0x16 + -0x7780) * 2) + 0x710) != '\n')) &&
         (((*(byte *)(iVar6 * 0x16 + -0x7772) & 0xf) == 1 &&
          (*(char *)(*(byte *)(*(int *)((uint)(*(byte *)(iVar6 * 0x16 + -0x7771) >> 1) * 0x41 +
                                       0x157f) * 0xe0 +
                              *(int *)((uint)(*(byte *)(iVar6 * 0x16 + -0x7771) >> 1) * 0x41 +
                                      0x157d) * 2) + 0x710) != '\n')))) {
        bVar2 = *(byte *)(iVar6 * 0x16 + -0x7771);
        iVar3 = army_count;
        while (iVar4 = iVar3 + -1, iVar3 != 0) {
          iVar3 = iVar4;
          if (((*(char *)(iVar4 * 0x16 + -0x777b) == (char)current_player) &&
              (*(int *)(iVar4 * 0x16 + -0x7780) == *(int *)(iVar6 * 0x16 + -0x7780))) &&
             ((*(int *)(iVar4 * 0x16 + -0x777e) == *(int *)(iVar6 * 0x16 + -0x777e) &&
              (((*(byte *)(iVar4 * 0x16 + -0x7772) & 0xf) == 1 &&
               (*(byte *)(iVar4 * 0x16 + -0x7771) >> 1 == bVar2 >> 1)))))) {
            pbVar1 = (byte *)(iVar4 * 0x16 + -0x7772);
            *pbVar1 = *pbVar1 & 0xf0;
            pbVar1 = (byte *)(iVar4 * 0x16 + -0x7772);
            *pbVar1 = *pbVar1 & 0x8f;
          }
        }
      }
      if ((((*(uint *)(iVar6 * 0x16 + -0x7774) & 0x20) == 0) &&
          ((*(byte *)(iVar6 * 0x16 + -0x7772) & 0xf) == 0)) &&
         (*(char *)(*(byte *)(*(int *)(iVar6 * 0x16 + -0x777e) * 0xe0 +
                             *(int *)(iVar6 * 0x16 + -0x7780) * 2) + 0x710) != '\n')) {
        ai_send_army_home(iVar6);
      }
    }
  }
  return;
}


