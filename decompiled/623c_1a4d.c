
undefined2 __cdecl16far FUN_623c_1a4d(int param_1,int *param_2)

{
  int iVar1;
  undefined2 uVar2;
  
  iVar1 = best_production_for(param_1,3,-1,1);
  if (iVar1 != -1) {
    iVar1 = FUN_7087_0000(param_1,iVar1);
    *param_2 = iVar1;
    iVar1 = (int)*(char *)(param_1 * 0x41 + *param_2 + 0x159b);
    if (((undefined1 *)&DAT_451b_0669)[*(char *)(param_1 * 0x41 + *param_2 + 0x1593) * 6] == '\x01')
    {
      iVar1 = iVar1 + 2;
    }
    uVar2 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    if (((*(int *)((int)DAT_451b_018e + 2) < 0xc) || (2 < iVar1)) &&
       ((*(int *)((int)DAT_451b_018e + 2) < 8 || (1 < iVar1)))) {
      return 1;
    }
  }
  return 0;
}


