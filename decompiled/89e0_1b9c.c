
void __cdecl16far FUN_89e0_1b9c(undefined4 param_1)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  undefined1 local_52 [32];
  int local_32;
  int local_30;
  int local_2e;
  int local_2c;
  int local_2a;
  int local_28;
  int local_26;
  int local_24;
  int local_14;
  int local_12;
  int local_10;
  int local_e;
  undefined4 local_c;
  int local_8;
  int local_6;
  int local_4;
  
  iVar6 = 0;
  local_e = 0;
  local_10 = 0;
  local_12 = 0;
  local_14 = 0;
  local_6 = combat_terrain_class(*DAT_451b_1f02,((int *)DAT_451b_1f02)[1]);
  for (iVar5 = 0; iVar5 < DAT_4125_30f6; iVar5 = iVar5 + 1) {
    if (((undefined2 *)&DAT_451b_28e2)[iVar5] != 0) {
      uVar7 = ((undefined2 *)&DAT_451b_2904)[iVar5 * 2];
      iVar3 = ((undefined2 *)&DAT_451b_2902)[iVar5 * 2];
      local_c = CONCAT22(uVar7,iVar3);
      local_4 = (int)*(char *)(iVar3 + 4);
      if ((*(uint *)(iVar3 + 0xc) & 0x1000) == 0) {
        if (local_4 != 0x1c) {
          armytype_lookup(local_4,local_52);
        }
        if (local_4 == 0x1c) {
          local_8 = hero_battle_items(local_c);
          uVar7 = (undefined2)((ulong)local_c >> 0x10);
          if (*(char *)((int)local_c + 8) + local_8 < 9) {
            local_8 = *(char *)((int)local_c + 8) + local_8;
          }
          else {
            local_8 = 9;
          }
          iVar3 = *(int *)(local_8 * 2 + 0xce4);
          iVar2 = hero_command_items(local_c);
          iVar6 = iVar6 + iVar3 + iVar2;
        }
        else if (((local_24 == 0) || (local_6 != 3)) || (local_24 <= local_e)) {
          if (((local_26 == 0) || (local_6 != 2)) || (local_26 <= local_10)) {
            if (((local_28 == 0) || (local_6 != 1)) || (local_28 <= local_12)) {
              if (((local_2a != 0) && (local_6 == 0)) && (local_14 < local_2a)) {
                iVar6 = iVar6 + (local_2a - local_14);
                local_14 = local_2a;
              }
            }
            else {
              iVar6 = iVar6 + (local_28 - local_12);
              local_12 = local_28;
            }
          }
          else {
            iVar6 = iVar6 + (local_26 - local_10);
            local_10 = local_26;
          }
        }
        else {
          iVar6 = iVar6 + (local_24 - local_e);
          local_e = local_24;
        }
      }
    }
  }
  if (combat_modifier_cap < iVar6) {
    iVar6 = combat_modifier_cap;
  }
  iVar5 = 0;
  do {
    if (DAT_4125_30f6 <= iVar5) {
      return;
    }
    if (((undefined2 *)&DAT_451b_28e2)[iVar5] != 0) {
      uVar7 = ((undefined2 *)&DAT_451b_2904)[iVar5 * 2];
      iVar3 = ((undefined2 *)&DAT_451b_2902)[iVar5 * 2];
      local_c = CONCAT22(uVar7,iVar3);
      local_4 = (int)*(char *)(iVar3 + 4);
      uVar8 = (undefined2)((ulong)param_1 >> 0x10);
      iVar2 = (int)param_1;
      *(int *)(iVar2 + iVar5 * 2) = (int)*(char *)(iVar3 + 8);
      if (local_4 != 0x1c) {
        armytype_lookup(local_4,local_52);
      }
      if ((*(uint *)((int)local_c + 0xc) & 0x1000) == 0) {
        if (local_4 == 0x1c) {
          iVar3 = hero_battle_items(local_c);
          piVar4 = (int *)(iVar2 + iVar5 * 2);
        }
        else if ((local_2c == 0) || (local_6 != 3)) {
          if ((local_2e == 0) || (local_6 != 2)) {
            if ((local_30 == 0) || (local_6 != 1)) {
              if ((local_32 == 0) || (local_6 != 0)) goto LAB_89e0_1e07;
              piVar4 = (int *)(iVar2 + iVar5 * 2);
              iVar3 = local_32;
            }
            else {
              piVar4 = (int *)(iVar2 + iVar5 * 2);
              iVar3 = local_30;
            }
          }
          else {
            piVar4 = (int *)(iVar2 + iVar5 * 2);
            iVar3 = local_2e;
          }
        }
        else {
          piVar4 = (int *)(iVar2 + iVar5 * 2);
          iVar3 = local_2c;
        }
        *piVar4 = *piVar4 + iVar3;
      }
      else {
        *(undefined2 *)(iVar2 + iVar5 * 2) = 4;
      }
LAB_89e0_1e07:
      piVar4 = (int *)(iVar2 + iVar5 * 2);
      if (*piVar4 < 9) {
        iVar3 = *piVar4;
      }
      else {
        iVar3 = 9;
      }
      piVar4 = (int *)(iVar2 + iVar5 * 2);
      *piVar4 = iVar3;
      piVar1 = piVar4;
      *piVar1 = *piVar1 + iVar6;
    }
    iVar5 = iVar5 + 1;
  } while( true );
}


