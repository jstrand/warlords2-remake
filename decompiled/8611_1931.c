
void __cdecl16far FUN_8611_1931(void)

{
  int iVar1;
  int iVar2;
  
  DAT_4125_2f1a = 1;
  for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
    for (iVar1 = 0; iVar1 < 9; iVar1 = iVar1 + 1) {
      ((undefined2 *)&DAT_451b_2287)[iVar1 * 4 + iVar2 * 0x24] =
           ((undefined2 *)&DAT_451b_2287)[iVar1 * 4 + iVar2 * 0x24] & 0x1f;
      ((undefined2 *)&DAT_451b_1fe1)[iVar1 * 4 + iVar2 * 0x24] =
           ((undefined2 *)&DAT_451b_1fe1)[iVar1 * 4 + iVar2 * 0x24] & 0x1f;
    }
  }
  return;
}


