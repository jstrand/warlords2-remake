
void __cdecl16far ai_abandon_assaults(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  
  iVar5 = -1;
  uVar6 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  iVar4 = (int)DAT_451b_018e;
  iVar1 = -1;
  iVar2 = *(int *)(iVar4 + 0x24a);
  while (iVar3 = iVar2 + -1, iVar2 != 0) {
    iVar2 = iVar3;
    if ((*(int *)(iVar4 + iVar3 * 0x5c + 0x24c) != 0) &&
       (iVar5 < *(int *)(iVar4 + iVar3 * 0x5c + 0x24c))) {
      iVar5 = *(int *)(iVar4 + iVar3 * 0x5c + 0x24c);
      iVar1 = iVar3;
    }
  }
  if (iVar1 != -1) {
    FUN_563e_066d(iVar1,2);
  }
  return;
}


