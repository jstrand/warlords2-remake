
undefined2 __cdecl16far FUN_1a8b_0b9a(int param_1,int param_2,int param_3,undefined2 param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  int local_4;
  int iVar6;
  
  local_4 = 9;
  piVar4 = (int *)(param_3 + 0x12);
  iVar6 = 0x12;
  while( true ) {
    iVar5 = iVar6 + -2;
    piVar4 = piVar4 + -1;
    iVar1 = local_4 + -1;
    if (local_4 == 0) break;
    *piVar4 = 0xff;
    iVar3 = param_1 + *(int *)((int)(undefined2 *)&DAT_4125_2b37 + iVar6 + 1);
    iVar2 = param_2 + *(int *)(iVar6 + 0x2b26);
    iVar6 = iVar5;
    local_4 = iVar1;
    if ((((-1 < iVar3) && (iVar3 < 0x70)) && (-1 < iVar2)) && (iVar2 < 0x9c)) {
      if ((*(byte *)(iVar2 * 0x70 + iVar3) & 0x1f) == 0) {
        *piVar4 = (int)*(char *)(*(byte *)(iVar2 * 0xe0 + iVar3 * 2) + 0x710);
      }
      else {
        *piVar4 = 0;
      }
    }
  }
  return 1;
}


