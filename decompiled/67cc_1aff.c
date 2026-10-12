
void __cdecl16far FUN_67cc_1aff(void)

{
  int *piVar1;
  bool bVar2;
  int in_DX;
  int iVar3;
  int iVar4;
  bool bVar5;
  bool bVar6;
  
  bVar5 = DAT_451b_029a != '\x0f';
  bVar6 = DAT_451b_029b != '\x0f';
  if ((bVar5) && (bVar6)) {
    bVar2 = true;
  }
  else {
    bVar2 = false;
  }
  for (iVar4 = 0; iVar4 < combat_atk_count; iVar4 = iVar4 + 1) {
    if (((undefined1 *)&combat_atk_alive)[iVar4] == '\0') {
      if (((undefined1 *)&DAT_451b_029c)[iVar4] != '\0') {
        if (bVar5) {
          piVar1 = (int *)(DAT_451b_029a * 0x50 + DAT_451b_029a * 10 + 0x1169);
          *piVar1 = *piVar1 + 1;
        }
        if (bVar2) {
          piVar1 = (int *)(DAT_451b_029b * 0x50 + DAT_451b_029a * 10 + 0x1169);
          *piVar1 = *piVar1 + 1;
        }
      }
      if (((undefined1 *)&combat_atk_type)[iVar4] == '\x1c') {
        if (bVar5) {
          *(int *)((undefined1 *)&LAB_4125_1166_1 + DAT_451b_029a * 0x50 + DAT_451b_029a * 10) =
               *(int *)((undefined1 *)&LAB_4125_1166_1 + DAT_451b_029a * 0x50 + DAT_451b_029a * 10)
               + 1;
        }
        if (bVar2) {
          *(int *)((undefined1 *)&LAB_4125_1166_1 + DAT_451b_029b * 0x50 + DAT_451b_029a * 10) =
               *(int *)((undefined1 *)&LAB_4125_1166_1 + DAT_451b_029b * 0x50 + DAT_451b_029a * 10)
               + 1;
        }
      }
      else if (((undefined1 *)&DAT_451b_0668)[(char)((undefined1 *)&combat_atk_type)[iVar4] * 6] ==
               '\0') {
        if (((undefined1 *)&DAT_451b_0668)[(char)((undefined1 *)&combat_atk_type)[iVar4] * 6] ==
            '\0') {
          if (bVar5) {
            piVar1 = (int *)(DAT_451b_029a * 0x50 + DAT_451b_029a * 10 + 0x1163);
            *piVar1 = *piVar1 + 1;
          }
          if (bVar2) {
            piVar1 = (int *)(DAT_451b_029b * 0x50 + DAT_451b_029a * 10 + 0x1163);
            *piVar1 = *piVar1 + 1;
          }
        }
      }
      else {
        if (bVar5) {
          piVar1 = (int *)(DAT_451b_029a * 0x50 + DAT_451b_029a * 10 + 0x1165);
          *piVar1 = *piVar1 + 1;
        }
        if (bVar2) {
          piVar1 = (int *)(DAT_451b_029b * 0x50 + DAT_451b_029a * 10 + 0x1165);
          *piVar1 = *piVar1 + 1;
        }
      }
    }
    else {
      in_DX = 1;
    }
  }
  for (iVar4 = 0; iVar4 < combat_def_count; iVar4 = iVar4 + 1) {
    if (((undefined1 *)&combat_def_alive)[iVar4] == '\0') {
      if (((undefined1 *)&DAT_451b_02a4)[iVar4] != '\0') {
        if (bVar6) {
          piVar1 = (int *)(DAT_451b_029b * 0x50 + DAT_451b_029b * 10 + 0x1169);
          *piVar1 = *piVar1 + 1;
        }
        if (bVar2) {
          piVar1 = (int *)(DAT_451b_029a * 0x50 + DAT_451b_029b * 10 + 0x1169);
          *piVar1 = *piVar1 + 1;
        }
      }
      if (((undefined1 *)&combat_def_type)[iVar4] == '\x1c') {
        if (bVar6) {
          *(int *)((undefined1 *)&LAB_4125_1166_1 + DAT_451b_029b * 0x50 + DAT_451b_029b * 10) =
               *(int *)((undefined1 *)&LAB_4125_1166_1 + DAT_451b_029b * 0x50 + DAT_451b_029b * 10)
               + 1;
        }
        if (bVar2) {
          *(int *)((undefined1 *)&LAB_4125_1166_1 + DAT_451b_029a * 0x50 + DAT_451b_029b * 10) =
               *(int *)((undefined1 *)&LAB_4125_1166_1 + DAT_451b_029a * 0x50 + DAT_451b_029b * 10)
               + 1;
        }
      }
      else if (((undefined1 *)&DAT_451b_0668)[(char)((undefined1 *)&combat_def_type)[iVar4] * 6] ==
               '\0') {
        if (((undefined1 *)&DAT_451b_0668)[(char)((undefined1 *)&combat_def_type)[iVar4] * 6] ==
            '\0') {
          if (bVar6) {
            piVar1 = (int *)(DAT_451b_029b * 0x50 + DAT_451b_029b * 10 + 0x1163);
            *piVar1 = *piVar1 + 1;
          }
          if (bVar2) {
            piVar1 = (int *)(DAT_451b_029a * 0x50 + DAT_451b_029b * 10 + 0x1163);
            *piVar1 = *piVar1 + 1;
          }
        }
      }
      else {
        if (bVar6) {
          piVar1 = (int *)(DAT_451b_029b * 0x50 + DAT_451b_029b * 10 + 0x1165);
          *piVar1 = *piVar1 + 1;
        }
        if (bVar2) {
          piVar1 = (int *)(DAT_451b_029a * 0x50 + DAT_451b_029b * 10 + 0x1165);
          *piVar1 = *piVar1 + 1;
        }
      }
    }
    else {
      in_DX = 0;
    }
  }
  if (in_DX == 0) {
    for (iVar4 = 0; iVar4 < combat_atk_count; iVar4 = iVar4 + 1) {
      if (((undefined1 *)&combat_atk_type)[iVar4] == '\x1c') {
        list_carried_items(2,0);
        for (iVar3 = 0; iVar3 < DAT_451b_08ce; iVar3 = iVar3 + 1) {
          if ((int)((undefined2 *)&DAT_451b_0c9c)[iVar3] < 8) {
            if (bVar2) {
              piVar1 = (int *)(DAT_451b_029b * 0x50 + DAT_451b_029a * 10 + 0x116b);
              *piVar1 = *piVar1 + 1;
            }
            if (bVar5) {
              piVar1 = (int *)(DAT_451b_029a * 0x50 + DAT_451b_029a * 10 + 0x116b);
              *piVar1 = *piVar1 + 1;
            }
          }
        }
      }
    }
  }
  else {
    for (iVar4 = 0; iVar4 < combat_def_count; iVar4 = iVar4 + 1) {
      if (((undefined1 *)&combat_def_type)[iVar4] == '\x1c') {
        list_carried_items(2,0);
        for (iVar3 = 0; iVar3 < DAT_451b_08ce; iVar3 = iVar3 + 1) {
          if ((int)((undefined2 *)&DAT_451b_0c9c)[iVar3] < 8) {
            if (bVar2) {
              piVar1 = (int *)(DAT_451b_029a * 0x50 + DAT_451b_029b * 10 + 0x116b);
              *piVar1 = *piVar1 + 1;
            }
            if (bVar6) {
              piVar1 = (int *)(DAT_451b_029b * 0x50 + DAT_451b_029b * 10 + 0x116b);
              *piVar1 = *piVar1 + 1;
            }
          }
        }
      }
    }
  }
  return;
}


