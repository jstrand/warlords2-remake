
int __cdecl16far FUN_1b62_024e(int *param_1)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar2 = 0;
  iVar5 = 0;
  iVar3 = army_count;
  while (iVar4 = iVar3 + -1, iVar3 != 0) {
    iVar3 = iVar4;
    if ((*(int *)(iVar4 * 0x16 + -0x7780) == *param_1) &&
       (*(int *)(iVar4 * 0x16 + -0x777e) == ((int *)param_1)[1])) {
      iVar5 = iVar5 + 1;
      iVar2 = iVar4 * 0x16 + -0x7780;
    }
  }
  if (iVar5 == 0) {
    pbVar1 = (byte *)(((int *)param_1)[1] * 0xe0 + *param_1 * 2 + 1);
    *pbVar1 = *pbVar1 & 0xef;
  }
  return iVar2;
}


