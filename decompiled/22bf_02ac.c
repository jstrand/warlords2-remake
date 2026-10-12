
void __cdecl16far FUN_22bf_02ac(void)

{
  undefined2 uVar1;
  
  uVar1 = 0x22bf;
  if (DAT_4125_3574 == 0) {
    DAT_451b_2fac = DAT_4125_356c;
    DAT_451b_2fae = DAT_4125_356e;
  }
  else {
    uVar1 = 0x1ec0;
    FUN_1ec0_000e((undefined2 *)&DAT_451b_2fac,0x4125,(undefined2 *)&DAT_451b_2fae,0x4125);
    DAT_451b_2fac = DAT_4125_3560;
    DAT_451b_2fae = DAT_4125_3562;
  }
  if (((DAT_451b_2fac != DAT_4125_3568) || (DAT_451b_2fae != DAT_4125_356a)) && (DAT_4125_3570 == 0)
     ) {
    (*DAT_451b_2a74)(uVar1,0x357e,0x4125,0,0,DAT_4125_3670,DAT_4125_3674,0x342a,0x4125,
                     DAT_4125_3568 - DAT_451b_2fba & 0xfff8,DAT_4125_356a - DAT_451b_2fb8);
    (*DAT_451b_2a74)(uVar1,0x342a,0x4125,DAT_451b_2fac - DAT_451b_2fba & 0xfff8,
                     DAT_451b_2fae - DAT_451b_2fb8,DAT_4125_3670,DAT_4125_3674,0x357e,0x4125,0,0);
    FUN_22bf_098d(DAT_451b_2fac,DAT_451b_2fae);
    DAT_4125_3568 = DAT_451b_2fac;
    DAT_4125_356a = DAT_451b_2fae;
  }
  return;
}


