
int __cdecl16far FUN_59bf_0a85(int param_1,int *param_2,int *param_3,undefined4 param_4,int param_5)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  undefined2 uVar7;
  int local_4;
  
  if (param_5 == 0) {
    iVar3 = 100;
  }
  else {
    iVar3 = 1000;
  }
  iVar6 = 0;
  local_4 = *(int *)(param_1 * 0x41 + 0x157d);
  iVar1 = *(int *)(param_1 * 0x41 + 0x157f);
  *param_3 = -1;
  *param_2 = -1;
  while( true ) {
    if (*(int *)(iVar6 * 2 + 0x7d8) == -1) {
      return iVar3;
    }
    iVar4 = FUN_1a8b_0b4d(&local_4);
    if (iVar4 == 0) break;
    uVar7 = (undefined2)((ulong)param_4 >> 0x10);
    uVar2 = *(uint *)((int)param_4 + local_4 * 0x138 + iVar1 * 2);
    uVar5 = (int)uVar2 >> 0xf;
    if ((int)((uVar2 ^ uVar5) - uVar5) < iVar3) {
      uVar2 = *(uint *)((int)param_4 + local_4 * 0x138 + iVar1 * 2);
      uVar5 = (int)uVar2 >> 0xf;
      iVar3 = (uVar2 ^ uVar5) - uVar5;
      *param_2 = local_4;
      *param_3 = iVar1;
    }
    iVar6 = iVar6 + 1;
  }
  return iVar3;
}


