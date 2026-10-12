
void __cdecl16far
FUN_24d0_0497(undefined2 param_1,undefined2 param_2,undefined2 *param_3,undefined2 param_4)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 uVar3;
  
  uVar3 = (undefined2)((ulong)param_3 >> 0x10);
  puVar1 = (undefined2 *)param_3;
  for (iVar2 = puVar1[1]; iVar2 < (int)(puVar1[1] + puVar1[3]); iVar2 = iVar2 + 1) {
    FUN_2012_0bb7(param_1,param_2,*param_3,iVar2,puVar1[2],param_4);
  }
  return;
}


