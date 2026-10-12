
void __cdecl16far FUN_5ca7_00ca(undefined4 param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 local_4;
  
  iVar5 = -1;
  local_4 = 0xffff;
  iVar3 = 8;
  while (iVar4 = iVar3 + -1, iVar3 != 0) {
    uVar6 = (undefined2)((ulong)param_1 >> 0x10);
    iVar1 = (int)param_1;
    piVar2 = (int *)(iVar1 + iVar4 * 2);
    iVar3 = iVar4;
    if (*piVar2 != -1) {
      iVar5 = *(int *)(*piVar2 * 0x16 + -0x7780);
      local_4 = *(undefined2 *)(*(int *)(iVar1 + iVar4 * 2) * 0x16 + -0x777e);
      FUN_5ca7_015e(*(undefined2 *)(iVar1 + iVar4 * 2),0);
    }
  }
  if ((param_2 != 0) && (iVar5 != -1)) {
    FUN_5ca7_10a1(iVar5,local_4);
  }
  refresh_map_owners();
  return;
}


