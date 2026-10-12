
void __cdecl16far ui_diplomatic_ratings(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  undefined2 unaff_SS;
  char *pcVar5;
  int local_30 [8];
  undefined2 local_20 [8];
  int local_10;
  undefined4 local_e;
  undefined1 local_a [8];
  
  if (DAT_4125_166a == 0) {
    FUN_54f6_0000();
  }
  FUN_78a8_06ae(0xf0001,0x30000);
  pcVar5 = get_string(0x6b,0);
  uVar2 = (undefined2)((ulong)pcVar5 >> 0x10);
  FUN_7ecb_00d6(0x400140,pcVar5);
  FUN_24d0_0002();
  FUN_2133_0344();
  for (iVar3 = 0; iVar3 < 9; iVar3 = iVar3 + 1) {
    FUN_2133_0344();
  }
  FUN_2133_02fe();
  for (iVar3 = 0; iVar3 < 9; iVar3 = iVar3 + 1) {
    FUN_2133_02fe();
  }
  for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
    if (*(int *)(iVar3 * 2 + 0x137) != 0) {
      FUN_8611_0bf7(iVar3,3,0x342a,0x4125,iVar3 * 0x20 + 0x80,0x75);
      FUN_8611_0bf7(iVar3,3);
    }
  }
  for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
    for (iVar4 = 0; iVar4 < 8; iVar4 = iVar4 + 1) {
      if (((iVar3 == iVar4) ||
          ((*(int *)(iVar3 * 2 + 0x137) != 0 && (*(int *)(iVar4 * 2 + 0x137) != 0)))) &&
         ((iVar3 == iVar4 ||
          (((*(byte *)(iVar3 * 8 + iVar4 + 0x153b) & 3) == 2 ||
           ((*(byte *)(iVar3 * 8 + iVar4 + 0x153b) & 3) == 0)))))) {
        FUN_216d_002e(local_a);
        FUN_216d_000e(&local_e);
        uVar2 = unaff_SS;
        __F_SPUSH(local_e);
        FUN_1997_0129();
      }
    }
  }
  FUN_24d0_0002();
  uVar1 = FUN_216d_002e(local_a);
  FUN_216d_01fd(uVar1,uVar2);
  FUN_78a8_06ae(0xf0002,0x30000);
  pcVar5 = get_string(0x6d,0);
  FUN_7ecb_00d6(0x7401d8,(char *)pcVar5);
  diplomatic_rating(&local_10);
  for (iVar3 = 0; iVar3 < local_10; iVar3 = iVar3 + 1) {
    FUN_8611_0bf7(local_20[iVar3],3,0x342a,0x4125,400);
    pcVar5 = get_string(0x6a,local_30[iVar3]);
    FUN_7ecb_00bf(0x1b0,iVar3 * 0x1d + 0x92,(char *)pcVar5);
  }
  return;
}


