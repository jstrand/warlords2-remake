
void __cdecl16far FUN_7563_120c(void)

{
  int iVar1;
  bool bVar2;
  undefined2 uVar3;
  
  FUN_18a9_066e(0x1011f);
  FUN_18a9_066e(0x10121);
  bVar2 = DAT_451b_0570 != 0;
  if (bVar2) {
    FUN_18a9_066e(0x10122);
  }
  else {
    FUN_18a9_066e(0x122);
  }
  FUN_18a9_066e(0x123,!bVar2);
  iVar1 = game_turn();
  if (iVar1 == 1) {
    uVar3 = 2;
  }
  else {
    uVar3 = 1;
  }
  FUN_18a9_066e(0x120,uVar3);
  return;
}


