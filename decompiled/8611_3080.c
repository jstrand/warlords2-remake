
void __cdecl16far FUN_8611_3080(uint param_1)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined2 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  
  if ((DAT_451b_2509 & 1) == 0) {
    return;
  }
  if (param_1 < 8) {
                    /* WARNING: Switch is manually overridden */
    switch((undefined4)*(undefined2 *)(param_1 * 2 + 0x34dd)) {
    case 0x891b4:
      for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
        for (iVar1 = 0; iVar1 < 9; iVar1 = iVar1 + 1) {
          __F_SCOPY(iVar1 * 0x48 + iVar2 * 8 + 0x61e9,0x4125,
                    (undefined1 *)&DAT_451b_2281 + iVar1 * 0x48 + iVar2 * 8,0x4125);
        }
      }
      for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
        ((undefined1 *)&DAT_451b_22c1)[iVar2 * 0x48] =
             ((undefined1 *)&DAT_451b_22c1)[iVar2 * 0x48] & 0xfe;
      }
      break;
    case 0x89209:
      for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
        for (iVar1 = 8; 0 < iVar1; iVar1 = iVar1 + -1) {
          __F_SCOPY(iVar1 * 0x48 + iVar2 * 8 + 0x61a1,0x4125,
                    (undefined1 *)&DAT_451b_2281 + iVar1 * 0x48 + iVar2 * 8,0x4125);
        }
      }
      for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
        ((undefined1 *)&DAT_451b_22c1)[iVar2 * 0x48] =
             ((undefined1 *)&DAT_451b_22c1)[iVar2 * 0x48] & 0xfe;
      }
      for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
        ((undefined1 *)&DAT_451b_2281)[iVar2 * 8] = ((undefined1 *)&DAT_451b_2281)[iVar2 * 8] & 0xfe
        ;
      }
      break;
    case 0x89272:
      for (iVar2 = 8; 0 < iVar2; iVar2 = iVar2 + -1) {
        for (iVar1 = 0; iVar1 < 9; iVar1 = iVar1 + 1) {
          __F_SCOPY(iVar2 * 0x48 + iVar1 * 8 + 0x6199,0x4125,
                    (undefined1 *)&DAT_451b_2281 + iVar2 * 0x48 + iVar1 * 8,0x4125);
        }
      }
      for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
        ((undefined1 *)&DAT_451b_2281)[iVar2 * 8] = ((undefined1 *)&DAT_451b_2281)[iVar2 * 8] & 0xfe
        ;
      }
      break;
    case 0x892c7:
      for (iVar2 = 8; 0 < iVar2; iVar2 = iVar2 + -1) {
        for (iVar1 = 8; 0 < iVar1; iVar1 = iVar1 + -1) {
          __F_SCOPY(iVar1 * 0x48 + iVar2 * 8 + 0x6191,0x4125,
                    (undefined1 *)&DAT_451b_2281 + iVar1 * 0x48 + iVar2 * 8,0x4125);
        }
      }
      for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
        ((undefined1 *)&DAT_451b_2281)[iVar2 * 0x48] =
             ((undefined1 *)&DAT_451b_2281)[iVar2 * 0x48] & 0xfe;
      }
      for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
        ((undefined1 *)&DAT_451b_2281)[iVar2 * 8] = ((undefined1 *)&DAT_451b_2281)[iVar2 * 8] & 0xfe
        ;
      }
      break;
    case 0x89330:
      for (iVar2 = 8; 0 < iVar2; iVar2 = iVar2 + -1) {
        for (iVar1 = 0; iVar1 < 9; iVar1 = iVar1 + 1) {
          __F_SCOPY(iVar1 * 0x48 + iVar2 * 8 + 0x61d9,0x4125,
                    (undefined1 *)&DAT_451b_2281 + iVar1 * 0x48 + iVar2 * 8,0x4125);
        }
      }
      for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
        ((undefined1 *)&DAT_451b_2281)[iVar2 * 0x48] =
             ((undefined1 *)&DAT_451b_2281)[iVar2 * 0x48] & 0xfe;
      }
      break;
    case 0x89385:
      for (iVar2 = 8; 0 < iVar2; iVar2 = iVar2 + -1) {
        for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
          __F_SCOPY((undefined1 *)&DAT_451b_22c1 + iVar1 * 0x48 + iVar2 * 8,0x4125,
                    (undefined1 *)&DAT_451b_2281 + iVar1 * 0x48 + iVar2 * 8,0x4125);
        }
      }
      for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
        ((undefined1 *)&DAT_451b_2281)[iVar2 * 0x48] =
             ((undefined1 *)&DAT_451b_2281)[iVar2 * 0x48] & 0xfe;
      }
      for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
        ((undefined1 *)&DAT_451b_24c1)[iVar2 * 8] = ((undefined1 *)&DAT_451b_24c1)[iVar2 * 8] & 0xfe
        ;
      }
      break;
    case 0x893ee:
      for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
        for (iVar1 = 0; iVar1 < 9; iVar1 = iVar1 + 1) {
          __F_SCOPY(iVar2 * 0x48 + iVar1 * 8 + 0x6229,0x4125,
                    (undefined1 *)&DAT_451b_2281 + iVar2 * 0x48 + iVar1 * 8,0x4125);
        }
      }
      for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
        ((undefined1 *)&DAT_451b_24c1)[iVar2 * 8] = ((undefined1 *)&DAT_451b_24c1)[iVar2 * 8] & 0xfe
        ;
      }
      break;
    case 0x89442:
      for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
        for (iVar1 = 0; iVar1 < 9; iVar1 = iVar1 + 1) {
          __F_SCOPY(iVar2 * 0x48 + iVar1 * 8 + 0x6231,0x4125,
                    (undefined1 *)&DAT_451b_2281 + iVar2 * 0x48 + iVar1 * 8,0x4125);
        }
      }
      for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
        ((undefined1 *)&DAT_451b_24c1)[iVar2 * 8] = ((undefined1 *)&DAT_451b_24c1)[iVar2 * 8] & 0xfe
        ;
      }
      for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
        ((undefined1 *)&DAT_451b_22c1)[iVar2 * 0x48] =
             ((undefined1 *)&DAT_451b_22c1)[iVar2 * 0x48] & 0xfe;
      }
    }
  }
  if (7 < param_1) {
    return;
  }
                    /* WARNING: Switch is manually overridden */
  switch((undefined4)*(undefined2 *)(param_1 * 2 + 0x34cd)) {
  case 0x894ba:
    uVar6 = 0;
    uVar5 = 0x1400168;
    goto LAB_8611_33b7;
  case 0x894db:
    uVar6 = 0x28;
    uVar5 = 0x1400140;
LAB_8611_33b7:
    uVar3 = 0x280000;
LAB_8611_33bd:
    (*DAT_451b_2a74)(0x8000,0x6795,0x4125,uVar3,uVar5,0x6795,0x4125,uVar6);
    break;
  case 0x894ea:
    for (iVar2 = 8; 0 < iVar2; iVar2 = iVar2 + -1) {
      (*DAT_451b_2a74)(0x8000,0x6795,0x4125,(iVar2 + -1) * 0x28,0,0x1680028,0x6795,0x4125,
                       iVar2 * 0x28,0);
    }
    break;
  case 0x8951d:
    for (iVar2 = 8; 0 < iVar2; iVar2 = iVar2 + -1) {
      (*DAT_451b_2a74)(0x8000,0x6795,0x4125,(iVar2 + -1) * 0x28,0,0x1400028,0x6795,0x4125,
                       iVar2 * 0x28,0x28);
    }
    break;
  case 0x89550:
    for (iVar2 = 8; 0 < iVar2; iVar2 = iVar2 + -1) {
      (*DAT_451b_2a74)(0x8000,0x6795,0x4125,0,(iVar2 + -1) * 0x28,0x280168,0x6795,0x4125,0,
                       iVar2 * 0x28);
    }
    break;
  case 0x89582:
    for (iVar2 = 8; 0 < iVar2; iVar2 = iVar2 + -1) {
      (*DAT_451b_2a74)(0x8000,0x6795,0x4125,0x28,(iVar2 + -1) * 0x28,0x280140,0x6795,0x4125,0,
                       iVar2 * 0x28);
    }
    break;
  case 0x895b4:
    uVar5 = 0x1680140;
    uVar4 = 0;
    goto LAB_8611_34c4;
  case 0x895c5:
    uVar5 = 0x1400140;
    uVar4 = 0x28;
LAB_8611_34c4:
    uVar6 = 0;
    uVar3 = CONCAT22(uVar4,0x28);
    goto LAB_8611_33bd;
  }
  return;
}


