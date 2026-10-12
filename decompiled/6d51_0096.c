
void __cdecl16far auto_ui_history_lines(int param_1,int param_2)

{
  int *piVar1;
  undefined2 uVar2;
  undefined2 in_DX;
  int iVar3;
  int iVar4;
  undefined2 unaff_SS;
  char *pcVar5;
  char local_58 [80];
  int local_8;
  undefined4 local_6;
  
  FUN_22bf_07a5();
  if (param_1 != 0) {
    if (DAT_4125_166a == 0) {
      DAT_4125_166a = 1;
      FUN_54f6_0000(10);
      auto_file_current_hst_2();
    }
    else {
      FUN_54f6_07e4(0,0,0,0x10f);
      if (DAT_4125_0dc4 != 1) {
        FUN_54f6_07e4(0,0x54,0,0x1e);
      }
    }
    FUN_78a8_06ae(1,0xf,0,3);
    pcVar5 = get_string(0x5b,DAT_4125_0dc4);
    in_DX = (undefined2)((ulong)pcVar5 >> 0x10);
    FUN_7ecb_00d6(0x1b0,0x40,(char *)pcVar5,in_DX);
  }
  FUN_78a8_06ae(2,0xf,0,3);
  if ((param_2 != 0) || (param_1 != 0)) {
    FUN_54f6_070b(0x10,0x122,0x50,0x14);
    sprintf((char *)CONCAT22(unaff_SS,local_58),s_Turn__d_4125_0dde,DAT_451b_04de);
    FUN_7ecb_00d6(0x138,0x15e,local_58);
  }
  if (param_1 != 0) {
    if (DAT_4125_0dc4 == 1) {
      FUN_6d51_18da(1);
    }
    else {
      FUN_6d51_034a();
    }
  }
  if ((((param_1 != 0) || (param_2 != 0)) && (DAT_4125_0dc4 == 1)) &&
     (FUN_6d51_16ba(), param_1 == 0)) {
    FUN_6d51_18da(0);
  }
  if (((param_1 != 0) || (param_2 != 0)) && (DAT_4125_0dc4 != 1)) {
    iVar3 = DAT_4125_0dc6 +
            (int)(((long)DAT_451b_04de * (long)DAT_4125_0dca) / (long)(DAT_451b_04dc + -1));
    DAT_451b_04c8 = CONCAT22(DAT_4125_0dc8 + -2,iVar3);
    DAT_451b_04cc = CONCAT22(DAT_4125_0dc8 + DAT_4125_0dcc + 6,iVar3);
    FUN_24d0_0002(0xd);
    FUN_2133_0095((int)DAT_451b_04c8,(int)((ulong)DAT_451b_04c8 >> 0x10),(int)DAT_451b_04cc,
                  (int)((ulong)DAT_451b_04cc >> 0x10),(int)DAT_451b_04d0,
                  (int)((ulong)DAT_451b_04d0 >> 0x10));
    if (DAT_4125_0dc4 == 3) {
      piVar1 = (int *)FUN_6d51_1161(DAT_451b_04de);
      local_6 = (int *)CONCAT22(iVar3,piVar1);
      iVar3 = 0;
      local_8 = 0;
      for (iVar4 = 0; iVar4 < 8; iVar4 = iVar4 + 1) {
        if (iVar3 < *local_6) {
          iVar3 = *local_6;
          local_8 = iVar4;
        }
        local_6 = (int *)CONCAT22((int)((ulong)local_6 >> 0x10),(int *)local_6 + 1);
      }
      iVar4 = local_8 * 0x14;
      uVar2 = 0x3c04;
      iVar3 = DAT_451b_04de;
      pcVar5 = get_string(0x5c,DAT_4125_0dc4);
      in_DX = (undefined2)((ulong)pcVar5 >> 0x10);
      sprintf((char *)CONCAT22(unaff_SS,local_58),pcVar5,iVar3,iVar4,uVar2);
    }
    else {
      iVar4 = (*DAT_451b_04c4)(0x2133,DAT_451b_04de);
      piVar1 = (int *)(iVar4 + current_player * 2);
      local_6 = (int *)CONCAT22(iVar3,piVar1);
      iVar3 = *piVar1;
      iVar4 = DAT_451b_04de;
      pcVar5 = get_string(0x5c,DAT_4125_0dc4);
      in_DX = (undefined2)((ulong)pcVar5 >> 0x10);
      sprintf((char *)CONCAT22(unaff_SS,local_58),pcVar5,iVar4,iVar3);
    }
    FUN_54f6_070b(0,0x102,0x150,0x14);
    FUN_7ecb_00d6(0x1b0,0x13e,local_58);
  }
  if (param_1 != 0) {
    FUN_834b_0000(0,0,0,0);
  }
  if ((param_2 != 0) || (param_1 != 0)) {
    uVar2 = FUN_6d51_1126(DAT_451b_04de);
    FUN_834b_12d3(0,0,uVar2,in_DX);
    FUN_834b_04d3(0,0);
  }
  FUN_22bf_0757();
  return;
}


