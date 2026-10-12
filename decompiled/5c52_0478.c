
void __cdecl16far FUN_5c52_0478(undefined2 param_1,undefined2 param_2,int param_3,int param_4)

{
  undefined2 uVar1;
  
  if ((2 < param_3) || (*(int *)(current_player * 2 + 0xd0) == 0)) {
    if (param_3 != 4) {
      FUN_22bf_07a5();
    }
    if (param_4 == 0) {
      uVar1 = 7;
    }
    else {
      uVar1 = 0xf;
    }
    FUN_5c52_0000(*(undefined2 *)(param_3 * 2 + 0x7f6),*(undefined2 *)(param_3 * 2 + 0x800),param_1,
                  param_2,uVar1,0,*(undefined2 *)(param_3 * 2 + 0x80a));
    if (param_3 != 4) {
      FUN_22bf_0757();
    }
    if (param_4 != 0) {
      FUN_7ecb_0142();
      if (param_3 != 4) {
        FUN_22bf_07a5();
      }
      FUN_5c52_0000(*(undefined2 *)(param_3 * 2 + 0x7f6),*(undefined2 *)(param_3 * 2 + 0x800),
                    param_1,param_2,7,0,*(undefined2 *)(param_3 * 2 + 0x80a));
      if (param_3 != 4) {
        FUN_22bf_0757();
      }
    }
  }
  return;
}


