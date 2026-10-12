
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl16near move_stack_to(undefined2 param_1,int param_2,int param_3,int param_4)

{
  byte *pbVar1;
  byte bVar2;
  bool bVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  int iVar8;
  undefined2 uVar9;
  int local_a;
  uint local_6;
  
  bVar3 = true;
  iVar8 = 0;
  uVar9 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
  local_6 = (uint)(*(byte *)((int)(int *)DAT_451b_1f02 + 0xf) >> 1);
  bVar2 = *(byte *)((int *)DAT_451b_1f02 + 7);
  if ((((DAT_451b_1f02 == (int *)0x0) || (param_2 < 0)) || (param_3 < 0)) ||
     ((0x6f < param_2 || (0x9b < param_3)))) {
    return 0;
  }
  do {
    do {
      if (!bVar3) {
        if (param_4 == 0) {
          FUN_8c07_08fb();
          FUN_1b62_08b3(1,0x10001);
        }
        return local_a;
      }
      bVar3 = false;
      FUN_6087_0427(*DAT_451b_1f02,((int *)DAT_451b_1f02)[1]);
      local_a = move_stack_step(param_2);
      uVar9 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
      piVar6 = (int *)DAT_451b_1f02;
      if ((piVar6[9] == *DAT_451b_1f02) && (piVar6[1] == piVar6[10])) {
        local_a = 4;
        piVar6[10] = -1;
        piVar6[9] = -1;
      }
    } while (3 < local_a - 2U);
                    /* WARNING: Switch is manually overridden */
    switch((undefined4)*(undefined2 *)((local_a - 2U) * 2 + 0x24c)) {
    case 0x1a96b:
      FUN_8c07_08fb();
      break;
    case 0x1a973:
      if (*(char *)(*(byte *)(param_3 * 0xe0 + param_2 * 2) + 0x710) == '\n') {
        if (*(char *)(local_6 * 0x41 + 0x1592) != (char)current_player) {
          iVar8 = enter_or_attack_city(local_6,DAT_451b_1e68,DAT_451b_1e66,&param_2);
        }
        if (iVar8 != 0) {
          bVar3 = true;
          local_6 = (uint)(*(byte *)((int)(int *)DAT_451b_1f02 + 0xf) >> 1);
        }
      }
      break;
    case 0x1aa00:
      if (((bVar2 & 0xf) == 3) && (_DAT_451b_1efe != 0)) {
        iVar4 = army_count * 0x16 + -0x7780;
        iVar5 = army_count;
        do {
          iVar4 = iVar4 + -0x16;
          iVar7 = iVar5 + -1;
          if (iVar5 == 0) goto LAB_1a8b_01dc;
          iVar5 = iVar7;
        } while ((_DAT_451b_1f00 != 0x2e1d) || (iVar4 != DAT_451b_1efe));
        iVar5 = FUN_828e_04b5();
        if (iVar5 != -1) {
          FUN_5ad0_15c3();
        }
LAB_1a8b_01dc:
        piVar6 = (int *)&DAT_451b_1ede;
        do {
          if (*piVar6 != 0 || piVar6[1] != 0) {
            uVar9 = (undefined2)((ulong)*(undefined4 *)piVar6 >> 0x10);
            pbVar1 = (byte *)((int)*(undefined4 *)piVar6 + 0xe);
            *pbVar1 = *pbVar1 & 0xf0;
            *(byte *)(*piVar6 + 0xe) = *(byte *)(*piVar6 + 0xe) & 0x8f;
          }
          piVar6 = piVar6 + 2;
        } while (piVar6 != (int *)&DAT_451b_1efe);
      }
      break;
    case 0x1aab6:
      iVar8 = FUN_1a8b_0476();
      if (iVar8 != 0) {
        bVar3 = true;
      }
    }
  } while( true );
}


