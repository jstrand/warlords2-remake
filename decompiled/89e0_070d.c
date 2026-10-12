
void __cdecl16far FUN_89e0_070d(void)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  undefined2 unaff_SS;
  int iVar4;
  undefined2 *puVar5;
  undefined4 local_14;
  char local_c [10];
  
  bVar1 = false;
  if (DAT_4125_30f8 == 0) {
    FUN_22bf_07a5();
    FUN_78a8_06ae(2,0xf,0,3);
    FUN_1c8c_0912((undefined2 *)&DAT_451b_2902,0x4125,(undefined2 *)&DAT_451b_28e2,0x4125,
                  DAT_4125_30f6);
    if (DAT_451b_1f02 == 0) {
      strcpy((char *)CONCAT22(unaff_SS,local_c),(char *)0x4125313a);
    }
    else {
      sprintf((char *)CONCAT22(unaff_SS,local_c),(char *)0x4125313d,DAT_451b_1ea0);
    }
    __F_SPUSH((int)DAT_4125_3036,(int)((ulong)DAT_4125_3036 >> 0x10));
    FUN_1997_0129(8);
    __F_SPUSH(DAT_4125_303a);
    FUN_1997_0129(8);
    iVar3 = 0;
    while( true ) {
      local_14 = 0x1997a5f7;
      uVar2 = strlen((char *)CONCAT22(unaff_SS,local_c));
      if ((int)uVar2 <= iVar3) break;
      local_14 = 0x1000a5e2;
      __F_SPUSH();
      local_14 = 0x81000;
      FUN_1997_0129();
      iVar3 = iVar3 + 1;
    }
    for (iVar3 = 0; iVar3 < DAT_4125_30f6; iVar3 = iVar3 + 1) {
      if ((((undefined2 *)&DAT_451b_28e2)[iVar3] != 0) &&
         (((undefined1 *)&DAT_451b_0665)
          [*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar3 * 2) + 4) * 6] !=
          '\0')) {
        bVar1 = true;
      }
    }
    local_14 = 0x68424125;
    puVar5 = (undefined2 *)&DAT_451b_2902;
    iVar4 = 0x1000;
    iVar3 = FUN_1c8c_07fa();
    if ((((iVar3 != 0) || (puVar5 != (undefined2 *)0x0)) || ((bVar1 && (iVar4 != 0)))) ||
       ((bVar1 || (iVar4 != 0)))) {
      local_14 = 0x1c8ca690;
      __F_SPUSH();
      local_14 = 0x81000;
      FUN_1997_0129();
    }
    else {
      local_14 = CONCAT22(0xa6e5,(undefined2)local_14);
      FUN_24d0_0002();
      local_14 = DAT_4125_306e;
      FUN_216d_002e(&local_14);
      local_14 = 0x216da704;
      FUN_216d_0288();
    }
    FUN_22bf_0757();
  }
  return;
}


