
undefined2 __cdecl16far FUN_22bf_08b1(int *param_1)

{
  int *piVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  piVar1 = (int *)param_1;
  if ((((DAT_4125_356a < piVar1[1]) || (piVar1[1] + piVar1[3] <= DAT_4125_356a)) ||
      (DAT_4125_3568 < *param_1)) || (*param_1 + piVar1[2] <= DAT_4125_3568)) {
    uVar2 = 0;
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}


