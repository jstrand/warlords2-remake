
void __cdecl16far end_game_check(void)

{
  bool bVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 unaff_SS;
  char *pcVar6;
  char *pcVar7;
  undefined4 uVar8;
  char local_58 [80];
  int local_8;
  int local_6;
  int local_4;
  
  local_4 = 0;
  FUN_828e_03f9();
  for (iVar5 = 0; iVar5 < city_count; iVar5 = iVar5 + 1) {
    if (*(char *)(*(byte *)(*(int *)(iVar5 * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(iVar5 * 0x41 + 0x157d) * 2) + 0x710) == '\n') {
      local_4 = local_4 + 1;
    }
  }
  local_6 = 0;
  iVar4 = 0;
  bVar1 = true;
  bVar2 = false;
  iVar5 = 8;
  while (iVar3 = iVar5 + -1, iVar5 != 0) {
    if (*(int *)(iVar3 * 2 + 0x137) != 0) {
      bVar1 = false;
    }
    if ((*(int *)(iVar3 * 2 + 0xd0) == 0) && (*(int *)(iVar3 * 2 + 0x137) == 0)) {
      bVar2 = true;
      *(undefined2 *)(iVar3 * 2 + 0xd0) = 1;
    }
    if ((*(int *)(iVar3 * 2 + 0x137) != 0) && (*(int *)(iVar3 * 2 + 0xd0) == 0)) {
      bVar2 = true;
      iVar4 = iVar4 + 1;
      local_8 = iVar3;
    }
    iVar5 = iVar3;
    if ((*(int *)(iVar3 * 2 + 0x137) != 0) && (*(int *)(iVar3 * 2 + 0xd0) == 1)) {
      local_6 = local_6 + 1;
    }
  }
  if (bVar1) {
    pcVar6 = get_string(0xc,1);
    pcVar7 = get_string(0xc,0);
    FUN_8065_1160((char *)pcVar7,(int)((ulong)pcVar7 >> 0x10),pcVar6);
    pcVar6 = get_string(0xc,3);
    pcVar7 = get_string(0xc,2);
    FUN_8065_1160((char *)pcVar7,(int)((ulong)pcVar7 >> 0x10),pcVar6);
    auto_str_warlords_c_strategic_studies();
  }
  if (((game_won == 0) && (iVar4 == 0)) && (bVar2)) {
    pcVar6 = get_string(0xd,1);
    pcVar7 = get_string(0xd,0);
    FUN_8065_1160((char *)pcVar7,(int)((ulong)pcVar7 >> 0x10),pcVar6);
    pcVar6 = get_string(0xd,3);
    pcVar7 = get_string(0xd,2);
    FUN_8065_1160((char *)pcVar7,(int)((ulong)pcVar7 >> 0x10),pcVar6);
  }
  if (((game_won == 0) && (iVar4 == 1)) &&
     ((local_6 == 0 && (local_4 / 2 < (int)((undefined2 *)&side_city_count)[local_8])))) {
    game_won = 1;
    FUN_6d51_1244(local_8,7,local_8,0,0x2b01,0x4125);
  }
  if (((game_won == 0) && (iVar4 == 0)) && (local_6 == 1)) {
    game_won = 1;
    iVar5 = 0;
    while ((iVar5 < 8 && (*(int *)(iVar5 * 2 + 0x137) == 0))) {
      iVar5 = iVar5 + 1;
    }
    uVar8 = CONCAT22(0x3c04,iVar5 * 0x14);
    pcVar6 = get_string(0xf,0);
    sprintf((char *)CONCAT22(unaff_SS,local_58),pcVar6,uVar8);
    FUN_8065_10fb(local_58);
    *(undefined2 *)(iVar5 * 2 + 0xd0) = 0;
    FUN_6d51_1244(iVar5,7,iVar5,0,0x2b02,0x4125);
  }
  if ((((game_won == 0) && (iVar4 == 1)) && (0 < local_6)) && (surrender_offered == 0)) {
    iVar5 = 0;
    for (iVar4 = 0; iVar4 < 8; iVar4 = iVar4 + 1) {
      if (((*(int *)(iVar4 * 2 + 0x137) != 0) && (*(int *)(iVar4 * 2 + 0xd0) == 1)) &&
         (iVar5 < (int)((undefined2 *)&side_city_count)[iVar4])) {
        iVar5 = ((undefined2 *)&side_city_count)[iVar4];
      }
    }
    if ((city_count / 2 < (int)((undefined2 *)&side_city_count)[local_8]) &&
       (iVar5 + city_count / 8 < (int)((undefined2 *)&side_city_count)[local_8])) {
      surrender_offered = 1;
    }
  }
  return;
}


