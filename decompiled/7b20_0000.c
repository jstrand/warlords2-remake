
void __cdecl16far auto_file_standard_pal(int param_1)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_SS;
  char *pcVar4;
  undefined2 local_8c [16];
  undefined2 local_6c [16];
  undefined2 local_4c [6];
  undefined2 uStack_40;
  char local_2c [40];
  undefined2 local_4;
  
  if (param_1 == 0) {
    pcVar4 = (char *)s_standard_pal_4125_1f14;
  }
  else if (param_1 == 1) {
    sprintf((char *)CONCAT22(unaff_SS,local_2c),s_terrain_d_war2_pal_4125_1f2b,DAT_3c04_0161);
    pcVar4 = local_2c;
  }
  else {
    if (param_1 != 2) goto LAB_7b20_007f;
    pcVar4 = (char *)s_black_pal_4125_1f21;
  }
  FUN_7b20_0229(pcVar4);
LAB_7b20_007f:
  iVar2 = 0x10;
  while (iVar3 = iVar2 + -1, iVar2 != 0) {
    if (iVar3 == 6) {
      uVar1 = __ftol();
      __ftol(uVar1);
      local_4 = uStack_40;
      __ftol();
    }
    else if (iVar3 < 8) {
      local_4 = local_8c[iVar3];
      uVar1 = __ftol();
      local_4 = local_6c[iVar3];
      __ftol(uVar1);
      local_4 = local_4c[iVar3];
      __ftol();
    }
    else {
      local_4 = local_8c[iVar3];
      uVar1 = __ftol();
      local_4 = local_6c[iVar3];
      __ftol(uVar1);
      local_4 = local_4c[iVar3];
      __ftol();
    }
    FUN_7b20_01f3();
    iVar2 = iVar3;
  }
  return;
}


