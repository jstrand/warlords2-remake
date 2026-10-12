
undefined2 __cdecl16far FUN_5311_0a7f(int param_1,int param_2)

{
  int iVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
  iVar1 = (int)DAT_451b_002a;
  if (((*(char *)(iVar1 + param_2 * 0x70 + param_1 + 0x6120) == '\a') ||
      (*(char *)(iVar1 + param_2 * 0x70 + param_1 + 0x6120) == '\x04')) ||
     (*(char *)(iVar1 + param_2 * 0x70 + param_1 + 0x6120) == '\x05')) {
    uVar2 = 1;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}


