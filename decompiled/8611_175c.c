
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_ui_all_cities_seen(void)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  byte *pbVar4;
  int iVar5;
  char *pcVar6;
  undefined2 uVar7;
  uint local_10;
  
  bVar1 = true;
  iVar5 = 0;
  local_10 = 0;
  if (opt_hidden_map != 0) {
    for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
      if ((*(int *)(iVar3 * 2 + 0xd0) == 0) && (*(int *)(iVar3 * 2 + 0x137) != 0)) {
        iVar5 = iVar5 + 1;
      }
      if (*(int *)(iVar3 * 2 + 0x137) == 0) {
        local_10 = local_10 | 1 << ((byte)iVar3 & 0x1f);
      }
    }
    _LAB_4125_126b_1 = mem_lock((int)_LAB_4125_126f_1,(int)((ulong)_LAB_4125_126f_1 >> 0x10));
    for (iVar2 = 0; iVar2 < city_count; iVar2 = iVar2 + 1) {
      if (*(char *)(*(byte *)(*(int *)(iVar2 * 0x41 + 0x157f) * 0xe0 +
                             *(int *)(iVar2 * 0x41 + 0x157d) * 2) + 0x710) == '\n') {
        pbVar4 = (byte *)(_LAB_4125_126b_1 + *(int *)(iVar2 * 0x41 + 0x157f) * 0x70 +
                         *(int *)(iVar2 * 0x41 + 0x157d));
        if ((byte)(*pbVar4 | pbVar4[1] | pbVar4[0x70] | pbVar4[0x71] | (byte)local_10) != 0xff) {
          bVar1 = false;
          break;
        }
      }
    }
    _LAB_4125_126d_1 = iVar3;
    mem_unlock((int)_LAB_4125_126f_1,(int)((ulong)_LAB_4125_126f_1 >> 0x10));
    if (bVar1) {
      opt_hidden_map = 0;
      FUN_8611_15da();
      if (iVar5 < 1) {
        uVar7 = 0xf;
      }
      else {
        uVar7 = 0x19;
      }
      pcVar6 = get_string(0x12,0);
      FUN_8065_11d1(pcVar6,uVar7);
    }
  }
  return;
}


