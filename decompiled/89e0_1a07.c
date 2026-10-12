
undefined1 * __cdecl16far auto_ui_army_bonus_text(int param_1,long param_2)

{
  int iVar1;
  undefined2 uVar2;
  char *pcVar3;
  int iVar4;
  undefined1 local_42 [32];
  int local_22;
  int local_20;
  int local_1e;
  int local_1c;
  int local_1a;
  int local_18;
  int local_16;
  int local_14;
  int local_12;
  int local_10;
  int local_e;
  int local_4;
  
  pcVar3 = get_string(0xa3,0);
  strcpy(&DAT_451b_1d28,pcVar3);
  iVar4 = (int)param_2;
  uVar2 = (undefined2)((ulong)param_2 >> 0x10);
  if ((param_2 == 0) || ((*(uint *)(iVar4 + 0xc) & 0x1000) == 0)) {
    if (param_1 == 0x1c) {
      if (param_2 != 0) {
        local_4 = hero_battle_items(iVar4,uVar2);
        iVar1 = *(int *)((*(char *)(iVar4 + 8) + local_4) * 2 + 0xce4);
        iVar4 = hero_command_items(iVar4,uVar2);
        iVar1 = iVar1 + iVar4;
        if (6 < iVar1) {
          iVar1 = 6;
        }
        iVar4 = 2;
        goto LAB_89e0_1b7b;
      }
    }
    else {
      armytype_lookup(param_1,local_42);
    }
    if (local_e == 2) {
      iVar4 = 3;
    }
    else {
      if (local_e != 3) {
        iVar1 = local_18;
        if (local_12 == 0) {
          if ((((local_14 == 0) || (local_16 == 0)) || (local_18 == 0)) || (local_1a == 0)) {
            if (local_e == 1) {
              iVar4 = 6;
              goto LAB_89e0_1a41;
            }
            if (local_10 == 0) {
              if (local_14 == 0) {
                if (local_16 == 0) {
                  if (local_18 == 0) {
                    if (local_1a == 0) {
                      if (local_1c == 0) {
                        if (local_1e == 0) {
                          if (local_20 == 0) {
                            if (local_22 == 0) {
                              return (undefined1 *)&DAT_451b_1d28;
                            }
                            iVar4 = 0xf;
                            iVar1 = local_22;
                          }
                          else {
                            iVar4 = 0xe;
                            iVar1 = local_20;
                          }
                        }
                        else {
                          iVar4 = 0xd;
                          iVar1 = local_1e;
                        }
                      }
                      else {
                        iVar4 = 0xc;
                        iVar1 = local_1c;
                      }
                    }
                    else {
                      iVar4 = 0xb;
                      iVar1 = local_1a;
                    }
                  }
                  else {
                    iVar4 = 10;
                  }
                }
                else {
                  iVar4 = 9;
                  iVar1 = local_16;
                }
              }
              else {
                iVar4 = 8;
                iVar1 = local_14;
              }
            }
            else {
              iVar4 = 7;
              iVar1 = local_10;
            }
          }
          else {
            iVar4 = 0x10;
            iVar1 = local_1a;
          }
        }
        else {
          iVar4 = 5;
        }
LAB_89e0_1b7b:
        pcVar3 = get_string(0xa3,iVar4);
        sprintf(&DAT_451b_1d28,pcVar3,iVar1);
        return (undefined1 *)&DAT_451b_1d28;
      }
      iVar4 = 4;
    }
  }
  else {
    iVar4 = 1;
  }
LAB_89e0_1a41:
  pcVar3 = get_string(0xa3,iVar4);
  strcpy(&DAT_451b_1d28,pcVar3);
  return (undefined1 *)&DAT_451b_1d28;
}


