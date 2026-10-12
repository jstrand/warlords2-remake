
void __cdecl16far
FUN_57ea_1c9d(int param_1,int param_2,int param_3,int *param_4,int *param_5,int *param_6)

{
  int iVar1;
  int iVar2;
  
  if ((*param_4 == 0) && ((*(uint *)(param_1 * 0x16 + -0x7774) & 0x1000) == 0)) {
    for (iVar2 = param_2 + -1; iVar2 <= param_2 + 1; iVar2 = iVar2 + 1) {
      for (iVar1 = param_3 + -1; iVar1 <= param_3 + 1; iVar1 = iVar1 + 1) {
        if ((((-1 < iVar2) && (-1 < iVar1)) && (iVar2 < 0x70)) &&
           ((iVar1 < 0x9c && ((*(byte *)(iVar1 * 0x70 + iVar2) & 0x20) != 0)))) {
          *param_4 = 2;
          *param_5 = param_2;
          *param_6 = param_3;
        }
      }
    }
  }
  return;
}


