
undefined2 __cdecl16far is_computer_turn(void)

{
  undefined2 uVar1;
  
  if ((*(int *)(current_player * 2 + 0xd0) == 1) || (DAT_4125_0836 != 0)) {
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}


