
void __cdecl16far FUN_8065_0b3b(int param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined2 uVar4;
  int local_6;
  int local_4;
  
  if (DAT_451b_1f02 == (int *)0x0) {
    return;
  }
  if ((param_1 < 0) && (param_2 < 0)) {
    FUN_22bf_0af4(&local_6);
    piVar2 = (int *)DAT_451b_1f02;
    uVar4 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
    if ((0xf < local_6) &&
       ((((local_6 < 0x178 && (0x1d < local_4)) && (local_4 < 0x186)) &&
        ((*DAT_451b_1f02 == DAT_3c04_0177 + (local_6 + -0x10) / 0x28 &&
         (piVar2[1] == DAT_3c04_0175 + (local_4 + -0x1e) / 0x28)))))) {
      return;
    }
    auto_tutorial_hook_2(piVar2[9],piVar2[10]);
    FUN_8065_0174();
    DAT_4125_2ad8 = 0xffff;
    DAT_4125_2ada = 0xffff;
    return;
  }
  if (((param_1 < 0x10) || (0x177 < param_1)) || ((param_2 < 0x1e || (0x185 < param_2)))) {
    if ((param_1 < 0x10) && (param_2 < 0x1e)) {
      DAT_3c04_017d = DAT_3c04_017d + -1;
LAB_8065_0c1b:
      DAT_3c04_017f = DAT_3c04_017f + -1;
      goto LAB_8065_0c25;
    }
    if ((param_1 < 0x178) || (0x1d < param_2)) {
      if ((param_1 < 0x10) && (0x185 < param_2)) {
        DAT_3c04_017d = DAT_3c04_017d + -1;
      }
      else {
        if ((0x177 < param_1) && (0x185 < param_2)) goto LAB_8065_0c49;
        if (param_1 < 0x10) {
          DAT_3c04_017d = DAT_3c04_017d + -1;
          goto LAB_8065_0c25;
        }
        if (param_2 < 0x1e) goto LAB_8065_0c1b;
        if (0x177 < param_1) {
          DAT_3c04_017d = DAT_3c04_017d + 1;
          goto LAB_8065_0c25;
        }
        if (param_2 < 0x186) {
          return;
        }
      }
    }
    else {
LAB_8065_0c49:
      DAT_3c04_017d = DAT_3c04_017d + 1;
    }
    DAT_3c04_017f = DAT_3c04_017f + 1;
LAB_8065_0c25:
    FUN_8611_0629(DAT_3c04_017d,DAT_3c04_017f);
    return;
  }
  iVar1 = DAT_3c04_0177 + (param_1 + -0x10) / 0x28;
  iVar3 = DAT_3c04_0175 + (param_2 + -0x1e) / 0x28;
  if ((DAT_4125_2ad8 == iVar1) && (DAT_4125_2ada == iVar3)) {
    return;
  }
  if ((*DAT_451b_1f02 == iVar1) && (((int *)DAT_451b_1f02)[1] == iVar3)) {
    DAT_4125_2ad8 = iVar1;
    DAT_4125_2ada = iVar3;
    return;
  }
  DAT_4125_2ad8 = iVar1;
  DAT_4125_2ada = iVar3;
  if ((DAT_3c04_0177 == iVar1) && (DAT_3c04_0175 == iVar3)) {
    DAT_3c04_017d = DAT_3c04_017d + -1;
    goto LAB_8065_0d26;
  }
  if ((DAT_3c04_0177 == iVar1) && (DAT_3c04_0179 == iVar3)) {
    DAT_3c04_017d = DAT_3c04_017d + -1;
LAB_8065_0dd3:
    DAT_3c04_017f = DAT_3c04_017f + 1;
  }
  else {
    if ((DAT_3c04_017b == iVar1) && (DAT_3c04_0175 == iVar3)) {
      DAT_3c04_017d = DAT_3c04_017d + 1;
      goto LAB_8065_0dd3;
    }
    if ((DAT_3c04_017b == iVar1) && (DAT_3c04_0179 == iVar3)) {
      DAT_3c04_017d = DAT_3c04_017d + 1;
LAB_8065_0d26:
      DAT_3c04_017f = DAT_3c04_017f + -1;
    }
    else if (DAT_3c04_0177 == iVar1) {
      DAT_3c04_017d = DAT_3c04_017d + -1;
    }
    else {
      if (DAT_3c04_017b != iVar1) {
        if (DAT_3c04_0175 != iVar3) {
          if (DAT_3c04_0179 != iVar3) goto LAB_8065_0df6;
          goto LAB_8065_0dd3;
        }
        goto LAB_8065_0d26;
      }
      DAT_3c04_017d = DAT_3c04_017d + 1;
    }
  }
  FUN_8611_0629(DAT_3c04_017d,DAT_3c04_017f);
LAB_8065_0df6:
  FUN_1c8c_0007(1);
  return;
}


