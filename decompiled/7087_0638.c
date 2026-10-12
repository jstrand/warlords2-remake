
void __cdecl16far auto_ui_vector_help(int param_1)

{
  char *pcVar1;
  undefined2 uVar2;
  int iVar3;
  
  if (param_1 == 0) {
    FUN_54f6_070b(0xa10040,0x2800bc);
    if (DAT_4125_0e94 == 1) {
      pcVar1 = get_string(0x9b,2);
      FUN_7ecb_00bf(0xdd0170,(char *)pcVar1);
      iVar3 = 3;
    }
    else {
      pcVar1 = get_string(0x9b,0);
      FUN_7ecb_00bf(0xdd0170,(char *)pcVar1);
      iVar3 = 1;
    }
    pcVar1 = get_string(0x9b,iVar3);
    uVar2 = 0xf1;
  }
  else {
    FUN_54f6_070b(0xd40040,0x2800bc);
    if (DAT_4125_0e94 == 2) {
      pcVar1 = get_string(0x9c,2);
      FUN_7ecb_00bf(0x1100170,(char *)pcVar1);
      iVar3 = 3;
    }
    else {
      pcVar1 = get_string(0x9c,0);
      FUN_7ecb_00bf(0x1100170,(char *)pcVar1);
      iVar3 = 1;
    }
    pcVar1 = get_string(0x9c,iVar3);
    uVar2 = 0x124;
  }
  FUN_7ecb_00bf(0x170,uVar2,pcVar1);
  return;
}


