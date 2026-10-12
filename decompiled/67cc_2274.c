
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_ui_medal_effect(int param_1)

{
  char cVar1;
  bool bVar2;
  bool bVar3;
  char cVar4;
  undefined2 uVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  int iVar9;
  int iVar10;
  undefined2 uVar11;
  char *pcVar12;
  undefined4 uVar13;
  undefined2 local_8;
  undefined2 uStack_6;
  
  iVar9 = 0;
  bVar2 = false;
  for (iVar6 = 0; iVar6 < 8; iVar6 = iVar6 + 1) {
    if (((undefined2 *)&DAT_451b_1ede)[iVar6 * 2] != 0 ||
        *(int *)((undefined *)&DAT_451b_1ee0 + iVar6 * 4) != 0) {
      uVar11 = (undefined2)
               ((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar6 * 2) >> 0x10);
      if ((*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar6 * 2) + 4) != '\x1c')
         && (((undefined1 *)&DAT_451b_0668)
             [*(char *)(((undefined2 *)&DAT_451b_1ede)[iVar6 * 2] + 4) * 6] == '\0')) {
        iVar9 = iVar9 + 1;
      }
      if ((*(uint *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar6 * 2) + 0xc) & 0x1000)
          != 0) {
        bVar2 = true;
      }
    }
  }
  iVar10 = 0;
  iVar6 = army_count;
  while (iVar7 = iVar6 + -1, iVar6 != 0) {
    iVar6 = iVar7;
    if ((((*(char *)(iVar7 * 0x16 + -0x777b) == current_player) &&
         (*(char *)(iVar7 * 0x16 + -0x777c) != '\x1c')) &&
        (((undefined1 *)&DAT_451b_0668)[*(char *)(iVar7 * 0x16 + -0x777c) * 6] == '\0')) &&
       ('\0' < *(char *)(iVar7 * 0x16 + -0x7776))) {
      iVar10 = iVar10 + 1;
    }
  }
  if (iVar10 * -5 + 0x28 < 0xc) {
    iVar6 = 0xc;
  }
  else {
    iVar6 = iVar10 * -5 + 0x28;
  }
  if ((combat_atk_count < '\x02') || (combat_def_count < '\x02')) {
    bVar3 = false;
  }
  else {
    bVar3 = true;
  }
  iVar10 = game_turn();
  if (((iVar10 < 0x14) && (!bVar3)) && (('\0' < combat_def_count && ('\x03' < combat_def_str)))) {
    bVar3 = true;
  }
  if ((((!bVar2) && (param_1 == 1)) && (bVar3)) &&
     (((0 < iVar9 && (iVar10 = dice(1,100,0), iVar10 < iVar6)) && (_DAT_451b_1efe != 0)))) {
    iVar9 = dice(1,iVar9,-1);
    iVar6 = 0;
    for (iVar10 = 0; iVar10 < 8; iVar10 = iVar10 + 1) {
      iVar7 = iVar6;
      if (((((undefined2 *)&DAT_451b_1ede)[iVar10 * 2] != 0 ||
            *(int *)((undefined *)&DAT_451b_1ee0 + iVar10 * 4) != 0) &&
          (uVar11 = (undefined2)
                    ((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar10 * 2) >> 0x10),
          *(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar10 * 2) + 4) != '\x1c'))
         && ((((undefined1 *)&DAT_451b_0668)
              [*(char *)(((undefined2 *)&DAT_451b_1ede)[iVar10 * 2] + 4) * 6] == '\0' &&
             (iVar7 = iVar6 + 1, iVar6 == iVar9)))) {
        _local_8 = CONCAT22(*(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar10 * 4),
                            ((undefined2 *)&DAT_451b_1ede)[iVar10 * 2]);
        break;
      }
      iVar6 = iVar7;
    }
    uVar11 = (undefined2)((ulong)_local_8 >> 0x10);
    iVar6 = (int)_local_8;
    if (*(char *)(iVar6 + 10) < '\x04') {
      *(char *)(iVar6 + 10) = *(char *)(iVar6 + 10) + '\x01';
      bVar2 = '\x0e' < *(char *)(iVar6 + 8);
      if (bVar2) {
        *(char *)(iVar6 + 6) = *(char *)(iVar6 + 6) + '\x01';
      }
      else {
        army_add_strength();
      }
      if (*(int *)(DAT_451b_029a * 2 + 0xd0) == 0) {
        music_cue(8);
        FUN_22bf_07a5();
        FUN_54f6_0000();
        FUN_8611_08be((int)*(char *)(iVar6 + 4),(int)*(char *)(iVar6 + 5),0x342a,0x4125,0xb0,0x9b,0,
                      0,0);
        FUN_78a8_06ae(0xf0002,0x30000);
        pcVar12 = get_string(0x9e,-1);
        uVar8 = (undefined2)((ulong)pcVar12 >> 0x10);
        FUN_7ecb_00bf(0x9b00d8,pcVar12);
        uVar5 = FUN_7087_14f4();
        uVar13 = CONCAT22(uVar8,uVar5);
        pcVar12 = get_string(0x9f,0);
        sprintf(&DAT_451b_1d28,pcVar12,uVar13);
        FUN_7ecb_00bf(0xaf00d8,(undefined1 *)&DAT_451b_1d28,0x4125);
        pcVar12 = get_string(0xa0,-1);
        FUN_7ecb_00bf(0xc300b0,(char *)pcVar12);
        pcVar12 = get_string(0xa1,*(char *)(iVar6 + 10) + -1);
        FUN_7ecb_00bf(0xd700b0,(char *)pcVar12,(int)((ulong)pcVar12 >> 0x10));
        if (bVar2) {
          pcVar12 = get_string(0xa2,1);
          FUN_7ecb_00bf(0xf500b0,(char *)pcVar12,(int)((ulong)pcVar12 >> 0x10));
          cVar1 = *(char *)(iVar6 + 6);
          cVar4 = *(char *)(iVar6 + 6);
        }
        else {
          pcVar12 = get_string(0xa2,0);
          FUN_7ecb_00bf(0xf500b0,(char *)pcVar12,(int)((ulong)pcVar12 >> 0x10));
          cVar1 = *(char *)(iVar6 + 8);
          cVar4 = *(char *)(iVar6 + 8);
        }
        uVar13 = CONCAT22((int)cVar1,cVar4 + -1);
        pcVar12 = get_string(0xa2,2);
        sprintf(&DAT_451b_1d28,pcVar12,uVar13);
        FUN_7ecb_00bf(0x10900b0,(undefined1 *)&DAT_451b_1d28,0x4125);
        FUN_22bf_0757();
        FUN_8065_10fb(0);
        FUN_54f6_069c();
        music_cue(1);
      }
    }
  }
  return;
}


