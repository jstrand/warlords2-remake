
void __cdecl16far FUN_18a9_0493(int param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  
  iVar1 = 0;
  while( true ) {
    if (*(int *)((undefined4 *)DAT_451b_0ce0)[2] <= iVar1) {
      return;
    }
    if (*(int *)((int)*DAT_451b_0ce0 + iVar1 * 0x21) == param_1) break;
    iVar1 = iVar1 + 1;
  }
  uVar3 = (undefined2)((ulong)*DAT_451b_0ce0 >> 0x10);
  iVar2 = (int)*DAT_451b_0ce0 + iVar1 * 0x21;
  if ((*(int *)(iVar2 + 0xe) == param_3) && (*(int *)(iVar2 + 0xc) == param_2)) {
    return;
  }
  uVar3 = (undefined2)((ulong)*DAT_451b_0ce0 >> 0x10);
  iVar2 = (int)*DAT_451b_0ce0 + iVar1 * 0x21;
  *(int *)(iVar2 + 0xe) = param_3;
  *(int *)(iVar2 + 0xc) = param_2;
  *(undefined1 *)((int)*DAT_451b_0ce0 + iVar1 * 0x21 + 5) = 0;
  return;
}


