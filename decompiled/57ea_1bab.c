
void __cdecl16far
FUN_57ea_1bab(int param_1,int param_2,int param_3,undefined2 *param_4,int *param_5,int *param_6)

{
  int iVar1;
  int iVar2;
  
  if (DAT_451b_1e9c == 2) {
    iVar1 = 2;
  }
  else {
    iVar1 = map_distance(*(int *)(param_1 * 0x16 + -0x7780),*(int *)(param_1 * 0x16 + -0x777e),
                         param_2,param_3);
  }
  if ((iVar1 < 4) &&
     (((*(uint *)(param_1 * 0x16 + -0x7774) & 0x1000) == 0 ||
      (iVar1 = FUN_1555_109d(param_2,param_3), iVar1 != 0)))) {
    for (iVar1 = param_2 + -1; iVar1 <= param_2 + 1; iVar1 = iVar1 + 1) {
      for (iVar2 = param_3 + -1; iVar2 <= param_3 + 1; iVar2 = iVar2 + 1) {
        if ((((-1 < iVar1) && (-1 < iVar2)) && (iVar1 < 0x70)) &&
           ((iVar2 < 0x9c && ((*(byte *)(iVar2 * 0x70 + iVar1) & 0x20) != 0)))) {
          *param_4 = 1;
          *param_5 = param_2;
          *param_6 = param_3;
        }
      }
    }
  }
  return;
}


