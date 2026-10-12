
void __cdecl16far FUN_1c8c_0963(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  undefined2 uVar2;
  
  for (DAT_4125_2ea6 = 0;
      (DAT_4125_2ea6 < 200 &&
      (uVar2 = (undefined2)((ulong)param_3 >> 0x10), iVar1 = (int)param_3,
      *(char *)(iVar1 + DAT_4125_2ea6) != -1)); DAT_4125_2ea6 = DAT_4125_2ea6 + 1) {
    param_1 = param_1 + *(int *)((int)(undefined2 *)&DAT_4125_2b4b +
                                (uint)*(byte *)(iVar1 + DAT_4125_2ea6) * 2 + 1);
    param_2 = param_2 + ((undefined2 *)&DAT_4125_2b5c)[*(byte *)(iVar1 + DAT_4125_2ea6)];
    ((undefined2 *)&DAT_451b_2515)[DAT_4125_2ea6 * 2] = param_1;
    ((undefined2 *)&DAT_451b_2517)[DAT_4125_2ea6 * 2] = param_2;
  }
  DAT_4125_2ea8 = 0;
  DAT_451b_2511 = param_1;
  DAT_451b_2513 = param_2;
  return;
}


