
int * __cdecl16far FUN_216d_0082(int *param_1,int param_2,int param_3)

{
  int *piVar1;
  undefined2 uVar2;
  
  *param_1 = *param_1 + param_2;
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  piVar1 = (int *)param_1;
  piVar1[1] = piVar1[1] + param_3;
  piVar1[2] = piVar1[2] + param_2 * -2;
  piVar1[3] = piVar1[3] + param_3 * -2;
  return piVar1;
}


