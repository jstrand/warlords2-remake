
int __cdecl16far FUN_229d_00a9(undefined4 param_1,undefined2 param_2)

{
  undefined4 uVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined4 local_6;
  
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  if (*(long *)(iVar3 + 6) == 0) {
    uVar2 = FUN_229d_000e();
    *(undefined2 *)(iVar3 + 8) = param_2;
    *(undefined2 *)(iVar3 + 6) = uVar2;
    if (*(long *)(iVar3 + 6) != 0) {
      local_6 = *(undefined4 *)(iVar3 + 6);
      *(undefined4 *)((int)local_6 + 0x10) = 0;
LAB_229d_013a:
      *(undefined4 *)(iVar3 + 10) = local_6;
      *(undefined4 *)((int)local_6 + 0xc) = 0;
      *(int *)(iVar3 + 4) = *(int *)(iVar3 + 4) + 1;
      return (int)local_6;
    }
  }
  else {
    uVar1 = *(undefined4 *)(iVar3 + 10);
    uVar2 = FUN_229d_000e();
    uVar6 = (undefined2)((ulong)uVar1 >> 0x10);
    iVar4 = (int)uVar1;
    *(undefined2 *)(iVar4 + 0xe) = param_2;
    *(undefined2 *)(iVar4 + 0xc) = uVar2;
    if (*(long *)(iVar4 + 0xc) != 0) {
      local_6 = *(undefined4 *)(iVar4 + 0xc);
      *(undefined4 *)((int)local_6 + 0x10) = *(undefined4 *)(iVar3 + 10);
      goto LAB_229d_013a;
    }
  }
  return 0;
}


