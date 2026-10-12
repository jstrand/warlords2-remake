
void __cdecl16far difficulty_rating(void)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_SI;
  int iVar3;
  undefined2 unaff_DI;
  int iVar4;
  undefined2 unaff_SS;
  char *fmt;
  undefined4 uVar5;
  int local_10;
  int local_e;
  undefined1 local_8 [4];
  int local_4;
  
  uVar5 = CONCAT22(unaff_SI,unaff_DI);
  iVar1 = ((DAT_4125_23b4 * 4 + DAT_4125_23b6 * 4 + DAT_4125_23b8 * 3 + DAT_4125_23ba * 4) -
          DAT_4125_23bc) + DAT_4125_23be;
  if (0x13 < iVar1) {
    iVar1 = 0x14;
  }
  iVar4 = 0;
  iVar3 = 0;
  for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
    if (((*(int *)(iVar2 * 2 + 0x137) != 0) && (*(int *)(iVar2 * 2 + 0xd0) == 1)) &&
       (*(int *)(iVar2 * 2 + 0xc0) != 3)) {
      iVar4 = iVar4 + *(int *)(iVar2 * 2 + 0xc0) + 1;
      iVar3 = iVar3 + 1;
    }
  }
  if (iVar3 == 0) {
    iVar2 = 0x50;
  }
  else {
    iVar2 = (iVar4 * 0x50) / (iVar3 * 3);
  }
  if (0x4d < iVar2) {
    iVar2 = 0x50;
  }
  iVar2 = iVar2 + iVar1;
  local_4 = iVar2;
  fmt = get_string(6,0);
  sprintf((char *)0x41255938,fmt,iVar2,uVar5);
  FUN_78a8_06ae(2,0xf,0,3);
  FUN_22bf_07a5();
  FUN_216d_000e(local_8);
  __F_SCOPY((undefined4 *)&DAT_4125_24e4,0x4125,&local_10,unaff_SS);
  local_10 = local_10 + -0x10;
  local_e = local_e + -0x157;
  __F_SPUSH(local_8._0_2_,local_8._2_2_);
  FUN_1997_0129(0x19);
  local_e = 0x4125;
  local_10 = 0x5938;
  FUN_7ecb_00d6(DAT_4125_24e0);
  local_e = 0x7ecb;
  local_10 = -0x3860;
  FUN_22bf_0757();
  return;
}


