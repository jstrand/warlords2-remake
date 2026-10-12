
undefined2 __cdecl16far FUN_4fef_0f72(undefined2 param_1,undefined2 param_2)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  
  iVar1 = FUN_4fef_0c9d(param_1,param_2);
  uVar3 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
  iVar2 = (int)DAT_451b_002a;
  if ((((*(int *)(iVar2 + *(char *)(iVar2 + iVar1 + 0x264) * 4 + 0x4e4) == 0x24) ||
       (*(int *)(iVar2 + *(char *)(iVar2 + iVar1 + 0x264) * 4 + 0x4e4) == 0x20)) ||
      (*(int *)(iVar2 + *(char *)(iVar2 + iVar1 + 0x264) * 4 + 0x4e4) == 0x26)) ||
     (*(int *)(iVar2 + *(char *)(iVar2 + iVar1 + 0x264) * 4 + 0x4e4) == 0x28)) {
    uVar3 = 1;
  }
  else {
    uVar3 = 0;
  }
  return uVar3;
}


