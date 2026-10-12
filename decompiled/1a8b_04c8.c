
void __cdecl16far place_stack_at_step(int *param_1,int param_2)

{
  int *piVar1;
  uint *puVar2;
  byte *pbVar3;
  char cVar4;
  int *piVar5;
  bool bVar6;
  byte bVar7;
  ulong uVar8;
  int iVar9;
  int iVar10;
  clock_t *pcVar11;
  int iVar12;
  int iVar13;
  int *piVar14;
  ulong in_EDX;
  int *piVar16;
  int iVar17;
  clock_t *unaff_DI;
  undefined2 uVar18;
  long local_12;
  undefined4 local_e;
  undefined4 local_a;
  int local_6;
  int local_4;
  int iVar15;
  
  uVar18 = (undefined2)((ulong)param_1 >> 0x10);
  piVar16 = (int *)param_1;
  local_4 = *param_1;
  local_6 = piVar16[1];
  FUN_22bf_07a5();
  iVar17 = 0;
  if (0 < param_2) {
    do {
      iVar9 = local_4;
      iVar13 = local_6;
      iVar10 = FUN_7ecb_0a7c();
      if (iVar10 == 0) {
        _bios_timeofday(0,&local_a);
      }
      else {
        local_a = 0;
      }
      local_12 = 0;
      FUN_1a8b_0b4d(&local_4);
      if (DAT_4125_2bda != 0) {
        in_EDX = 0x5e5e;
        iVar10 = 8;
        while( true ) {
          iVar15 = (int)in_EDX;
          piVar14 = (int *)(iVar15 + -4);
          in_EDX = ZEXT24(piVar14);
          iVar12 = iVar10 + -1;
          if (iVar10 == 0) break;
          iVar10 = iVar12;
          if (*piVar14 != 0 || *(int *)(iVar15 + -2) != 0) {
            piVar5 = *(int **)piVar14;
            *piVar5 = local_4;
            *(int *)(*piVar14 + 2) = local_6;
          }
        }
      }
      uVar8 = (ulong)DAT_451b_1f02 >> 0x10;
      piVar14 = (int *)DAT_451b_1f02;
      *DAT_451b_1f02 = local_4;
      piVar14[1] = local_6;
      iVar10 = is_computer_turn();
      if (iVar10 != 0) {
        FUN_6087_0427(local_4,local_6);
      }
      FUN_1b62_01a7(iVar9,iVar13);
      DAT_4125_2ea8 = DAT_4125_2ea8 + 1;
      FUN_1b62_0e9b(iVar9,iVar13);
      iVar13 = FUN_8611_1298(current_player,*DAT_451b_1f02,((int *)DAT_451b_1f02)[1]);
      if (iVar13 != 0) {
        FUN_834b_1fed(*DAT_451b_1f02,((int *)DAT_451b_1f02)[1],1);
      }
      FUN_8611_0565();
      pcVar11 = clock(unaff_DI);
      if ((ZEXT24(pcVar11) | in_EDX << 0x10) != 0xffffffff) {
        while( true ) {
          FUN_251d_008a();
          iVar13 = FUN_7ecb_0a7c();
          if (iVar13 == 0) {
            _bios_timeofday(0,&local_e);
          }
          else {
            local_e = 1000;
          }
          if ((((long)local_e < local_a) || (2 < (long)(local_e - local_a))) || (10000 < local_12))
          break;
          local_12 = local_12 + 1;
        }
      }
      iVar17 = iVar17 + 1;
    } while (iVar17 < param_2);
  }
  if (piVar16[5] == 1) {
    local_a = CONCAT22(*DAT_451b_1f02,((int *)DAT_451b_1f02)[1]);
    cVar4 = *(char *)(*(byte *)(((int *)DAT_451b_1f02)[1] * 0xe0 + *DAT_451b_1f02 * 2) + 0x710);
    if ((piVar16[4] & 8U) == 0) {
      bVar6 = false;
      local_e = local_e & 0xffff;
      if ((cVar4 == '\x02') || (cVar4 == '\x03')) {
        iVar17 = 8;
        piVar16 = (int *)&DAT_451b_1efe;
        while( true ) {
          piVar14 = piVar16 + -2;
          iVar13 = iVar17 + -1;
          if (iVar17 == 0) break;
          piVar1 = piVar16 + -1;
          iVar17 = iVar13;
          piVar16 = piVar14;
          if (*piVar14 != 0 || *piVar1 != 0) {
            if (((undefined1 *)&army_move_flags)[*(char *)((int)*(undefined4 *)piVar14 + 4) * 6] ==
                '\0') {
              if (*(char *)((int)*(undefined4 *)piVar14 + 4) != '\x1c') {
                local_e = CONCAT22(1,(undefined2)local_e);
              }
            }
            else {
              bVar6 = true;
            }
          }
        }
        if (local_e._2_2_ != 0) {
          bVar6 = false;
        }
        iVar17 = 8;
        piVar16 = (int *)&DAT_451b_1efe;
        while( true ) {
          piVar14 = piVar16 + -2;
          iVar13 = iVar17 + -1;
          if (iVar17 == 0) break;
          piVar1 = piVar16 + -1;
          iVar17 = iVar13;
          piVar16 = piVar14;
          if (((*piVar14 != 0 || *piVar1 != 0) &&
              (uVar18 = (undefined2)((ulong)*(undefined4 *)piVar14 >> 0x10),
              ((undefined1 *)&army_move_flags)[*(char *)((int)*(undefined4 *)piVar14 + 4) * 6] ==
              '\0')) && ((*(char *)(*piVar14 + 4) != '\x1c' || (!bVar6)))) {
            puVar2 = (uint *)((int)*(undefined4 *)piVar14 + 0xc);
            *puVar2 = *puVar2 | 0x1000;
            DAT_451b_1e64 = 1;
          }
        }
      }
    }
    else if (((cVar4 != '\x02') && (cVar4 != '\x03')) && (cVar4 != '\x01')) {
      iVar17 = 8;
      piVar16 = (int *)&DAT_451b_1efe;
      while( true ) {
        piVar14 = piVar16 + -2;
        iVar13 = iVar17 + -1;
        if (iVar17 == 0) break;
        piVar1 = piVar16 + -1;
        iVar17 = iVar13;
        piVar16 = piVar14;
        if (*piVar14 != 0 || *piVar1 != 0) {
          puVar2 = (uint *)((int)*(undefined4 *)piVar14 + 0xc);
          *puVar2 = *puVar2 & 0xefff;
          DAT_451b_1e64 = 2;
        }
      }
    }
  }
  pbVar3 = (byte *)(((int *)DAT_451b_1f02)[1] * 0xe0 + *DAT_451b_1f02 * 2 + 1);
  *pbVar3 = *pbVar3 | 0x10;
  bVar7 = (byte)current_player;
  iVar17 = ((int *)DAT_451b_1f02)[1] * 0xe0 + *DAT_451b_1f02 * 2;
  pbVar3 = (byte *)(iVar17 + 1);
  *pbVar3 = *pbVar3 & 0xf0;
  pbVar3 = (byte *)(iVar17 + 1);
  *pbVar3 = *pbVar3 | bVar7 & 0xf;
  FUN_22bf_0757();
  return;
}


