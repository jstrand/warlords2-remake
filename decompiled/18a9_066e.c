
void __cdecl16far FUN_18a9_066e(int param_1,char param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined4 local_6;
  
  iVar2 = 0;
  while( true ) {
    uVar4 = (undefined2)((ulong)DAT_451b_0ce0 >> 0x10);
    if (**(int **)((int *)DAT_451b_0ce0 + 4) <= iVar2) {
      return;
    }
    if (*(int *)((int)*(undefined4 *)DAT_451b_0ce0 + iVar2 * 0x21) == param_1) break;
    iVar2 = iVar2 + 1;
  }
  iVar1 = ((int *)DAT_451b_0ce0)[1];
  iVar3 = *DAT_451b_0ce0 + iVar2 * 0x21;
  local_6 = CONCAT22(iVar1,iVar3);
  if ((*(char *)(iVar3 + 4) == param_2) && (*(char *)(iVar3 + 5) == '\x01')) {
    return;
  }
  *(char *)((int)*(undefined4 *)DAT_451b_0ce0 + iVar2 * 0x21 + 4) = param_2;
  *(undefined1 *)((int)*(undefined4 *)DAT_451b_0ce0 + iVar2 * 0x21 + 5) = 1;
  FUN_1a0a_0005(local_6);
  return;
}


