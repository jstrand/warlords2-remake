
void __cdecl16far FUN_216d_01fd(int *param_1)

{
  int *piVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  piVar1 = (int *)param_1;
  FUN_2133_02fe(*param_1,piVar1[1],piVar1[2]);
  FUN_2133_02fe(*param_1,piVar1[1] + piVar1[3] + -1,piVar1[2]);
  FUN_2133_0344(*param_1,piVar1[1],piVar1[3]);
  FUN_2133_0344(*param_1 + piVar1[2] + -1,piVar1[1],piVar1[3]);
  return;
}


