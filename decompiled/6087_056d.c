
int __cdecl16far FUN_6087_056d(int param_1,int param_2,int *param_3)

{
  int iVar1;
  
  if (((*(char *)(*(byte *)(param_2 * 0xe0 + param_1 * 2) + 0x710) != '\v') ||
      (iVar1 = FUN_828e_04b5(param_1,param_2), iVar1 < 0)) ||
     ((*param_3 = (int)*(char *)(iVar1 * 0x1f + 0x829), *param_3 != 1 &&
      ((*(byte *)(param_2 * 0xe0 + param_1 * 2 + 1) & 0x40) != 0)))) {
    iVar1 = -1;
  }
  return iVar1;
}


