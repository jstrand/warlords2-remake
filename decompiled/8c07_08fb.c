
undefined2 __cdecl16far FUN_8c07_08fb(void)

{
  uint *puVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  
  if (DAT_451b_1f02 == 0) {
    uVar2 = 0;
  }
  else {
    if (DAT_4125_2bda == 0) {
      puVar1 = (uint *)((int)DAT_451b_1f02 + 0xc);
      *puVar1 = *puVar1 | 0x40;
    }
    else {
      iVar3 = 8;
      while (iVar4 = iVar3 + -1, iVar3 != 0) {
        iVar3 = iVar4;
        if (((undefined2 *)&DAT_451b_1ede)[iVar4 * 2] != 0 ||
            *(int *)((undefined *)&DAT_451b_1ee0 + iVar4 * 4) != 0) {
          puVar1 = (uint *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar4 * 2) + 0xc);
          *puVar1 = *puVar1 | 0x40;
        }
      }
      DAT_4125_2ea6 = 0;
      FUN_8611_006c();
    }
    uVar2 = 1;
  }
  return uVar2;
}


