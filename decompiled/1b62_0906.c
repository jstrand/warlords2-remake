
int __cdecl16far FUN_1b62_0906(int *param_1)

{
  int iVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  if ((*param_1 == 0) && (((int *)param_1)[1] == 0)) {
    iVar1 = 0xff;
  }
  else {
    iVar1 = (int)*(char *)((int)(int *)param_1 + 5);
  }
  return iVar1;
}


