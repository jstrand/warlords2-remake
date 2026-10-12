
undefined2 __cdecl16far FUN_216d_0143(int *param_1,int *param_2)

{
  int *piVar1;
  int *piVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  
  if ((((*param_1 == *param_2) &&
       (uVar3 = (undefined2)((ulong)param_1 >> 0x10), piVar1 = (int *)param_1,
       uVar4 = (undefined2)((ulong)param_2 >> 0x10), piVar2 = (int *)param_2, piVar1[1] == piVar2[1]
       )) && (piVar1[2] == piVar2[2])) && (piVar1[3] == piVar2[3])) {
    uVar3 = 1;
  }
  else {
    uVar3 = 0;
  }
  return uVar3;
}


