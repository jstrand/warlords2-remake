
void __cdecl16far FUN_8065_0fe9(void)

{
  int iVar1;
  undefined2 uVar2;
  
  if (DAT_451b_1f02 != 0) {
    uVar2 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
    if ((-1 < *(int *)((int)DAT_451b_1f02 + 0x12)) && (-1 < *(int *)((int)DAT_451b_1f02 + 0x14))) {
      for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
        if (((undefined2 *)&DAT_451b_1ede)[iVar1 * 2] != 0 ||
            *(int *)((undefined *)&DAT_451b_1ee0 + iVar1 * 4) != 0) {
          uVar2 = (undefined2)
                  ((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar1 * 2) >> 0x10);
          *(undefined2 *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar1 * 2) + 0x12) =
               0xffff;
          *(undefined2 *)(((undefined2 *)&DAT_451b_1ede)[iVar1 * 2] + 0x14) = 0xffff;
        }
      }
      DAT_4125_2ea6 = 0;
      FUN_8611_006c();
    }
  }
  FUN_8065_0174();
  return;
}


