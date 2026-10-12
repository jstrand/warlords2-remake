
void __cdecl16far auto_file_current_hst_2(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  uint unaff_SI;
  void *unaff_SS;
  char *name;
  undefined4 local_6;
  
  iVar1 = 2;
  name = get_file_string(6,0);
  uVar3 = (undefined2)((ulong)name >> 0x10);
  iVar1 = open(name,iVar1);
  if (-1 < iVar1) {
    FUN_170b_008c();
    read(iVar1,(void *)CONCAT22(4,unaff_SS),unaff_SI);
    DAT_451b_04d8._0_2_ = mem_alloc(local_6 + -4,0,0x4d2,0);
    DAT_451b_04d8._2_2_ = uVar3;
    uVar2 = mem_lock((undefined2)DAT_451b_04d8,uVar3);
    DAT_451b_04d4 = CONCAT22(uVar3,uVar2);
    FUN_170b_008c();
    local_6._0_2_ = (undefined2)(local_6 + -4);
    read(iVar1,(void *)CONCAT22((undefined2)local_6,(void *)((ulong)DAT_451b_04d4 >> 0x10)),unaff_SI
        );
    close(iVar1);
    DAT_451b_04d0._0_2_ = FUN_1d28_01d5(DAT_4125_0dcc + 10);
    DAT_451b_04d0._2_2_ = uVar3;
  }
  return;
}


