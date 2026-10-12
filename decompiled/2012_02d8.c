
uint __cdecl16far FUN_2012_02d8(uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = param_1 & DAT_451b_2a50;
  for (uVar2 = 1; uVar2 < DAT_451b_2a42; uVar2 = uVar2 + 1) {
    uVar1 = uVar1 << (DAT_451b_2a52 & 0x1f) | param_1 & DAT_451b_2a50;
  }
  return uVar1;
}


