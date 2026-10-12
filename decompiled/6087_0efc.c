
undefined1 __cdecl16far ai_choose_target(int param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined1 uVar5;
  
  iVar1 = dice(1,0x14,0);
  uVar4 = (undefined2)((ulong)param_2 >> 0x10);
  iVar2 = (int)param_2;
  uVar5 = *(int *)(iVar2 + 0x30) != -1;
  iVar3 = 0;
  if ((bool)uVar5) {
    iVar3 = (100 - *(int *)(iVar2 + 0x32)) + iVar1;
  }
  iVar1 = dice(1,0x14,0);
  iVar1 = (100 - *(int *)(iVar2 + 0x36)) + iVar1;
  if ((*(int *)(iVar2 + 0x34) != -1) && (iVar3 < iVar1)) {
    uVar5 = 3;
    iVar3 = iVar1;
  }
  iVar1 = iVar2 + param_1 * 2;
  if ((*(int *)(iVar1 + 0xc) == -1) ||
     (4 < *(byte *)((int)DAT_451b_018e + *(int *)(iVar1 + 0xc) + 0x182))) {
    iVar1 = dice(1,0x14,0);
    iVar1 = (100 - *(int *)(iVar2 + 0x3a)) + iVar1;
    if ((*(int *)(iVar2 + 0x38) != -1) && (iVar3 < iVar1)) {
      uVar5 = 4;
      iVar3 = iVar1;
    }
    iVar1 = dice(1,0x14,0);
    if ((*(int *)(iVar2 + 0x3c) != -1) && (iVar3 < (100 - *(int *)(iVar2 + 0x3e)) + iVar1)) {
      uVar5 = 2;
    }
  }
  return uVar5;
}


