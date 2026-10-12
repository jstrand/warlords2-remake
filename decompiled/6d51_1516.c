
undefined1 * __cdecl16far
auto_ui_history_events(char param_1,int param_2,int param_3,undefined4 param_4)

{
  char *pcVar1;
  char *pcVar2;
  char *fmt;
  int iVar3;
  undefined2 uVar4;
  
  if (10 < (uint)(int)param_1) goto LAB_6d51_169d;
                    /* WARNING: Switch is manually overridden */
  switch((undefined4)*(undefined2 *)(param_1 * 2 + 0x16a4)) {
  case 0x6ea44:
    pcVar2 = (char *)CONCAT22(0x3c04,(char *)(param_2 * 0x41 + 0x1581));
    iVar3 = 0;
    break;
  case 0x6ea59:
    if (param_2 == -1) {
      iVar3 = 1;
    }
    else {
      if (param_2 != -2) {
        pcVar2 = (char *)CONCAT22(0x3c04,(char *)(param_2 * 0x41 + 0x1581));
        iVar3 = 3;
        break;
      }
      iVar3 = 2;
    }
    goto LAB_6d51_1554;
  case 0x6eaa1:
    iVar3 = 4;
    goto LAB_6d51_1554;
  case 0x6eaa9:
    iVar3 = 5;
    goto LAB_6d51_1554;
  case 0x6eab1:
    param_4 = CONCAT22(0x3c04,param_2 * 0x14);
    iVar3 = 6;
    goto LAB_6d51_1554;
  case 0x6eac1:
    if (param_2 == -1) {
      pcVar2 = (char *)CONCAT22(0x3c04,(char *)(param_3 * 0x41 + 0x1581));
      iVar3 = 7;
    }
    else {
      pcVar2 = (char *)CONCAT22(0x3c04,(char *)(param_3 * 0x41 + 0x1581));
      param_4 = CONCAT22(0x3c04,param_2 * 0x14);
      iVar3 = 8;
    }
    break;
  case 0x6eaf8:
    if (param_2 == 100) {
      iVar3 = 0xd;
LAB_6d51_15ef:
      pcVar2 = get_string(0x5e,iVar3);
      pcVar1 = (char *)pcVar2;
      uVar4 = (undefined2)((ulong)pcVar2 >> 0x10);
    }
    else {
      if (param_2 == 0x65) {
        iVar3 = 0xe;
        goto LAB_6d51_15ef;
      }
      if (param_2 == 0x66) {
        iVar3 = 0xf;
        goto LAB_6d51_15ef;
      }
      pcVar1 = (char *)(param_2 * 0x1d + 0xce9);
      uVar4 = 0x3c04;
    }
    pcVar2 = (char *)CONCAT22(uVar4,pcVar1);
    iVar3 = 9;
    break;
  case 0x6eb32:
    param_4 = CONCAT22(0x3c04,param_2 * 0x14);
    iVar3 = 10;
LAB_6d51_1554:
    pcVar2 = get_string(0x5e,iVar3);
    sprintf(&DAT_451b_1cd8,pcVar2,param_4);
    goto LAB_6d51_169d;
  case 0x6eb43:
    pcVar2 = (char *)CONCAT22(0x3c04,(char *)(param_3 * 0x14));
    param_4 = CONCAT22(0x3c04,param_2 * 0x14);
    iVar3 = 0xb;
    break;
  case 0x6eb5f:
    pcVar2 = (char *)CONCAT22(0x3c04,(char *)(param_3 * 0x14));
    param_4 = CONCAT22(0x3c04,param_2 * 0x14);
    iVar3 = 0xc;
    break;
  case 0x6eb7b:
    pcVar2 = (char *)CONCAT22(0x3c04,(char *)(param_3 * 0x14));
    param_4 = CONCAT22(0x3c04,param_2 * 0x14);
    iVar3 = 0x10;
  }
  fmt = get_string(0x5e,iVar3);
  sprintf(&DAT_451b_1cd8,fmt,param_4,pcVar2);
LAB_6d51_169d:
  return (undefined1 *)&DAT_451b_1cd8;
}


