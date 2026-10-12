
void __cdecl16far advisor_speak(int what)

{
  char cVar1;
  int iVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  undefined2 in_DX;
  int *piVar6;
  undefined2 unaff_SI;
  int iVar7;
  undefined2 unaff_DI;
  undefined2 uVar8;
  int unaff_SS;
  char *pcVar9;
  undefined4 uVar10;
  undefined1 local_24;
  undefined4 local_1e;
  undefined1 local_1a [8];
  undefined1 local_12 [8];
  undefined2 local_a;
  undefined2 local_8;
  undefined2 local_6;
  undefined2 local_4;
  
  uVar10 = CONCAT22(unaff_SI,unaff_DI);
  local_4 = 0x98;
  local_6 = 0xf;
  local_8 = 0x1ba;
  local_a = 0x1ba;
  if (DAT_4125_26c8 != 0) {
    return;
  }
  FUN_7ecb_0a1b();
  iVar2 = current_player;
  if (what == 5) {
    if (0x27 < (int)((undefined2 *)&side_city_count)[current_player]) {
      return;
    }
    iVar7 = 0;
    iVar4 = army_count;
    while (iVar5 = iVar4 + -1, iVar4 != 0) {
      iVar4 = iVar5;
      if ((*(char *)(iVar5 * 0x16 + -0x777c) == '\x1c') &&
         (*(char *)(iVar5 * 0x16 + -0x777b) == current_player)) {
        iVar7 = iVar7 + 1;
      }
    }
    if (((*(char *)(current_player + 0x100) == '\x02') &&
        ((int)((undefined2 *)&side_city_count)[current_player] <
         (int)*(char *)(current_player + 0x108))) ||
       ((*(char *)(current_player + 0x100) != '\x02' &&
        ((int)((undefined2 *)&side_city_count)[current_player] <
         (int)*(char *)(current_player + 0x108))))) {
      iVar4 = ((int)((undefined2 *)&side_city_count)[current_player] / 5) * 5;
      *(undefined1 *)(current_player + 0x100) = 2;
      local_24 = (undefined1)iVar4;
      *(undefined1 *)(iVar2 + 0x108) = local_24;
      iVar2 = 6;
      piVar6 = (int *)0xb3b;
      do {
        if (*piVar6 == iVar4 + 5) {
                    /* WARNING: Could not recover jumptable at 0x0007e172. Too many branches */
                    /* WARNING: Treating indirect jump as call */
          (*(code *)piVar6[6])();
          return;
        }
        piVar6 = piVar6 + 1;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
      if (DAT_4125_37a4 == 0) {
        uVar10 = 0xffff004c;
      }
      else {
        uVar10 = 0xffff002e;
      }
LAB_7dda_03e3:
      pcVar9 = get_file_string((int)uVar10,(int)((ulong)uVar10 >> 0x10));
      pcVar9 = strcpy((char *)CONCAT22(unaff_SS,&stack0xff86),pcVar9);
      in_DX = (undefined2)((ulong)pcVar9 >> 0x10);
    }
    else {
      if (((*(char *)(current_player + 0x100) == '\x02') &&
          (*(char *)(current_player + 0x108) + 5 <=
           (int)((undefined2 *)&side_city_count)[current_player])) ||
         ((*(char *)(current_player + 0x100) != '\x02' &&
          (*(char *)(current_player + 0x108) + 5 <=
           (int)((undefined2 *)&side_city_count)[current_player])))) {
        iVar4 = ((int)((undefined2 *)&side_city_count)[current_player] / 5) * 5;
        *(undefined1 *)(current_player + 0x100) = 1;
        local_24 = (undefined1)iVar4;
        *(undefined1 *)(iVar2 + 0x108) = local_24;
        iVar2 = 6;
        piVar6 = (int *)0xb23;
        do {
          if (*piVar6 == iVar4) {
                    /* WARNING: Could not recover jumptable at 0x0007e306. Too many branches */
                    /* WARNING: Treating indirect jump as call */
            (*(code *)piVar6[6])();
            return;
          }
          piVar6 = piVar6 + 1;
          iVar2 = iVar2 + -1;
        } while (iVar2 != 0);
        if (DAT_4125_37a4 == 0) {
          uVar10 = 0xffff004d;
        }
        else {
          uVar10 = 0xffff002f;
        }
        goto LAB_7dda_03e3;
      }
      iVar2 = game_turn(uVar10);
      if (iVar2 % 7 != 0) {
        return;
      }
      if (*(int *)(current_player * 0x14 + 0x185) < 100) {
        if (DAT_4125_37a4 == 0) {
          uVar10 = 0xffff0054;
        }
        else {
          uVar10 = 0xffff0036;
        }
        goto LAB_7dda_03e3;
      }
      if (0xaf0 < *(int *)(current_player * 0x14 + 0x185)) {
        if (DAT_4125_37a4 == 0) {
          uVar10 = 0xffff0055;
        }
        else {
          uVar10 = 0xffff0037;
        }
        goto LAB_7dda_03e3;
      }
      if (iVar7 < 1) {
        if (DAT_4125_37a4 == 0) {
          uVar10 = 0xffff0056;
        }
        else {
          uVar10 = 0xffff0038;
        }
        goto LAB_7dda_03e3;
      }
      if (4 < iVar7) {
        if (DAT_4125_37a4 == 0) {
          uVar10 = 0xffff0057;
        }
        else {
          uVar10 = 0xffff0039;
        }
        goto LAB_7dda_03e3;
      }
      iVar2 = dice(1,5,0);
      if (iVar2 != 1) {
        return;
      }
      if (DAT_4125_37a4 == 0) {
        uVar10 = 0xffff0058;
      }
      else {
        uVar10 = 0xffff003a;
      }
      pcVar9 = get_file_string((int)uVar10,(int)((ulong)uVar10 >> 0x10));
      in_DX = (undefined2)((ulong)pcVar9 >> 0x10);
      sprintf((char *)CONCAT22(unaff_SS,&stack0xff86),pcVar9);
    }
    iVar2 = game_turn();
    if (iVar2 == 1) {
      return;
    }
  }
  FUN_22bf_07a5();
  sample_wait();
  FUN_216d_002e(local_12);
  FUN_216d_000e(&local_1e);
  uVar8 = 0x2012;
  iVar2 = FUN_2012_01fe(0x2634,0x4125,0x160,local_8);
  if (iVar2 == 0) {
    return;
  }
  if ((DAT_4125_263e != 0) && ((DAT_4125_2669 & 10) == 0)) {
    for (iVar2 = 0; iVar2 < (int)(uint)DAT_4125_2668; iVar2 = iVar2 + 1) {
      uVar8 = 0x1d28;
      uVar3 = mem_lock(((undefined4 *)&DAT_4125_263e)[iVar2]);
      ((undefined2 *)&DAT_4125_2650)[iVar2 * 2] = in_DX;
      ((undefined2 *)&DAT_4125_264e)[iVar2 * 2] = uVar3;
    }
  }
  (*DAT_451b_2a74)(uVar8,0x342a,0x4125,local_4,local_6,0x160,local_8,0x2634,0x4125,0);
  if ((DAT_4125_263e != 0) && ((DAT_4125_2669 & 10) == 0)) {
    for (iVar2 = 0; iVar2 < (int)(uint)DAT_4125_2668; iVar2 = iVar2 + 1) {
      mem_unlock(((undefined4 *)&DAT_4125_263e)[iVar2]);
    }
  }
  __F_SCOPY(local_12,unaff_SS,local_1a,unaff_SS);
  FUN_1e68_000a();
  iVar2 = unaff_SS;
  __F_SPUSH(local_1e);
  FUN_1997_027e();
  if (5 < (uint)what) goto override_jmp_7dda_0833_case_3;
                    /* WARNING: Switch is manually overridden */
  switch((undefined4)*(undefined2 *)(what * 2 + 0xb17)) {
  case 0x7e5d8:
    if (DAT_4125_37a4 != 0) {
      get_file_string(0x3b,0);
      iVar2 = sample_load();
joined_r0x0007e6d7:
      if (-1 < iVar2) {
        sample_play();
      }
      goto override_jmp_7dda_0833_case_3;
    }
    uVar10 = 0x1003b;
    break;
  case 0x7e61b:
    if (DAT_4125_37a4 != 0) {
      sample_wait();
      get_file_string(0x3c,0);
      iVar2 = sample_load();
      goto joined_r0x0007e6d7;
    }
    uVar10 = 0x1003c;
    break;
  case 0x7e650:
    if (DAT_4125_37a4 != 0) {
      sample_wait();
      get_file_string(0x3d,0);
      iVar2 = sample_load();
      goto joined_r0x0007e6d7;
    }
    uVar10 = 0x1003d;
    break;
  case 0x7e681:
    if (DAT_4125_37a4 != 0) {
      sample_wait();
      get_file_string(0x3e,0);
      iVar2 = sample_load();
      goto joined_r0x0007e6d7;
    }
    uVar10 = 0x1003e;
    break;
  case 0x7e6b3:
    if (DAT_4125_37a4 != 0) {
      sample_wait();
      iVar2 = sample_load();
      goto joined_r0x0007e6d7;
    }
    goto LAB_7dda_084e;
  case 0x7e6e4:
    goto override_jmp_7dda_0833_case_3;
  }
  pcVar9 = get_file_string((int)uVar10,(int)((ulong)uVar10 >> 0x10));
  iVar2 = (int)((ulong)pcVar9 >> 0x10);
LAB_7dda_084e:
  advisor_subtitles();
override_jmp_7dda_0833_case_3:
  iVar4 = dice(1,0x1e,10);
  while ((cVar1 = sample_playing(), cVar1 != '\0' && (DAT_4125_37a4 != 0))) {
    FUN_7ecb_0000();
    iVar4 = iVar4 + 1;
    if (0x28 < iVar4) {
      FUN_216d_000e(&local_1e);
      iVar4 = 0;
      FUN_1e68_000a();
      FUN_216d_002e();
      __F_SPUSH();
      FUN_1997_0129();
      FUN_7ecb_0000();
      FUN_1e68_000a();
      FUN_216d_002e();
      __F_SPUSH();
      FUN_1997_0129();
      FUN_7ecb_0000();
      FUN_1e68_000a();
      FUN_216d_002e();
      __F_SPUSH();
      FUN_1997_0129();
      FUN_7ecb_0000();
      FUN_1e68_000a();
      FUN_216d_002e();
      iVar2 = unaff_SS;
      __F_SPUSH();
      FUN_1997_0129();
    }
  }
  sample_wait();
  if (DAT_4125_37a4 != 0) {
    FUN_7ecb_0000();
  }
  if ((DAT_4125_263e != 0) && ((DAT_4125_2669 & 10) == 0)) {
    for (iVar4 = 0; iVar4 < (int)(uint)DAT_4125_2668; iVar4 = iVar4 + 1) {
      uVar8 = mem_lock();
      ((undefined2 *)&DAT_4125_2650)[iVar4 * 2] = iVar2;
      ((undefined2 *)&DAT_4125_264e)[iVar4 * 2] = uVar8;
    }
  }
  FUN_1e68_000a();
  (*DAT_451b_2a74)(0x1e68,0x2634,0x4125,0,0x160,local_8,0x342a,0x4125);
  FUN_2012_0975();
  FUN_22bf_0757();
  FUN_7ecb_0a1b();
  return;
}


