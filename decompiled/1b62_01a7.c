
int __cdecl16far FUN_1b62_01a7(int param_1,int param_2)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 local_8;
  
  iVar2 = 0;
  local_8 = 0;
  iVar3 = army_count;
  while (iVar4 = iVar3 + -1, iVar3 != 0) {
    iVar3 = iVar4;
    if ((*(int *)(iVar4 * 0x16 + -0x7780) == param_1) &&
       (*(int *)(iVar4 * 0x16 + -0x777e) == param_2)) {
      local_8 = local_8 + 1;
      iVar2 = iVar4 * 0x16 + -0x7780;
    }
  }
  if (local_8 == 0) {
    pbVar1 = (byte *)(param_2 * 0xe0 + param_1 * 2 + 1);
    *pbVar1 = *pbVar1 & 0xef;
    pbVar1 = (byte *)(param_2 * 0xe0 + param_1 * 2 + 1);
    *pbVar1 = *pbVar1 & 0xdf;
  }
  return iVar2;
}


