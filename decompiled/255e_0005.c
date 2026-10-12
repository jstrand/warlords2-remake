
undefined2 __cdecl16far sound_init(void)

{
  byte *pbVar1;
  char *pcVar2;
  byte *pbVar3;
  int iVar4;
  uint uVar5;
  undefined2 uVar6;
  undefined1 *puVar7;
  byte *pbVar8;
  uint unaff_DI;
  char *pcVar9;
  byte *pbVar10;
  void *unaff_SS;
  bool bVar11;
  bool bVar12;
  byte local_c4 [50];
  byte local_92 [50];
  char local_60 [80];
  void *local_10;
  int local_c;
  void *local_a;
  void *local_6;
  
  puVar7 = (undefined1 *)&DAT_451b_3108;
  do {
    *puVar7 = 0;
    puVar7 = puVar7 + 0xb;
  } while (puVar7 != (undefined1 *)0x70d6);
  iVar4 = open(s_sound_dat_4125_37ae,2);
  if (iVar4 < 0) {
    return 0;
  }
  read(iVar4,(void *)CONCAT22(0x50,unaff_SS),unaff_DI);
  sscanf((char *)CONCAT22(unaff_SS,local_60),s__s__s__d_4125_37b8,local_c4);
  close(iVar4);
  pbVar8 = local_c4;
  pcVar9 = (char *)s_NO_SOUND_4125_37c1;
  uVar5 = 0xffff;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    pcVar2 = pcVar9;
    pcVar9 = pcVar9 + 1;
  } while (*pcVar2 != '\0');
  uVar5 = ~uVar5;
  pbVar10 = (byte *)(pcVar9 + -uVar5);
  bVar11 = false;
  iVar4 = 0;
  bVar12 = true;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    pbVar3 = pbVar10;
    pbVar10 = pbVar10 + 1;
    pbVar1 = pbVar8;
    pbVar8 = pbVar8 + 1;
    bVar11 = *pbVar1 < *pbVar3;
    bVar12 = *pbVar1 == *pbVar3;
  } while (bVar12);
  if (!bVar12) {
    iVar4 = (1 - (uint)bVar11) - (uint)(bVar11 != 0);
  }
  if (iVar4 == 0) {
    DAT_4125_37a2 = '\0';
    DAT_4125_37a4 = 0;
  }
  pbVar8 = local_92;
  pcVar9 = (char *)s_NO_SOUND_4125_37ca;
  uVar5 = 0xffff;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    pcVar2 = pcVar9;
    pcVar9 = pcVar9 + 1;
  } while (*pcVar2 != '\0');
  uVar5 = ~uVar5;
  pbVar10 = (byte *)(pcVar9 + -uVar5);
  bVar11 = false;
  iVar4 = 0;
  bVar12 = true;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    pbVar3 = pbVar10;
    pbVar10 = pbVar10 + 1;
    pbVar1 = pbVar8;
    pbVar8 = pbVar8 + 1;
    bVar11 = *pbVar1 < *pbVar3;
    bVar12 = *pbVar1 == *pbVar3;
  } while (bVar12);
  if (!bVar12) {
    iVar4 = (1 - (uint)bVar11) - (uint)(bVar11 != 0);
  }
  if (iVar4 == 0) {
    DAT_4125_37a3 = '\0';
  }
  if ((DAT_4125_37a2 == '\0') && (DAT_4125_37a3 == '\0')) {
    DAT_4125_37a6 = 1;
    return 1;
  }
  if (DAT_4125_37a2 != '\0') {
    pbVar8 = local_c4;
    pcVar9 = (char *)s_IBMBAK_COM_4125_37d3;
    uVar5 = 0xffff;
    do {
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      pcVar2 = pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (*pcVar2 != '\0');
    uVar5 = ~uVar5;
    pbVar10 = (byte *)(pcVar9 + -uVar5);
    bVar11 = false;
    iVar4 = 0;
    bVar12 = true;
    do {
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      pbVar3 = pbVar10;
      pbVar10 = pbVar10 + 1;
      pbVar1 = pbVar8;
      pbVar8 = pbVar8 + 1;
      bVar11 = *pbVar1 < *pbVar3;
      bVar12 = *pbVar1 == *pbVar3;
    } while (bVar12);
    if (!bVar12) {
      iVar4 = (1 - (uint)bVar11) - (uint)(bVar11 != 0);
    }
    if (iVar4 == 0) {
      DAT_4125_37ab = '\x01';
    }
    if ((DAT_4125_37ab == '\0') && (iVar4 = digpak_load_driver(local_c4), iVar4 == 0)) {
      return 0;
    }
  }
  if (DAT_4125_37a3 != '\0') {
    local_6 = dos_load_file((char *)CONCAT22(unaff_SS,local_92),(long *)0x0);
    if (local_6 == (void *)0x0) {
      printf(s_Unable_to_find_ADV_file_4125_37de);
      return 0;
    }
    local_10 = local_6;
    local_a = dos_load_file((char *)0x412537f7,(long *)0x0);
    uVar6 = (undefined2)((ulong)local_a >> 0x10);
    if (local_a == (void *)0x0) {
      printf(s_Unable_to_find_AD_file_4125_3801);
      return 0;
    }
    iVar4 = midpak_load_driver((char *)s_MIDPAK_COM_4125_3819,0x4125);
    if (iVar4 == 0) {
      printf(s_Unable_to_load_MIDPAK_COM_4125_3824);
      return 0;
    }
    local_c = midpak_0710(local_10,local_a);
    if (local_c != 0) {
      printf(s_Error_initilizing_driver__4125_383f);
      return 0;
    }
    pbVar8 = local_92;
    pcVar9 = (char *)&DAT_4125_385a;
    uVar5 = 0xffff;
    do {
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      pcVar2 = pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (*pcVar2 != '\0');
    uVar5 = ~uVar5;
    pbVar10 = (byte *)(pcVar9 + -uVar5);
    bVar11 = false;
    iVar4 = 0;
    bVar12 = true;
    do {
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      pbVar3 = pbVar10;
      pbVar10 = pbVar10 + 1;
      pbVar1 = pbVar8;
      pbVar8 = pbVar8 + 1;
      bVar11 = *pbVar1 < *pbVar3;
      bVar12 = *pbVar1 == *pbVar3;
    } while (bVar12);
    if (!bVar12) {
      iVar4 = (1 - (uint)bVar11) - (uint)(bVar11 != 0);
    }
    if (iVar4 == 0) {
      DAT_4125_37ac = 1;
    }
    pbVar8 = local_92;
    pcVar9 = (char *)s_SC32MPU_ADV_4125_3866;
    uVar5 = 0xffff;
    do {
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      pcVar2 = pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (*pcVar2 != '\0');
    uVar5 = ~uVar5;
    pbVar10 = (byte *)(pcVar9 + -uVar5);
    bVar11 = false;
    iVar4 = 0;
    bVar12 = true;
    do {
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      pbVar3 = pbVar10;
      pbVar10 = pbVar10 + 1;
      pbVar1 = pbVar8;
      pbVar8 = pbVar8 + 1;
      bVar11 = *pbVar1 < *pbVar3;
      bVar12 = *pbVar1 == *pbVar3;
    } while (bVar12);
    if (!bVar12) {
      iVar4 = (1 - (uint)bVar11) - (uint)(bVar11 != 0);
    }
    if (iVar4 == 0) {
      DAT_4125_37ad = 1;
    }
    DAT_451b_30f4 = mem_alloc((char *)s__x__Use_extended_memory_4125_2703 + 0xd,0,0,3,0);
    DAT_451b_30f6 = uVar6;
    DAT_451b_30f0 = mem_lock(DAT_451b_30f4,uVar6);
    DAT_451b_30f2 = uVar6;
  }
  return 1;
}


