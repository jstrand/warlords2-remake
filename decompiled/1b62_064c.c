
undefined2 __cdecl16far FUN_1b62_064c(int param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 local_6;
  
  iVar2 = army_count;
  do {
    while( true ) {
      do {
        iVar3 = iVar2 + -1;
        if (iVar2 == 0) {
          return 0;
        }
        iVar1 = iVar3 * 0x16;
        local_6 = (int *)CONCAT22(0x2e1d,(int *)(iVar1 + -0x7780));
        iVar2 = iVar3;
      } while ((*local_6 != param_1) || (*(int *)(iVar1 + -0x777e) != param_2));
      if (param_3 == 100) break;
      if (*(char *)(iVar1 + -0x777c) == (char)param_3) {
        return 1;
      }
    }
  } while (((*(char *)(iVar1 + -0x777c) == '\x05') || (*(char *)(iVar1 + -0x777c) == '\b')) ||
          (*(char *)(iVar1 + -0x777c) == '\t'));
  return 1;
}


