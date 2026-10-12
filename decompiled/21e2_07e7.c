
undefined2 __cdecl16far
FUN_21e2_07e7(undefined2 param_1,undefined2 param_2,char *param_3,undefined2 param_4,
             undefined2 param_5)

{
  undefined4 *puVar1;
  char *pcVar2;
  undefined2 in_DX;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  
  puVar1 = (undefined4 *)*(undefined4 *)((int)DAT_451b_30e9 + 0x61);
  uVar5 = (undefined2)((ulong)*puVar1 >> 0x10);
  iVar3 = (int)*puVar1;
  if ((*(int *)(iVar3 + 10) != 0 || *(int *)(iVar3 + 0xc) != 0) &&
     ((*(uint *)((int)*puVar1 + 0x35) & 10) == 0)) {
    for (iVar3 = 0; iVar3 < (int)(uint)*(byte *)((int)*puVar1 + 0x34); iVar3 = iVar3 + 1) {
      uVar5 = (undefined2)((ulong)*puVar1 >> 0x10);
      iVar4 = (int)*puVar1 + iVar3 * 4;
      uVar5 = mem_lock(*(undefined2 *)(iVar4 + 10),*(undefined2 *)(iVar4 + 0xc));
      uVar6 = (undefined2)((ulong)*puVar1 >> 0x10);
      iVar4 = (int)*puVar1 + iVar3 * 4;
      *(undefined2 *)(iVar4 + 0x1c) = in_DX;
      *(undefined2 *)(iVar4 + 0x1a) = uVar5;
    }
  }
  while (pcVar2 = param_3, *param_3 != '\0') {
    param_3 = (char *)CONCAT22(param_3._2_2_,(char *)param_3 + 1);
    param_1 = FUN_21e2_0553(param_1,param_2,(int)*pcVar2,0,param_4,param_5);
  }
  return param_1;
}


