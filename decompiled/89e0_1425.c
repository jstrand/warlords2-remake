
void __cdecl16far FUN_89e0_1425(void)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  
  FUN_18a9_066e(0x1014c);
  FUN_18a9_066e(0x1014d);
  if (DAT_4125_30f6 < 2) {
    FUN_18a9_066e(0x2014f);
    uVar3 = 2;
  }
  else {
    FUN_18a9_066e(0x1014f);
    uVar3 = 1;
  }
  FUN_18a9_066e(0x14e,uVar3);
  for (iVar1 = 0; iVar2 = DAT_4125_30f6, iVar1 < DAT_4125_30f6; iVar1 = iVar1 + 1) {
    FUN_18a9_066e(iVar1 + 0x150,1);
    FUN_18a9_066e();
  }
  for (; iVar2 < 8; iVar2 = iVar2 + 1) {
    FUN_18a9_066e(iVar2 + 0x150,2);
    FUN_18a9_066e();
  }
  return;
}


