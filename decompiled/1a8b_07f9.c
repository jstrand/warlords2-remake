
int __cdecl16far walk_path(int *param_1)

{
  int *piVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  int *piVar5;
  int *piVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 unaff_SS;
  byte local_7c [100];
  uint local_18;
  int local_16;
  char local_13;
  int local_12;
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  uVar8 = (undefined2)((ulong)param_1 >> 0x10);
  piVar6 = (int *)param_1;
  local_4 = *param_1;
  local_6 = piVar6[1];
  local_8 = 0;
  local_a = 0;
  local_c = 0;
  local_10 = DAT_451b_1ea0;
  local_e = DAT_451b_1e9e;
  if (DAT_4125_2bda == 0) {
    local_e = 1;
    local_10 = (int)*(char *)((int)DAT_451b_1f02 + 7);
  }
  DAT_451b_1e64 = 0;
  DAT_4125_2eaa = path_step_costs(param_1,local_10,local_7c);
  local_12 = DAT_4125_2eaa;
  iVar7 = 0;
  do {
    if (local_12 <= iVar7) {
LAB_1a8b_09d1:
      if (local_8 != 0) {
        local_18 = (uint)local_7c[local_8 + -1];
        place_stack_at_step(piVar6,uVar8,local_8);
        if (DAT_4125_2bda == 0) {
          uVar8 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
          local_13 = *(char *)((int)DAT_451b_1f02 + 7);
          if ((int)local_13 < (int)local_18) {
            local_13 = '\0';
          }
          else {
            local_13 = local_13 - (char)local_18;
          }
          *(char *)((int)DAT_451b_1f02 + 7) = local_13;
        }
        else {
          local_16 = DAT_451b_1ea0 - local_18;
          if (DAT_451b_1ea0 < (int)local_18) {
            local_16 = 0;
          }
          iVar7 = 8;
          piVar6 = (int *)&DAT_451b_1efe;
          DAT_451b_1ea0 = local_16;
          while( true ) {
            piVar5 = piVar6 + -2;
            iVar3 = iVar7 + -1;
            if (iVar7 == 0) break;
            piVar1 = piVar6 + -1;
            iVar7 = iVar3;
            piVar6 = piVar5;
            if (*piVar5 != 0 || *piVar1 != 0) {
              if ((int)*(char *)((int)*(undefined4 *)piVar5 + 7) < (int)local_18) {
                cVar2 = '\0';
              }
              else {
                cVar2 = *(char *)((int)*(undefined4 *)piVar5 + 7) - (char)local_18;
              }
              *(char *)((int)*(undefined4 *)piVar5 + 7) = cVar2;
            }
          }
        }
      }
      iVar7 = DAT_451b_1ea0;
      if (DAT_4125_2bda == 0) {
        iVar7 = (int)*(char *)((int)DAT_451b_1f02 + 7);
      }
      if ((iVar7 < 2) ||
         (((local_8 != 0 && (local_c != 5)) &&
          ((local_c != 3 && ((local_7c[local_8] != 0xff && (iVar7 < (int)(uint)local_7c[local_8]))))
          )))) {
        local_c = 2;
      }
      return local_c;
    }
    if ((*(char *)((int)*(undefined4 *)(piVar6 + 6) + iVar7) == -1) ||
       (iVar3 = FUN_1a8b_0b4d(&local_4), iVar3 == 0)) {
      local_c = 4;
      goto LAB_1a8b_09d1;
    }
    iVar3 = local_6 * 0xe0;
    uVar4 = *(byte *)(iVar3 + local_4 * 2 + 1) & 0xf;
    if ((*(char *)(*(byte *)(iVar3 + local_4 * 2) + 0x710) == '\n') && (current_player != uVar4)) {
      local_c = 5;
      DAT_451b_1e68 = local_4;
      DAT_451b_1e66 = local_6;
      goto LAB_1a8b_09d1;
    }
    if ((((*(byte *)(iVar3 + local_4 * 2 + 1) & 0x10) != 0) && (current_player != uVar4)) &&
       ((*(byte *)(current_player * 8 + uVar4 + 0x153b) & 3) != 0)) {
      local_c = 3;
      DAT_451b_1e68 = local_4;
      DAT_451b_1e66 = local_6;
      goto LAB_1a8b_09d1;
    }
    armies_at_tile(local_4,local_6,(int *)CONCAT22(unaff_SS,&local_18));
    if (((int)(local_e + local_18) < 9) &&
       (((*(byte *)(local_6 * 0xe0 + local_4 * 2 + 1) & 0x10) == 0 || (current_player == uVar4)))) {
      local_8 = local_8 + local_a + 1;
      local_a = 0;
    }
    else {
      local_a = local_a + 1;
    }
    iVar7 = iVar7 + 1;
  } while( true );
}


