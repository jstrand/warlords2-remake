
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far ruin_search(undefined4 param_1,int param_2,int param_3)

{
  byte *pbVar1;
  int *piVar2;
  char cVar3;
  int iVar4;
  undefined2 uVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 in_DX;
  bool bVar9;
  char *pcVar10;
  undefined2 uVar11;
  int local_4;
  
  iVar4 = is_computer_turn();
  bVar9 = iVar4 == 0;
  uVar5 = FUN_828e_04b5(param_2,param_3);
  FUN_828e_0af3();
  if (bVar9) {
    FUN_22bf_07a5();
    FUN_67cc_1a41(*_DAT_451b_1efe,((undefined2 *)_DAT_451b_1efe)[1],0);
    FUN_6536_175b(uVar5);
    sound_dramatic(0);
    FUN_22bf_0757();
  }
  hero_add_experience((undefined2 *)_DAT_451b_1efe,(int)((ulong)_DAT_451b_1efe >> 0x10),3);
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  iVar4 = (int)param_1;
  if (*(char *)(iVar4 + 0x18) != '\x05') {
    if (*(char *)(iVar4 + 0x1a) == '\0') {
      if (bVar9) {
        pcVar10 = get_string(0x32,0);
        in_DX = (undefined2)((ulong)pcVar10 >> 0x10);
        FUN_6536_17c9(pcVar10);
        FUN_8065_10fb(0,0);
      }
    }
    else {
      if (bVar9) {
        iVar7 = *(char *)(iVar4 + 0x1a) * 0x10 + 0xf67;
        uVar11 = 0x3c04;
        iVar6 = *(char *)((undefined2 *)_DAT_451b_1efe + 5) * 0x14 + 0x223;
        uVar8 = 0x3c04;
        pcVar10 = get_string(0x33,0);
        in_DX = (undefined2)((ulong)pcVar10 >> 0x10);
        sprintf(&DAT_451b_1cd8,pcVar10,iVar6,uVar8,iVar7,uVar11);
        FUN_6536_17c9((undefined1 *)&DAT_451b_1cd8,0x4125);
        FUN_8065_10fb(0,0);
        sound_dramatic(1);
      }
      iVar7 = guardian_fight_hero_dies(iVar4,uVar5);
      if (iVar7 != 0) {
        FUN_6d51_1244(current_player,1,0xfffe,0,
                      *(char *)((undefined2 *)_DAT_451b_1efe + 5) * 0x14 + 0x223,0x3c04);
        *(int *)((undefined1 *)&LAB_4125_1166_1 + current_player * 0x5a) =
             *(int *)((undefined1 *)&LAB_4125_1166_1 + current_player * 0x5a) + 1;
        list_carried_items(2,0,0,(undefined2 *)_DAT_451b_1efe,(int)((ulong)_DAT_451b_1efe >> 0x10));
        for (iVar4 = 0; iVar4 < DAT_451b_08ce; iVar4 = iVar4 + 1) {
          if ((int)((undefined2 *)&DAT_451b_0c9c)[iVar4] < 8) {
            piVar2 = (int *)(current_player * 0x5a + 0x116b);
            *piVar2 = *piVar2 + 1;
          }
        }
        uVar5 = (undefined2)((ulong)_DAT_451b_1efe >> 0x10);
        hero_drop_items((undefined2 *)_DAT_451b_1efe,uVar5,*_DAT_451b_1efe,
                        ((undefined2 *)_DAT_451b_1efe)[1]);
        FUN_1b62_03be((undefined2 *)_DAT_451b_1efe,(int)((ulong)_DAT_451b_1efe >> 0x10));
        _DAT_451b_1efe = (undefined2 *)0x0;
        refresh_map_owners();
        if (!bVar9) {
          return;
        }
        pcVar10 = get_string(0x33,1);
        FUN_6536_17c9(pcVar10);
        FUN_6536_1812(0);
        return;
      }
      if (bVar9) {
        pcVar10 = get_string(0x33,2);
        in_DX = (undefined2)((ulong)pcVar10 >> 0x10);
        FUN_6536_17c9(pcVar10);
        FUN_8065_10fb(0,0);
      }
      *(undefined1 *)(iVar4 + 0x1a) = 0;
    }
  }
  cVar3 = *(char *)(iVar4 + 0x18);
  if (cVar3 == '\x02') {
    if (bVar9) {
      iVar7 = *(char *)(iVar4 + 0x19) * 0x1d + 0xce9;
      uVar11 = 0x3c04;
      iVar6 = *(char *)((undefined2 *)_DAT_451b_1efe + 5) * 0x14 + 0x223;
      uVar8 = 0x3c04;
      pcVar10 = get_string(0x34,0);
      sprintf(&DAT_451b_1cd8,pcVar10,iVar6,uVar8,iVar7,uVar11);
      FUN_6536_17c9((undefined1 *)&DAT_451b_1cd8,0x4125);
      FUN_6536_1812(1);
      *(undefined1 *)(*(char *)(iVar4 + 0x19) * 0x1d + 0xcff) = 1;
      *(undefined2 *)(*(char *)(iVar4 + 0x19) * 0x1d + 0xd00) = 0;
      *(int *)(*(char *)(iVar4 + 0x19) * 0x1d + 0xd02) = param_2;
      *(int *)(*(char *)(iVar4 + 0x19) * 0x1d + 0xd04) = param_3;
    }
    else {
      *(undefined1 *)(*(char *)(iVar4 + 0x19) * 0x1d + 0xcff) = 3;
      uVar8 = FUN_1b62_0044((undefined2 *)_DAT_451b_1efe,(int)((ulong)_DAT_451b_1efe >> 0x10));
      *(undefined2 *)(*(char *)(iVar4 + 0x19) * 0x1d + 0xd00) = uVar8;
      *(undefined2 *)(*(char *)(iVar4 + 0x19) * 0x1d + 0xd02) = 0;
      *(undefined2 *)(*(char *)(iVar4 + 0x19) * 0x1d + 0xd04) = 0;
    }
    FUN_6d51_1244(current_player,6,(int)*(char *)(iVar4 + 0x19),0,
                  *(char *)((undefined2 *)_DAT_451b_1efe + 5) * 0x14 + 0x223,0x3c04);
    pbVar1 = (byte *)(param_3 * 0xe0 + param_2 * 2 + 1);
    *pbVar1 = *pbVar1 | 0x40;
    *(undefined1 *)(iVar4 + 0x18) = 0;
  }
  else {
    if (cVar3 == '\x04') {
      if (*(int *)(iVar4 + 0x1b) == 0) {
        iVar4 = dice(3,500,500);
      }
      else {
        iVar4 = dice(3,1000,1000);
      }
      pbVar1 = (byte *)(param_3 * 0xe0 + param_2 * 2 + 1);
      *pbVar1 = *pbVar1 | 0x40;
      piVar2 = (int *)(current_player * 0x14 + 0x185);
      *piVar2 = *piVar2 + iVar4;
      if (bVar9) {
        iVar7 = *(char *)((undefined2 *)_DAT_451b_1efe + 5) * 0x14 + 0x223;
        uVar5 = 0x3c04;
        pcVar10 = get_string(0x35,0);
        sprintf(&DAT_451b_1cd8,pcVar10,iVar7,uVar5,iVar4);
        FUN_6536_17c9((undefined1 *)&DAT_451b_1cd8,0x4125);
        FUN_6536_1812(0);
      }
      cVar3 = *(char *)((undefined2 *)_DAT_451b_1efe + 5);
      uVar5 = 0x66;
    }
    else {
      if (cVar3 != '\x05') {
        return;
      }
      pbVar1 = (byte *)(param_3 * 0xe0 + param_2 * 2 + 1);
      *pbVar1 = *pbVar1 | 0x40;
      local_4 = dice(1,2,0);
      if (*(int *)(iVar4 + 0x1b) != 0) {
        local_4 = local_4 + 2;
      }
      uVar8 = (undefined2)((ulong)_DAT_451b_1efe >> 0x10);
      if (local_4 < 2) {
        uVar8 = FUN_7087_14f4((int)*(char *)(iVar4 + 0x1a),
                              *(char *)((undefined2 *)_DAT_451b_1efe + 5) * 0x14 + 0x223,0x3c04);
        pcVar10 = get_string(0x36,0);
        uVar11 = (undefined2)((ulong)pcVar10 >> 0x10);
        sprintf(&DAT_451b_1cd8,pcVar10,uVar8,in_DX);
      }
      else {
        uVar8 = FUN_7087_14f4((int)*(char *)(iVar4 + 0x1a),
                              *(char *)((undefined2 *)_DAT_451b_1efe + 5) * 0x14 + 0x223,0x3c04);
        iVar7 = local_4;
        pcVar10 = get_string(0x36,1);
        uVar11 = (undefined2)((ulong)pcVar10 >> 0x10);
        sprintf(&DAT_451b_1cd8,pcVar10,iVar7,uVar8,in_DX);
      }
      if (bVar9) {
        FUN_6536_17c9((undefined1 *)&DAT_451b_1cd8,0x4125);
        FUN_6536_1812(0);
      }
      pbVar1 = (byte *)(param_3 * 0xe0 + param_2 * 2 + 1);
      *pbVar1 = *pbVar1 | 0x40;
      cVar3 = *(char *)(iVar4 + 0x1a);
      for (iVar4 = 0; iVar4 < local_4; iVar4 = iVar4 + 1) {
        uVar5 = create_ally_army((int)cVar3,param_2,param_3);
        FUN_1b62_02e2(uVar5,uVar11);
      }
      cVar3 = *(char *)((undefined2 *)_DAT_451b_1efe + 5);
      uVar5 = 100;
    }
    FUN_6d51_1244(current_player,6,uVar5,0,cVar3 * 0x14 + 0x223,0x3c04);
  }
  return;
}


