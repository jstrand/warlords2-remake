
void __cdecl16far FUN_21b5_010a(void)

{
  uint uVar1;
  uint uVar2;
  undefined2 uVar3;
  int iVar4;
  
  uVar1 = FUN_22bf_0944();
  uVar2 = (uint)((uVar1 & 1) != 0);
  uVar1 = (uint)((uVar1 & 2) != 0);
  if (uVar2 != DAT_4125_353e) {
    if (uVar2 == 0) {
      uVar3 = 2;
    }
    else {
      uVar3 = 1;
    }
    FUN_21b5_01a9(uVar3,0);
  }
  if (uVar1 != DAT_4125_3540) {
    if (uVar1 == 0) {
      uVar3 = 4;
    }
    else {
      uVar3 = 3;
    }
    FUN_21b5_01a9(uVar3,0);
  }
  DAT_4125_353e = uVar2;
  DAT_4125_3540 = uVar1;
  iVar4 = kbhit();
  if (iVar4 != 0) {
    uVar1 = getch();
    if (uVar1 == 0) {
      uVar1 = getch();
      uVar1 = uVar1 | 0x100;
    }
    FUN_21b5_01a9(7,(long)(int)uVar1);
  }
  return;
}


