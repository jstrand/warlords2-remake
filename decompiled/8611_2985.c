
void __cdecl16far FUN_8611_2985(undefined4 param_1)

{
  uint *puVar1;
  byte *pbVar2;
  uint uVar3;
  char cVar4;
  byte bVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int *piVar11;
  int iVar12;
  undefined2 uVar13;
  undefined2 uVar14;
  
  iVar12 = army_count;
  while( true ) {
    iVar8 = iVar12 + -1;
    iVar10 = (int)param_1;
    uVar13 = (undefined2)((ulong)param_1 >> 0x10);
    if (iVar12 == 0) break;
    iVar12 = iVar8;
    if (((((-1 < *(int *)(iVar8 * 0x16 + -0x7780)) && (*(char *)(iVar8 * 0x16 + -0x777b) != -1)) &&
         (DAT_3c04_0175 <= *(int *)(iVar8 * 0x16 + -0x777e))) &&
        ((*(int *)(iVar8 * 0x16 + -0x777e) <= DAT_3c04_0179 &&
         (DAT_3c04_0177 <= *(int *)(iVar8 * 0x16 + -0x7780))))) &&
       (*(int *)(iVar8 * 0x16 + -0x7780) <= DAT_3c04_017b)) {
      iVar6 = *(int *)(iVar8 * 0x16 + -0x7780) - DAT_3c04_0177;
      iVar7 = *(int *)(iVar8 * 0x16 + -0x777e) - DAT_3c04_0175;
      uVar3 = *(uint *)(iVar10 + iVar6 * 0x48 + iVar7 * 8 + 0x17);
      iVar9 = iVar10 + iVar6 * 0x48 + iVar7 * 8;
      puVar1 = (uint *)(iVar9 + 0x17);
      *puVar1 = *puVar1 & 0xf07f;
      puVar1 = (uint *)(iVar9 + 0x17);
      *puVar1 = *puVar1 | ((uVar3 >> 7 & 0x1f) + 1 & 0x1f) << 7;
      if (*(char *)(iVar8 * 0x16 + -0x777b) == '\x0f') {
        cVar4 = '\b';
      }
      else {
        cVar4 = *(char *)(iVar8 * 0x16 + -0x777b);
      }
      iVar9 = iVar10 + iVar6 * 0x48 + iVar7 * 8;
      pbVar2 = (byte *)(iVar9 + 0x18);
      *pbVar2 = *pbVar2 & 0xf;
      pbVar2 = (byte *)(iVar9 + 0x18);
      *pbVar2 = *pbVar2 | cVar4 << 4;
      if (*(char *)((uint)(*(byte *)(iVar10 + iVar6 * 0x48 + iVar7 * 8 + 0x18) >> 4) * 0x1d +
                    ((*(byte *)(iVar10 + iVar6 * 0x48 + iVar7 * 8 + 0x17) & 0x7c) >> 2) + 0x60b) <
          *(char *)((uint)(*(byte *)(iVar10 + iVar6 * 0x48 + iVar7 * 8 + 0x18) >> 4) * 0x1d +
                    (int)*(char *)(iVar8 * 0x16 + -0x777c) + 0x60b)) {
        bVar5 = *(byte *)(iVar8 * 0x16 + -0x777c);
        iVar9 = iVar10 + iVar6 * 0x48 + iVar7 * 8;
        pbVar2 = (byte *)(iVar9 + 0x17);
        *pbVar2 = *pbVar2 & 0x83;
        pbVar2 = (byte *)(iVar9 + 0x17);
        *pbVar2 = *pbVar2 | (bVar5 & 0x1f) << 2;
      }
      if (((*(uint *)(iVar8 * 0x16 + -0x7774) & 0x1000) != 0) &&
         (-1 < *(char *)(*(int *)(iVar8 * 0x16 + -0x777e) * 0xe0 +
                         *(int *)(iVar8 * 0x16 + -0x7780) * 2 + 1))) {
        iVar8 = iVar10 + iVar6 * 0x48 + iVar7 * 8;
        pbVar2 = (byte *)(iVar8 + 0x17);
        *pbVar2 = *pbVar2 & 0x83;
        pbVar2 = (byte *)(iVar8 + 0x17);
        *pbVar2 = *pbVar2 | 0x14;
      }
      *(undefined2 *)
       (iVar10 + (uint)(*(byte *)(iVar10 + iVar6 * 0x48 + iVar7 * 8 + 0x18) >> 4) * 2 + 4) = 1;
    }
  }
  if (DAT_451b_1f02 != (int *)0x0) {
    piVar11 = (int *)DAT_451b_1f02;
    if ((DAT_3c04_0177 <= *DAT_451b_1f02) && (*DAT_451b_1f02 <= DAT_3c04_017b)) {
      if ((DAT_3c04_0175 <= piVar11[1]) && (piVar11[1] <= DAT_3c04_0179)) {
        if ((piVar11[6] & 0x1000U) == 0) {
          bVar5 = *(byte *)(piVar11 + 2);
        }
        else {
          bVar5 = 5;
        }
        iVar12 = iVar10 + (*DAT_451b_1f02 - DAT_3c04_0177) * 0x48 + (piVar11[1] - DAT_3c04_0175) * 8
        ;
        pbVar2 = (byte *)(iVar12 + 0x17);
        *pbVar2 = *pbVar2 & 0x83;
        pbVar2 = (byte *)(iVar12 + 0x17);
        *pbVar2 = *pbVar2 | (bVar5 & 0x1f) << 2;
        pbVar2 = (byte *)(iVar10 + (*DAT_451b_1f02 - DAT_3c04_0177) * 0x48 +
                          (piVar11[1] - DAT_3c04_0175) * 8 + 0x1c);
        *pbVar2 = *pbVar2 | 0x10;
        uVar14 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
        cVar4 = *(char *)((int)(int *)DAT_451b_1f02 + 5);
        iVar12 = iVar10 + (*DAT_451b_1f02 - DAT_3c04_0177) * 0x48 + (piVar11[1] - DAT_3c04_0175) * 8
        ;
        pbVar2 = (byte *)(iVar12 + 0x18);
        *pbVar2 = *pbVar2 & 0xf;
        pbVar2 = (byte *)(iVar12 + 0x18);
        *pbVar2 = *pbVar2 | cVar4 << 4;
      }
    }
  }
  return;
}


