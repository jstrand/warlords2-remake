
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far sage_offer_knowledge(int choice)

{
  uint *puVar1;
  int iVar2;
  char *pcVar3;
  int iVar4;
  int iVar5;
  undefined2 unaff_SS;
  char *pcVar6;
  undefined2 uVar8;
  undefined4 uVar7;
  char local_86 [128];
  char *local_6;
  int local_4;
  
  auto_str_join_data_not_found();
  DAT_4125_166a = 1;
  if (choice < 0) {
    FUN_18a9_03b3();
    if (DAT_451b_08ce < 1) {
      uVar8 = 2;
    }
    else {
      uVar8 = 1;
    }
    FUN_18a9_066e(0x117,uVar8);
    FUN_18a9_066e(0x10118);
    if (opt_hidden_map == 0) {
      uVar8 = 2;
    }
    else {
      uVar8 = 1;
    }
    uVar7 = CONCAT22(uVar8,0x119);
  }
  else {
    local_4 = ((undefined2 *)&DAT_451b_0c9c)[choice];
    if (local_4 == -1) {
      pcVar6 = get_string(0x3e,2);
      FUN_6536_15d5((char *)pcVar6,(int)((ulong)pcVar6 >> 0x10));
      pcVar6 = get_string(0x3e,1);
      FUN_6536_15d5((char *)pcVar6);
      iVar5 = 0;
      local_6 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
      for (iVar4 = 0; iVar4 < site_count; iVar4 = iVar4 + 1) {
        if ((((*(int *)(iVar4 * 0x1f + 0x82c) != 0) &&
             ((*(uint *)(iVar4 * 0x1f + 0x82e) & 1 << ((byte)current_player & 0x1f)) == 0)) &&
            (iVar2 = map_distance(*(int *)(iVar4 * 0x1f + 0x811),*(int *)(iVar4 * 0x1f + 0x813),
                                  DAT_451b_01ca,DAT_451b_01c8), iVar2 < 0x23)) &&
           ((*(char *)(iVar4 * 0x1f + 0x829) == '\x04' &&
            (pcVar3 = (char *)map_distance(*(int *)(iVar4 * 0x1f + 0x811),
                                           *(int *)(iVar4 * 0x1f + 0x813),DAT_451b_01ca,
                                           DAT_451b_01c8), (int)pcVar3 < (int)local_6)))) {
          iVar5 = iVar4;
          local_6 = pcVar3;
        }
      }
    }
    else if (local_4 == -2) {
      pcVar6 = get_string(0x3e,3);
      FUN_6536_15d5((char *)pcVar6,(int)((ulong)pcVar6 >> 0x10));
      pcVar6 = get_string(0x3e,1);
      FUN_6536_15d5((char *)pcVar6);
      iVar5 = 0;
      local_6 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
      for (iVar4 = 0; iVar4 < site_count; iVar4 = iVar4 + 1) {
        if (((*(int *)(iVar4 * 0x1f + 0x82c) != 0) &&
            ((*(uint *)(iVar4 * 0x1f + 0x82e) & 1 << ((byte)current_player & 0x1f)) == 0)) &&
           ((iVar2 = map_distance(*(int *)(iVar4 * 0x1f + 0x811),*(int *)(iVar4 * 0x1f + 0x813),
                                  DAT_451b_01ca,DAT_451b_01c8), iVar2 < 0x23 &&
            ((*(char *)(iVar4 * 0x1f + 0x829) == '\x05' &&
             (pcVar3 = (char *)map_distance(*(int *)(iVar4 * 0x1f + 0x811),
                                            *(int *)(iVar4 * 0x1f + 0x813),DAT_451b_01ca,
                                            DAT_451b_01c8), (int)pcVar3 < (int)local_6)))))) {
          iVar5 = iVar4;
          local_6 = pcVar3;
        }
      }
    }
    else {
      uVar7 = CONCAT22(0x3c04,local_4 * 0x1d + 0xce9);
      pcVar6 = get_string(0x3e,0);
      sprintf((char *)CONCAT22(unaff_SS,local_86),pcVar6,uVar7);
      FUN_6536_15d5(local_86);
      pcVar6 = get_string(0x3e,1);
      FUN_6536_15d5((char *)pcVar6,(int)((ulong)pcVar6 >> 0x10));
      iVar5 = 0;
      while ((iVar5 < site_count && (*(char *)(iVar5 * 0x1f + 0x82a) != (char)local_4))) {
        iVar5 = iVar5 + 1;
      }
    }
    puVar1 = (uint *)(iVar5 * 0x1f + 0x82e);
    *puVar1 = *puVar1 | 1 << ((byte)current_player & 0x1f);
    FUN_8611_1298();
    FUN_834b_2200();
    FUN_66d4_09f2();
    FUN_834b_0000(0x10000,0);
    uVar7 = CONCAT22(0x3c04,iVar5 * 0x1f + 0x815);
    pcVar6 = get_string(0x3e,7);
    sprintf(&DAT_451b_1cd8,pcVar6,uVar7);
    FUN_6536_15d5((undefined1 *)&DAT_451b_1cd8,0x4125);
    FUN_828e_0806(iVar5,8);
    FUN_828e_0868(iVar5,8);
    FUN_828e_0a3f();
    FUN_834b_1f5f();
    FUN_834b_04d3(0);
    FUN_54f6_070b(0x1160008,0x1700f0);
    FUN_18a9_03b3();
    FUN_18a9_03b3();
    FUN_18a9_03b3();
    uVar7 = 0x1011a;
  }
  FUN_18a9_066e(uVar7);
  return;
}


