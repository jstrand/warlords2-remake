
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far music_cue(int cue)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  char *pcVar4;
  char *pcVar5;
  undefined1 *puVar6;
  undefined2 uVar8;
  undefined4 uVar7;
  
  if (_DAT_4125_0bee == 0) {
    return;
  }
  if (cue == DAT_4125_2632) {
    cVar2 = music_playing();
    if (cVar2 != '\0') {
      return;
    }
    music_is_loaded();
  }
  DAT_4125_2632 = cue;
  cVar2 = music_playing();
  if (cVar2 != '\0') {
    music_stop();
    music_is_loaded();
  }
  if (cue == 0) {
    puVar6 = (undefined1 *)0x8;
  }
  else if (cue == 3) {
    puVar6 = (undefined1 *)0xffff0009;
  }
  else if (cue == 1) {
    puVar6 = (undefined1 *)0xffff000a;
  }
  else {
    if (cue == 2) {
      bVar1 = true;
      for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
        if ((*(int *)(iVar3 * 2 + 0x137) != 0) && (*(int *)(iVar3 * 2 + 0xd0) == 0)) {
          bVar1 = false;
          break;
        }
      }
      if (bVar1) {
        iVar3 = dice(1,100,0);
        if (iVar3 < 6) {
          uVar7 = 0xffff000c;
        }
        else {
          if (iVar3 < 0x35) goto LAB_7dda_00f6;
          uVar7 = 0xffff000a;
        }
      }
      else {
LAB_7dda_00f6:
        uVar7 = 0xffff000b;
      }
      pcVar4 = get_file_string((int)uVar7,(int)((ulong)uVar7 >> 0x10));
      pcVar5 = get_file_string(7,0);
      music_load_xmi((char *)pcVar5,(int)((ulong)pcVar5 >> 0x10),pcVar4);
      uVar8 = 0;
      goto LAB_7dda_0067;
    }
    if (cue == 4) {
      puVar6 = (undefined1 *)0xc;
    }
    else if (cue == 5) {
      puVar6 = (undefined1 *)0xffff000d;
    }
    else if (cue == 6) {
      puVar6 = &DAT_f000_fffe;
    }
    else if (cue == 7) {
      puVar6 = (undefined1 *)0xf;
    }
    else if (cue == 8) {
      puVar6 = (undefined1 *)0x10;
    }
    else if (cue == 9) {
      puVar6 = (undefined1 *)0x11;
    }
    else if (cue == 10) {
      puVar6 = (undefined1 *)0x12;
    }
    else {
      if (cue != 0xb) {
        return;
      }
      puVar6 = (undefined1 *)0x13;
    }
  }
  pcVar4 = get_file_string((int)(undefined1 *)puVar6,(int)((ulong)puVar6 >> 0x10));
  pcVar5 = get_file_string(7,0);
  music_load_xmi((char *)pcVar5,(int)((ulong)pcVar5 >> 0x10),pcVar4);
  uVar8 = 1;
LAB_7dda_0067:
  music_play(0,uVar8);
  return;
}


