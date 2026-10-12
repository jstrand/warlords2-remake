
uint __cdecl16far FUN_8611_1055(int param_1,int param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  
  uVar5 = 0;
  if (DAT_4125_3254 == 0) {
    for (iVar4 = 0; iVar4 < 8; iVar4 = iVar4 + 1) {
      iVar3 = param_1 + ((undefined2 *)&DAT_4125_2efa)[iVar4];
      iVar1 = param_2 + ((undefined2 *)&DAT_4125_2f0a)[iVar4];
      if ((((iVar3 < 0) || (iVar1 < 0)) || (0x6f < iVar3)) || (0x9b < iVar1)) {
        uVar2 = param_3 << ((byte)iVar4 & 0x1f);
      }
      else if ((*(byte *)(iVar1 * 0x70 + iVar3) & 0x20) == 0) {
        uVar2 = 1 << ((byte)iVar4 & 0x1f);
      }
      else {
        uVar2 = 0;
      }
      uVar5 = uVar5 | uVar2;
    }
  }
  else {
    for (iVar4 = 0; iVar4 < 8; iVar4 = iVar4 + 1) {
      iVar3 = param_1 + ((undefined2 *)&DAT_4125_2efa)[iVar4];
      iVar1 = param_2 + ((undefined2 *)&DAT_4125_2f0a)[iVar4];
      if (((iVar3 < 0) || (iVar1 < 0)) || ((0x6f < iVar3 || (0x9b < iVar1)))) {
        uVar2 = param_3 << ((byte)iVar4 & 0x1f);
      }
      else if (*(char *)(iVar1 * 0x70 + iVar3) < '\0') {
        uVar2 = 0;
      }
      else {
        uVar2 = 1 << ((byte)iVar4 & 0x1f);
      }
      uVar5 = uVar5 | uVar2;
    }
  }
  return uVar5;
}


