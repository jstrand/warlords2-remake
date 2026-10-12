
void __cdecl16far auto_file_x_sgn(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  void *pvVar4;
  uint unaff_DI;
  undefined2 unaff_SS;
  long lVar5;
  char local_2e [40];
  undefined4 local_6;
  
  sprintf((char *)CONCAT22(unaff_SS,local_2e),(char *)0x412503c9,0x5da0,0x4125,0x5da0,0x4125);
  iVar1 = open((char *)CONCAT22(unaff_SS,local_2e),2);
  if (-1 < iVar1) {
    lVar5 = filelength(iVar1);
    pvVar4 = (void *)((ulong)lVar5 >> 0x10);
    uVar2 = (undefined2)lVar5;
    uVar3 = FUN_1d28_01d5(uVar2);
    local_6 = CONCAT22(pvVar4,uVar3);
    if (local_6 != 0) {
      read(iVar1,(void *)CONCAT22(uVar2,pvVar4),unaff_DI);
      close(iVar1);
      iVar1 = open(s_current_sgn_4125_03d3,9);
      if (-1 < iVar1) {
        write(iVar1,(void *)CONCAT22(uVar2,(void *)((ulong)local_6 >> 0x10)),unaff_DI);
        close(iVar1);
        FUN_1d28_050d((int)local_6,(int)((ulong)local_6 >> 0x10));
      }
    }
  }
  return;
}


