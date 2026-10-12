
void __cdecl16far FUN_7f77_0a86(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 local_8;
  undefined2 local_6;
  undefined2 local_4;
  
  uVar2 = 1;
  local_4 = 1;
  local_6 = 1;
  local_8 = 1;
  for (iVar1 = 0; iVar1 < 7; iVar1 = iVar1 + 1) {
    FUN_18a9_066e(iVar1 + 0x1dc,1);
  }
  uVar3 = (undefined2)((ulong)DAT_451b_1d82 >> 0x10);
  if ((DAT_451b_1d80 + -1 <= ((int *)DAT_451b_1d82)[6]) || (((int *)DAT_451b_1d82)[6] < 0)) {
    local_6 = 2;
    local_8 = 2;
  }
  if (*DAT_451b_1d82 < 1) {
    uVar2 = 2;
    local_4 = 2;
  }
  FUN_18a9_066e(0x1d8,uVar2);
  FUN_18a9_066e(0x1d6,local_4);
  FUN_18a9_066e(0x1d7,local_8);
  FUN_18a9_066e(0x1d9,local_6);
  FUN_18a9_066e(0x101da);
  FUN_18a9_066e(0x101db);
  return;
}


