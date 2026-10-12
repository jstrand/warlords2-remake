
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far FUN_17be_0064(int param_1,int param_2)

{
  int iVar1;
  undefined2 uVar2;
  int *piVar3;
  int iVar4;
  
  if ((((DAT_4125_166c == 0) || (param_2 == 0)) && (_LAB_4125_1174 == 0)) &&
     (((_LAB_4125_1176 == 0 && (_LAB_4125_1172 == 0)) && (DAT_4125_3700 == 0)))) {
    if ((param_2 != 0) && (DAT_4125_166a == 0)) {
      FUN_2372_0047(&param_1);
    }
    if ((param_1 < 0x41) || (0x5a < param_1)) {
      if (param_1 == 0x1b) {
        for (iVar4 = 0; ((undefined2 *)&DAT_4125_1514)[iVar4] != 0; iVar4 = iVar4 + 1) {
          uVar2 = ((undefined2 *)&DAT_4125_1514)[iVar4];
          iVar1 = FUN_18a9_05ab(uVar2);
          if (iVar1 == 1) goto LAB_17be_00ea;
        }
      }
      if (param_1 == 0xd) {
        iVar4 = 0;
        while (*(int *)((undefined1 *)&LAB_4125_155b_1 + iVar4 * 2) != 0) {
          uVar2 = *(undefined2 *)((undefined1 *)&LAB_4125_155b_1 + iVar4 * 2);
          iVar1 = FUN_18a9_05ab(uVar2);
          if (iVar1 == 1) {
LAB_17be_00ea:
            FUN_17be_04d8(0x18a9,uVar2,0);
            return 0;
          }
          iVar4 = iVar4 + 1;
        }
      }
      iVar4 = FUN_17be_0444(param_1);
      if (((iVar4 == 0) && (iVar4 = FUN_17be_03ef(param_1), iVar4 == 0)) &&
         (iVar4 = FUN_17be_0430(param_1), iVar4 == 0)) {
        iVar4 = 0x2e;
        piVar3 = (int *)((char *)s___03d_The__s_can_bless_your_armi_4125_0329 + 0xe);
        do {
          if (*piVar3 == param_1) {
                    /* WARNING: Could not recover jumptable at 0x00017d5c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
            uVar2 = (*(code *)piVar3[0x2e])();
            return uVar2;
          }
          piVar3 = piVar3 + 1;
          iVar4 = iVar4 + -1;
        } while (iVar4 != 0);
        sound_chord();
      }
    }
  }
  return 0;
}


