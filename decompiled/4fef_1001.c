
bool __cdecl16far auto_file_x_scn(void)

{
  bool bVar1;
  int iVar2;
  uint unaff_DI;
  int iVar3;
  undefined2 unaff_SS;
  char *fmt;
  char *pcVar4;
  undefined1 local_a6 [20];
  char local_92 [62];
  int local_54;
  char local_52 [40];
  char local_2a [40];
  
  bVar1 = false;
  strcpy((char *)CONCAT22(unaff_SS,local_52),s_Erythea_4125_02b2);
  iVar3 = 0;
  while ((!bVar1 && (DAT_4125_28ca != 0))) {
    iVar2 = auto_file_scenario_dat(iVar3,local_a6);
    if (iVar2 == 0) {
      bVar1 = true;
    }
    else {
      iVar3 = iVar3 + 1;
      if (local_54 == DAT_4125_28ca) {
        bVar1 = true;
        strcpy((char *)CONCAT22(unaff_SS,local_52),(char *)CONCAT22(unaff_SS,local_92));
      }
    }
  }
  sprintf((char *)CONCAT22(unaff_SS,local_2a),(char *)0x412502ba,local_52);
  iVar3 = open((char *)CONCAT22(unaff_SS,local_2a),2);
  if (-1 < iVar3) {
    read(iVar3,(void *)0x2ee13c04,unaff_DI);
    close(iVar3);
  }
  else {
    pcVar4 = local_2a;
    fmt = get_error_string(0,0);
    printf(fmt,pcVar4);
  }
  return -1 < iVar3;
}


