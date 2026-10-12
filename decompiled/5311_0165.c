
void __cdecl16far FUN_5311_0165(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 *puVar4;
  undefined2 uVar5;
  
  iVar1 = 0x32;
  while (iVar3 = (int)param_1, uVar5 = (undefined2)((ulong)param_1 >> 0x10), iVar1 != 0) {
    puVar4 = (undefined2 *)(iVar3 + (iVar1 + -1) * 2);
    puVar4[0xfa] = 0xffff;
    puVar4[200] = 0xffff;
    puVar4[100] = 0xffff;
    puVar4[0x32] = 0xffff;
    *puVar4 = 0xffff;
    puVar4[0x96] = 0;
    iVar1 = iVar1 + -1;
  }
  FUN_5311_05b4(param_1);
  iVar1 = 0x32;
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    iVar1 = iVar2;
    if (*(int *)(iVar3 + iVar2 * 2 + 300) == 0) {
      puVar4 = (undefined2 *)(iVar3 + iVar2 * 2);
      puVar4[0xfa] = 0xffff;
      puVar4[200] = 0xffff;
      puVar4[100] = 0xffff;
      puVar4[0x32] = 0xffff;
      *puVar4 = 0xffff;
    }
  }
  FUN_5311_01f6(param_1);
  return;
}


