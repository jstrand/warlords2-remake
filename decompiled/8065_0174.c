
void __cdecl16far FUN_8065_0174(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  
  FUN_18a9_066e(0xad);
  iVar1 = FUN_8c07_09e7();
  if (iVar1 == 0) {
    uVar2 = 2;
  }
  else {
    uVar2 = 1;
  }
  FUN_18a9_066e(0xae,uVar2);
  iVar1 = FUN_8c07_09e7();
  if ((iVar1 == 0) || (DAT_451b_1f02 == 0)) {
    uVar2 = 2;
  }
  else {
    uVar2 = 1;
  }
  FUN_18a9_066e(0xaf,uVar2);
  if (DAT_451b_1f02 == 0) {
    uVar2 = 2;
  }
  else {
    uVar2 = 1;
  }
  FUN_18a9_066e(0xb0,uVar2);
  if (DAT_451b_1f02 == 0) {
    uVar2 = 2;
  }
  else {
    uVar2 = 1;
  }
  FUN_18a9_066e(0xba,uVar2);
  FUN_18a9_066e(0x100bc);
  if (DAT_451b_1f02 == 0) {
LAB_8065_0234:
    uVar2 = 2;
  }
  else {
    uVar2 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
    if ((*(int *)((int)DAT_451b_1f02 + 0x12) < 0) || (*(int *)((int)DAT_451b_1f02 + 0x14) < 0))
    goto LAB_8065_0234;
    uVar2 = 1;
  }
  FUN_18a9_066e(0xbb,uVar2);
  if (DAT_451b_1f02 == 0) {
    FUN_18a9_066e(0x200b1);
    FUN_18a9_066e(0x200b2);
    FUN_18a9_066e(0x200f0);
    uVar2 = 2;
  }
  else {
    FUN_18a9_066e(0x100b1);
    FUN_18a9_066e(0x100b2);
    iVar1 = FUN_89e0_0567();
    if (iVar1 == 0) {
      FUN_18a9_066e(0x100f0);
      FUN_18a9_03b3();
      goto LAB_8065_02cb;
    }
    FUN_18a9_03b3();
    uVar2 = 1;
  }
  FUN_18a9_066e(0xf1,uVar2);
LAB_8065_02cb:
  for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
    FUN_18a9_066e(iVar1 + 0x140,1);
  }
  for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
    if (iVar1 < DAT_4125_30f6) {
      FUN_18a9_066e(iVar1 + 0xe0,1);
      FUN_18a9_066e();
    }
    else {
      FUN_18a9_03b3();
      FUN_18a9_03b3();
    }
  }
  if (opt_diplomacy == 0) {
    FUN_18a9_066e(0x200b7);
    FUN_18a9_03b3();
    FUN_18a9_03b3();
  }
  else {
    if ((*(byte *)(current_player * 9 + 0x153b) & 0x10) == 0) {
      FUN_18a9_03b3();
      FUN_18a9_03b3();
      uVar3 = 0x100b7;
    }
    else if ((*(byte *)(current_player * 9 + 0x153b) & 0x20) == 0) {
      FUN_18a9_03b3();
      FUN_18a9_03b3();
      uVar3 = 0x100b8;
    }
    else {
      FUN_18a9_03b3();
      FUN_18a9_03b3();
      uVar3 = 0x100b9;
    }
    FUN_18a9_05f0(uVar3);
  }
  FUN_8065_0519();
  FUN_545c_00aa();
  return;
}


