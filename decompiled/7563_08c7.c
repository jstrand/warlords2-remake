
void __cdecl16far FUN_7563_08c7(int *param_1,int param_2)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  
  if (-1 < param_2) {
    *(undefined1 *)(param_2 * 0x1d + 0xcff) = 3;
    iVar2 = army_count;
    do {
      iVar3 = iVar2 + -1;
      if (iVar2 == 0) goto LAB_7563_0921;
      iVar2 = iVar3;
    } while ((param_1._2_2_ != 0x2e1d) || ((int *)(iVar3 * 0x16 + -0x7780) != (int *)param_1));
    *(int *)(param_2 * 0x1d + 0xd00) = iVar3;
LAB_7563_0921:
    if (param_2 < 8) {
      pbVar1 = (byte *)(((int *)param_1)[1] * 0x70 + *param_1);
      *pbVar1 = *pbVar1 & 0xbf;
    }
  }
  return;
}


