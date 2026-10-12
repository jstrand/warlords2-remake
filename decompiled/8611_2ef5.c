
void __cdecl16far FUN_8611_2ef5(undefined4 param_1)

{
  byte *pbVar1;
  uint *puVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  char cVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  undefined2 uVar10;
  
  iVar5 = DAT_3c04_017b;
  iVar9 = DAT_3c04_0179;
  iVar4 = DAT_3c04_0177;
  iVar3 = DAT_3c04_0175;
  if (((DAT_451b_1f02 != 0) && (0 < DAT_4125_2ea6)) &&
     (iVar7 = DAT_4125_2ea8, DAT_4125_2ea8 < DAT_4125_2ea6)) {
    while( true ) {
      uVar10 = (undefined2)((ulong)param_1 >> 0x10);
      if (DAT_4125_2ea6 + -1 <= iVar7) break;
      if (((iVar4 <= (int)((undefined2 *)&DAT_451b_2515)[iVar7 * 2]) &&
          ((int)((undefined2 *)&DAT_451b_2515)[iVar7 * 2] <= iVar5)) &&
         ((iVar3 <= (int)((undefined2 *)&DAT_451b_2517)[iVar7 * 2] &&
          ((int)((undefined2 *)&DAT_451b_2517)[iVar7 * 2] <= iVar9)))) {
        if (iVar7 < DAT_4125_2eaa) {
          cVar6 = '\x01';
        }
        else {
          cVar6 = '\x02';
        }
        iVar8 = (int)param_1 + (((undefined2 *)&DAT_451b_2515)[iVar7 * 2] - iVar4) * 0x48 +
                (((undefined2 *)&DAT_451b_2517)[iVar7 * 2] - iVar3) * 8;
        pbVar1 = (byte *)(iVar8 + 0x1b);
        *pbVar1 = *pbVar1 & 0xcf;
        pbVar1 = (byte *)(iVar8 + 0x1b);
        *pbVar1 = *pbVar1 | cVar6 << 4;
        *(byte *)((int)param_1 + 0x29e) = *(byte *)((int)param_1 + 0x29e) | 0x80;
      }
      iVar7 = iVar7 + 1;
    }
    if (((iVar4 <= DAT_451b_2511) && (DAT_451b_2511 <= iVar5)) &&
       ((iVar3 <= DAT_451b_2513 && (DAT_451b_2513 <= iVar9)))) {
      pbVar1 = (byte *)((int)param_1 + (DAT_451b_2511 - iVar4) * 0x48 + (DAT_451b_2513 - iVar3) * 8
                       + 0x1b);
      *pbVar1 = *pbVar1 | 0x40;
      cVar6 = *(char *)((int)DAT_451b_1f02 + 4);
      iVar9 = (int)param_1 + (DAT_451b_2511 - iVar4) * 0x48 + (DAT_451b_2513 - iVar3) * 8;
      puVar2 = (uint *)(iVar9 + 0x1b);
      *puVar2 = *puVar2 & 0xf07f;
      puVar2 = (uint *)(iVar9 + 0x1b);
      *puVar2 = *puVar2 | ((int)cVar6 & 0x1fU) << 7;
      *(byte *)((int)param_1 + 0x29f) = *(byte *)((int)param_1 + 0x29f) | 1;
      *(int *)((int)param_1 + 0x2a0) = DAT_451b_2511 - iVar4;
      *(int *)((int)param_1 + 0x2a2) = DAT_451b_2513 - iVar3;
      *(int *)((int)param_1 + 0x2a4) = (int)*(char *)((int)DAT_451b_1f02 + 4);
    }
  }
  return;
}


