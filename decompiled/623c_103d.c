
int __cdecl16far best_production_for(int city,int purpose,int arg,int dry_run)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  int local_a;
  int local_8;
  int local_6;
  
  local_6 = 0;
  local_8 = -1;
  iVar4 = 4;
  while (iVar5 = iVar4 + -1, iVar4 != 0) {
    iVar1 = (int)*(char *)(city * 0x41 + iVar5 + 0x1593);
    iVar4 = iVar5;
    if (-1 < iVar1) {
      iVar2 = (int)*(char *)(city * 0x41 + iVar5 + 0x1597);
      if (*(int *)(current_player * 2 + 0xf0) == 0) {
        iVar3 = 0;
      }
      else {
        iVar3 = 2;
      }
      if (*(char *)(city * 0x41 + iVar5 + 0x159b) + iVar3 < 10) {
        if (*(int *)(current_player * 2 + 0xf0) == 0) {
          local_a = 0;
        }
        else {
          local_a = 2;
        }
        local_a = *(char *)(city * 0x41 + iVar5 + 0x159b) + local_a;
      }
      else {
        local_a = 9;
      }
      if (((undefined1 *)&DAT_451b_0669)[iVar1 * 6] == '\x01') {
        local_a = local_a + 2;
      }
      if ((purpose != 6) && (local_a < 3)) {
        iVar2 = iVar2 + 1;
      }
      if (10 < iVar2) {
        iVar2 = 10;
      }
      if (((((undefined1 *)&army_move_flags)[iVar1 * 6] != '\0') || (purpose != 4)) &&
         (iVar5 = (10 - iVar2) * *(int *)(purpose * 2 + 0xa10) +
                  local_a * *(int *)(purpose * 2 + 0xa1e) +
                  ((int)*(char *)(city * 0x41 + iVar5 + 0x159f) * *(int *)(purpose * 2 + 0xa2c)) / 2
         , local_6 < iVar5)) {
        local_8 = iVar1;
        local_6 = iVar5;
      }
    }
  }
  if (dry_run == 0) {
    if (local_8 == -1) {
      local_8 = 0;
    }
    else {
      if (arg == -1) {
        uVar6 = 0;
      }
      else {
        uVar6 = CONCAT22(arg,1);
      }
      set_city_production(city,local_8,uVar6);
      local_8 = 1;
    }
  }
  return local_8;
}


