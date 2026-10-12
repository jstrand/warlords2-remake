
int __cdecl16far FUN_6c1b_0c27(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  iVar1 = FUN_1b62_0044(param_1);
  for (iVar2 = 0; iVar2 < 0x16; iVar2 = iVar2 + 1) {
    if ((*(char *)(iVar2 * 0x1d + 0xcff) == '\x03') && (*(int *)(iVar2 * 0x1d + 0xd00) == iVar1)) {
      if (*(char *)(iVar2 * 0x1d + 0xcfd) == '\b') {
        iVar3 = iVar3 + 1;
      }
      else if (*(char *)(iVar2 * 0x1d + 0xcfd) == '\x02') {
        iVar3 = iVar3 + *(char *)(iVar2 * 0x1d + 0xcfe);
      }
    }
  }
  return iVar3;
}


