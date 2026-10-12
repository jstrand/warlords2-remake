
undefined2 __cdecl16far FUN_2372_00f0(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  uint local_12;
  undefined2 uStack_10;
  
  DAT_451b_300f = param_1;
  DAT_451b_301b = 0xff;
  DAT_451b_3019 = 0xff;
  DAT_451b_3011 = FUN_2372_0bcb();
  if (DAT_451b_3011 != 0xff) {
    FUN_22bf_07a5();
    FUN_2372_102d(DAT_451b_3011);
    FUN_2372_049b(DAT_451b_3011);
    FUN_22bf_0757();
    DAT_4125_3700 = 1;
    while (DAT_451b_300f == 2) {
      DAT_451b_3013 = FUN_2372_0c98(DAT_451b_3011);
      FUN_251d_008a();
      FUN_21b5_0003(0xffffffff,&local_12);
      if (local_12 < 8) {
                    /* WARNING: Switch is manually overridden */
        switch((undefined4)*(undefined2 *)(local_12 * 2 + 0x285)) {
        case 0x2388c:
          break;
        case 0x2388e:
          DAT_451b_300f = 1;
          break;
        case 0x23896:
          FUN_2372_0295(uStack_10);
        }
      }
      FUN_2372_0ad8(DAT_451b_3011,DAT_451b_3013);
    }
    while (DAT_451b_300f == 1) {
      FUN_251d_008a();
      FUN_21b5_0003(0xffffffff,&local_12);
      if (local_12 < 8) {
                    /* WARNING: Switch is manually overridden */
        switch((undefined4)*(undefined2 *)(local_12 * 2 + 0x275)) {
        case 0x238e2:
          break;
        case 0x238e4:
          break;
        case 0x238e6:
          DAT_451b_300f = 3;
        }
      }
      FUN_2372_0428();
    }
    DAT_4125_3700 = 0;
    FUN_22bf_07a5();
    FUN_2372_102d(0xff);
    FUN_2372_09f9(DAT_451b_3011);
    FUN_22bf_0757();
    if (((DAT_451b_300f != 4) && (DAT_451b_3011 != 0xff)) && (DAT_451b_3013 != 0xff)) {
      uVar4 = (undefined2)((ulong)DAT_451b_2fc8 >> 0x10);
      iVar3 = (int)DAT_451b_2fc8;
      iVar1 = *(int *)(iVar3 + 10);
      while (iVar2 = iVar1 + -1, iVar1 != 0) {
        iVar1 = iVar2;
        if ((*(int *)((int)*(undefined4 *)(iVar3 + 4) + iVar2 * 8 + 2) == DAT_451b_3011) &&
           (*(int *)((int)*(undefined4 *)(iVar3 + 4) + iVar2 * 8 + 4) == DAT_451b_3013)) {
          return *(undefined2 *)((int)*(undefined4 *)(iVar3 + 4) + iVar2 * 8 + 6);
        }
      }
    }
  }
  return 0;
}


