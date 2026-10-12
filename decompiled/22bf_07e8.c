
void __cdecl16far FUN_22bf_07e8(int *param_1)

{
  int iVar1;
  int *piVar2;
  undefined2 uVar3;
  
  if (*param_1 < DAT_4125_3670) {
    iVar1 = 0;
  }
  else {
    iVar1 = *param_1 - DAT_4125_3670;
  }
  *param_1 = iVar1;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  piVar2 = (int *)param_1;
  if (piVar2[1] < DAT_4125_3674) {
    iVar1 = 0;
  }
  else {
    iVar1 = piVar2[1] - DAT_4125_3674;
  }
  piVar2[1] = iVar1;
  piVar2[2] = piVar2[2] + DAT_4125_3670;
  piVar2[3] = piVar2[3] + DAT_4125_3674;
  DAT_4125_3576 = DAT_4125_3576 + 1;
  iVar1 = FUN_22bf_08b1(piVar2,uVar3);
  if (iVar1 != 0) {
    DAT_4125_357a = 1;
    FUN_22bf_07a5();
  }
  DAT_4125_3578 = 1;
  return;
}


