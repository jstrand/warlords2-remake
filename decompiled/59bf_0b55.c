
void __cdecl16far ai_clear_seen_flags(void)

{
  int iVar1;
  int iVar2;
  
  iVar1 = city_count;
  if (opt_hidden_map != 0) {
    while (iVar2 = iVar1 + -1, iVar1 != 0) {
      iVar1 = iVar2;
      if ((*(byte *)((int)DAT_451b_018e + iVar2 + 0x11e) & 1) != 0) {
        ai_city_seen_check(iVar2);
      }
    }
  }
  return;
}


