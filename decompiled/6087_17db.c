
void __cdecl16far FUN_6087_17db(undefined4 param_1)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  char cVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 unaff_SS;
  char local_e0 [176];
  int local_30;
  char local_2e [8];
  char local_26 [8];
  char local_1e [8];
  char local_16 [8];
  char local_e [8];
  int local_6;
  int local_4;
  
  for (iVar7 = 0; iVar7 < 8; iVar7 = iVar7 + 1) {
    local_2e[iVar7] = '\0';
    local_26[iVar7] = '\0';
    local_1e[iVar7] = '\0';
    local_16[iVar7] = '\0';
    local_e[iVar7] = '\0';
    iVar5 = 0x16;
    while (iVar5 != 0) {
      local_e0[iVar7 * 0x16 + iVar5 + -1] = -1;
      iVar5 = iVar5 + -1;
    }
  }
  iVar7 = 0;
  while( true ) {
    iVar5 = (int)param_1;
    uVar8 = (undefined2)((ulong)param_1 >> 0x10);
    if (7 < iVar7) break;
    local_4 = *(int *)(iVar5 + iVar7 * 2);
    if (local_4 != -1) {
      list_carried_items(2,0,local_4 * 0x16 + -0x7780,0x2e1d);
      for (iVar5 = 0; iVar5 < DAT_451b_08ce; iVar5 = iVar5 + 1) {
        local_e0[iVar7 * 0x16 + iVar5] = *(char *)((undefined2 *)&DAT_451b_0c9c + iVar5);
        uVar2 = (int)*(char *)(((undefined2 *)&DAT_451b_0c9c)[iVar5] * 0x1d + 0xcfd) - 1;
        if (uVar2 < 8) {
                    /* WARNING: Switch is manually overridden */
          switch((undefined4)*(undefined2 *)(uVar2 * 2 + 0x1a2d)) {
          case 0x62106:
            local_16[iVar7] = local_16[iVar7] + '\x01';
            break;
          case 0x6210b:
            local_26[iVar7] = local_26[iVar7] + '\x01';
            break;
          case 0x62110:
            local_1e[iVar7] = local_1e[iVar7] + '\x01';
            break;
          case 0x62115:
            local_2e[iVar7] = local_2e[iVar7] + '\x01';
          }
        }
        local_e[iVar7] = local_e[iVar7] + '\x01';
      }
    }
    iVar7 = iVar7 + 1;
  }
  FUN_6087_1a3d(param_1,local_1e);
  FUN_6087_1a3d(param_1,local_26);
  FUN_6087_1a3d(param_1,local_2e);
  local_6 = 0;
  do {
    if (local_6 != 0) {
      return;
    }
    local_6 = 1;
    for (iVar7 = 0; iVar7 < 8; iVar7 = iVar7 + 1) {
      if ((*(int *)(iVar5 + iVar7 * 2) != -1) && (local_16[iVar7] != '\0')) {
        cVar4 = local_16[iVar7];
        local_30 = -1;
        for (iVar6 = 0; iVar6 < 8; iVar6 = iVar6 + 1) {
          if (((iVar7 != iVar6) && (*(int *)(iVar5 + iVar6 * 2) != -1)) && (local_16[iVar6] < cVar4)
             ) {
            cVar4 = local_16[iVar6];
            local_30 = iVar6;
          }
        }
        if (local_30 != -1) {
          for (iVar6 = 0; iVar6 < 0x16; iVar6 = iVar6 + 1) {
            iVar3 = (int)local_e0[iVar7 * 0x16 + iVar6];
            if ((iVar3 != 0xff) &&
               (((*(char *)(iVar3 * 0x1d + 0xcfd) == '\x01' ||
                 (*(char *)(iVar3 * 0x1d + 0xcfd) == '\x02')) ||
                (*(char *)(iVar3 * 0x1d + 0xcfd) == '\b')))) {
              local_e0[iVar7 * 0x16 + iVar6] = -1;
              local_6 = 0;
              piVar1 = (int *)((int)DAT_451b_018e + 0x4c);
              *piVar1 = *piVar1 + 1;
              *(undefined2 *)(iVar3 * 0x1d + 0xd00) = *(undefined2 *)(iVar5 + local_30 * 2);
              local_16[iVar7] = local_16[iVar7] + -1;
              local_16[local_30] = local_16[local_30] + '\x01';
              break;
            }
          }
        }
      }
    }
  } while( true );
}


