
void __cdecl16far FUN_796c_0301(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 local_8;
  undefined2 local_6;
  undefined2 local_4;
  
  uVar2 = 1;
  local_4 = 1;
  local_6 = 1;
  local_8 = 1;
  for (iVar1 = 0; iVar1 < 5; iVar1 = iVar1 + 1) {
    FUN_18a9_066e(iVar1 + 0x72,1);
  }
  if (DAT_451b_08ce < 6) {
    FUN_18a9_03b3();
    FUN_18a9_03b3();
    FUN_18a9_03b3();
    FUN_18a9_03b3();
  }
  else {
    if ((DAT_451b_08ce + -1 <= DAT_451b_08d8) || (DAT_451b_08d8 < 0)) {
      local_6 = 2;
      local_8 = 2;
    }
    if (DAT_451b_08d0 < 1) {
      uVar2 = 2;
      local_4 = 2;
    }
    FUN_18a9_066e(0x77,uVar2);
    FUN_18a9_066e(0x79,local_4);
    FUN_18a9_066e(0x7a,local_8);
    FUN_18a9_066e(0x78,local_6);
  }
  FUN_18a9_066e(0x1007b);
  FUN_18a9_066e(0x1007c);
  return;
}


