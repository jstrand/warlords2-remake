
void __cdecl16far auto_ui_army_info_labels_2(void)

{
  undefined2 uVar1;
  undefined2 in_DX;
  uint unaff_SI;
  undefined2 unaff_SS;
  char *pcVar2;
  undefined2 uVar3;
  char *pcVar4;
  undefined2 uVar5;
  int iVar6;
  ulong uVar7;
  undefined1 local_e [4];
  undefined1 local_a [8];
  
  uVar7 = (ulong)unaff_SI;
  DAT_4125_0d6c =
       (uint)(*(char *)(((undefined2 *)&DAT_451b_0c9c)[DAT_451b_04aa] * 0x1d + 0xcff) != '\x03');
  FUN_24d0_0002(3);
  uVar1 = FUN_216d_002e(local_a);
  FUN_216d_0288(uVar1,in_DX,uVar7);
  FUN_24d0_02e5(local_a);
  uVar3 = 2;
  uVar5 = 4;
  uVar1 = FUN_216d_002e(local_a);
  FUN_24d0_02e5(uVar1,in_DX,uVar3,uVar5);
  FUN_54f6_070b(0x18,0x97,200,0x14);
  FUN_78a8_06ae(2,0xf,0,3);
  if (DAT_4125_0d6c == 0) {
    iVar6 = 8;
  }
  else {
    iVar6 = 7;
  }
  pcVar2 = get_string(0x83,iVar6);
  uVar1 = (undefined2)((ulong)pcVar2 >> 0x10);
  FUN_7ecb_00d6(0x1b0,0xd3,pcVar2);
  for (iVar6 = 0; iVar6 < 3; iVar6 = iVar6 + 1) {
    if (-1 < (int)((undefined2 *)&DAT_451b_04a8)[iVar6]) {
      if (*(char *)(((undefined2 *)&DAT_451b_0c9c)[((undefined2 *)&DAT_451b_04a8)[iVar6]] * 0x1d +
                   0xcff) == '\x01') {
        uVar3 = 10;
      }
      else if (*(char *)(((undefined2 *)&DAT_451b_0c9c)[((undefined2 *)&DAT_451b_04a8)[iVar6]] *
                         0x1d + 0xcff) == '\0') {
        uVar3 = 5;
      }
      else {
        uVar3 = 7;
      }
      FUN_78a8_06ae(2,uVar3,0,3);
      FUN_7ecb_00bf(0x178,iVar6 * 0x16 + 0xee,
                    (undefined1 *)&DAT_451b_08dc + ((undefined2 *)&DAT_451b_04a8)[iVar6] * 0x20,
                    0x4125);
    }
  }
  FUN_24d0_0002(3);
  uVar3 = FUN_216d_002e(local_a);
  FUN_216d_0288(uVar3,uVar1);
  if (DAT_451b_04aa < 0) goto LAB_6c1b_0a71;
  if (*(char *)(((undefined2 *)&DAT_451b_0c9c)[DAT_451b_04aa] * 0x1d + 0xcfd) == '\b') {
    pcVar4 = (char *)0xd7c;
    goto LAB_6c1b_08fb;
  }
  if (*(char *)(((undefined2 *)&DAT_451b_0c9c)[DAT_451b_04aa] * 0x1d + 0xcfd) == '\x02') {
    iVar6 = (int)*(char *)(((undefined2 *)&DAT_451b_0c9c)[DAT_451b_04aa] * 0x1d + 0xcfe);
    pcVar4 = (char *)0xd83;
LAB_6c1b_09f7:
    sprintf((char *)CONCAT22(unaff_SS,&stack0xffde),(char *)CONCAT22(0x4125,pcVar4),iVar6);
  }
  else {
    if (*(char *)(((undefined2 *)&DAT_451b_0c9c)[DAT_451b_04aa] * 0x1d + 0xcfd) == '\x01') {
      iVar6 = (int)*(char *)(((undefined2 *)&DAT_451b_0c9c)[DAT_451b_04aa] * 0x1d + 0xcfe);
      pcVar4 = (char *)0xd8b;
      goto LAB_6c1b_09f7;
    }
    if (*(char *)(((undefined2 *)&DAT_451b_0c9c)[DAT_451b_04aa] * 0x1d + 0xcfd) == '\x05') {
      pcVar4 = (char *)0xd93;
LAB_6c1b_08fb:
      sprintf((char *)CONCAT22(unaff_SS,&stack0xffde),(char *)CONCAT22(0x4125,pcVar4));
    }
    else {
      if (*(char *)(((undefined2 *)&DAT_451b_0c9c)[DAT_451b_04aa] * 0x1d + 0xcfd) == '\x06') {
        pcVar4 = (char *)0xd97;
        goto LAB_6c1b_08fb;
      }
      if (*(char *)(((undefined2 *)&DAT_451b_0c9c)[DAT_451b_04aa] * 0x1d + 0xcfd) == '\a') {
        iVar6 = (int)*(char *)(((undefined2 *)&DAT_451b_0c9c)[DAT_451b_04aa] * 0x1d + 0xcfe);
        pcVar4 = (char *)0xd9c;
        goto LAB_6c1b_09f7;
      }
    }
  }
  if (*(char *)(((undefined2 *)&DAT_451b_0c9c)[DAT_451b_04aa] * 0x1d + 0xcff) == '\x01') {
    uVar1 = 10;
  }
  else if (*(char *)(((undefined2 *)&DAT_451b_0c9c)[DAT_451b_04aa] * 0x1d + 0xcff) == '\0') {
    uVar1 = 5;
  }
  else {
    uVar1 = 7;
  }
  FUN_78a8_06ae(2,uVar1,0,3);
  FUN_7ecb_00d6(0x154,0x104,&stack0xffde);
LAB_6c1b_0a71:
  FUN_216d_000e(local_e);
  __F_SPUSH(local_e._0_2_,local_e._2_2_);
  FUN_1997_0129(8);
  FUN_78a8_06ae(0x70002);
  FUN_7ecb_00bf(0x1390150);
  FUN_216d_000e(local_e);
  __F_SPUSH();
  FUN_1997_0129();
  FUN_78a8_06ae();
  FUN_7ecb_00bf();
  return;
}


