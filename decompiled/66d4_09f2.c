
void __cdecl16far FUN_66d4_09f2(byte param_1)

{
  byte *pbVar1;
  byte bVar2;
  byte *pbVar3;
  int iVar4;
  
  if (opt_quests != 0) {
    auto_file_mapcolor_dat();
  }
  for (iVar4 = 0; iVar4 < site_count; iVar4 = iVar4 + 1) {
    if (*(char *)(iVar4 * 0x1f + 0x829) == '\x01') {
      bVar2 = 0xb;
    }
    else if (((*(int *)(iVar4 * 0x1f + 0x82c) == 0) ||
             ((*(uint *)(iVar4 * 0x1f + 0x82e) & 1 << (param_1 & 0x1f)) == 0)) &&
            ((*(int *)(iVar4 * 0x1f + 0x82c) == 0 || (opt_quests != 0)))) {
      if (*(int *)(iVar4 * 0x1f + 0x82c) == 0) {
        bVar2 = 0xc;
      }
      else {
        bVar2 = 9;
      }
    }
    else {
      bVar2 = 10;
    }
    pbVar3 = (byte *)(*(int *)(iVar4 * 0x1f + 0x813) * 0xe0 + *(int *)(iVar4 * 0x1f + 0x811) * 2);
    *pbVar3 = 0;
    pbVar1 = pbVar3;
    *pbVar1 = *pbVar1 | bVar2;
    if (opt_quests != 0) {
      FUN_834b_2785(0,*(undefined2 *)(iVar4 * 0x1f + 0x811),*(undefined2 *)(iVar4 * 0x1f + 0x813));
    }
  }
  if (opt_quests != 0) {
    FUN_834b_2bb6();
  }
  return;
}


