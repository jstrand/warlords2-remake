
void __cdecl16far ai_phase_update_hide(void)

{
  byte *pbVar1;
  int iVar2;
  int city;
  uint unaff_DI;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  uint unaff_SS;
  
  ai_clear_seen_flags();
  iVar2 = city_count;
LAB_59bf_0c87:
  do {
    do {
      city = iVar2 + -1;
      if (iVar2 == 0) {
        return;
      }
      iVar2 = city;
    } while (*(char *)((int)DAT_451b_018e + city + 0x56) != '\x02');
    ai_neutral_neighbours(city,(char *)(ulong)unaff_SS,(char *)((ulong)unaff_DI << 0x10));
    iVar3 = 6;
    do {
      iVar4 = iVar3 + -1;
      if (iVar3 == 0) goto LAB_59bf_0c87;
      pbVar1 = &stack0xfff5 + iVar3;
      iVar3 = iVar4;
    } while ((*pbVar1 == 0xff) ||
            (uVar5 = (undefined2)((ulong)DAT_451b_018e >> 0x10),
            (*(byte *)((int)DAT_451b_018e + (uint)*pbVar1 + 0x11e) & 1) != 0));
    *(undefined1 *)((int)DAT_451b_018e + city + 0x56) = 3;
  } while( true );
}


