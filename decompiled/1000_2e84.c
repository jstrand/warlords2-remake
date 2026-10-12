
byte __cputn(undefined2 param_1,undefined2 param_2,int param_3,byte *param_4)

{
  byte *pbVar1;
  byte bVar2;
  int iVar3;
  undefined2 unaff_SS;
  undefined4 uVar4;
  undefined2 local_a;
  byte local_7;
  uint local_6;
  uint local_4;
  
  local_7 = 0;
  bVar2 = __wherexy();
  local_4 = (uint)bVar2;
  local_6 = __wherexy();
  local_6 = local_6 >> 8;
  do {
    pbVar1 = param_4;
    iVar3 = param_3 + -1;
    if (param_3 == 0) {
      __video_int();
      return local_7;
    }
    param_4 = (byte *)CONCAT22(param_4._2_2_,(byte *)param_4 + 1);
    local_7 = *pbVar1;
    if (local_7 - 7 < 7) {
                    /* WARNING: Switch is manually overridden */
      switch(local_7 - 7) {
      case 0:
        __video_int();
        break;
      case 1:
        if ((int)(uint)DAT_4125_3e72 < (int)local_4) {
          local_4 = local_4 - 1;
        }
        break;
      default:
        goto switchD_1000_2ec3_caseD_9;
      case 3:
        local_6 = local_6 + 1;
        break;
      case 6:
        local_4 = (uint)DAT_4125_3e72;
      }
    }
    else {
switchD_1000_2ec3_caseD_9:
      if ((DAT_4125_3e7b == '\0') && (DAT_4125_3e81 != 0)) {
        local_a = CONCAT11(DAT_4125_3e76,local_7);
        uVar4 = __screen_offset(local_6 + 1,local_4 + 1);
        __vram_copy(1,&local_a,unaff_SS,uVar4);
      }
      else {
        __video_int();
        __video_int();
      }
      local_4 = local_4 + 1;
    }
    if ((int)(uint)DAT_4125_3e74 < (int)local_4) {
      local_4 = (uint)DAT_4125_3e72;
      local_6 = local_6 + DAT_4125_3e70;
    }
    param_3 = iVar3;
    if ((int)(uint)DAT_4125_3e75 < (int)local_6) {
      __scroll(1,DAT_4125_3e75,DAT_4125_3e74,DAT_4125_3e73,DAT_4125_3e72,6);
      local_6 = local_6 - 1;
    }
  } while( true );
}


