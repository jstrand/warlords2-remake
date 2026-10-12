
void __cdecl16far FUN_8611_25ec(undefined4 param_1)

{
  byte *pbVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 unaff_SS;
  int local_1c [9];
  undefined4 local_a;
  undefined4 local_6;
  
  iVar2 = 0;
  do {
    if (8 < iVar2) {
      iVar2 = 0;
      while( true ) {
        if (8 < iVar2) break;
        *(undefined2 *)((int)param_1 + iVar2 * 2 + 4) = 0;
        iVar2 = iVar2 + 1;
      }
      for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
        for (iVar6 = 0; iVar6 < 9; iVar6 = iVar6 + 1) {
          iVar4 = (int)param_1 + iVar2 * 0x48 + iVar6 * 8;
          local_6 = (undefined4 *)CONCAT22(param_1._2_2_,(undefined4 *)(iVar4 + 0x16));
          local_a = (undefined4 *)CONCAT22(param_1._2_2_,(undefined4 *)(iVar4 + 0x1a));
          *local_a = 0;
          *local_6 = 0;
          pbVar1 = (byte *)((int)param_1 + iVar2 * 0x48 + iVar6 * 8 + 0x16);
          *pbVar1 = *pbVar1 | 1;
          uVar3 = *(byte *)((DAT_3c04_0175 + iVar6) * 0xe0 + (DAT_3c04_0177 + iVar2) * 2 + 1) & 0xf;
          if (7 < uVar3) {
            uVar3 = 8;
          }
          iVar4 = local_1c[uVar3];
          iVar5 = (int)param_1 + iVar2 * 0x48 + iVar6 * 8;
          pbVar1 = (byte *)(iVar5 + 0x17);
          *pbVar1 = *pbVar1 & 0x83;
          pbVar1 = (byte *)(iVar5 + 0x17);
          *pbVar1 = *pbVar1 | ((byte)iVar4 & 0x1f) << 2;
        }
      }
      *(byte *)((int)param_1 + 0x29e) = *(byte *)((int)param_1 + 0x29e) | 1;
      *(byte *)((int)param_1 + 0x29e) = *(byte *)((int)param_1 + 0x29e) & 0xfd;
      *(byte *)((int)param_1 + 0x29e) = *(byte *)((int)param_1 + 0x29e) & 0xfb;
      *(byte *)((int)param_1 + 0x29e) = *(byte *)((int)param_1 + 0x29e) & 0xf7;
      *(byte *)((int)param_1 + 0x29e) = *(byte *)((int)param_1 + 0x29e) & 0xef;
      *(byte *)((int)param_1 + 0x29e) = *(byte *)((int)param_1 + 0x29e) & 0xdf;
      *(byte *)((int)param_1 + 0x29e) = *(byte *)((int)param_1 + 0x29e) & 0xbf;
      *(byte *)((int)param_1 + 0x29e) = *(byte *)((int)param_1 + 0x29e) & 0x7f;
      *(byte *)((int)param_1 + 0x29f) = *(byte *)((int)param_1 + 0x29f) & 0xfe;
      return;
    }
    for (iVar6 = 0; iVar6 < 0x1d; iVar6 = iVar6 + 1) {
      if (*(char *)(iVar2 * 0x1d + iVar6 + 0x60b) == '\0') {
        local_1c[iVar2] = iVar6;
        break;
      }
    }
    iVar2 = iVar2 + 1;
  } while( true );
}


