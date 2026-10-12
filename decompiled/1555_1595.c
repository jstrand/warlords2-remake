
undefined2 __cdecl16far FUN_1555_1595(undefined2 *param_1,int param_2)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  int iVar3;
  undefined2 uVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  
  uVar4 = 0;
  uVar6 = (undefined2)((ulong)param_1 >> 0x10);
  puVar2 = (undefined2 *)param_1;
  uVar7 = (undefined2)((ulong)DAT_451b_0012 >> 0x10);
  iVar3 = (int)DAT_451b_0012 + param_2 * 0xd6;
  if ((((puVar2[2] == *(int *)(iVar3 + 10)) && (puVar2[3] == *(int *)(iVar3 + 0xc))) &&
      (puVar2[5] == *(int *)(iVar3 + 2))) &&
     ((puVar2[4] == *(int *)(iVar3 + 4) &&
      (iVar3 = FUN_1555_165c(*param_1,puVar2[1],param_2), iVar3 != -1)))) {
    iVar5 = 0;
    puVar1 = (undefined2 *)((int)DAT_451b_0012 + param_2 * 0xd6);
    for (; iVar3 < 200; iVar3 = iVar3 + 1) {
      *(undefined1 *)((int)*(undefined4 *)(puVar2 + 6) + iVar5) =
           *(undefined1 *)((int)puVar1 + iVar3 + 0xe);
      iVar5 = iVar5 + 1;
    }
    *puVar1 = 2;
    uVar4 = 1;
  }
  return uVar4;
}


