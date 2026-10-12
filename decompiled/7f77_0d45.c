
void __cdecl16far FUN_7f77_0d45(void)

{
  undefined2 uVar1;
  undefined2 in_DX;
  int iVar2;
  undefined1 local_a [8];
  
  FUN_22bf_07a5();
  if (*DAT_451b_1d82 != DAT_4125_2a4b) {
    FUN_24d0_0002();
    uVar1 = FUN_216d_002e(local_a);
    FUN_216d_0288(uVar1,in_DX);
  }
  DAT_4125_2a4b = *DAT_451b_1d82;
  for (iVar2 = 0; iVar2 < 7; iVar2 = iVar2 + 1) {
    if (-1 < ((int *)DAT_451b_1d82)[iVar2]) {
      FUN_78a8_06ae(2);
      FUN_7ecb_00bf(0x70,iVar2 * 0x14 + 0x99,
                    ((undefined4 *)&DAT_451b_1d88)[((int *)DAT_451b_1d82)[iVar2]]);
    }
  }
  FUN_22bf_0757();
  return;
}


