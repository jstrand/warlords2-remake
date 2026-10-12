
void __cdecl16far FUN_7bab_1ae4(void)

{
  undefined2 uVar1;
  int iVar2;
  
  for (iVar2 = 0; iVar2 < 5; iVar2 = iVar2 + 1) {
    if ((int)((undefined2 *)&DAT_451b_19fa)[iVar2] < 0) {
      uVar1 = 2;
    }
    else {
      uVar1 = 1;
    }
    FUN_18a9_066e(iVar2 + 0x1d0,uVar1);
  }
  FUN_18a9_066e(0x101c9);
  FUN_18a9_066e(0x101ca);
  FUN_18a9_066e(0x101cb);
  if (*(int *)(DAT_4125_2563 * 2 + 0xd0) == 0) {
    FUN_18a9_066e(0x201cc);
    FUN_18a9_066e(0x201ce);
  }
  else {
    if (DAT_451b_19fa < 1) {
      FUN_18a9_066e(0x201cc);
      uVar1 = 2;
    }
    else {
      FUN_18a9_066e(0x101cc);
      uVar1 = 1;
    }
    FUN_18a9_066e(0x1ce,uVar1);
    if ((DAT_451b_1a02 < DAT_451b_08ce + -1) && (0 < DAT_451b_1a02)) {
      FUN_18a9_066e(0x101cd);
      uVar1 = 1;
      goto LAB_7bab_1b7d;
    }
  }
  FUN_18a9_066e(0x201cd);
  uVar1 = 2;
LAB_7bab_1b7d:
  FUN_18a9_066e(0x1cf,uVar1);
  return;
}


