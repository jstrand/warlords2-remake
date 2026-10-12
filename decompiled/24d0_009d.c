
void __cdecl16far
FUN_24d0_009d(undefined2 param_1,undefined2 param_2,undefined2 *param_3,int param_4,int param_5)

{
  int iVar1;
  undefined2 *puVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  
  FUN_22bf_07a5();
  uVar3 = (undefined2)((ulong)DAT_451b_30e9 >> 0x10);
  iVar1 = (int)DAT_451b_30e9;
  uVar4 = (undefined2)((ulong)param_3 >> 0x10);
  puVar2 = (undefined2 *)param_3;
  (*DAT_451b_2a74)(0x22bf,param_1,param_2,*param_3,puVar2[1],puVar2[2],puVar2[3],
                   *(undefined2 *)(iVar1 + 4),*(undefined2 *)(iVar1 + 6),
                   param_4 + *(int *)(iVar1 + 8),param_5 + *(int *)(iVar1 + 10));
  FUN_22bf_0757();
  return;
}


