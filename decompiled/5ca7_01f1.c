
void __cdecl16far ai_phase_clean_city(void)

{
  int iVar1;
  int iVar2;
  
  iVar1 = city_count;
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    iVar1 = iVar2;
    if ((*(char *)(iVar2 * 0x41 + 0x1592) == (char)current_player) &&
       ((*(byte *)((int)DAT_451b_018e + iVar2 + 0x11e) & 4) == 0)) {
      ai_city_garrison_check(iVar2,0);
    }
  }
  return;
}


