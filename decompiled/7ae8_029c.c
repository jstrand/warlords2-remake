
void __cdecl16far FUN_7ae8_029c(int param_1)

{
  undefined2 uVar1;
  undefined4 uVar2;
  
  if (param_1 == 0) {
    uVar2 = 0x80008;
  }
  else {
    if (param_1 != 1) {
      if (param_1 != 2) {
        return;
      }
      uVar2 = 0xf000f;
      uVar1 = 3;
      goto LAB_7ae8_02cc;
    }
    uVar2 = 0xf000f;
  }
  uVar1 = 0;
LAB_7ae8_02cc:
  FUN_78a8_06ae(0,uVar1,uVar2);
  return;
}


