
int __cdecl16far FUN_1a8b_0476(undefined2 param_1,undefined2 param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int iVar6;
  int *piVar5;
  
  iVar6 = 0;
  if (DAT_3c04_0159 != 0) {
    FUN_8611_0565();
  }
  auto_ui_being_attacked(param_1,param_2,1);
  iVar2 = 8;
  piVar4 = (int *)&DAT_451b_1efe;
  while( true ) {
    piVar5 = piVar4 + -2;
    iVar3 = iVar2 + -1;
    if (iVar2 == 0) break;
    piVar1 = piVar4 + -1;
    iVar2 = iVar3;
    piVar4 = piVar5;
    if (*piVar5 != 0 || *piVar1 != 0) {
      iVar6 = iVar6 + 1;
    }
  }
  if (iVar6 != 0) {
    stack_movement_mode();
  }
  return iVar6;
}


