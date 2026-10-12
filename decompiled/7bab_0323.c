
void __cdecl16far load_scenario_map(void)

{
  int iVar1;
  uint unaff_SI;
  undefined2 unaff_SS;
  char *pcVar2;
  char *pcVar3;
  char local_98 [100];
  char local_34 [50];
  
  sprintf((char *)CONCAT22(unaff_SS,local_34),s__s__s_map_4125_2578,0x5da0,0x4125,0x5da0,0x4125);
  iVar1 = open((char *)CONCAT22(unaff_SS,local_34),2);
  if (iVar1 < 0) {
    pcVar3 = local_34;
    pcVar2 = get_error_string(1,0);
    sprintf((char *)CONCAT22(unaff_SS,local_98),pcVar2,pcVar3);
    auto_str_fatal_error(local_34);
  }
  read(iVar1,(void *)0x88802e1d,unaff_SI);
  close(iVar1);
  sprintf((char *)CONCAT22(unaff_SS,local_34),(char *)0x41252582,0x5da0,0x4125,0x5da0,0x4125);
  iVar1 = open((char *)CONCAT22(unaff_SS,local_34),2);
  if (iVar1 < 0) {
    pcVar3 = local_34;
    pcVar2 = get_error_string(1,0);
    sprintf((char *)CONCAT22(unaff_SS,local_98),pcVar2,pcVar3);
    auto_str_fatal_error(local_34);
  }
  read(iVar1,(void *)0x444029d9,unaff_SI);
  close(iVar1);
  return;
}


