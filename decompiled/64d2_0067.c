
/* WARNING: Removing unreachable block (ram,0x00064e14) */

void __cdecl16far FUN_64d2_0067(void)

{
  int iVar1;
  undefined2 uVar2;
  
  FUN_18a9_066e(0x100fb);
  if (DAT_4125_37a3 == '\0') {
    uVar2 = 2;
  }
  else {
    uVar2 = 1;
  }
  FUN_18a9_066e(0x114,uVar2);
  if (DAT_4125_37a2 == '\0') {
    uVar2 = 2;
  }
  else {
    uVar2 = 1;
  }
  FUN_18a9_066e(0x115,uVar2);
  FUN_18a9_066e(0x10116);
  for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
    if (*(int *)(iVar1 * 2 + 0x137) == 0) {
      FUN_18a9_03b3();
      FUN_18a9_03b3();
      FUN_18a9_03b3();
    }
    else {
      FUN_18a9_066e(iVar1 + 0xfc,1);
      FUN_18a9_066e(iVar1 + 0x104);
      FUN_18a9_066e(iVar1 + 0x10c,1);
    }
  }
  return;
}


