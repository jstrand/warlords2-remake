
int __cdecl16far FUN_1555_109d(int param_1,int param_2)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = 0;
  iVar3 = 0;
  if (*(char *)(*(byte *)(param_2 * 0xe0 + param_1 * 2 + -2) + 0x710) == '\n') {
    param_1 = param_1 + -1;
  }
  if (*(char *)(*(byte *)(param_2 * 0xe0 + param_1 * 2 + -0xe0) + 0x710) == '\n') {
    param_2 = param_2 + -1;
  }
  while ((*(int *)(iVar3 * 2 + 0x1f8) != -1 && (iVar4 == 0))) {
    iVar2 = FUN_1a8b_0b4d(&param_1);
    if (iVar2 == 0) {
      return 0;
    }
    cVar1 = *(char *)(*(byte *)(param_2 * 0xe0 + param_1 * 2) + 0x710);
    if (((cVar1 == '\x01') || (cVar1 == '\x02')) || (cVar1 == '\x03')) {
      iVar4 = 1;
    }
    iVar3 = iVar3 + 1;
  }
  return iVar4;
}


