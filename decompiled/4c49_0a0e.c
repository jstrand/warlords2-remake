
void __cdecl16far FUN_4c49_0a0e(undefined4 param_1,undefined4 param_2)

{
  bool bVar1;
  int iVar2;
  undefined4 local_e;
  undefined4 local_a;
  undefined4 local_6;
  
  bVar1 = false;
  local_6 = param_1;
  FUN_4c49_0dd0(param_1,param_2,&local_a);
  do {
    if (bVar1) {
      return;
    }
    FUN_4c49_0dd0(local_6,param_2,&local_e);
    iVar2 = FUN_4c49_0fed(local_6,param_2);
    if ((iVar2 < 3) || (iVar2 = dice(1,3,0), iVar2 == 1)) {
      iVar2 = local_e._2_2_;
      local_6._0_2_ = (int)local_6 + (int)local_e;
LAB_4c49_0a6a:
      local_6 = CONCAT22(local_6._2_2_ + iVar2,(int)local_6);
    }
    else {
      if (iVar2 == 2) {
        iVar2 = local_a._2_2_;
        local_6._0_2_ = (int)local_6 + (int)local_a;
        goto LAB_4c49_0a6a;
      }
      if (iVar2 == 3) {
        FUN_4c49_104a(&local_e);
        local_6 = CONCAT22(local_6._2_2_ + local_e._2_2_,(int)local_6 + (int)local_e);
        local_a = local_e;
      }
    }
    FUN_4bed_0470(&local_6);
    *(undefined1 *)((int)DAT_451b_002a + local_6._2_2_ * 0x70 + (int)local_6 + 0x6120) = 7;
    FUN_4bed_03aa(local_6);
    if ((int)local_6 == (int)param_2) {
      if (local_6._2_2_ == param_2._2_2_) {
        bVar1 = true;
      }
    }
  } while( true );
}


