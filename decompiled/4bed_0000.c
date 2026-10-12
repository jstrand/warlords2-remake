
void __cdecl16far random_map_load_params(int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  undefined2 uVar2;
  int iVar3;
  void *in_DX;
  void *pvVar4;
  uint unaff_DI;
  
  FUN_22bf_07a5();
  FUN_4bed_01b5();
  uVar2 = mem_alloc(0xa560,0x20,0x29b,0);
  pvVar4 = in_DX;
  DAT_451b_002a._0_2_ = mem_lock(uVar2,in_DX);
  DAT_451b_002a._2_2_ = pvVar4;
  iVar3 = open(s_RANDOM_RANDOM_DAT_4125_0262,2);
  if (iVar3 < 0) {
    printf(s_Random_data_not_found_4125_0274);
    auto_str_warlords_c_strategic_studies();
  }
  else {
    read(iVar3,(void *)CONCAT22(0xa560,DAT_451b_002a._2_2_),unaff_DI);
    close(iVar3);
    pvVar4 = DAT_451b_002a._2_2_;
    iVar3 = (int)DAT_451b_002a;
    *(int *)((int)DAT_451b_002a + 0x2a) =
         *(int *)((int)DAT_451b_002a + 0x2a) + *(int *)((int)DAT_451b_002a + param_3 * 2 + 0x6e);
    iVar1 = (int)DAT_451b_002a;
    *(int *)((int)DAT_451b_002a + 0x3c) =
         *(int *)((int)DAT_451b_002a + 0x3c) + *(int *)(iVar3 + param_4 * 2 + 0x60);
    iVar3 = (int)DAT_451b_002a;
    *(int *)((int)DAT_451b_002a + 0x36) =
         *(int *)((int)DAT_451b_002a + 0x36) + *(int *)(iVar1 + param_2 * 2 + 0x44);
    iVar1 = (int)DAT_451b_002a;
    *(int *)((int)DAT_451b_002a + 0x34) =
         *(int *)((int)DAT_451b_002a + 0x34) + *(int *)(iVar3 + param_2 * 2 + 0x44);
    iVar3 = (int)DAT_451b_002a;
    *(int *)((int)DAT_451b_002a + 0x38) =
         *(int *)((int)DAT_451b_002a + 0x38) + *(int *)(iVar1 + param_1 * 2 + 0x52);
    *(int *)((int)DAT_451b_002a + 0x3a) =
         *(int *)((int)DAT_451b_002a + 0x3a) + *(int *)(iVar3 + param_1 * 2 + 0x52);
    random_map_generate();
    mem_unlock(uVar2,in_DX);
    FUN_1d28_048e(uVar2,in_DX);
    FUN_22bf_0757();
  }
  return;
}


