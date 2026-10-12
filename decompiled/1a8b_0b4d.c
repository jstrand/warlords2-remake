
undefined2 __cdecl16far FUN_1a8b_0b4d(int *param_1,int *param_2,int param_3)

{
  int iVar1;
  int iVar2;
  
  iVar2 = *param_1 + *(int *)(param_3 * 2 + 0x2b16);
  iVar1 = *param_2 + *(int *)(param_3 * 2 + 0x2b04);
  if ((((-1 < iVar2) && (-1 < iVar1)) && (iVar2 != 0x70)) && (iVar1 != 0x9c)) {
    *param_1 = iVar2;
    *param_2 = iVar1;
    return 1;
  }
  return 0;
}


