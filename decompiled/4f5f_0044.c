
void __cdecl16far random_map_walk_line(int kind)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined4 local_16;
  int local_12;
  int local_10;
  undefined4 local_e;
  undefined4 local_a;
  undefined4 local_6;
  
  iVar2 = dice(1,0x70,-1);
  local_6 = CONCAT22(local_6._2_2_,iVar2);
  iVar2 = dice(1,0x9c,-1);
  local_6 = CONCAT22(iVar2,(int)local_6);
  iVar2 = dice(1,0x70,-1);
  local_a = CONCAT22(local_a._2_2_,iVar2);
  iVar2 = dice(1,0x9c,-1);
  local_a = CONCAT22(iVar2,(int)local_a);
  local_e = local_6;
  if (((int)local_6 == (int)local_a) && (local_6._2_2_ == iVar2)) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
  }
  while (!bVar1) {
    FUN_4c49_0dd0(local_e,local_a,&local_12);
    iVar3 = 0;
    iVar2 = -1;
    while( true ) {
      uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      if (7 < iVar3) break;
      iVar4 = (int)DAT_451b_002a + iVar3 * 4;
      if ((*(int *)(iVar4 + 0xbc) == local_12) && (*(int *)(iVar4 + 0xbe) == local_10)) {
        iVar2 = iVar3;
      }
      iVar3 = iVar3 + 1;
    }
    iVar3 = (iVar2 + 1) % 8;
    iVar2 = (iVar2 + 7) % 8;
    if (kind != 0) {
      if ((*(char *)((int)DAT_451b_002a + local_e._2_2_ * 0x70 + (int)local_e + 0x6120) == '\x06')
         || (*(char *)((int)DAT_451b_002a + local_e._2_2_ * 0x70 + (int)local_e + 0x6120) == '\x05')
         ) {
        local_16 = local_e;
        FUN_4bed_0470(&local_16);
        *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 7;
        FUN_4bed_03aa(local_16);
        uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar4 = (int)DAT_451b_002a + iVar3 * 4;
        local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar4 + 0xbe),
                            (int)local_e + *(int *)(iVar4 + 0xbc));
        FUN_4bed_0470(&local_16);
        *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 7;
        FUN_4bed_03aa(local_16);
        uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar4 = (int)DAT_451b_002a + iVar2 * 4;
        local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar4 + 0xbe),
                            (int)local_e + *(int *)(iVar4 + 0xbc));
        FUN_4bed_0470(&local_16);
        *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 7;
        FUN_4bed_03aa(local_16);
        uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar4 = (int)DAT_451b_002a + iVar3 * 4;
        local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar4 + 0xbe) * 2,
                            (int)local_e + *(int *)(iVar4 + 0xbc) * 2);
        FUN_4bed_0470(&local_16);
        *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 7;
        FUN_4bed_03aa(local_16);
        uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar4 = (int)DAT_451b_002a + iVar2 * 4;
        local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar4 + 0xbe) * 2,
                            (int)local_e + *(int *)(iVar4 + 0xbc) * 2);
        FUN_4bed_0470(&local_16);
        *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 7;
        FUN_4bed_03aa(local_16);
      }
    }
    if (kind == 0) {
      if (*(char *)((int)DAT_451b_002a + local_e._2_2_ * 0x70 + (int)local_e + 0x6120) == '\x06') {
        local_16 = local_e;
        FUN_4bed_0470(&local_16);
        *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 5;
        FUN_4bed_03aa(local_16);
        uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar4 = (int)DAT_451b_002a + iVar3 * 4;
        local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar4 + 0xbe),
                            (int)local_e + *(int *)(iVar4 + 0xbc));
        FUN_4bed_0470(&local_16);
        *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 5;
        FUN_4bed_03aa(local_16);
        uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar4 = (int)DAT_451b_002a + iVar2 * 4;
        local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar4 + 0xbe),
                            (int)local_e + *(int *)(iVar4 + 0xbc));
        FUN_4bed_0470(&local_16);
        *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 5;
        FUN_4bed_03aa(local_16);
        uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar3 = (int)DAT_451b_002a + iVar3 * 4;
        local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar3 + 0xbe) * 2,
                            (int)local_e + *(int *)(iVar3 + 0xbc) * 2);
        FUN_4bed_0470(&local_16);
        *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 5;
        FUN_4bed_03aa(local_16);
        uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar2 = (int)DAT_451b_002a + iVar2 * 4;
        local_16 = CONCAT22(local_e._2_2_ + *(int *)(iVar2 + 0xbe) * 2,
                            (int)local_e + *(int *)(iVar2 + 0xbc) * 2);
        FUN_4bed_0470(&local_16);
        *(undefined1 *)((int)DAT_451b_002a + local_16._2_2_ * 0x70 + (int)local_16 + 0x6120) = 5;
        FUN_4bed_03aa(local_16);
      }
    }
    iVar2 = (int)local_e + local_12;
    local_e._2_2_ = local_e._2_2_ + local_10;
    local_e = CONCAT22(local_e._2_2_,iVar2);
    if ((iVar2 == (int)local_a) && (local_e._2_2_ == local_a._2_2_)) {
      bVar1 = true;
    }
  }
  return;
}


