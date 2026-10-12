
undefined2 __cdecl16far FUN_1d28_0a38(undefined4 param_1,undefined2 param_2)

{
  undefined2 uVar1;
  int iVar2;
  
  uVar1 = DAT_4125_3274;
  iVar2 = (int)((ulong)param_1 >> 0x10);
  if (iVar2 == 0 && (int)param_1 == 0) {
    uVar1 = FUN_1d28_01d5(param_2);
  }
  else {
    *(int *)0x0 = (int)param_1;
    *(int *)&DAT_4125_0002 = iVar2;
    FUN_1d28_08a3(0,uVar1,param_2,0);
    uVar1 = *(undefined2 *)0x0;
  }
  return uVar1;
}


