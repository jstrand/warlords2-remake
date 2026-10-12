
void __cdecl16far auto_ui_creating_strategic_map(void)

{
  byte bVar1;
  undefined2 uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  char *pcVar7;
  char *pcVar8;
  char *pcVar9;
  char *pcVar10;
  char local_10c [256];
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  local_8 = FUN_54f6_08bf();
  if (local_8 == 0) {
    FUN_54f6_0000();
  }
  else {
    FUN_54f6_08f6(&local_4);
    FUN_54f6_070b(200 - local_4,0xaf - local_6,0x6900f8);
  }
  get_string(0x89,6);
  get_string(0x89,5);
  get_string(0x89,4);
  pcVar7 = get_string(0x89,3);
  pcVar8 = get_string(0x89,2);
  pcVar9 = get_string(0x89,1);
  pcVar10 = get_string(0x89,0);
  FUN_8065_1471((char *)pcVar10,(int)((ulong)pcVar10 >> 0x10),pcVar9,pcVar8,pcVar7);
  FUN_4bed_01b5();
  random_map_progress();
  auto_file_mapcolor_dat();
  DAT_451b_1f06 = DAT_451b_1f12;
  if (DAT_3c04_0161 == 1) {
    DAT_451b_1f06 = DAT_451b_1f0e;
  }
  if (DAT_3c04_0161 == 2) {
    DAT_451b_1f06 = DAT_451b_1f0a;
  }
  uVar6 = 0x1997;
  iVar5 = DAT_3c04_0161;
  FUN_1997_034a();
  if ((DAT_451b_18c7 != 0) && ((DAT_451b_18f2 & 10) == 0)) {
    for (iVar4 = 0; iVar4 < (int)(uint)DAT_451b_18f1; iVar4 = iVar4 + 1) {
      uVar6 = 0x1d28;
      uVar2 = mem_lock(((undefined4 *)&DAT_451b_18c7)[iVar4]);
      ((undefined2 *)&DAT_451b_18d9)[iVar4 * 2] = iVar5;
      ((undefined2 *)&DAT_451b_18d7)[iVar4 * 2] = uVar2;
    }
  }
  for (iVar5 = 0; iVar5 < 0x100; iVar5 = iVar5 + 1) {
    uVar6 = 0x7ecb;
    iVar4 = dice(1,3,1);
    local_10c[iVar5] = (char)iVar4;
  }
  iVar5 = 0;
  local_a = 0;
  for (local_4 = 0; local_4 < 0x70; local_4 = local_4 + 1) {
    local_c = (local_4 * 10) / 0x70;
    if (local_a < local_c) {
      local_a = local_a + 1;
      uVar6 = 0x4bed;
      random_map_progress();
    }
    for (local_6 = 0; local_6 < 0x9c; local_6 = local_6 + 1) {
      uVar3 = (uint)*(byte *)(local_6 * 0xe0 + local_4 * 2);
      bVar1 = *(byte *)(local_6 * 0x70 + local_4);
      if ((bVar1 & 0x1f) != 0) {
        uVar3 = (*(byte *)(local_6 * 0x70 + local_4) & 0x1f) - 1;
      }
      if (((int)uVar3 < 0x50) || (0x5f < (int)uVar3)) {
        if ((bVar1 & 0x1f) == 0) {
          (*DAT_451b_2a70)(uVar6,0x581d,0x4125,local_4 << 1,local_6 << 1);
          (*DAT_451b_2a70)(uVar6,0x581d,0x4125,local_4 * 2 + 1,local_6 << 1);
          (*DAT_451b_2a70)(uVar6,0x581d,0x4125,local_4 << 1,local_6 * 2 + 1);
        }
        else {
          (*DAT_451b_2a70)(uVar6,0x581d,0x4125,local_4 << 1,local_6 << 1);
          (*DAT_451b_2a70)(uVar6,0x581d,0x4125,local_4 * 2 + 1,local_6 << 1);
          (*DAT_451b_2a70)(uVar6,0x581d,0x4125,local_4 << 1,local_6 * 2 + 1);
        }
        (*DAT_451b_2a70)();
      }
      else {
        (*DAT_451b_2a70)(uVar6,0x581d,0x4125,local_4 << 1,local_6 << 1,(int)local_10c[iVar5]);
        (*DAT_451b_2a70)(uVar6,0x581d,0x4125,local_4 * 2 + 1,local_6 << 1);
        (*DAT_451b_2a70)(uVar6,0x581d,0x4125,local_4 << 1,local_6 * 2 + 1);
        iVar5 = iVar5 + 4;
        (*DAT_451b_2a70)();
        if (0xff < iVar5) {
          iVar5 = 0;
        }
      }
    }
  }
  if ((DAT_451b_18c7 != 0) && ((DAT_451b_18f2 & 10) == 0)) {
    for (iVar5 = 0; iVar5 < (int)(uint)DAT_451b_18f1; iVar5 = iVar5 + 1) {
      mem_unlock(((undefined4 *)&DAT_451b_18c7)[iVar5]);
    }
  }
  FUN_834b_2bb6();
  if (local_8 == 0) {
    FUN_54f6_069c();
  }
  return;
}


