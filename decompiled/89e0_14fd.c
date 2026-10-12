
void __cdecl16far FUN_89e0_14fd(void)

{
  undefined2 uVar1;
  int iVar2;
  
  DAT_4125_30f6 = DAT_4125_3126;
  for (iVar2 = 0; iVar2 < DAT_4125_30f6; iVar2 = iVar2 + 1) {
    uVar1 = ((undefined2 *)&DAT_451b_28b0)[iVar2 * 2];
    ((undefined2 *)&DAT_451b_2904)[iVar2 * 2] = ((undefined2 *)&DAT_451b_28b2)[iVar2 * 2];
    ((undefined2 *)&DAT_451b_2902)[iVar2 * 2] = uVar1;
    ((undefined2 *)&DAT_451b_28f2)[iVar2] = ((undefined2 *)&DAT_451b_28a0)[iVar2];
    ((undefined2 *)&DAT_451b_28e2)[iVar2] = ((undefined2 *)&DAT_451b_2890)[iVar2];
    ((undefined2 *)&DAT_451b_28d2)[iVar2] = ((undefined2 *)&DAT_451b_2880)[iVar2];
  }
  DAT_451b_28d0 = DAT_451b_287e;
  FUN_22bf_07a5();
  FUN_54f6_069c();
  game_screen_start(0);
  FUN_22bf_0757();
  return;
}


