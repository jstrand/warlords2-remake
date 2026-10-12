
int * __cdecl16far FUN_6d51_119c(int param_1)

{
  int iVar1;
  int *local_a;
  
  local_a = DAT_451b_04d4;
  for (iVar1 = 1; iVar1 < param_1; iVar1 = iVar1 + 1) {
    local_a = (int *)CONCAT22(local_a._2_2_,(int *)local_a + *local_a / 2);
  }
  return (int *)local_a + 1;
}


