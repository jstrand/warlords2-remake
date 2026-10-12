
void __cdecl16far FUN_4c49_0b0e(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int unaff_DI;
  int local_e;
  int iStack_c;
  undefined4 local_a;
  int local_6;
  int local_4;
  
  iVar2 = FUN_4c49_1016(param_1);
  iVar3 = FUN_4c49_1016(param_2);
  if (iVar2 == iVar3) {
    FUN_4c49_0dd0(param_1,param_2,&local_6);
    local_a = param_1;
    while( true ) {
      if (((int)local_a == (int)param_2) && (local_a._2_2_ == param_2._2_2_)) break;
      local_a = CONCAT22(local_a._2_2_ + local_4,(int)local_a + local_6);
      FUN_4bed_0470(&local_a);
      *(undefined1 *)((int)DAT_451b_002a + local_a._2_2_ * 0x70 + (int)local_a + 0x6120) = 7;
      FUN_4bed_03aa(local_a);
    }
  }
  else {
    uVar4 = iVar2 - iVar3 >> 0xf;
    if ((iVar2 - iVar3 ^ uVar4) - uVar4 == 2) {
      FUN_4c49_0a0e(param_1,param_2);
    }
    else {
      uVar4 = iVar2 - iVar3 >> 0xf;
      if ((iVar2 - iVar3 ^ uVar4) - uVar4 == 3) {
        unaff_DI = 0;
      }
      else {
        uVar4 = iVar2 - iVar3 >> 0xf;
        if (((iVar2 - iVar3 ^ uVar4) - uVar4 == 1) && (unaff_DI = iVar3, iVar3 < iVar2)) {
          unaff_DI = iVar2;
        }
      }
      uVar1 = *(undefined4 *)((int)DAT_451b_002a + unaff_DI * 4 + 0x7c);
      FUN_4c49_0dd0(param_1,uVar1,&local_6);
      local_a = param_1;
      while( true ) {
        local_e = (int)uVar1;
        if (((int)local_a == local_e) &&
           (iStack_c = (int)((ulong)uVar1 >> 0x10), local_a._2_2_ == iStack_c)) break;
        local_a = CONCAT22(local_a._2_2_ + local_4,(int)local_a + local_6);
        FUN_4bed_0470(&local_a);
        *(undefined1 *)((int)DAT_451b_002a + local_a._2_2_ * 0x70 + (int)local_a + 0x6120) = 7;
        FUN_4bed_03aa(local_a);
      }
      FUN_4c49_0dd0(param_2,uVar1,&local_6);
      local_a = param_2;
      while( true ) {
        if (((int)local_a == local_e) && (local_a._2_2_ == iStack_c)) break;
        local_a = CONCAT22(local_a._2_2_ + local_4,(int)local_a + local_6);
        FUN_4bed_0470(&local_a);
        *(undefined1 *)((int)DAT_451b_002a + local_a._2_2_ * 0x70 + (int)local_a + 0x6120) = 7;
        FUN_4bed_03aa(local_a);
      }
    }
  }
  return;
}


