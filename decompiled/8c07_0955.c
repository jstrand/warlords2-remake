
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far FUN_8c07_0955(int param_1)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  
  DAT_4125_2bda = param_1;
  if (_DAT_451b_1ee2 == 0) {
    DAT_4125_2bda = 0;
  }
  _DAT_451b_1efe = 0;
  if (param_1 == 0) {
    uVar4 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
    iVar3 = (int)DAT_451b_1f02;
    puVar1 = (uint *)(iVar3 + 0xc);
    *puVar1 = *puVar1 | 1;
    if (*(char *)(iVar3 + 4) == '\x1c') {
      _DAT_451b_1efe = DAT_451b_1f02;
    }
  }
  else {
    iVar3 = 8;
    while (iVar2 = iVar3 + -1, iVar3 != 0) {
      iVar3 = iVar2;
      if ((((undefined2 *)&DAT_451b_1ede)[iVar2 * 2] != 0 ||
           *(int *)((undefined *)&DAT_451b_1ee0 + iVar2 * 4) != 0) &&
         (uVar4 = (undefined2)
                  ((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar2 * 2) >> 0x10),
         puVar1 = (uint *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar2 * 2) + 0xc),
         *puVar1 = *puVar1 | 1, *(char *)(((undefined2 *)&DAT_451b_1ede)[iVar2 * 2] + 4) == '\x1c'))
      {
        _DAT_451b_1efe =
             CONCAT22(*(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar2 * 4),
                      ((undefined2 *)&DAT_451b_1ede)[iVar2 * 2]);
      }
    }
  }
  return 1;
}


