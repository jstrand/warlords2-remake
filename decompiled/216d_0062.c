
int * __cdecl16far FUN_216d_0062(int *param_1,int param_2,int param_3)

{
  int *piVar1;
  
  *param_1 = *param_1 + param_2;
  piVar1 = (int *)param_1 + 1;
  *piVar1 = *piVar1 + param_3;
  return (int *)param_1;
}


