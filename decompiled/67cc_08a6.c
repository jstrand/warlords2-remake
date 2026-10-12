
int __cdecl16far combat_resolve(void)

{
  bool bVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int local_e;
  int local_c;
  uint local_6;
  
  local_6 = 1;
  iVar8 = 0;
  iVar7 = 0;
  combat_log_len = '\0';
  DAT_451b_0252 = 0;
  bVar2 = false;
  if ((combat_atk_count == '\0') || (combat_def_count == '\0')) {
    bVar2 = true;
    DAT_451b_0252 = 1;
  }
  do {
    if (bVar2) {
      return local_6;
    }
    bVar1 = false;
    local_e = 0;
    if ((char)((undefined1 *)&combat_def_str)[iVar8] < '\x01') {
      iVar3 = 1;
    }
    else {
      iVar3 = (int)(char)((undefined1 *)&combat_def_str)[iVar8];
    }
    if ((char)((undefined1 *)&combat_atk_str)[iVar7] < '\x01') {
      iVar4 = 1;
    }
    else {
      iVar4 = (int)(char)((undefined1 *)&combat_atk_str)[iVar7];
    }
    while (!bVar1) {
      if (opt_intense_combat == 0) {
        iVar5 = 0x14;
      }
      else {
        iVar5 = 0x18;
      }
      local_c = dice(1,iVar5,0);
      if (opt_intense_combat == 0) {
        iVar5 = 0x14;
      }
      else {
        iVar5 = 0x18;
      }
      iVar6 = dice(1,iVar5,0);
      iVar5 = local_e + 1;
      if (10000 < local_e) {
        local_c = 0;
        iVar6 = 100;
      }
      local_e = iVar5;
      if (((iVar3 < local_c) || (iVar4 < iVar6)) && ((local_c <= iVar3 || (iVar6 <= iVar4)))) {
        if (((iVar3 < local_c) || (iVar6 <= iVar4)) ||
           ((opt_tutorial != 0 &&
            (((((undefined1 *)&combat_atk_type)[iVar7] == '\x1c' &&
              (*(int *)(DAT_451b_029a * 2 + 0xd0) == 0)) && (DAT_451b_029b == '\x0f')))))) {
          if (((iVar3 < local_c) && (iVar6 <= iVar4)) &&
             (((undefined1 *)&combat_def_hits)[iVar8] = ((undefined1 *)&combat_def_hits)[iVar8] + -1
             , (char)((undefined1 *)&combat_def_hits)[iVar8] < '\0')) {
            ((undefined1 *)&combat_def_alive)[iVar8] = 0;
            iVar8 = iVar8 + 1;
            ((undefined1 *)&combat_log)[combat_log_len] = 0;
            goto LAB_67cc_0a20;
          }
        }
        else {
          ((undefined1 *)&combat_atk_hits)[iVar7] = ((undefined1 *)&combat_atk_hits)[iVar7] + -1;
          if ((char)((undefined1 *)&combat_atk_hits)[iVar7] < '\0') {
            ((undefined1 *)&combat_atk_alive)[iVar7] = 0;
            iVar7 = iVar7 + 1;
            ((undefined1 *)&combat_log)[combat_log_len] = 1;
LAB_67cc_0a20:
            bVar1 = true;
            combat_log_len = combat_log_len + '\x01';
          }
        }
        if ((combat_def_count == iVar8) || (combat_atk_count == iVar7)) {
          bVar2 = true;
          local_6 = (uint)(combat_def_count == iVar8);
        }
      }
    }
  } while( true );
}


