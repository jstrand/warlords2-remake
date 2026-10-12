
void __cdecl16far ai_phase_last_rescue(void)

{
  uint *puVar1;
  byte *pbVar2;
  int iVar3;
  int iVar4;
  
  iVar3 = army_count;
  while (iVar4 = iVar3 + -1, iVar3 != 0) {
    iVar3 = iVar4;
    if ((((*(char *)(iVar4 * 0x16 + -0x777b) == current_player) &&
         (*(int *)(iVar4 * 0x16 + -0x7780) != -1)) &&
        ((int)*(char *)(iVar4 * 0x16 + -0x7779) == *(char *)(iVar4 * 0x16 + -0x777a) + 2)) &&
       (*(char *)(*(byte *)(*(int *)(iVar4 * 0x16 + -0x777e) * 0xe0 +
                           *(int *)(iVar4 * 0x16 + -0x7780) * 2) + 0x710) != '\n')) {
      puVar1 = (uint *)(iVar4 * 0x16 + -0x7774);
      *puVar1 = *puVar1 & 0xff5f;
      pbVar2 = (byte *)(iVar4 * 0x16 + -0x7772);
      *pbVar2 = *pbVar2 & 0xf0;
      *(undefined2 *)(iVar4 * 0x16 + -0x776c) = 0xffff;
      *(undefined2 *)(iVar4 * 0x16 + -0x776e) = 0xffff;
      ai_send_army_home(iVar4);
    }
  }
  return;
}


