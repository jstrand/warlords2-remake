
void __cdecl16far FUN_18a9_0345(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  
  iVar2 = **(int **)((int *)DAT_451b_0ce0 + 4);
  while (uVar4 = (undefined2)((ulong)DAT_451b_0ce0 >> 0x10), iVar2 != 0) {
    iVar1 = ((int *)DAT_451b_0ce0)[1];
    iVar3 = *DAT_451b_0ce0 + (iVar2 + -1) * 0x21;
    *(undefined1 *)(iVar3 + 5) = 0;
    *(undefined1 *)(iVar3 + 4) = 0;
    iVar2 = iVar2 + -1;
  }
  iVar2 = **(int **)((int *)DAT_451b_0ce0 + 6);
  while (iVar2 != 0) {
    *(undefined2 *)((int)*(undefined4 *)((int *)DAT_451b_0ce0 + 2) + (iVar2 + -1) * 0xe + 2) = 0;
    iVar2 = iVar2 + -1;
  }
  return;
}


