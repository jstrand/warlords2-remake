
undefined2 __cdecl16far FUN_8c07_03a6(void)

{
  uint *puVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  
  if (DAT_451b_1f02 == 0) {
    uVar2 = 0;
  }
  else {
    FUN_177b_003d();
    if (DAT_4125_2bda == 0) {
      uVar2 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
      iVar4 = (int)DAT_451b_1f02;
      puVar1 = (uint *)(iVar4 + 0xc);
      *puVar1 = *puVar1 & 0xfffe;
      *(undefined1 *)(iVar4 + 0x11) = 1;
    }
    else {
      iVar4 = 8;
      while (iVar3 = iVar4 + -1, iVar4 != 0) {
        iVar4 = iVar3;
        if (((undefined2 *)&DAT_451b_1ede)[iVar3 * 2] != 0 ||
            *(int *)((undefined *)&DAT_451b_1ee0 + iVar3 * 4) != 0) {
          uVar2 = (undefined2)
                  ((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar3 * 2) >> 0x10);
          puVar1 = (uint *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar3 * 2) + 0xc);
          *puVar1 = *puVar1 & 0xfffe;
          *(undefined1 *)(((undefined2 *)&DAT_451b_1ede)[iVar3 * 2] + 0x11) = 1;
        }
      }
    }
    uVar2 = auto_tutorial_hook_5();
  }
  return uVar2;
}


