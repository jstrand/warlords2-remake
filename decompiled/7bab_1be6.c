
void __cdecl16far FUN_7bab_1be6(int param_1)

{
  undefined2 uVar1;
  undefined2 in_DX;
  int iVar2;
  undefined4 uVar3;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  if (param_1 != 0) {
    uVar3 = 0x20004;
    uVar1 = FUN_216d_002e(&local_a);
    FUN_24d0_02e5(uVar1,in_DX,uVar3);
    local_a = local_a + 1;
    local_8 = local_8 + 1;
    local_6 = local_6 + -2;
    local_4 = local_4 + -2;
    FUN_24d0_0002();
    FUN_216d_0288(&local_a);
  }
  if (*(int *)(DAT_4125_2563 * 2 + 0xd0) != 0) {
    for (iVar2 = 0; (iVar2 < 5 && (-1 < (int)((undefined2 *)&DAT_451b_19fa)[iVar2]));
        iVar2 = iVar2 + 1) {
      if ((((undefined2 *)&DAT_451b_19fa)[iVar2] == DAT_451b_19f8) ||
         (((*(int *)(DAT_4125_2563 * 2 + 0xc0) == 0 || (*(int *)(DAT_4125_2563 * 2 + 0xc0) == 1)) ||
          (*(int *)(DAT_4125_2563 * 2 + 0xc0) == 2)))) {
        FUN_78a8_06ae(2);
      }
      FUN_7ecb_00bf(0xc4,iVar2 * 0x13 + 0xfc,
                    (undefined1 *)&DAT_451b_08dc + ((undefined2 *)&DAT_451b_19fa)[iVar2] * 0x20,
                    0x4125);
    }
  }
  return;
}


