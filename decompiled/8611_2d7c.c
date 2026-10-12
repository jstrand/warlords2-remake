
void __cdecl16far FUN_8611_2d7c(undefined4 param_1)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  
  iVar4 = 0x16;
  while (iVar5 = iVar4 + -1, iVar4 != 0) {
    iVar4 = iVar5;
    if ((((*(char *)(iVar5 * 0x1d + 0xcff) == '\x01') &&
         (DAT_3c04_0177 <= *(int *)(iVar5 * 0x1d + 0xd02))) &&
        (*(int *)(iVar5 * 0x1d + 0xd02) <= DAT_3c04_017b)) &&
       ((DAT_3c04_0175 <= *(int *)(iVar5 * 0x1d + 0xd04) &&
        (*(int *)(iVar5 * 0x1d + 0xd04) <= DAT_3c04_0179)))) {
      iVar2 = *(int *)(iVar5 * 0x1d + 0xd02) - DAT_3c04_0177;
      iVar3 = *(int *)(iVar5 * 0x1d + 0xd04) - DAT_3c04_0175;
      uVar6 = (undefined2)((ulong)param_1 >> 0x10);
      if ((*(byte *)(*(int *)(iVar5 * 0x1d + 0xd04) * 0x70 + *(int *)(iVar5 * 0x1d + 0xd02)) & 0x40)
          == 0) {
        pbVar1 = (byte *)((int)param_1 + iVar2 * 0x48 + iVar3 * 8 + 0x19);
        *pbVar1 = *pbVar1 | 0x40;
        *(byte *)((int)param_1 + 0x29e) = *(byte *)((int)param_1 + 0x29e) | 0x20;
      }
      else {
        pbVar1 = (byte *)((int)param_1 + iVar2 * 0x48 + iVar3 * 8 + 0x19);
        *pbVar1 = *pbVar1 | 1;
        iVar2 = (int)param_1 + iVar2 * 0x48 + iVar3 * 8;
        pbVar1 = (byte *)(iVar2 + 0x19);
        *pbVar1 = *pbVar1 & 0xe1;
        pbVar1 = (byte *)(iVar2 + 0x19);
        *pbVar1 = *pbVar1 | ((byte)iVar5 & 0xf) << 1;
        *(byte *)((int)param_1 + 0x29e) = *(byte *)((int)param_1 + 0x29e) | 0x40;
        *(undefined2 *)((int)param_1 + iVar5 * 2 + 4) = 1;
      }
    }
  }
  return;
}


