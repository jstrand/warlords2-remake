
void __cdecl16far FUN_251d_0009(void)

{
  uint uVar1;
  int iVar2;
  undefined2 uVar3;
  
  DAT_4125_32d0 = 0x4125;
  DAT_4125_32ce = (char *)s_SWAP_DAT_4125_377e;
  FUN_1d28_000e(0x80012c,0x20000,0xffff0001);
  FUN_24b0_000a();
  FUN_21b5_02d7();
  if (DAT_4125_3384 != 0) {
    uVar3 = (undefined2)((ulong)DAT_4125_34e7 >> 0x10);
    iVar2 = (int)DAT_4125_34e7;
    uVar1 = (uint)(*(int *)(iVar2 + 2) * *(int *)(iVar2 + 6) * -2) >> 4;
    FUN_1ecc_0009(*(int *)(iVar2 + 8) + ((*(int *)(iVar2 + 2) * *(int *)(iVar2 + 6) & 0x7fffU) >> 3)
                  ,uVar1,uVar1);
  }
  return;
}


