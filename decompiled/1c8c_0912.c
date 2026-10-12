
void __cdecl16far FUN_1c8c_0912(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined4 uVar1;
  int iVar2;
  undefined2 uVar3;
  
  DAT_451b_1ea0 = 100;
  for (iVar2 = 0; iVar2 < param_3; iVar2 = iVar2 + 1) {
    if ((*(int *)((int)param_2 + iVar2 * 2) != 0) &&
       (uVar3 = (undefined2)((ulong)param_1 >> 0x10),
       uVar1 = *(undefined4 *)((int)param_1 + iVar2 * 4), *(char *)((int)uVar1 + 7) < DAT_451b_1ea0)
       ) {
      uVar1 = *(undefined4 *)((int)param_1 + iVar2 * 4);
      DAT_451b_1ea0 = (int)*(char *)((int)uVar1 + 7);
    }
  }
  return;
}


