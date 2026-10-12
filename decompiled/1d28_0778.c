
int __cdecl16far FUN_1d28_0778(undefined4 param_1)

{
  int iVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  if ((*(byte *)((int)param_1 + 4) & 8) == 0) {
    iVar1 = (*(int *)&DAT_4125_0002 + -1) * 0x10;
  }
  else {
    iVar1 = 0;
  }
  return iVar1;
}


