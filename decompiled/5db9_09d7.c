
void __cdecl16far ai_record_battle(int param_1,int param_2,undefined4 param_3,undefined4 *param_4)

{
  char cVar1;
  char cVar2;
  undefined4 uVar3;
  byte bVar4;
  int iVar5;
  undefined2 in_DX;
  int iVar6;
  int iVar7;
  int local_a;
  
  local_a = 0;
  iVar7 = 0;
  if (DAT_4125_0832 == 0) {
    auto_str_no_room_ai_memory(1);
  }
  bVar4 = *(byte *)(param_2 * 0xe0 + param_1 * 2 + 1) & 0xf;
  if (bVar4 != 0xf) {
    cVar1 = *(char *)(*(byte *)(param_2 * 0xe0 + param_1 * 2) + 0x710);
    for (iVar6 = 0; iVar6 < combat_def_count; iVar6 = iVar6 + 1) {
      uVar3 = *(undefined4 *)((int)param_3 + iVar6 * 4);
      if ((*(char *)((int)uVar3 + 4) == '\x1c') &&
         (((undefined1 *)&combat_def_alive)[iVar6] == '\0')) {
        local_a = local_a + 1;
      }
      if (((undefined1 *)&combat_def_alive)[iVar6] == '\0') {
        iVar7 = iVar7 + 1;
      }
    }
    cVar2 = *(char *)((int)*param_4 + 5);
    iVar6 = (int)combat_def_count;
    iVar5 = FUN_5db9_056c(bVar4);
    iVar5 = iVar5 + cVar2 * 2;
    *(int *)(iVar5 + 0x3bc) = *(int *)(iVar5 + 0x3bc) + local_a;
    *(int *)(iVar5 + 0x3cc) = *(int *)(iVar5 + 0x3cc) + iVar7;
    *(int *)(iVar5 + 0x3dc) = *(int *)(iVar5 + 0x3dc) + 1;
    *(int *)(iVar5 + 0x3ec) = *(int *)(iVar5 + 0x3ec) + (uint)(iVar6 == iVar7);
    if (cVar1 == '\n') {
      *(int *)(iVar5 + 0x3fc) = *(int *)(iVar5 + 0x3fc) + 1;
      *(int *)(iVar5 + 0x40c) = *(int *)(iVar5 + 0x40c) + (uint)(iVar6 == iVar7);
    }
    FUN_5db9_05b9(bVar4);
  }
  return;
}


