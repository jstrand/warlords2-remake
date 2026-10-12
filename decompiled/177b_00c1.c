
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far
FUN_177b_00c1(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4)

{
  int iVar1;
  
  if (_LAB_4125_1257_1 < 0x16) {
    *(undefined2 *)(_LAB_4125_1257_1 * 2 + 0x459a) = param_1;
    *(undefined2 *)(_LAB_4125_1257_1 * 2 + 0x4570) = param_2;
    *(undefined2 *)(_LAB_4125_1257_1 * 2 + 0x4546) = param_3;
    iVar1 = dice(1,3,-1);
    *(int *)(_LAB_4125_1257_1 * 2 + 0x451c) = iVar1;
    *(undefined2 *)(_LAB_4125_1257_1 * 2 + 0x44f2) = param_4;
    _LAB_4125_1257_1 = _LAB_4125_1257_1 + 1;
  }
  return;
}


