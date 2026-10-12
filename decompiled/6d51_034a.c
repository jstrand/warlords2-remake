
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_6d51_034a(void)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined4 in_EDX;
  ulong uVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 unaff_SS;
  uint local_40 [8];
  uint local_30 [8];
  int local_20;
  float local_1e;
  char local_1a [10];
  uint local_10;
  uint local_e;
  int *local_c;
  undefined4 local_8;
  int local_4;
  
  if (DAT_4125_0dc4 == 0) {
    DAT_451b_04c6 = 0x4042;
    DAT_451b_04c4 = (code *)((int)(undefined2 *)&DAT_4125_0083 + 1);
    local_4 = 10;
    for (iVar6 = 1; iVar6 <= DAT_451b_04dc + -1; iVar6 = iVar6 + 1) {
      iVar7 = FUN_6d51_10eb(iVar6);
      uVar8 = (undefined2)in_EDX;
      local_8 = CONCAT22(uVar8,iVar7);
      for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
        piVar5 = (int *)(iVar7 + iVar2 * 2);
        local_c = (int *)CONCAT22(uVar8,piVar5);
        if (local_4 <= *piVar5) {
          local_4 = *piVar5;
        }
      }
    }
  }
  else if (DAT_4125_0dc4 == 2) {
    DAT_451b_04c6 = 0x4042;
    DAT_451b_04c4 = (code *)0x93;
    local_4 = 500;
    for (iVar6 = 1; iVar6 <= DAT_451b_04dc + -1; iVar6 = iVar6 + 1) {
      iVar7 = FUN_6d51_119c(iVar6);
      uVar8 = (undefined2)in_EDX;
      local_8 = CONCAT22(uVar8,iVar7);
      for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
        piVar5 = (int *)(iVar7 + iVar2 * 2);
        local_c = (int *)CONCAT22(uVar8,piVar5);
        if (local_4 <= *piVar5) {
          local_4 = *piVar5;
        }
      }
    }
  }
  else if (DAT_4125_0dc4 == 3) {
    DAT_451b_04c6 = 0x4042;
    DAT_451b_04c4 = (code *)((int)(undefined2 *)&DAT_4125_008d + 1);
    local_4 = 100;
    for (iVar6 = 1; iVar6 <= DAT_451b_04dc + -1; iVar6 = iVar6 + 1) {
      iVar7 = FUN_6d51_1161(iVar6);
      uVar8 = (undefined2)in_EDX;
      local_8 = CONCAT22(uVar8,iVar7);
      for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
        piVar5 = (int *)(iVar7 + iVar2 * 2);
        local_c = (int *)CONCAT22(uVar8,piVar5);
        if (local_4 <= *piVar5) {
          local_4 = *piVar5;
        }
      }
    }
  }
  FUN_22bf_07a5();
  FUN_24d0_0002(0);
  FUN_2133_0344(DAT_4125_0dc6 + -1,DAT_4125_0dc8 + -1,DAT_4125_0dcc);
  FUN_2133_02fe(DAT_4125_0dc6 + -1,DAT_4125_0dc8 + DAT_4125_0dcc + -2,DAT_4125_0dca);
  FUN_2133_02fe(DAT_4125_0dc6 + -5,DAT_4125_0dc8 + -1,4);
  FUN_2133_02fe(DAT_4125_0dc6 + -5,DAT_4125_0dc8 + DAT_4125_0dcc / 2 + -1,4);
  FUN_2133_02fe(DAT_4125_0dc6 + -5,DAT_4125_0dc8 + DAT_4125_0dcc + -2,4);
  FUN_2133_0344(DAT_4125_0dc6 + -1,DAT_4125_0dc8 + DAT_4125_0dcc + -2,4);
  FUN_2133_0344(DAT_4125_0dc6 + DAT_4125_0dca / 2 + -1,DAT_4125_0dc8 + DAT_4125_0dcc + -2,4);
  FUN_2133_0344(DAT_4125_0dc6 + DAT_4125_0dca + -2,DAT_4125_0dc8 + DAT_4125_0dcc + -2,4);
  FUN_24d0_0002(0xf);
  FUN_2133_0344((int)_DAT_4125_0dc6,(int)((ulong)_DAT_4125_0dc6 >> 0x10),DAT_4125_0dcc);
  FUN_2133_02fe(DAT_4125_0dc6,DAT_4125_0dc8 + DAT_4125_0dcc + -1,DAT_4125_0dca);
  FUN_2133_02fe(DAT_4125_0dc6 + -4,DAT_4125_0dc8,4);
  FUN_2133_02fe(DAT_4125_0dc6 + -4,DAT_4125_0dc8 + DAT_4125_0dcc / 2,4);
  FUN_2133_02fe(DAT_4125_0dc6 + -4,DAT_4125_0dc8 + DAT_4125_0dcc + -1,4);
  FUN_2133_0344(DAT_4125_0dc6,DAT_4125_0dc8 + DAT_4125_0dcc + -1,4);
  FUN_2133_0344(DAT_4125_0dc6 + DAT_4125_0dca / 2,DAT_4125_0dc8 + DAT_4125_0dcc + -1,4);
  FUN_2133_0344(DAT_4125_0dc6 + DAT_4125_0dca + -1,DAT_4125_0dc8 + DAT_4125_0dcc + -1,4);
  sprintf((char *)CONCAT22(unaff_SS,local_1a),(char *)0x41250de6,local_4);
  FUN_7ecb_0103();
  FUN_7ecb_0103();
  sprintf((char *)CONCAT22(unaff_SS,local_1a),(char *)0x41250deb,DAT_451b_04dc + -1);
  FUN_7ecb_00d6();
  FUN_7ecb_0103();
  uVar4 = (ulong)(uint)(DAT_4125_0dc6 + DAT_4125_0dca / 2);
  uVar8 = 0x7ecb;
  FUN_7ecb_00d6();
  for (iVar6 = 0; iVar6 < 8; iVar6 = iVar6 + 1) {
    local_30[iVar6] = DAT_4125_0dc6 + 1;
    local_40[iVar6] = (DAT_4125_0dc8 + DAT_4125_0dcc) - 2;
  }
  for (iVar6 = 1; uVar3 = (undefined2)uVar4, iVar6 <= DAT_451b_04dc + -1; iVar6 = iVar6 + 1) {
    uVar1 = (*DAT_451b_04c4)(uVar8,iVar6);
    local_8 = CONCAT22(uVar3,uVar1);
    local_e = DAT_4125_0dc6 +
              (int)(((long)iVar6 * (long)DAT_4125_0dca) / (long)(DAT_451b_04dc + -1));
    uVar4 = (ulong)local_e;
    for (iVar7 = 0; iVar7 < 8; iVar7 = iVar7 + 1) {
      piVar5 = (int *)((int)local_8 + iVar7 * 2);
      uVar8 = (undefined2)((ulong)local_8 >> 0x10);
      local_c = (int *)CONCAT22(uVar8,piVar5);
      local_1e = (float)*piVar5 / (float)local_4;
      local_20 = DAT_4125_0dcc;
      iVar2 = __ftol();
      local_10 = (DAT_4125_0dc8 + DAT_4125_0dcc + -1) - iVar2;
      uVar4 = (ulong)local_10;
      FUN_24d0_0002(*(undefined2 *)(iVar7 * 2 + 0xa0));
      uVar8 = 0x2133;
      FUN_2133_0095(local_30[iVar7],local_40[iVar7],local_e,local_10,0,0);
      local_30[iVar7] = local_e;
      local_40[iVar7] = local_10;
    }
  }
  FUN_22bf_0757();
  return;
}


