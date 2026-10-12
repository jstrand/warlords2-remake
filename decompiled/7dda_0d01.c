
void __cdecl16far advisor_subtitles(char *param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  char *pcVar4;
  uint unaff_DI;
  int iVar5;
  undefined2 uVar6;
  void *unaff_SS;
  undefined1 auStack_c6 [10];
  long alStack_bc [4];
  int aiStack_ac [13];
  byte bStack_92;
  uint uStack_91;
  char local_8a [128];
  int iStack_a;
  int local_8;
  int iStack_6;
  int local_4;
  
  local_8 = 0;
  iVar1 = open(param_1,2);
  if (-1 < iVar1) {
    read(iVar1,(void *)CONCAT22(0x80,unaff_SS),unaff_DI);
    close(iVar1);
    iVar1 = 0;
    local_4 = 0;
    FUN_78a8_06ae(2,0xf,0,3);
    while ((iVar2 = local_8, local_4 == 0 && (local_8 = local_8 + 1, iVar2 < 5))) {
      iStack_6 = iVar1;
      for (; iVar1 < 0x80; iVar1 = iVar1 + 1) {
        if (local_8a[iVar1] == '|') {
          local_8a[iVar1] = '\0';
          iVar1 = iVar1 + 1;
          break;
        }
        if (local_8a[iVar1] == '~') {
          local_8a[iVar1] = '\0';
          iVar1 = iVar1 + 1;
          local_4 = 1;
          break;
        }
      }
      FUN_7ecb_0a1b();
      pcVar4 = local_8a + iStack_6;
      iVar2 = FUN_21e2_0a8e(pcVar4);
      iVar5 = (iVar2 >> 4) * 0x10 + 0x10;
      bStack_92 = 0;
      uStack_91 = 0;
      iVar2 = FUN_2012_01fe(auStack_c6);
      if (iVar2 == 0) {
        return;
      }
      uVar6 = 0x7ecb;
      FUN_7ecb_0a1b();
      if ((alStack_bc[0] != 0) && ((uStack_91 & 10) == 0)) {
        for (iStack_a = 0; iStack_a < (int)(uint)bStack_92; iStack_a = iStack_a + 1) {
          uVar6 = 0x1d28;
          iVar3 = mem_lock((int)alStack_bc[iStack_a],(int)((ulong)alStack_bc[iStack_a] >> 0x10));
          iVar2 = iStack_a * 2;
          aiStack_ac[iStack_a * 2 + 1] = (int)pcVar4;
          aiStack_ac[iVar2] = iVar3;
        }
      }
      (*DAT_451b_2a74)(uVar6,0x342a,0x4125,0x138 - iVar5 / 2,0x198,iVar5,0x11,auStack_c6);
      if ((alStack_bc[0] != 0) && ((uStack_91 & 10) == 0)) {
        for (iStack_a = 0; iStack_a < (int)(uint)bStack_92; iStack_a = iStack_a + 1) {
          mem_unlock((int)alStack_bc[iStack_a],(int)((ulong)alStack_bc[iStack_a] >> 0x10));
        }
      }
      FUN_7ecb_00d6(0x138,0x198,local_8a + iStack_6);
      FUN_7ecb_015e(0x28);
      (*DAT_451b_2a74)(0x7ecb,auStack_c6);
      FUN_2012_0975(auStack_c6);
    }
    FUN_7ecb_0a1b();
  }
  return;
}


