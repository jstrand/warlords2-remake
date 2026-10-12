
void __cdecl16far FUN_7bab_0634(int param_1,int param_2)

{
  undefined4 uVar1;
  undefined2 uVar2;
  undefined2 extraout_DX;
  undefined2 uVar3;
  int iVar4;
  undefined2 unaff_SS;
  undefined4 local_2c;
  int local_28;
  int iStack_26;
  int iStack_24;
  int local_22;
  undefined2 local_20;
  undefined2 uStack_1e;
  char *pcStack_1c;
  undefined2 uStack_1a;
  int local_18;
  int iStack_16;
  int iStack_14;
  int local_12;
  int local_10;
  int iStack_e;
  undefined2 local_8;
  int local_6;
  int local_4;
  
  if (param_1 < 0) {
    local_4 = 0;
    local_6 = 8;
  }
  else {
    local_6 = param_1 + 1;
    local_4 = param_1;
  }
  FUN_22bf_07a5();
  for (iVar4 = local_4; iVar4 < local_6; iVar4 = iVar4 + 1) {
    if (param_2 != 0) {
      FUN_216d_002e(&local_10);
      FUN_216d_000e(&local_2c);
      _local_10 = CONCAT22(iStack_e + -0x1e,local_10 + -0x10);
      __F_SPUSH((int)local_2c,(int)((ulong)local_2c >> 0x10));
      FUN_1997_0129(0x19);
    }
    if ((param_1 < 0) || (param_2 != 0)) {
      __F_SCOPY(iVar4 * 8 + 0x23c8,0x4125,&local_10,unaff_SS);
      __F_SCOPY(&local_10,unaff_SS,&local_18,unaff_SS);
      local_18 = local_18 + -1;
      iStack_16 = iStack_16 + -1;
      iStack_14 = iStack_14 + 2;
      local_12 = local_12 + 2;
      __F_SCOPY(&local_10,unaff_SS,&local_20,unaff_SS);
      __F_SCOPY(&local_10,unaff_SS,&local_28,unaff_SS);
      local_28 = local_28 + 1;
      iStack_26 = iStack_26 + 1;
      iStack_24 = iStack_24 + -2;
      local_22 = local_22 + -2;
      uVar3 = extraout_DX;
      FUN_24d0_0002(*(undefined2 *)(iVar4 * 2 + 0xa0));
      FUN_216d_01fd(&local_20);
      FUN_2133_02fe(local_18,iStack_16,iStack_14);
      FUN_2133_0344(local_18,iStack_16,local_12);
      FUN_2133_02fe(local_28,iStack_26 + local_22 + -1,iStack_24);
      FUN_2133_0344(local_28 + iStack_24 + -1,iStack_26,local_22);
      FUN_24d0_0002(*(undefined2 *)(iVar4 * 2 + 0xb0));
      FUN_2133_02fe(local_28,iStack_26,iStack_24);
      FUN_2133_0344(local_28,iStack_26,local_22);
      FUN_2133_02fe(local_18,iStack_16 + local_12 + -1,iStack_14);
      FUN_2133_0344(local_18 + iStack_14 + -1,iStack_16,local_12);
      FUN_78a8_06ae(2,*(undefined2 *)(iVar4 * 2 + 0xa0),*(undefined2 *)(iVar4 * 2 + 0xb0),3);
      local_8 = FUN_21e2_0a8e(iVar4 * 0x14,0x3c04);
      FUN_24d0_0002(3);
      uVar2 = FUN_216d_002e(&local_10);
      FUN_216d_0288(uVar2,uVar3);
      FUN_7ecb_00bf(*(undefined2 *)(iVar4 * 4 + 0x2478),*(undefined2 *)(iVar4 * 4 + 0x247a),
                    iVar4 * 0x14,0x3c04);
    }
    uVar1 = *(undefined4 *)(iVar4 * 4 + 0x2458);
    __F_SPUSH((int)uVar1,(int)((ulong)uVar1 >> 0x10));
    FUN_1997_0129(0x1f);
    local_2c = *(undefined4 *)(iVar4 * 4 + 0x2498);
    __F_SPUSH();
    FUN_1997_0129(0x1f);
    uVar1 = *(undefined4 *)(iVar4 * 4 + 0x24fc);
    iStack_24 = (int)uVar1;
    local_22 = (int)((ulong)uVar1 >> 0x10);
    iStack_26 = 0x1997;
    local_28 = -0x3b64;
    __F_SPUSH();
    iStack_26 = 0x1f;
    local_28 = 0x1000;
    local_2c = CONCAT22(0xc4a3,(undefined2)local_2c);
    FUN_1997_0129();
    uStack_1a = 3;
    pcStack_1c = (char *)*(undefined2 *)(iVar4 * 2 + 0xb0);
    uStack_1e = *(undefined2 *)(iVar4 * 2 + 0xa0);
    local_20 = 2;
    local_22 = 0x1997;
    iStack_24 = 0xc4cb;
    FUN_78a8_06ae();
    uStack_1a = 0x4125;
    pcStack_1c = (char *)s_Character_4125_258b;
    uStack_1e = *(undefined2 *)(iVar4 * 4 + 0x251e);
    local_20 = *(undefined2 *)(iVar4 * 4 + 0x251c);
    local_22 = 0x78a8;
    iStack_24 = -0x3b17;
    FUN_7ecb_00bf();
  }
  FUN_22bf_0757();
  return;
}


