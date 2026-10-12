
void __cdecl16far FUN_6a35_0160(int param_1)

{
  char cVar1;
  char cVar2;
  undefined2 uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  undefined2 uVar9;
  undefined1 local_12 [8];
  undefined4 local_a;
  uint local_6;
  uint local_4;
  
  FUN_6a35_041c();
  cVar2 = combat_def_count;
  if (param_1 != 0) {
    cVar2 = *(char *)((int)DAT_451b_027d + 0x14);
  }
  local_6 = (uint)cVar2;
  local_4 = (uint)combat_atk_count;
  iVar8 = 0;
  if (param_1 != 0) {
    DAT_451b_029b = '\b';
  }
  DAT_4125_0d44 = local_4 + local_6;
  for (iVar7 = 0; iVar7 < 0x28; iVar7 = iVar7 + 1) {
    ((undefined2 *)&DAT_451b_0458)[iVar7] = 0xffff;
    ((undefined2 *)&DAT_451b_0408)[iVar7] = 0xffff;
  }
  while (local_6 != 0) {
    if ((int)local_6 < 8) {
      iVar7 = 4 - (int)(local_6 + 1) / 2;
      uVar5 = local_6;
      if (local_6 != 0) {
        while (uVar6 = uVar5 - 1, uVar5 != 0) {
          if ((local_6 & 1) == 0) {
            uVar3 = *(undefined2 *)((iVar7 + uVar6) * 2 + 0xd1e);
          }
          else {
            uVar3 = *(undefined2 *)((iVar7 + uVar6) * 2 + 0xd2e);
          }
          ((undefined2 *)&DAT_451b_0458)[iVar8 + uVar6] = uVar3;
          ((undefined2 *)&DAT_451b_0408)[iVar8 + uVar6] = *(undefined2 *)((iVar8 / 8) * 2 + 0xd14);
          uVar5 = uVar6;
        }
      }
      iVar8 = iVar8 + local_6;
      local_6 = 0;
    }
    else {
      iVar7 = 8;
      while (iVar4 = iVar7 + -1, iVar7 != 0) {
        ((undefined2 *)&DAT_451b_0458)[iVar8 + iVar4] = *(undefined2 *)(iVar4 * 2 + 0xd1e);
        ((undefined2 *)&DAT_451b_0408)[iVar8 + iVar4] = *(undefined2 *)((iVar8 / 8) * 2 + 0xd14);
        iVar7 = iVar4;
      }
      iVar8 = iVar8 + 8;
      local_6 = local_6 - 8;
    }
  }
  iVar8 = 4 - (int)(local_4 + 1) / 2;
  uVar5 = local_4;
  if (local_4 != 0) {
    while (uVar6 = uVar5 - 1, uVar5 != 0) {
      if ((local_4 & 1) == 0) {
        uVar3 = *(undefined2 *)((iVar8 + uVar6) * 2 + 0xd1e);
      }
      else {
        uVar3 = *(undefined2 *)((iVar8 + uVar6) * 2 + 0xd2e);
      }
      ((undefined2 *)&DAT_451b_0458)[uVar5 + 0x1f] = uVar3;
      ((undefined2 *)&DAT_451b_0408)[uVar5 + 0x1f] = DAT_4125_0d1c;
      uVar5 = uVar6;
    }
  }
  for (iVar8 = 0; iVar8 < 0x28; iVar8 = iVar8 + 1) {
    if (((undefined2 *)&DAT_451b_0458)[iVar8] != -1) {
      if (iVar8 < 0x20) {
        if (((undefined1 *)&DAT_451b_02a4)[iVar8] != '\0') {
          FUN_216d_000e(&local_a);
          FUN_216d_002e(local_12);
          __F_SPUSH(local_a);
          FUN_1997_0129();
        }
        uVar3 = ((undefined2 *)&DAT_451b_0408)[iVar8];
        uVar9 = ((undefined2 *)&DAT_451b_0458)[iVar8];
        cVar2 = ((undefined1 *)&combat_def_type)[iVar8];
        cVar1 = DAT_451b_029b;
      }
      else {
        if (((undefined1 *)&DAT_451b_027c)[iVar8] != '\0') {
          FUN_216d_000e(&local_a);
          FUN_216d_002e(local_12);
          __F_SPUSH(local_a);
          FUN_1997_0129();
        }
        uVar3 = ((undefined2 *)&DAT_451b_0408)[iVar8];
        uVar9 = ((undefined2 *)&DAT_451b_0458)[iVar8];
        cVar2 = ((undefined1 *)&combat_def_alive)[iVar8];
        cVar1 = DAT_451b_029a;
      }
      FUN_8611_08be((int)cVar2,(int)cVar1,0x342a,0x4125,uVar9,uVar3,0,0,0,0);
    }
  }
  return;
}


