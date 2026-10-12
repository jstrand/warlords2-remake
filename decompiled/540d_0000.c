
void __cdecl16far auto_file_current_sgn(int param_1,int param_2,char *param_3,char *param_4)

{
  int iVar1;
  uint unaff_DI;
  int iVar2;
  void *unaff_SS;
  char *pcVar3;
  char local_be [80];
  int local_6e;
  int local_6c;
  char local_6a [50];
  char local_38 [50];
  int local_6;
  int local_4;
  
  local_6 = 0;
  sprintf((char *)CONCAT22(unaff_SS,local_be),s_current_sgn_4125_03b2);
  iVar1 = open((char *)CONCAT22(unaff_SS,local_be),2);
  if (-1 < iVar1) {
    read(iVar1,(void *)CONCAT22(2,unaff_SS),unaff_DI);
    iVar2 = 0;
    do {
      read(iVar1,(void *)CONCAT22(0x68,unaff_SS),unaff_DI);
      if ((local_6e == param_1) && (local_6c == param_2)) {
        local_6 = 1;
      }
      iVar2 = iVar2 + 1;
    } while ((local_6 == 0) && (iVar2 < local_4));
    close(iVar1);
    if (local_6 == 0) {
      strcpy(param_3,s_current_sgn_4125_03b2 + 0xc);
      unaff_SS = (void *)((int)(undefined2 *)&DAT_451b_01c4 + 1);
      pcVar3 = (char *)s_current_sgn_4125_03b2 + 0xd;
    }
    else {
      strcpy(param_3,(char *)CONCAT22(unaff_SS,local_6a));
      pcVar3 = local_38;
    }
    strcpy(param_4,(char *)CONCAT22(unaff_SS,pcVar3));
    iVar1 = strcmp(param_3,(char *)0x412503c0);
    if ((iVar1 == 0) && (iVar1 = strcmp(param_4,(char *)0x412503c1), iVar1 == 0)) {
      sprintf(param_3,s__d___d_4125_03c2,param_1,param_2);
    }
  }
  return;
}


