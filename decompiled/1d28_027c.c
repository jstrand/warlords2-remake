
void __cdecl16far FUN_1d28_027c(uint param_1,uint param_2,undefined2 param_3,undefined4 param_4)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  long lVar3;
  undefined4 *puVar4;
  int iVar5;
  int iVar6;
  undefined1 *puVar7;
  
  lVar3 = (ulong)param_2 * (ulong)param_1;
  iVar5 = (int)((ulong)lVar3 >> 0x10);
  puVar4 = (undefined4 *)mem_alloc(lVar3,param_3,(int)param_4,(int)((ulong)param_4 >> 0x10));
  if (puVar4 != (undefined4 *)0x0 || iVar5 != 0) {
    puVar2 = (undefined1 *)*puVar4;
    puVar7 = (undefined1 *)puVar2;
    while( true ) {
      iVar6 = (int)((ulong)lVar3 >> 0x10);
      for (iVar5 = (int)lVar3; iVar5 != 0; iVar5 = iVar5 + -1) {
        puVar1 = puVar7;
        puVar7 = puVar7 + 1;
        *puVar1 = 0;
      }
      if (iVar6 == 0) break;
      lVar3 = (ulong)(iVar6 - 1) << 0x10;
    }
  }
  return;
}


