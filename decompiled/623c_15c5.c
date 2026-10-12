
int __cdecl16far FUN_623c_15c5(undefined2 param_1,undefined2 param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 in_DX;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  
  iVar4 = 0;
  uVar6 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  if (*(int *)((int)DAT_451b_018e + 0x20) < 1) {
    iVar1 = 1;
    iVar3 = iVar1;
  }
  else {
    iVar1 = *(int *)((int)DAT_451b_018e + 0x20);
    iVar3 = iVar1;
  }
  while (iVar5 = iVar1 + -1, iVar1 != 0) {
    iVar2 = auto_ui_being_attacked(param_1,param_2,0);
    DAT_451b_01aa = CONCAT22(in_DX,iVar2);
    iVar1 = iVar5;
    if (*(int *)(iVar2 + 2) == 0) {
      iVar4 = iVar4 + 1;
    }
  }
  return (iVar4 * 100) / iVar3;
}


