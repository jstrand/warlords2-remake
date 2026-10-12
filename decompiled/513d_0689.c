
void __cdecl16far FUN_513d_0689(void)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int unaff_DI;
  undefined2 uVar4;
  undefined2 unaff_SS;
  int local_1e [8];
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  local_e = 0;
LAB_513d_0696:
  local_e = local_e + 1;
  for (local_4 = 0; local_4 < 8; local_4 = local_4 + 1) {
    local_1e[local_4] = 0;
    *(undefined2 *)(local_4 * 0x14 + 0x189) = 0xff9c;
    *(undefined2 *)(local_4 * 0x14 + 0x18b) = 0xff9c;
  }
  local_4 = 0;
  do {
    if (7 < local_4) {
      for (local_4 = 0; local_4 < 8; local_4 = local_4 + 1) {
        iVar2 = FUN_828e_044a(*(undefined2 *)(local_4 * 0x14 + 0x189),
                              *(undefined2 *)(local_4 * 0x14 + 0x18b));
        *(undefined1 *)(iVar2 * 0x41 + 0x1592) = (undefined1)local_4;
        FUN_513d_09b3(iVar2 * 0x41 + 0x157d);
      }
      return;
    }
    local_6 = 0;
    while (local_6 == 0) {
      local_a = dice(1,8,-1);
      if (local_1e[local_a] == 0) {
        local_6 = 1;
        local_1e[local_a] = 1;
      }
    }
    local_6 = 0;
    for (local_8 = 0; (local_6 == 0 && (local_8 < 100)); local_8 = local_8 + 1) {
      unaff_DI = FUN_513d_08f0();
      if ((0xb < *(int *)(unaff_DI * 0x41 + 0x157d)) &&
         (((*(int *)(unaff_DI * 0x41 + 0x157d) < 0x65 && (0xb < *(int *)(unaff_DI * 0x41 + 0x157f)))
          && (*(int *)(unaff_DI * 0x41 + 0x157f) < 0x91)))) {
        local_c = 1;
        for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
          uVar1 = *(int *)(unaff_DI * 0x41 + 0x157d) - *(int *)(iVar2 * 0x14 + 0x189);
          uVar3 = (int)uVar1 >> 0xf;
          if (((int)((uVar1 ^ uVar3) - uVar3) < 0x14) ||
             (uVar1 = *(int *)(unaff_DI * 0x41 + 0x157f) - *(int *)(iVar2 * 0x14 + 0x18b),
             uVar3 = (int)uVar1 >> 0xf, (int)((uVar1 ^ uVar3) - uVar3) < 0x14)) {
            local_c = 0;
            break;
          }
        }
        if (local_c != 0) {
          local_6 = 1;
        }
      }
    }
    if ((99 < local_8) && (local_e < 10)) goto LAB_513d_0696;
    uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    *(undefined2 *)((int)DAT_451b_002a + local_a * 2 + 0x1d72) =
         *(undefined2 *)((char *)s_Fight_Order_4125_1d58 + 10 + (int)DAT_451b_002a + local_4 * 2);
    *(undefined2 *)(local_4 * 0x14 + 0x189) = *(undefined2 *)(unaff_DI * 0x41 + 0x157d);
    *(undefined2 *)(local_4 * 0x14 + 0x18b) = *(undefined2 *)(unaff_DI * 0x41 + 0x157f);
    local_4 = local_4 + 1;
  } while( true );
}


