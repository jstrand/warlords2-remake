
void __cdecl16far auto_ui_items_list(void)

{
  int iVar1;
  int iVar2;
  int index;
  undefined2 unaff_SS;
  char *pcVar3;
  int local_32 [22];
  int local_6;
  int local_4;
  
  index = 0;
  local_6 = 0;
  for (iVar2 = 8; iVar2 < 0x16; iVar2 = iVar2 + 1) {
    local_32[local_6] = iVar2;
    local_6 = local_6 + 1;
  }
  for (iVar2 = 1; iVar1 = iVar2, iVar2 < local_6; iVar2 = iVar2 + 1) {
    for (; ((*(char *)(local_32[iVar1] * 0x1d + 0xcfd) <
             *(char *)(local_32[iVar1 + -1] * 0x1d + 0xcfd) ||
            ((*(char *)(local_32[iVar1] * 0x1d + 0xcfd) ==
              *(char *)(local_32[iVar1 + -1] * 0x1d + 0xcfd) &&
             (*(char *)(local_32[iVar1] * 0x1d + 0xcfe) <
              *(char *)(local_32[iVar1 + -1] * 0x1d + 0xcfe))))) && (0 < iVar1)); iVar1 = iVar1 + -1
        ) {
      local_4 = local_32[iVar1];
      local_32[iVar1] = local_32[iVar1 + -1];
      local_32[iVar1 + -1] = local_4;
    }
  }
  FUN_78a8_06ae(1,0xf,0,3);
  pcVar3 = get_string(0xa6,0);
  FUN_7ecb_00d6(0x140,0x34,pcVar3);
  FUN_78a8_06ae(2,0xf,0,3);
  pcVar3 = get_string(0xa7,0);
  FUN_7ecb_00d6(0x140,0x17e,pcVar3);
  FUN_78a8_06ae(2,7,0,3);
  for (iVar2 = 0; iVar2 < local_6; iVar2 = iVar2 + 1) {
    FUN_7ecb_0103(0x130,iVar2 * 0x14 + 0x5a,local_32[iVar2] * 0x1d + 0xce9,0x3c04);
    if (*(char *)(local_32[iVar2] * 0x1d + 0xcfd) == '\x01') {
      index = 1;
    }
    else if (*(char *)(local_32[iVar2] * 0x1d + 0xcfd) == '\x02') {
      index = 2;
    }
    else if (*(char *)(local_32[iVar2] * 0x1d + 0xcfd) == '\x05') {
      index = 3;
    }
    else if (*(char *)(local_32[iVar2] * 0x1d + 0xcfd) == '\x06') {
      index = 4;
    }
    else if (*(char *)(local_32[iVar2] * 0x1d + 0xcfd) == '\a') {
      index = 5;
    }
    iVar1 = (int)*(char *)(local_32[iVar2] * 0x1d + 0xcfe);
    pcVar3 = get_string(0xa7,index);
    sprintf(&DAT_451b_1d28,pcVar3,iVar1);
    FUN_7ecb_00bf(0x150,iVar2 * 0x14 + 0x5a,(undefined1 *)&DAT_451b_1d28,0x4125);
  }
  return;
}


