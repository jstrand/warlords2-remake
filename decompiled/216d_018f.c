
undefined2 __cdecl16far FUN_216d_018f(undefined4 param_1)

{
  undefined2 uVar1;
  
  uVar1 = (undefined2)((ulong)param_1 >> 0x10);
  if ((*(int *)((int)param_1 + 4) < 1) || (*(int *)((int)param_1 + 6) < 1)) {
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}


