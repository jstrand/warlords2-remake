
void __cdecl16far FUN_7bab_0416(void)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined2 uVar4;
  
  iVar2 = 0;
  for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
    FUN_18a9_066e(iVar1 + 0x7d,1);
    FUN_18a9_066e(iVar1 + 0x85);
    FUN_18a9_066e();
    if (((*(int *)(iVar1 * 2 + 0x137) != 0) && (*(int *)(iVar1 * 2 + 0xd0) == 1)) &&
       (*(int *)(iVar1 * 2 + 0xc0) != 3)) {
      iVar2 = iVar2 + 1;
    }
  }
  iVar1 = FUN_7bab_1241();
  FUN_18a9_066e(0x91,iVar1 == 0);
  iVar1 = FUN_7bab_1241();
  FUN_18a9_066e(0x92,iVar1 == 0);
  iVar1 = FUN_7bab_1241();
  FUN_18a9_066e(0x93,iVar1 == 0);
  FUN_18a9_066e(0x10094);
  if (opt_all_computer == 0) {
    FUN_18a9_03b3();
    uVar3 = 0x1008f;
  }
  else {
    FUN_18a9_03b3();
    uVar3 = 0x10090;
  }
  FUN_18a9_066e(uVar3);
  FUN_18a9_066e(0x1008d);
  FUN_18a9_066e(0x1008e);
  FUN_18a9_066e(0x1009d);
  if (iVar2 == 0) {
    uVar4 = 2;
  }
  else {
    uVar4 = 1;
  }
  FUN_18a9_066e(0x9e,uVar4);
  FUN_2372_0eab(0);
  FUN_2372_0ef8(0x1f2);
  FUN_2372_0ef8(500);
  FUN_2372_0ef8(0x1f5);
  iVar1 = FUN_7721_0e25();
  FUN_2372_0ef8(0x1f6,iVar1 != 0);
  iVar1 = FUN_7721_0e42();
  FUN_2372_0ef8(0x1f8,iVar1 != 0);
  FUN_2372_0ef8(0x1f7,DAT_3c04_0120 != 0);
  FUN_2372_0ef8(0x101f9);
  FUN_2372_0eab(2);
  FUN_2372_0eab(3);
  FUN_2372_0eab(4);
  FUN_2372_0eab(5);
  FUN_2372_0eab(7);
  return;
}


