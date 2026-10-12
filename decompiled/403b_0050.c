
/* WARNING: Instruction at (ram,0x00040413) overlaps instruction at (ram,0x00040411)
    */

void FUN_403b_0050(void)

{
  uint *puVar1;
  undefined1 uVar2;
  uint uVar3;
  uint uVar4;
  undefined2 in_DX;
  char in_BL;
  uint unaff_BP;
  uint uVar5;
  int unaff_SI;
  undefined1 *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  byte in_CF;
  bool bVar6;
  undefined2 uVar7;
  
  puVar1 = (uint *)(unaff_SI + -0x16);
  uVar3 = (uint)in_CF;
  uVar4 = unaff_BP - *puVar1;
  bVar6 = unaff_BP < *puVar1 || uVar4 < uVar3;
  uVar5 = uVar4 - uVar3;
  if (SBORROW2(unaff_BP,*puVar1) == SBORROW2(uVar4,uVar3)) {
    uVar3 = (uint)bVar6;
    uVar4 = uVar5 - *(uint *)(unaff_SI + -0x16);
    unaff_DI[((uVar4 - uVar3) - *(int *)(unaff_SI + -0x16)) -
             (uint)(uVar5 < *(uint *)(unaff_SI + -0x16) || uVar4 < uVar3)] =
         unaff_DI[((uVar4 - uVar3) - *(int *)(unaff_SI + -0x16)) -
                  (uint)(uVar5 < *(uint *)(unaff_SI + -0x16) || uVar4 < uVar3)] + in_BL;
    uVar2 = in(in_DX);
    *unaff_DI = uVar2;
    FUN_6c1b_10ac();
    return;
  }
  unaff_DI[uVar5] = unaff_DI[uVar5] + in_BL + bVar6;
  uVar2 = in(in_DX);
  *unaff_DI = uVar2;
  FUN_18a9_066e(0x100f2,uVar5);
  if (DAT_4125_0d68 < 2) {
    FUN_18a9_066e(0x200f3,uVar5);
    uVar7 = 2;
  }
  else {
    FUN_18a9_066e(0x100f3,uVar5);
    uVar7 = 1;
  }
  FUN_18a9_066e(0xf4,uVar7,uVar5);
  if (DAT_451b_04a8 < 0) {
    uVar7 = 2;
  }
  else {
    uVar7 = 1;
  }
  FUN_18a9_066e(0xf5,uVar7,uVar5);
  if (DAT_451b_04ac < 0) {
    uVar7 = 2;
  }
  else {
    uVar7 = 1;
  }
  FUN_18a9_066e(0xf6,uVar7);
  if (DAT_4125_0d6c == 0) {
    FUN_18a9_066e(0x100f9);
    FUN_18a9_066e(0xfa);
    if (DAT_451b_04aa < 0) {
      uVar7 = 2;
    }
    else {
      uVar7 = 1;
    }
    FUN_18a9_066e(0xf7,uVar7);
  }
  else {
    FUN_18a9_066e(0xf9);
    FUN_18a9_066e(0x100fa);
    if (DAT_451b_04aa < 0) {
      uVar7 = 2;
    }
    else {
      uVar7 = 1;
    }
    FUN_18a9_066e(0xf8,uVar7);
  }
  FUN_18a9_03b3();
  FUN_18a9_07ed();
  FUN_18a9_07ed();
  return;
}


