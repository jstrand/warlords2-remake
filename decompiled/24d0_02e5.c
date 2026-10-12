
void __cdecl16far FUN_24d0_02e5(int *param_1,int param_2,int param_3)

{
  int *piVar1;
  undefined2 uVar2;
  
  FUN_22bf_07a5();
  piVar1 = (int *)param_1;
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  if (param_3 < param_2) {
    FUN_24d0_0002(param_3);
    FUN_2133_0344(*param_1 + piVar1[2] + -1,piVar1[1],piVar1[3]);
    FUN_2133_02fe(*param_1,piVar1[1] + piVar1[3] + -1,piVar1[2]);
    FUN_24d0_0002(param_2);
    FUN_2133_02fe(*param_1,piVar1[1],piVar1[2]);
    FUN_2133_0344(*param_1,piVar1[1],piVar1[3]);
  }
  else {
    FUN_24d0_0002(param_2);
    FUN_2133_02fe(*param_1,piVar1[1],piVar1[2]);
    FUN_2133_0344(*param_1,piVar1[1],piVar1[3]);
    FUN_24d0_0002(param_3);
    FUN_2133_0344(*param_1 + piVar1[2] + -1,piVar1[1],piVar1[3]);
    FUN_2133_02fe(*param_1,piVar1[1] + piVar1[3] + -1,piVar1[2]);
  }
  FUN_22bf_0757();
  return;
}


