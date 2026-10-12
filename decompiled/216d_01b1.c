
undefined2 __cdecl16far FUN_216d_01b1(int *param_1,int param_2,int param_3)

{
  int *piVar1;
  undefined2 uVar2;
  
  if ((((param_2 < *param_1) ||
       (uVar2 = (undefined2)((ulong)param_1 >> 0x10), piVar1 = (int *)param_1,
       *param_1 + piVar1[2] <= param_2)) || (param_3 < piVar1[1])) ||
     (piVar1[1] + piVar1[3] <= param_3)) {
    uVar2 = 0;
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}


