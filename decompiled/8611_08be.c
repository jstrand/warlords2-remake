
void __cdecl16far
FUN_8611_08be(int param_1,int param_2,undefined4 param_3,int param_4,int param_5,undefined2 param_6,
             undefined2 param_7,undefined2 param_8,undefined2 param_9,int param_10,int param_11,
             int param_12)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 unaff_SS;
  undefined4 local_44;
  undefined2 uStack_40;
  undefined2 uStack_3e;
  char local_30 [10];
  char local_26 [10];
  int local_1c [8];
  undefined2 local_c;
  undefined2 uStack_a;
  undefined4 local_8;
  int local_4;
  
  if (param_2 == 0xf) {
    param_2 = 8;
  }
  FUN_1997_034a(8);
  for (iVar4 = 0; iVar4 < 4; iVar4 = iVar4 + 1) {
    local_1c[iVar4 * 2] = param_4;
    if (iVar4 < 2) {
      iVar3 = 0;
    }
    else {
      iVar3 = 0x24;
    }
    local_1c[iVar4 * 2 + 1] = param_5 + iVar4 * 10 + iVar3;
    if (param_11 == 2) {
      local_1c[iVar4 * 2 + 1] = local_1c[iVar4 * 2 + 1] + -0x14;
    }
  }
  if (param_11 != 0) {
    sprintf((char *)CONCAT22(unaff_SS,&local_44),(char *)0x41252f35,param_6);
    sprintf((char *)CONCAT22(unaff_SS,&stack0xffc6),(char *)0x41252f3a,param_7);
    sprintf((char *)CONCAT22(unaff_SS,local_30),(char *)0x41252f3f,param_9);
    sprintf((char *)CONCAT22(unaff_SS,local_26),(char *)0x41252f44,param_8);
    for (iVar4 = 0; iVar4 < 4; iVar4 = iVar4 + 1) {
      if (param_11 == 2) {
        if (2 < iVar4) {
          local_1c[iVar4 * 2] = local_1c[iVar4 * 2] + 8;
          local_1c[iVar4 * 2 + 1] = local_1c[iVar4 * 2 + 1] + -0xf;
          for (iVar3 = 0;
              uVar1 = strlen((char *)CONCAT22(unaff_SS,(char *)((int)&local_44 + iVar4 * 10))),
              iVar3 < (int)uVar1; iVar3 = iVar3 + 1) {
            __F_SPUSH((int)*(undefined4 *)(local_1c + iVar4 * 2),
                      (int)((ulong)*(undefined4 *)(local_1c + iVar4 * 2) >> 0x10));
            FUN_1997_0129(8);
            local_1c[iVar4 * 2] = local_1c[iVar4 * 2] + 8;
          }
        }
      }
      else {
        __F_SPUSH((int)*(undefined4 *)(local_1c + iVar4 * 2),
                  (int)((ulong)*(undefined4 *)(local_1c + iVar4 * 2) >> 0x10));
        FUN_1997_0129(8);
        local_1c[iVar4 * 2] = local_1c[iVar4 * 2] + 8;
        local_44 = *(undefined4 *)(local_1c + iVar4 * 2);
        __F_SPUSH();
        FUN_1997_0129(8);
        local_1c[iVar4 * 2] = local_1c[iVar4 * 2] + 8;
        iVar3 = 0;
        while( true ) {
          uStack_3e = 0x1997;
          uStack_40 = 0x6bf5;
          uVar1 = strlen((char *)CONCAT22(unaff_SS,(char *)((int)&local_44 + iVar4 * 10)));
          if ((int)uVar1 <= iVar3) break;
          uStack_3e = 0x1000;
          uStack_40 = 0x6bcb;
          __F_SPUSH();
          uStack_3e = 8;
          uStack_40 = 0x1000;
          local_44 = CONCAT22(0x6bd2,(undefined2)local_44);
          FUN_1997_0129();
          local_1c[iVar4 * 2] = local_1c[iVar4 * 2] + 8;
          iVar3 = iVar3 + 1;
        }
      }
    }
  }
  if (param_10 != 0) {
    FUN_1997_034a(8);
    if (param_11 == 1) {
      iVar4 = 0x14;
    }
    else {
      iVar4 = 0;
    }
    (*DAT_451b_2a74)(0x1997,0x57e2,0x4125,(param_10 + -1) * 0x20,0,0x20,0x1e,(int)param_3,
                     (int)((ulong)param_3 >> 0x10),param_4,param_5 + iVar4);
  }
  if (-1 < param_1) {
    if (param_12 == 0) {
      local_4 = param_2 + 0x10;
      iVar4 = param_2 * 0x3b + 0x4f20;
    }
    else {
      local_4 = 0x2a;
      iVar4 = 0x525a;
    }
    local_8 = CONCAT22(0x4125,iVar4);
    if (param_12 == 0) {
      local_c = 0x4ee5;
    }
    else {
      local_c = 0x521f;
    }
    uStack_a = 0x4125;
    FUN_1997_034a(local_4);
    iVar3 = param_1 % 0x10 << 5;
    iVar4 = (param_1 / 0x10) * 0x1e;
    FUN_1997_060a(local_4);
    if (param_11 == 1) {
      iVar2 = 0x14;
    }
    else {
      iVar2 = 0;
    }
    (*DAT_451b_2a68)(0x1997,local_8,iVar3,iVar4,0x1d0020,param_3,param_4,param_5 + iVar2,local_c,
                     uStack_a,iVar3,iVar4);
    FUN_1997_06bb(local_4);
  }
  return;
}


