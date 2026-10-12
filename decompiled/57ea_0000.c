
void __cdecl16far ai_phase_neutral(void)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_SS;
  char local_66 [100];
  
  iVar2 = 100;
  while (iVar1 = army_count, iVar2 != 0) {
    local_66[iVar2 + -1] = '\0';
    iVar2 = iVar2 + -1;
  }
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    iVar1 = iVar2;
    if (((*(char *)(iVar2 * 0x16 + -0x777b) == current_player) &&
        ((*(byte *)(iVar2 * 0x16 + -0x7772) & 0xf) == 1)) &&
       (*(byte *)(iVar2 * 0x16 + -0x7771) >> 1 < city_count)) {
      local_66[*(byte *)(iVar2 * 0x16 + -0x7771) >> 1] =
           local_66[*(byte *)(iVar2 * 0x16 + -0x7771) >> 1] + '\x01';
    }
  }
  iVar2 = 0;
  while ((iVar2 < 10 && (iVar1 = ai_neutral_targets(iVar2,local_66), iVar1 != 0))) {
    iVar2 = iVar2 + 1;
  }
  return;
}


