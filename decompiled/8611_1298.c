
undefined2 __cdecl16far FUN_8611_1298(undefined2 param_1,int param_2,int param_3)

{
  byte *pbVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  if (opt_hidden_map != 0) {
    if ((DAT_451b_1e9c == 2) || (*(char *)(*(byte *)(param_3 * 0xe0 + param_2 * 2) + 0x710) == '\n')
       ) {
      bVar2 = true;
    }
    else {
      bVar2 = false;
    }
    if (bVar2) {
      iVar3 = param_2 + -2;
      param_2 = param_2 + 2;
      iVar4 = param_3 + -2;
      param_3 = param_3 + 2;
      iVar6 = iVar3;
    }
    else {
      iVar3 = param_2 + -1;
      param_2 = param_2 + 1;
      iVar4 = param_3 + -1;
      param_3 = param_3 + 1;
      iVar6 = iVar3;
    }
    for (; iVar5 = iVar4, iVar3 <= param_2; iVar3 = iVar3 + 1) {
      for (; iVar5 <= param_3; iVar5 = iVar5 + 1) {
        if ((((-1 < iVar3) && (iVar3 < 0x70)) && (-1 < iVar5)) &&
           ((iVar5 < 0x9c && ((*(byte *)(iVar5 * 0x70 + iVar3) & 0x20) == 0)))) {
          for (; iVar3 = iVar4, iVar6 <= param_2; iVar6 = iVar6 + 1) {
            for (; iVar3 <= param_3; iVar3 = iVar3 + 1) {
              if (((-1 < iVar6) && (iVar6 < 0x70)) && ((-1 < iVar3 && (iVar3 < 0x9c)))) {
                pbVar1 = (byte *)(iVar3 * 0x70 + iVar6);
                *pbVar1 = *pbVar1 | 0x20;
              }
            }
          }
          return 1;
        }
      }
    }
  }
  return 0;
}


