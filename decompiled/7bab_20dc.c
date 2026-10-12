
undefined1 * __cdecl16far FUN_7bab_20dc(int param_1)

{
  int iVar1;
  undefined2 uVar2;
  void *pvVar3;
  int iVar4;
  uint unaff_DI;
  undefined2 unaff_SS;
  long lVar5;
  int local_42 [9];
  char local_30 [40];
  int local_8;
  int local_6;
  void *pvStack_4;
  
  DAT_451b_1d28 = 0;
  FUN_7bab_21c1(DAT_4125_2563,0,local_30);
  iVar1 = open((char *)CONCAT22(unaff_SS,local_30),2);
  if (-1 < iVar1) {
    lVar5 = filelength(iVar1);
    pvVar3 = (void *)((ulong)lVar5 >> 0x10);
    local_8 = (int)lVar5;
    uVar2 = FUN_1d28_01d5(local_8);
    _local_6 = CONCAT22(pvVar3,uVar2);
    read(iVar1,(void *)CONCAT22(local_8,pvVar3),unaff_DI);
    close(iVar1);
    local_42[0] = 0;
    iVar4 = 1;
    iVar1 = 0;
    while( true ) {
      if ((local_8 <= iVar1) || (8 < iVar4)) break;
      if (*(char *)(local_6 + iVar1) == '\r') {
        *(undefined1 *)(local_6 + iVar1) = 0;
        iVar1 = iVar1 + 2;
        local_42[iVar4] = iVar1;
        iVar4 = iVar4 + 1;
      }
      iVar1 = iVar1 + 1;
    }
    if (param_1 < iVar4 + -1) {
      strcpy(&DAT_451b_1d28,(char *)CONCAT22(pvStack_4,(char *)(local_6 + local_42[param_1])));
    }
    FUN_1d28_050d((int)_local_6,(int)((ulong)_local_6 >> 0x10));
  }
  return (undefined1 *)&DAT_451b_1d28;
}


