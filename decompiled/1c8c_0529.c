
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far stack_movement_mode(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 *puVar4;
  int iVar5;
  undefined2 uVar6;
  undefined1 local_ec [200];
  undefined1 local_24 [16];
  uint local_14;
  int local_12;
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  local_4 = 1;
  local_6 = 1;
  local_8 = 0;
  local_a = 0;
  local_c = 0;
  local_e = 0;
  local_10 = 0;
  local_12 = 0;
  DAT_451b_1e9e = 0;
  DAT_451b_1ea0 = 100;
  uVar6 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
  puVar4 = (undefined2 *)DAT_451b_1f02;
  if (*(char *)(puVar4 + 2) != '\x1c') {
    uVar6 = 0;
    puVar4 = (undefined2 *)0x0;
  }
  _DAT_451b_1efe = (undefined2 *)CONCAT22(uVar6,puVar4);
  for (iVar5 = 0; iVar5 < 8; iVar5 = iVar5 + 1) {
    if (((undefined2 *)&DAT_451b_1ede)[iVar5 * 2] != 0 ||
        *(int *)((undefined *)&DAT_451b_1ee0 + iVar5 * 4) != 0) {
      uVar6 = (undefined2)((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar5 * 2) >> 0x10)
      ;
      iVar1 = (int)*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar5 * 2) + 4);
      iVar2 = (int)*(char *)(((undefined2 *)&DAT_451b_1ede)[iVar5 * 2] + 7);
      local_14 = *(uint *)(((undefined2 *)&DAT_451b_1ede)[iVar5 * 2] + 0xc) & 0x1000;
      DAT_451b_1e9e = DAT_451b_1e9e + 1;
      iVar3 = (int)(char)DAT_451b_1ea0;
      if (iVar2 < iVar3) {
        DAT_451b_1ea0 = iVar2;
      }
      if ((iVar1 == 0x1c) &&
         (iVar2 = hero_flight_items(*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar5 * 2)),
         iVar2 != 0)) {
        local_a = 1;
      }
      if ((((undefined1 *)&army_move_flags)[iVar1 * 6] == '\0') || (local_14 != 0)) {
        local_4 = 0;
      }
      if (local_14 != 0) {
        local_c = 1;
      }
      if ((iVar1 != 0x1c) && (((undefined1 *)&army_move_flags)[iVar1 * 6] == '\0')) {
        local_6 = 0;
      }
      if ((((undefined1 *)&army_move_flags)[iVar1 * 6] != '\0') && (local_14 == 0)) {
        local_8 = 1;
      }
      if (((undefined1 *)&DAT_451b_0667)[iVar1 * 6] != '\0') {
        local_e = 1;
      }
      if (((undefined1 *)&DAT_451b_0666)[iVar1 * 6] != '\0') {
        local_12 = 1;
      }
      if (((undefined1 *)&DAT_451b_0665)[iVar1 * 6] != '\0') {
        local_10 = 1;
      }
      if ((_DAT_451b_1efe == (undefined2 *)0x0) && (iVar1 == 0x1c)) {
        _DAT_451b_1efe =
             (undefined2 *)
             CONCAT22(*(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar5 * 4),
                      (undefined2 *)((undefined2 *)&DAT_451b_1ede)[iVar5 * 2]);
      }
    }
  }
  DAT_4125_2bda = (uint)(1 < DAT_451b_1e9e);
  DAT_451b_1e9c = 1;
  DAT_451b_1e9a = 0;
  if (local_c == 0) {
    if (local_e == 0) {
      if (((local_a == 0) && (local_4 == 0)) && ((local_6 == 0 || (local_8 == 0)))) {
        DAT_451b_1e9a = (uint)(local_12 != 0);
        if (local_10 != 0) {
          DAT_451b_1e9a = DAT_451b_1e9a | 2;
        }
      }
      else {
        DAT_451b_1e9c = 2;
      }
    }
    else {
      DAT_451b_1e9c = 0;
    }
  }
  else {
    DAT_451b_1e9a = 8;
  }
  iVar5 = 8;
  while( true ) {
    iVar1 = iVar5 + -1;
    puVar4 = (undefined2 *)DAT_451b_1f02;
    uVar6 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
    if (iVar5 == 0) break;
    iVar5 = iVar1;
    if (((undefined2 *)&DAT_451b_1ede)[iVar1 * 2] != 0 ||
        *(int *)((undefined *)&DAT_451b_1ee0 + iVar1 * 4) != 0) {
      *(undefined2 *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar1 * 2) + 0x12) =
           puVar4[9];
      *(undefined2 *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar1 * 2) + 0x14) =
           ((undefined2 *)DAT_451b_1f02)[10];
    }
  }
  if (((*(int *)(current_player * 2 + 0xd0) == 0) && (-1 < (int)puVar4[9])) &&
     (-1 < (int)puVar4[10])) {
    FUN_1555_1868(local_24);
    pathfind(local_24);
    DAT_4125_2eaa = path_step_costs(local_24);
    local_14 = DAT_4125_2eaa;
    FUN_1c8c_0963(*DAT_451b_1f02,((undefined2 *)DAT_451b_1f02)[1],local_ec);
  }
  else {
    DAT_4125_2eaa = 0;
    DAT_4125_2ea6 = 0;
    DAT_4125_2ea8 = 0;
  }
  return;
}


