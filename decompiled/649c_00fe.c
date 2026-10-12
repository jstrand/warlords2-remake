
void FUN_649c_00fe(undefined2 param_1,int param_2,int param_3)

{
  undefined2 unaff_SS;
  char acStack_36 [50];
  int iStack_4;
  
  iStack_4 = city_make_ruins(param_2,param_3);
  FUN_22bf_07a5();
  FUN_834b_0000(0x10001,0);
  FUN_22bf_0757();
  if (DAT_451b_01c4 != 0) {
    sprintf((char *)CONCAT22(unaff_SS,acStack_36),s__s_is_in_ruins__4125_0ab3,
            iStack_4 * 0x41 + 0x1581,0x3c04);
    FUN_8065_10fb(acStack_36);
    FUN_8611_006c();
  }
  return;
}


