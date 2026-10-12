
undefined2 __cdecl16far ai_assault_attack(int param_1)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_SS;
  int local_1e [8];
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  iVar4 = *(int *)((int)DAT_451b_018e + param_1 * 0x5c + 0x250);
  local_6 = -1;
  local_8 = -1;
  local_a = FUN_623c_12c9(*(int *)(iVar4 * 0x41 + 0x157d) + 1,*(undefined2 *)(iVar4 * 0x41 + 0x157f)
                          ,local_1e);
  if (local_a != 0) {
    for (iVar4 = 0; iVar4 < 6; iVar4 = iVar4 + 1) {
      uVar5 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      local_e = *(int *)((int)DAT_451b_018e + param_1 * 0x5c + iVar4 * 2 + 0x25a);
      if (local_e != -1) {
        if (DAT_451b_1ea0 + -2 < 1) {
          iVar2 = 1;
        }
        else {
          iVar2 = DAT_451b_1ea0 + -2;
        }
        local_4 = *(int *)((int)DAT_451b_018e + param_1 * 0x5c + iVar4 * 2 + 0x272) / iVar2 + 1;
        local_c = FUN_623c_15c5(*(undefined2 *)(local_e * 0x41 + 0x157d),
                                *(undefined2 *)(local_e * 0x41 + 0x157f));
        iVar2 = dice(1,4,0);
        if (local_4 < 0xb) {
          iVar3 = 10 - local_4;
        }
        else {
          iVar3 = 0;
        }
        iVar2 = iVar3 + local_c + *(int *)((int)DAT_451b_018e + param_1 * 0x5c + iVar4 * 2 + 0x27e)
                + iVar2;
        if (local_4 == 1) {
          iVar2 = iVar2 + 100;
        }
        if (((0x4b < local_c) || (7 < local_a)) && (local_8 < iVar2)) {
          local_6 = local_e;
          local_8 = iVar2;
        }
      }
    }
    if (local_6 != -1) {
      FUN_563e_08b2(param_1,local_1e);
      iVar4 = 8;
      while (iVar2 = iVar4 + -1, iVar4 != 0) {
        iVar4 = iVar2;
        if (local_1e[iVar2] != -1) {
          iVar2 = local_1e[iVar2];
          pbVar1 = (byte *)(iVar2 * 0x16 + -0x7772);
          *pbVar1 = *pbVar1 & 0x8f;
          pbVar1 = (byte *)(iVar2 * 0x16 + -0x7772);
          *pbVar1 = *pbVar1 | ((char)param_1 + 1U & 7) << 4;
        }
      }
      FUN_563e_0f1b(param_1,local_1e);
      return 1;
    }
  }
  return 0;
}


