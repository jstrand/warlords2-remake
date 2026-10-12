
void __cdecl16far FUN_18a9_0702(int param_1)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  
  iVar2 = 0;
  while( true ) {
    uVar3 = (undefined2)((ulong)DAT_451b_0ce0 >> 0x10);
    if (**(int **)((int *)DAT_451b_0ce0 + 4) <= iVar2) {
      return;
    }
    if (*(int *)((int)*(undefined4 *)DAT_451b_0ce0 + iVar2 * 0x21) == param_1) break;
    iVar2 = iVar2 + 1;
  }
  iVar1 = ((int *)DAT_451b_0ce0)[1];
  iVar2 = *DAT_451b_0ce0 + iVar2 * 0x21;
  FUN_1a0a_0005(iVar2,iVar1,iVar2,iVar1);
  return;
}


