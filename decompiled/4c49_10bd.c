
undefined2 __cdecl16far FUN_4c49_10bd(int param_1,int param_2)

{
  int iVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
  iVar1 = (int)DAT_451b_002a;
  if (((*(char *)(iVar1 + param_2 * 0x70 + param_1 + 0x6120) == '\x03') ||
      (*(char *)(iVar1 + param_2 * 0x70 + param_1 + 0x6120) == '\x01')) ||
     ((((param_1 != 0 && (((param_2 != 0 && (param_1 != 0x6f)) && (param_2 != 0x9b)))) ||
       (*(char *)(iVar1 + param_2 * 0x70 + param_1 + 0x6120) != '\0')) &&
      (((*(char *)(iVar1 + param_2 * 0x70 + param_1 + 0x6121) != '\x02' &&
        (*(char *)(iVar1 + param_2 * 0x70 + param_1 + 0x611f) != '\x02')) &&
       ((*(char *)(iVar1 + (param_2 + 1) * 0x70 + param_1 + 0x6120) != '\x02' &&
        (*(char *)(iVar1 + (param_2 + -1) * 0x70 + param_1 + 0x6120) != '\x02')))))))) {
    uVar2 = 0;
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}


