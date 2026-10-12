
void __cdecl16far FUN_6a89_1067(void)

{
  int iVar1;
  undefined2 uVar2;
  
  FUN_18a9_066e(0x101a9);
  FUN_18a9_066e(0x101ab);
  FUN_18a9_066e(0x101aa);
  if (DAT_4125_0d56 < 0) {
    uVar2 = 2;
LAB_6a89_109e:
    FUN_18a9_066e(0x1ac,uVar2);
    uVar2 = 2;
  }
  else {
    if (DAT_4125_0d56 == 0) {
      uVar2 = 2;
    }
    else {
      if (DAT_4125_0d56 == 0x1a) {
        uVar2 = 1;
        goto LAB_6a89_109e;
      }
      uVar2 = 1;
    }
    FUN_18a9_066e(0x1ac,uVar2);
    uVar2 = 1;
  }
  FUN_18a9_066e(0x1ad,uVar2);
  for (iVar1 = 0; iVar1 < 0x1b; iVar1 = iVar1 + 1) {
    FUN_18a9_066e(iVar1 + 0x1ae,1);
  }
  return;
}


