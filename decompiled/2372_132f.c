
void __cdecl16far FUN_2372_132f(int param_1)

{
  undefined2 uVar1;
  undefined2 in_DX;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined1 local_e [8];
  int local_6;
  int iStack_4;
  
  uVar1 = FUN_216d_002e(local_e);
  uVar5 = 0x22bf;
  FUN_22bf_07e8(uVar1,in_DX);
  FUN_2372_102d(0xff);
  iVar4 = ((int *)DAT_451b_2fc8)[4];
  while (iVar4 != 0) {
    iVar3 = ((int *)DAT_451b_2fc8)[1];
    iVar2 = *DAT_451b_2fc8 + (iVar4 + -1) * 0x2e;
    _local_6 = CONCAT22(iVar3,iVar2);
    if ((*(long *)(iVar2 + 0x1a) == 0) || (param_1 != 0)) {
      uVar1 = 0;
    }
    else {
      uVar1 = 1;
    }
    *(undefined2 *)(iVar2 + 0x1e) = uVar1;
    if (*(int *)(iVar2 + 0x1e) == 0) {
      uVar1 = 2;
    }
    else {
      uVar1 = 1;
    }
    (*DAT_451b_2fc4)(uVar5,uVar1);
    uVar1 = (undefined2)((ulong)_local_6 >> 0x10);
    iVar3 = (int)_local_6;
    uVar5 = 0x21e2;
    FUN_21e2_0401(*(int *)(iVar3 + 0xe) + 2,1,*(undefined2 *)(iVar3 + 4),*(undefined2 *)(iVar3 + 6))
    ;
    iVar4 = iVar4 + -1;
  }
  FUN_22bf_086a();
  return;
}


