
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_623c_14d3(int *param_1)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  undefined2 uVar4;
  
  DAT_4125_2bda = 0;
  DAT_451b_1e9e = 0;
  DAT_451b_1ea0 = 100;
  DAT_451b_1f02 = 0;
  for (iVar2 = 0; piVar3 = (int *)param_1, uVar4 = (undefined2)((ulong)param_1 >> 0x10), iVar2 < 8;
      iVar2 = iVar2 + 1) {
    *(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar2 * 4) = 0;
    ((undefined2 *)&DAT_451b_1ede)[iVar2 * 2] = 0;
    if (piVar3[iVar2] != -1) {
      if (*(char *)(piVar3[iVar2] * 0x16 + -0x7779) < DAT_451b_1ea0) {
        DAT_451b_1ea0 = (int)*(char *)(piVar3[iVar2] * 0x16 + -0x7779);
      }
      DAT_451b_1e9e = DAT_451b_1e9e + 1;
    }
  }
  if (DAT_451b_1e9e != 0) {
    if (DAT_451b_1e9e == 1) {
      _DAT_451b_1ede = CONCAT22(0x2e1d,*param_1 * 0x16 + -0x7780);
    }
    else {
      DAT_4125_2bda = 1;
      for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
        if (piVar3[iVar2] != -1) {
          iVar1 = piVar3[iVar2];
          *(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar2 * 4) = 0x2e1d;
          ((undefined2 *)&DAT_451b_1ede)[iVar2 * 2] = iVar1 * 0x16 + -0x7780;
        }
      }
    }
    DAT_451b_1f02 = _DAT_451b_1ede;
  }
  return;
}


