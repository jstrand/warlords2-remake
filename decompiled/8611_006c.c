
void __cdecl16far FUN_8611_006c(void)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  int *piVar4;
  undefined1 local_a [8];
  
  if ((*(int *)(current_player * 2 + 0xd0) == 1) && (DAT_3c04_0159 == 0)) {
    return;
  }
  if (((DAT_4125_3254 == 0) || (DAT_451b_1f02 == (int *)0x0)) ||
     (iVar1 = FUN_8611_1055(), *(char *)(iVar1 + 0x2d56) != '\x0e')) {
LAB_8611_00ff:
    if ((DAT_4125_3254 == 0) || (iVar1 = FUN_8611_1055(), *(char *)(iVar1 + 0x2d56) != '\x0e')) {
      DAT_4125_2eac = 1;
    }
    else {
      if (((DAT_4125_2eac == 0) ||
          (uVar2 = DAT_3c04_017d - DAT_4125_2eae >> 0xf,
          1 < (int)((DAT_3c04_017d - DAT_4125_2eae ^ uVar2) - uVar2))) ||
         (uVar2 = DAT_3c04_017f - DAT_4125_2eb0 >> 0xf,
         1 < (int)((DAT_3c04_017f - DAT_4125_2eb0 ^ uVar2) - uVar2))) goto LAB_8611_00f6;
      DAT_4125_2eac = 0;
    }
    DAT_4125_2eae = DAT_3c04_017d;
    DAT_4125_2eb0 = DAT_3c04_017f;
    FUN_8961_0227(DAT_3c04_017d,DAT_3c04_017f);
    FUN_8611_0053();
    FUN_216d_002e(local_a);
    FUN_24d0_009d(0x6795,0x4125,local_a);
    if (DAT_451b_1f02 != (int *)0x0) {
      piVar4 = (int *)DAT_451b_1f02;
      if (((undefined1 *)&DAT_451b_1f74)
          [(*DAT_451b_1f02 - DAT_3c04_0177) * 9 + (piVar4[1] - DAT_3c04_0175)] != '\x0e') {
        if (DAT_4125_2bda == 0) {
          FUN_828e_0afd(*DAT_451b_1f02,piVar4[1]);
        }
        else {
          FUN_828e_0b27(*DAT_451b_1f02,piVar4[1]);
        }
      }
    }
  }
  else {
    if (DAT_4125_2eac != 0) {
      uVar2 = *DAT_451b_1f02 - DAT_4125_2eae >> 0xf;
      if (((int)((*DAT_451b_1f02 - DAT_4125_2eae ^ uVar2) - uVar2) < 2) &&
         (uVar2 = ((int *)DAT_451b_1f02)[1] - DAT_4125_2eb0, uVar3 = (int)uVar2 >> 0xf,
         (int)((uVar2 ^ uVar3) - uVar3) < 2)) goto LAB_8611_00ff;
    }
LAB_8611_00f6:
    DAT_4125_2eac = 0;
  }
  return;
}


