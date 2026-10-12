
uint __cdecl16far FUN_7ecb_020d(void)

{
  double dVar1;
  bool bVar2;
  clock_t *pcVar3;
  int iVar4;
  clock_t *pcVar5;
  uint uVar6;
  clock_t *unaff_DI;
  int iVar7;
  ulong local_c;
  
  bVar2 = false;
  iVar7 = 0;
  uVar6 = 0;
  pcVar3 = clock(unaff_DI);
  while( true ) {
    while( true ) {
      if (bVar2) {
        DAT_4125_2674 = DAT_4125_2676;
        return uVar6;
      }
      iVar4 = kbhit();
      if (iVar4 != 0) {
        uVar6 = getch();
        bVar2 = true;
        if (uVar6 == 0) {
          uVar6 = getch();
          uVar6 = uVar6 | 0x100;
          if (uVar6 == 0x153) {
            uVar6 = 0x1b;
          }
        }
      }
      iVar7 = iVar7 + 1;
      pcVar5 = clock(unaff_DI);
      if (pcVar5 != (clock_t *)0xffff) break;
      if (iVar7 == 1000) {
        DAT_4125_2674 = DAT_4125_2676;
        return uVar6;
      }
    }
    local_c = (ulong)(uint)(((int)pcVar5 - (int)pcVar3) * 10);
    dVar1 = (double)local_c;
    local_c = (ulong)DAT_4125_2674;
    if ((double)local_c < dVar1 / DAT_4125_2688) break;
    if (iVar7 == 20000) {
      DAT_4125_2674 = DAT_4125_2676;
      return uVar6;
    }
  }
  DAT_4125_2674 = DAT_4125_2676;
  return uVar6;
}


