
void __cdecl16far FUN_67cc_0522(int param_1,int param_2,int param_3,int param_4)

{
  uint *puVar1;
  int *piVar2;
  char cVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  undefined2 uVar7;
  int local_4;
  
  if (((((int *)DAT_451b_1f02)[6] & 0x1000U) == 0) &&
     (((*(char *)(*(byte *)(param_2 * 0xe0 + param_1 * 2) + 0x710) == '\x03' ||
       (*(char *)(*(byte *)(param_2 * 0xe0 + param_1 * 2) + 0x710) == '\x02')) &&
      (DAT_451b_1e9c != 2)))) {
    for (local_4 = 0; local_4 < 8; local_4 = local_4 + 1) {
      if (((((undefined2 *)&DAT_451b_1ede)[local_4 * 2] != 0 ||
            *(int *)((undefined *)&DAT_451b_1ee0 + local_4 * 4) != 0) &&
          (((undefined1 *)&army_move_flags)
           [*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + local_4 * 2) + 4) * 6] ==
           '\0')) &&
         (iVar4 = hero_flight_items(*(undefined4 *)((undefined2 *)&DAT_451b_1ede + local_4 * 2)),
         iVar4 == 0)) {
        uVar7 = (undefined2)
                ((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + local_4 * 2) >> 0x10);
        puVar1 = (uint *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + local_4 * 2) + 0xc);
        *puVar1 = *puVar1 | 0x1000;
        *(undefined1 *)(((undefined2 *)&DAT_451b_1ede)[local_4 * 2] + 7) = 0;
      }
    }
    DAT_451b_1ea0 = 0;
  }
  piVar2 = DAT_451b_1f02;
  if (DAT_4125_2bda == 0) {
    uVar7 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
    piVar6 = (int *)DAT_451b_1f02;
    if (*(char *)((int)piVar6 + 7) < DAT_451b_0200) {
      cVar3 = '\0';
    }
    else {
      cVar3 = *(char *)((int)piVar6 + 7) - (char)DAT_451b_0200;
    }
    *(char *)((int)piVar6 + 7) = cVar3;
    *piVar2 = param_1;
    piVar6[1] = param_2;
    piVar6[9] = -1;
    piVar6[10] = -1;
  }
  else {
    if (DAT_451b_1ea0 < DAT_451b_0200) {
      DAT_451b_1ea0 = 0;
    }
    else {
      DAT_451b_1ea0 = DAT_451b_1ea0 - DAT_451b_0200;
    }
    iVar4 = 8;
    while (iVar5 = iVar4 + -1, iVar4 != 0) {
      iVar4 = iVar5;
      if (((undefined2 *)&DAT_451b_1ede)[iVar5 * 2] != 0 ||
          *(int *)((undefined *)&DAT_451b_1ee0 + iVar5 * 4) != 0) {
        uVar7 = (undefined2)((ulong)*(int **)((undefined2 *)&DAT_451b_1ede + iVar5 * 2) >> 0x10);
        **(int **)((undefined2 *)&DAT_451b_1ede + iVar5 * 2) = param_1;
        *(int *)(((undefined2 *)&DAT_451b_1ede)[iVar5 * 2] + 2) = param_2;
        *(undefined2 *)(((undefined2 *)&DAT_451b_1ede)[iVar5 * 2] + 0x12) = 0xffff;
        *(undefined2 *)(((undefined2 *)&DAT_451b_1ede)[iVar5 * 2] + 0x14) = 0xffff;
        if (*(char *)(((undefined2 *)&DAT_451b_1ede)[iVar5 * 2] + 7) < DAT_451b_0200) {
          cVar3 = '\0';
        }
        else {
          cVar3 = *(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar5 * 2) + 7) -
                  (char)DAT_451b_0200;
        }
        *(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar5 * 2) + 7) = cVar3;
      }
    }
  }
  if (param_3 != -1) {
    if (((DAT_3c04_0177 < param_3) && (param_3 < DAT_3c04_017b)) &&
       ((DAT_3c04_0175 < param_4 && (param_4 < DAT_3c04_0179)))) {
      if (((param_3 < param_1) && (DAT_3c04_017d + 1 < param_1)) && (DAT_3c04_017b < 0x6f)) {
        DAT_3c04_017d = DAT_3c04_017d + 1;
        DAT_3c04_0177 = DAT_3c04_0177 + 1;
        DAT_3c04_017b = DAT_3c04_017b + 1;
      }
      if (((param_1 < param_3) && (param_1 < DAT_3c04_017d)) && (0 < DAT_3c04_0177)) {
        DAT_3c04_017d = DAT_3c04_017d + -1;
        DAT_3c04_0177 = DAT_3c04_0177 + -1;
        DAT_3c04_017b = DAT_3c04_017b + -1;
      }
      if (((param_4 < param_2) && (DAT_3c04_017f + 1 < param_2)) && (DAT_3c04_0179 < 0x9b)) {
        DAT_3c04_017f = DAT_3c04_017f + 1;
        DAT_3c04_0175 = DAT_3c04_0175 + 1;
        DAT_3c04_0179 = DAT_3c04_0179 + 1;
      }
      if (((param_2 < param_4) && (param_2 < DAT_3c04_017f)) && (0 < DAT_3c04_0175)) {
        DAT_3c04_017f = DAT_3c04_017f + -1;
        DAT_3c04_0175 = DAT_3c04_0175 + -1;
        DAT_3c04_0179 = DAT_3c04_0179 + -1;
      }
    }
    else {
      DAT_3c04_017d = param_3;
      DAT_3c04_017f = param_4;
      if (param_4 < 4) {
        DAT_3c04_0175 = 0;
      }
      else {
        DAT_3c04_0175 = param_4 + -4;
      }
      if (param_3 < 4) {
        DAT_3c04_0177 = 0;
      }
      else {
        DAT_3c04_0177 = param_3 + -4;
      }
      if (param_3 + 4 < 0x71) {
        DAT_3c04_017b = param_3 + 4;
      }
      else {
        DAT_3c04_017b = 0x70;
      }
      if (param_4 + 4 < 0x9d) {
        DAT_3c04_0179 = param_4 + 4;
      }
      else {
        DAT_3c04_0179 = 0x9c;
      }
    }
  }
  return;
}


