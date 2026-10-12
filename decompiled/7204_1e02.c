
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_7204_1e02(int param_1)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 in_DX;
  uint unaff_SI;
  undefined2 unaff_SS;
  char *pcVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  char *pcVar6;
  undefined2 uVar7;
  char acStack_66 [50];
  char acStack_34 [50];
  
  if (_LAB_4125_0fc0 != param_1) {
    if (_LAB_4125_0fc2 == 0) {
      _LAB_4125_0fc2 = 1;
      uVar1 = mem_alloc(8000,0,0,0x29d,0);
      DAT_451b_0524 = CONCAT22(in_DX,uVar1);
    }
    uVar1 = mem_lock((int)DAT_451b_0524,(int)((ulong)DAT_451b_0524 >> 0x10));
    DAT_451b_0520 = CONCAT22(in_DX,uVar1);
    _LAB_4125_0fc0 = param_1;
    uVar7 = 0x4125;
    uVar5 = 0x5da0;
    uVar4 = 0x4125;
    uVar1 = 0x5da0;
    pcVar3 = get_file_string(0x16,param_1 + -1);
    sprintf((char *)CONCAT22(unaff_SS,acStack_34),pcVar3,uVar1,uVar4,uVar5,uVar7);
    iVar2 = open((char *)CONCAT22(unaff_SS,acStack_34),2);
    if (iVar2 < 0) {
      pcVar6 = acStack_34;
      pcVar3 = get_error_string(1,0);
      sprintf((char *)CONCAT22(unaff_SS,acStack_66),pcVar3,pcVar6);
      auto_str_fatal_error(acStack_66);
    }
    read(iVar2,(void *)CONCAT22(8000,(void *)((ulong)DAT_451b_0520 >> 0x10)),unaff_SI);
    close(iVar2);
    for (iVar2 = 0; iVar2 < 8000; iVar2 = iVar2 + 1) {
      uVar1 = (undefined2)((ulong)DAT_451b_0520 >> 0x10);
      if (*(char *)((int)DAT_451b_0520 + iVar2) == '|') {
        *(undefined1 *)((int)DAT_451b_0520 + iVar2) = 0;
      }
    }
    mem_unlock((int)DAT_451b_0524,(int)((ulong)DAT_451b_0524 >> 0x10));
  }
  return;
}


