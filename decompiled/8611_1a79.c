
void __cdecl16far FUN_8611_1a79(void)

{
  int iVar1;
  uint uVar2;
  int in_DX;
  int extraout_DX;
  int extraout_DX_00;
  int extraout_DX_01;
  int extraout_DX_02;
  int extraout_DX_03;
  int extraout_DX_04;
  int extraout_DX_05;
  undefined2 *puVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  int local_154 [162];
  int local_10;
  undefined2 *local_e;
  int local_a;
  int local_8;
  uint local_6;
  int local_4;
  
  local_e = &DAT_451b_1fc5;
  local_10 = 0;
  if (DAT_4125_2f1a != 0) {
    DAT_4125_2f1a = 0;
    FUN_1997_034a();
    FUN_1997_034a();
    for (local_6 = 0; (int)local_6 < 9; local_6 = local_6 + 1) {
      FUN_1997_034a();
    }
    FUN_1997_034a();
    FUN_1997_034a();
  }
  uVar6 = 0x1997;
  FUN_1997_034a();
  if (((opt_hidden_map != 0) && (DAT_4125_1625 != 0)) && ((DAT_4125_1650 & 10) == 0)) {
    for (iVar4 = 0; iVar4 < (int)(uint)DAT_4125_164f; iVar4 = iVar4 + 1) {
      uVar6 = 0x1d28;
      uVar5 = mem_lock(((undefined4 *)&DAT_4125_1625)[iVar4]);
      ((undefined2 *)&DAT_4125_1637)[iVar4 * 2] = in_DX;
      ((undefined2 *)&DAT_4125_1635)[iVar4 * 2] = uVar5;
    }
  }
  for (iVar4 = 0; iVar4 < 9; iVar4 = iVar4 + 1) {
    for (local_4 = 0; local_4 < 9; local_4 = local_4 + 1) {
      uVar5 = (undefined2)((ulong)local_e >> 0x10);
      if ((*(byte *)((undefined2 *)local_e + iVar4 * 0x24 + local_4 * 4 + 0xb) & 1) != 0) {
        if ((*(byte *)((undefined2 *)local_e + iVar4 * 0x24 + local_4 * 4 + 0xd) & 0x1f) == 0xe) {
          uVar6 = 0x152a;
          FUN_152a_0009(0x161b,0x4125,0,0x6795,0x4125);
        }
        else {
          local_154[local_10 * 2] = iVar4;
          local_154[local_10 * 2 + 1] = local_4;
          local_10 = local_10 + 1;
        }
      }
    }
  }
  if (((opt_hidden_map != 0) && (DAT_4125_1625 != 0)) && ((DAT_4125_1650 & 10) == 0)) {
    for (iVar4 = 0; iVar4 < (int)(uint)DAT_4125_164f; iVar4 = iVar4 + 1) {
      uVar6 = 0x1d28;
      mem_unlock(((undefined4 *)&DAT_4125_1625)[iVar4]);
    }
  }
  if ((DAT_451b_11dd != 0) && ((DAT_451b_1208 & 10) == 0)) {
    for (iVar4 = 0; iVar4 < (int)(uint)DAT_451b_1207; iVar4 = iVar4 + 1) {
      uVar6 = 0x1d28;
      uVar5 = mem_lock(((undefined4 *)&DAT_451b_11dd)[iVar4]);
      ((undefined2 *)&DAT_451b_11ef)[iVar4 * 2] = in_DX;
      ((undefined2 *)&DAT_451b_11ed)[iVar4 * 2] = uVar5;
    }
  }
  for (iVar4 = 0; iVar4 < local_10; iVar4 = iVar4 + 1) {
    local_4 = local_154[iVar4 * 2 + 1];
    uVar5 = (undefined2)((ulong)local_e >> 0x10);
    if ((*(byte *)((undefined2 *)local_e + local_154[iVar4 * 2] * 0x24 + local_4 * 4 + 0xb) >> 1 & 1
        ) == 0) {
      uVar2 = (uint)((undefined2 *)local_e)[local_154[iVar4 * 2] * 0x24 + local_4 * 4 + 0xb] >> 2 &
              0xff;
      in_DX = (uVar2 % 0x10) * 0x28;
      uVar6 = 0x152a;
      FUN_152a_0009(0x5133,0x4125,in_DX,(int)(((ulong)uVar2 % 0x60) / 0x10) * 0x28,0x6795,0x4125);
    }
  }
  if ((DAT_451b_11dd != 0) && ((DAT_451b_1208 & 10) == 0)) {
    for (iVar4 = 0; iVar4 < (int)(uint)DAT_451b_1207; iVar4 = iVar4 + 1) {
      uVar6 = 0x1d28;
      mem_unlock(((undefined4 *)&DAT_451b_11dd)[iVar4]);
    }
  }
  if ((*(byte *)((undefined2 *)local_e + 0x14f) >> 1 & 1) != 0) {
    if ((DAT_451b_1218 != 0) && ((DAT_451b_1243 & 10) == 0)) {
      for (iVar4 = 0; iVar4 < (int)(uint)DAT_451b_1242; iVar4 = iVar4 + 1) {
        uVar6 = 0x1d28;
        uVar5 = mem_lock(((undefined4 *)&DAT_451b_1218)[iVar4]);
        ((undefined2 *)&DAT_451b_122a)[iVar4 * 2] = in_DX;
        ((undefined2 *)&DAT_451b_1228)[iVar4 * 2] = uVar5;
      }
    }
    for (iVar4 = 0; iVar4 < local_10; iVar4 = iVar4 + 1) {
      iVar1 = local_154[iVar4 * 2];
      local_4 = local_154[iVar4 * 2 + 1];
      uVar5 = (undefined2)((ulong)local_e >> 0x10);
      if ((*(byte *)((undefined2 *)local_e + iVar1 * 0x24 + local_4 * 4 + 0xb) >> 1 & 1) != 0) {
        uVar2 = (uint)((undefined2 *)local_e)[iVar1 * 0x24 + local_4 * 4 + 0xb] >> 2 & 0xff;
        in_DX = (uVar2 % 0x10) * 0x28;
        uVar6 = 0x152a;
        FUN_152a_0009(0x516e,0x4125,in_DX,(int)(((ulong)uVar2 % 0x60) / 0x10) * 0x28,0x6795,0x4125,
                      ((undefined2 *)&DAT_4125_2d2a)[iVar1],*(undefined2 *)(local_4 * 2 + 0x2d3c));
      }
    }
    if ((DAT_451b_1218 != 0) && ((DAT_451b_1243 & 10) == 0)) {
      for (iVar4 = 0; iVar4 < (int)(uint)DAT_451b_1242; iVar4 = iVar4 + 1) {
        uVar6 = 0x1d28;
        mem_unlock(((undefined4 *)&DAT_451b_1218)[iVar4]);
      }
    }
  }
  if ((*(byte *)((undefined2 *)local_e + 0x14f) & 8) != 0) {
    for (iVar4 = 0; iVar4 < local_10; iVar4 = iVar4 + 1) {
      iVar1 = local_154[iVar4 * 2];
      local_4 = local_154[iVar4 * 2 + 1];
      uVar5 = (undefined2)((ulong)local_e >> 0x10);
      if ((*(byte *)((undefined2 *)local_e + iVar1 * 0x24 + local_4 * 4 + 0xd) & 0x20) != 0) {
        uVar2 = (uint)((undefined2 *)local_e)[iVar1 * 0x24 + local_4 * 4 + 0xd] >> 6 & 0x1f;
        local_8 = (uVar2 / 0xd) * 0x28;
        local_a = (uVar2 % 0xd) * 0x30;
        (*DAT_451b_2a68)(uVar6,0x52d0,0x4125,local_a,local_8,0x280030,0x6795,0x4125,
                         ((undefined2 *)&DAT_4125_2d2a)[iVar1],*(undefined2 *)(local_4 * 2 + 0x2d3c)
                         ,0x5295,0x4125,local_a,local_8);
        in_DX = extraout_DX;
      }
    }
  }
  if ((*(byte *)((undefined2 *)local_e + 0x14f) & 0x10) != 0) {
    for (iVar4 = 0; iVar4 < local_10; iVar4 = iVar4 + 1) {
      local_4 = local_154[iVar4 * 2 + 1];
      if ((*(byte *)((int)(undefined2 *)local_e + local_4 * 8 + local_154[iVar4 * 2] * 0x48 + 0x19)
          & 0x20) != 0) {
        (*DAT_451b_2a68)(uVar6,0x57a7,0x4125,0,0x1d0020,0x6795,0x4125,
                         ((undefined2 *)&DAT_4125_2d2a)[local_154[iVar4 * 2]],
                         *(undefined2 *)(local_4 * 2 + 0x2d3c),0x576c,0x4125,0);
        in_DX = extraout_DX_00;
      }
    }
  }
  if ((*(byte *)((undefined2 *)local_e + 0x14f) & 4) != 0) {
    for (iVar4 = 0; iVar4 < local_10; iVar4 = iVar4 + 1) {
      iVar1 = local_154[iVar4 * 2];
      local_4 = local_154[iVar4 * 2 + 1];
      uVar5 = (undefined2)((ulong)local_e >> 0x10);
      if ((*(byte *)((int)(undefined2 *)local_e + local_4 * 8 + iVar1 * 0x48 + 0x1b) & 8) != 0) {
        uVar2 = (uint)(*(byte *)((undefined2 *)local_e + iVar1 * 0x24 + local_4 * 4 + 0xc) >> 4);
        (*DAT_451b_2a68)(uVar6,0x52d0,0x4125,uVar2 * 0x30 + 0xc0,0x28,0x280030,0x6795,0x4125,
                         ((undefined2 *)&DAT_4125_2d2a)[iVar1],*(undefined2 *)(local_4 * 2 + 0x2d3c)
                         ,0x5295,0x4125,uVar2 * 0x30 + 0xc0,0x28);
        in_DX = extraout_DX_01;
      }
    }
  }
  if ((*(byte *)((undefined2 *)local_e + 0x14f) & 0x20) != 0) {
    for (iVar4 = 0; iVar4 < local_10; iVar4 = iVar4 + 1) {
      local_4 = local_154[iVar4 * 2 + 1];
      if ((*(byte *)((int)(undefined2 *)local_e + local_4 * 8 + local_154[iVar4 * 2] * 0x48 + 0x19)
          & 0x40) != 0) {
        (*DAT_451b_2a68)(uVar6,0x57a7,0x4125,0x40,0x1d0020,0x6795,0x4125,
                         ((undefined2 *)&DAT_4125_2d2a)[local_154[iVar4 * 2]],
                         *(undefined2 *)(local_4 * 2 + 0x2d3c),0x576c,0x4125,0x40);
        in_DX = extraout_DX_02;
      }
    }
  }
  if ((*(byte *)((undefined2 *)local_e + 0x14f) & 0x40) != 0) {
    for (iVar4 = 0; iVar4 < local_10; iVar4 = iVar4 + 1) {
      iVar1 = local_154[iVar4 * 2];
      local_4 = local_154[iVar4 * 2 + 1];
      uVar5 = (undefined2)((ulong)local_e >> 0x10);
      if ((*(byte *)((int)(undefined2 *)local_e + local_4 * 8 + iVar1 * 0x48 + 0x19) & 1) != 0) {
        (*DAT_451b_2a68)(uVar6,(*(byte *)((int)(undefined2 *)local_e +
                                         local_4 * 8 + iVar1 * 0x48 + 0x19) >> 1 & 0xf) * 0x3b +
                               0x4f20,0x4125,0x1d01a0,0x1d0020,0x6795,0x4125,
                         ((undefined2 *)&DAT_4125_2d2a)[iVar1],*(undefined2 *)(local_4 * 2 + 0x2d3c)
                         ,0x4ee5,0x4125,0x1d01a0);
        in_DX = extraout_DX_03;
      }
    }
  }
  local_6 = 0;
  while( true ) {
    uVar5 = (undefined2)((ulong)local_e >> 0x10);
    if (8 < (int)local_6) break;
    if (((undefined2 *)local_e)[local_6 + 2] != 0) {
      for (iVar4 = 0; iVar4 < local_10; iVar4 = iVar4 + 1) {
        iVar1 = local_154[iVar4 * 2];
        local_4 = local_154[iVar4 * 2 + 1];
        uVar5 = (undefined2)((ulong)local_e >> 0x10);
        puVar3 = (undefined2 *)local_e;
        if (((((*(byte *)((int)puVar3 + local_4 * 8 + iVar1 * 0x48 + 0x1b) & 8) == 0) ||
             ((*(byte *)(puVar3 + iVar1 * 0x24 + local_4 * 4 + 0xe) & 0x10) != 0)) &&
            ((*(uint *)((int)puVar3 + local_4 * 8 + iVar1 * 0x48 + 0x17) >> 7 & 0x1f) != 0)) &&
           (*(byte *)(puVar3 + iVar1 * 0x24 + local_4 * 4 + 0xc) >> 4 == local_6)) {
          FUN_8611_0335(0x6795,0x4125,local_6,
                        (*(byte *)((int)puVar3 + local_4 * 8 + iVar1 * 0x48 + 0x17) & 0x7c) >> 2,
                        *(uint *)((int)puVar3 + local_4 * 8 + iVar1 * 0x48 + 0x17) >> 7 & 0x1f);
        }
      }
    }
    local_6 = local_6 + 1;
  }
  if (*(char *)((undefined2 *)local_e + 0x14f) < '\0') {
    for (iVar4 = 0; iVar4 < local_10; iVar4 = iVar4 + 1) {
      iVar1 = local_154[iVar4 * 2];
      local_4 = local_154[iVar4 * 2 + 1];
      uVar5 = (undefined2)((ulong)local_e >> 0x10);
      if ((*(byte *)((int)(undefined2 *)local_e + local_4 * 8 + iVar1 * 0x48 + 0x1b) & 0x30) != 0) {
        local_a = 0x1f0;
        if ((*(byte *)((int)(undefined2 *)local_e + local_4 * 8 + iVar1 * 0x48 + 0x1b) & 0x30) ==
            0x10) {
          local_8 = 0x1f;
        }
        else {
          local_8 = 0x30;
        }
        (*DAT_451b_2a68)(uVar6,0x525a,0x4125,0x1f0,local_8,0xe0010,0x6795,0x4125,
                         ((undefined2 *)&DAT_4125_2d2a)[iVar1] + 0x10,
                         *(int *)(local_4 * 2 + 0x2d3c) + 0xd,0x521f,0x4125,0x1f0,local_8);
        in_DX = extraout_DX_04;
      }
    }
  }
  uVar5 = (undefined2)((ulong)local_e >> 0x10);
  puVar3 = (undefined2 *)local_e;
  if ((*(byte *)((int)puVar3 + 0x29f) & 1) != 0) {
    local_a = (int)puVar3[0x152] % 0x10 << 5;
    local_8 = ((int)puVar3[0x152] / 0x10) * 0x1d;
    (*DAT_451b_2a68)(uVar6,0x525a,0x4125,local_a,local_8,0x1d0020,0x6795,0x4125,
                     ((undefined2 *)&DAT_4125_2d2a)[puVar3[0x150]] + 8,
                     *(int *)(puVar3[0x151] * 2 + 0x2d3c) + 5,0x521f,0x4125,local_a,local_8);
    in_DX = extraout_DX_05;
  }
  if (opt_hidden_map != 0) {
    if ((DAT_4125_1625 != 0) && ((DAT_4125_1650 & 10) == 0)) {
      for (iVar4 = 0; iVar4 < (int)(uint)DAT_4125_164f; iVar4 = iVar4 + 1) {
        uVar6 = mem_lock(((undefined4 *)&DAT_4125_1625)[iVar4]);
        ((undefined2 *)&DAT_4125_1637)[iVar4 * 2] = in_DX;
        ((undefined2 *)&DAT_4125_1635)[iVar4 * 2] = uVar6;
      }
    }
    if ((DAT_451b_15c8 != 0) && ((DAT_451b_15f3 & 10) == 0)) {
      for (iVar4 = 0; iVar4 < (int)(uint)DAT_451b_15f2; iVar4 = iVar4 + 1) {
        uVar6 = mem_lock(((undefined4 *)&DAT_451b_15c8)[iVar4]);
        ((undefined2 *)&DAT_451b_15da)[iVar4 * 2] = in_DX;
        ((undefined2 *)&DAT_451b_15d8)[iVar4 * 2] = uVar6;
      }
    }
    for (iVar4 = 0; iVar4 < local_10; iVar4 = iVar4 + 1) {
      iVar1 = local_154[iVar4 * 2];
      local_4 = local_154[iVar4 * 2 + 1];
      uVar6 = (undefined2)((ulong)local_e >> 0x10);
      if (*(char *)((int)(undefined2 *)local_e + local_4 * 8 + iVar1 * 0x48 + 0x19) < '\0') {
        uVar2 = *(byte *)((undefined2 *)local_e + iVar1 * 0x24 + local_4 * 4 + 0xd) & 0x1f;
        FUN_152a_017f(0x161b,0x4125,0,0x6795,0x4125,((undefined2 *)&DAT_4125_2d2a)[iVar1],
                      *(undefined2 *)(local_4 * 2 + 0x2d3c),0x551e,0x4125,(uVar2 % 7) * 0x30,
                      (uVar2 / 7) * 0x29);
      }
    }
    if ((DAT_451b_15c8 != 0) && ((DAT_451b_15f3 & 10) == 0)) {
      for (iVar4 = 0; iVar4 < (int)(uint)DAT_451b_15f2; iVar4 = iVar4 + 1) {
        mem_unlock(((undefined4 *)&DAT_451b_15c8)[iVar4]);
      }
    }
    if ((DAT_4125_1625 != 0) && ((DAT_4125_1650 & 10) == 0)) {
      for (iVar4 = 0; iVar4 < (int)(uint)DAT_4125_164f; iVar4 = iVar4 + 1) {
        mem_unlock(((undefined4 *)&DAT_4125_1625)[iVar4]);
      }
    }
  }
  return;
}


