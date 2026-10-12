
void __cdecl16far FUN_7204_03a9(void)

{
  bool bVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 uVar4;
  int local_12;
  undefined1 local_10 [8];
  undefined2 local_8;
  undefined2 local_6;
  undefined2 local_4;
  
  local_4 = 1;
  uVar4 = 1;
  local_6 = 1;
  local_8 = 1;
  if (LAB_4125_0e92 == 0) {
    FUN_18a9_07ed();
  }
  else {
    FUN_18a9_0839();
  }
  if (LAB_4125_0e92 == 4) {
    FUN_18a9_03b3();
    FUN_18a9_03b3();
    FUN_18a9_03b3();
    FUN_18a9_03b3();
  }
  else if (LAB_4125_0e92 == 0) {
    local_4 = 0;
  }
  else if (LAB_4125_0e92 == 1) {
    uVar4 = 0;
  }
  else if (LAB_4125_0e92 == 2) {
    local_6 = 0;
  }
  else if (LAB_4125_0e92 == 3) {
    local_8 = 0;
  }
  if (((int)((undefined2 *)&side_city_count)[current_player] < 1) ||
     (*(char *)(DAT_4125_0e98 * 0x41 + 0x1592) != current_player)) {
    uVar4 = 2;
    local_6 = 2;
    local_8 = 2;
  }
  if (LAB_4125_0e92 == 2) {
    for (iVar3 = 0; iVar3 < 4; iVar3 = iVar3 + 1) {
      FUN_18a9_066e(iVar3 + 0xc5,1);
    }
    FUN_18a9_066e(0x100c9);
    if (*(char *)(DAT_4125_0e98 * 0x41 + 0x15aa) < '\x01') {
      uVar2 = 2;
    }
    else {
      uVar2 = 1;
    }
    FUN_18a9_05f0(0xca,uVar2);
    FUN_18a9_03b3();
  }
  else {
    FUN_18a9_066e(0x100c0);
    FUN_18a9_03b3();
    FUN_18a9_03b3();
    for (iVar3 = 0; iVar3 < 4; iVar3 = iVar3 + 1) {
      FUN_18a9_03b3();
    }
  }
  if (LAB_4125_0e92 != 3) {
    for (iVar3 = 0; iVar3 < 3; iVar3 = iVar3 + 1) {
      FUN_18a9_03b3();
    }
    goto LAB_7204_064f;
  }
  if (DAT_4125_0e96 == 0) {
    FUN_18a9_05f0(0x100d4);
  }
  else {
    FUN_18a9_05f0(0x100d8);
  }
  FUN_18a9_03b3();
  FUN_7087_0410(DAT_4125_0e98,local_10);
  bVar1 = *(char *)(DAT_4125_0e98 * 0x41 + 0x15aa) < '\x01';
  if (DAT_4125_0e94 == 0) {
    if (bVar1) {
      uVar2 = 2;
    }
    else {
      uVar2 = 1;
    }
    FUN_18a9_05f0(0xd2,uVar2);
    if (local_12 < 1) {
      uVar2 = 2;
    }
    else {
      uVar2 = 1;
    }
    FUN_18a9_05f0(0xd3,uVar2);
LAB_7204_05b8:
    FUN_18a9_03b3();
  }
  else {
    if (DAT_4125_0e94 == 1) {
      FUN_18a9_05f0(0x100d6);
      if (local_12 < 1) {
        uVar2 = 2;
      }
      else {
        uVar2 = 1;
      }
      FUN_18a9_05f0(0xd3,uVar2);
      goto LAB_7204_05b8;
    }
    if (DAT_4125_0e94 != 2) goto LAB_7204_064f;
    if (bVar1) {
      uVar2 = 2;
    }
    else {
      uVar2 = 1;
    }
    FUN_18a9_05f0(0xd2,uVar2);
    FUN_18a9_05f0(0x100d7);
    FUN_18a9_03b3();
  }
  FUN_18a9_03b3();
LAB_7204_064f:
  if (LAB_4125_0e92 == 1) {
    FUN_18a9_066e(0x100cb);
    FUN_18a9_066e(0x100cd);
    FUN_18a9_066e(0x100cc);
  }
  else {
    FUN_18a9_03b3();
    FUN_18a9_03b3();
    FUN_18a9_03b3();
  }
  if (LAB_4125_0e92 != 4) {
    FUN_18a9_066e(0xc1,local_4);
    FUN_18a9_066e(0xc2,uVar4);
    FUN_18a9_066e(0xc3,local_6);
    FUN_18a9_066e(0xc4,local_8);
  }
  return;
}


