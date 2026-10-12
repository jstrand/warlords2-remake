
void __cdecl16far temple_bless(int param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  char *pcVar6;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  undefined4 local_6;
  
  local_8 = -1;
  local_a = -1;
  local_c = -1;
  local_e = -1;
  for (iVar3 = 0; iVar3 < site_count; iVar3 = iVar3 + 1) {
    iVar4 = local_c;
    iVar5 = local_a;
    iVar2 = local_8;
    if ((((*(char *)(iVar3 * 0x1f + 0x829) == '\x01') && (iVar2 = iVar3, -1 < local_8)) &&
        (iVar5 = iVar3, iVar2 = local_8, -1 < local_a)) &&
       ((iVar4 = iVar3, iVar5 = local_a, -1 < local_c && (iVar4 = local_c, local_e < 0)))) {
      local_e = iVar3;
    }
    local_8 = iVar2;
    local_a = iVar5;
    local_c = iVar4;
  }
  iVar3 = 0;
  for (iVar4 = 0; iVar5 = iVar3, iVar4 < 8; iVar4 = iVar4 + 1) {
    if (DAT_4125_2bda == 0) {
      local_6 = DAT_451b_1f02;
    }
    else {
      local_6 = CONCAT22(*(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar4 * 4),
                         ((undefined2 *)&DAT_451b_1ede)[iVar4 * 2]);
    }
    if (local_6 != 0) {
      iVar2 = (int)local_6;
      if (param_1 == local_8) {
        if ((*(uint *)(iVar2 + 0xc) & 2) == 0) {
          army_add_strength(iVar2,local_6._2_2_,1);
          *(uint *)(iVar2 + 0xc) = *(uint *)(iVar2 + 0xc) | 2;
          cVar1 = *(char *)(iVar2 + 4);
          goto joined_r0x00065d71;
        }
      }
      else if (param_1 == local_a) {
        if ((*(uint *)(iVar2 + 0xc) & 4) == 0) {
          army_add_strength(iVar2,local_6._2_2_,1);
          *(uint *)(iVar2 + 0xc) = *(uint *)(iVar2 + 0xc) | 4;
          cVar1 = *(char *)(iVar2 + 4);
          goto joined_r0x00065d71;
        }
      }
      else if (param_1 == local_c) {
        if ((*(uint *)(iVar2 + 0xc) & 8) == 0) {
          army_add_strength(iVar2,local_6._2_2_,1);
          *(uint *)(iVar2 + 0xc) = *(uint *)(iVar2 + 0xc) | 8;
          cVar1 = *(char *)(iVar2 + 4);
joined_r0x00065d71:
          iVar5 = iVar3 + 1;
          if (cVar1 == '\x1c') {
            hero_add_experience(iVar2,local_6._2_2_,1);
            iVar5 = iVar3 + 1;
          }
        }
      }
      else if ((param_1 == local_e) && ((*(uint *)(iVar2 + 0xc) & 0x10) == 0)) {
        army_add_strength(iVar2,local_6._2_2_,1);
        *(uint *)(iVar2 + 0xc) = *(uint *)(iVar2 + 0xc) | 0x10;
        cVar1 = *(char *)(iVar2 + 4);
        goto joined_r0x00065d71;
      }
      iVar3 = iVar5;
      if (DAT_4125_2bda == 0) break;
    }
  }
  if (iVar5 == 0) {
    iVar3 = 0x37;
  }
  else {
    if (iVar5 != 1) {
      pcVar6 = get_string(0x38,1);
      sprintf(&DAT_451b_1cd8,pcVar6,iVar5);
      goto LAB_6536_0a77;
    }
    iVar3 = 0x38;
  }
  pcVar6 = get_string(iVar3,0);
  sprintf(&DAT_451b_1cd8,pcVar6);
LAB_6536_0a77:
  iVar3 = is_computer_turn();
  if (iVar3 == 0) {
    pcVar6 = get_string(0x38,2);
    FUN_8065_1160((undefined1 *)&DAT_451b_1cd8,0x4125,(char *)pcVar6,(int)((ulong)pcVar6 >> 0x10));
  }
  return;
}


