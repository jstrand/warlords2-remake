
void __cdecl16far auto_ui_random_map_terrain(int param_1)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_SS;
  char *pcVar3;
  int iVar4;
  undefined2 uVar5;
  char local_16 [20];
  
  FUN_22bf_07a5();
  if (DAT_4125_28c8 == 0) {
    __F_SPUSH((int)DAT_4125_2938,(int)((ulong)DAT_4125_2938 >> 0x10));
    FUN_1997_0129();
  }
  else {
    FUN_24d0_0002(3);
    if (param_1 < 0) {
      iVar4 = 0x2930;
    }
    else {
      iVar4 = param_1 * 8 + 0x2948;
    }
    FUN_216d_0288(iVar4,0x4125);
    uVar2 = 0x78a8;
    FUN_78a8_06ae(0xf0002,0,3);
    if ((param_1 == -1) || (param_1 < 4)) {
      iVar4 = param_1;
      if (param_1 < 0) {
        iVar4 = 0;
      }
      while( true ) {
        iVar1 = param_1;
        if (param_1 < 0) {
          iVar1 = 3;
        }
        if (iVar1 < iVar4) break;
        pcVar3 = get_string(2,iVar4);
        FUN_7ecb_0103(((undefined2 *)&DAT_4125_2978)[iVar4 * 2],
                      ((undefined2 *)&DAT_4125_297a)[iVar4 * 2],pcVar3);
        if (((undefined2 *)&DAT_4125_28d8)[iVar4] == 0) {
          uVar2 = (undefined2)((undefined4 *)&DAT_4125_299c)[iVar4];
          uVar5 = (undefined2)((ulong)((undefined4 *)&DAT_4125_299c)[iVar4] >> 0x10);
        }
        else {
          uVar2 = (undefined2)((undefined4 *)&DAT_4125_299c)[iVar4];
          uVar5 = (undefined2)((ulong)((undefined4 *)&DAT_4125_299c)[iVar4] >> 0x10);
        }
        __F_SPUSH(uVar2,uVar5);
        FUN_1997_0129();
        if (((undefined2 *)&DAT_4125_28d8)[iVar4] == 0) {
          strcpy((char *)CONCAT22(unaff_SS,local_16),(char *)0x41252a76);
        }
        else {
          sprintf((char *)CONCAT22(unaff_SS,local_16),
                  *(char **)((char *)s_M__AT__A___A___A_4125_2918 + iVar4 * 4));
        }
        uVar2 = 0x7ecb;
        FUN_7ecb_00bf(((undefined2 *)&DAT_4125_298c)[iVar4 * 2],
                      ((undefined2 *)&DAT_4125_297a)[iVar4 * 2]);
        iVar4 = iVar4 + 1;
      }
    }
    if ((param_1 == -1) || (param_1 == 4)) {
      func_0x000705bd(uVar2,DAT_4125_28ca,&stack0xffb6);
      FUN_7ecb_00bf(0x1540150,&stack0xffb8);
    }
    if ((param_1 == -1) || (param_1 == 5)) {
      pcVar3 = get_string(3,DAT_4125_28cc);
      FUN_7ecb_00bf(0x16d0150,pcVar3);
    }
  }
  FUN_22bf_0757();
  return;
}


