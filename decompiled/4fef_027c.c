
void __cdecl16far auto_file_random_map(void)

{
  int iVar1;
  uint unaff_SI;
  void *pvVar2;
  undefined2 unaff_SS;
  char *pcVar3;
  char *pcVar4;
  char local_34 [50];
  
  pcVar3 = get_file_string(0x1e,0);
  sprintf((char *)CONCAT22(unaff_SS,local_34),pcVar3);
  iVar1 = open((char *)CONCAT22(unaff_SS,local_34),9);
  if (iVar1 < 0) {
    pcVar4 = local_34;
    pcVar3 = get_error_string(1,0);
    sprintf(&DAT_451b_1d28,pcVar3,pcVar4);
    auto_str_fatal_error((undefined1 *)&DAT_451b_1d28,0x4125);
  }
  write(iVar1,(void *)0x88802e1d,unaff_SI);
  close(iVar1);
  pcVar3 = get_file_string(0x1f,0);
  sprintf((char *)CONCAT22(unaff_SS,local_34),pcVar3);
  iVar1 = open((char *)CONCAT22(unaff_SS,local_34),9);
  if (iVar1 < 0) {
    pcVar4 = local_34;
    pcVar3 = get_error_string(1,0);
    sprintf(&DAT_451b_1d28,pcVar3,pcVar4);
    auto_str_fatal_error((undefined1 *)&DAT_451b_1d28,0x4125);
  }
  write(iVar1,(void *)0x444029d9,unaff_SI);
  close(iVar1);
  pcVar3 = get_file_string(0x23,0);
  sprintf((char *)CONCAT22(unaff_SS,local_34),pcVar3);
  iVar1 = open((char *)CONCAT22(unaff_SS,local_34),9);
  if (iVar1 < 0) {
    pcVar4 = local_34;
    pcVar3 = get_error_string(1,0);
    sprintf(&DAT_451b_1d28,pcVar3,pcVar4);
    auto_str_fatal_error((undefined1 *)&DAT_451b_1d28,0x4125);
  }
  pvVar2 = (void *)((ulong)DAT_451b_002a >> 0x10);
  write(iVar1,(void *)CONCAT22(*(undefined2 *)((int)DAT_451b_002a + 0x1064),pvVar2),unaff_SI);
  close(iVar1);
  pcVar3 = get_file_string(0x22,0);
  sprintf((char *)CONCAT22(unaff_SS,local_34),pcVar3);
  iVar1 = open((char *)CONCAT22(unaff_SS,local_34),9);
  if (iVar1 < 0) {
    pcVar4 = local_34;
    pcVar3 = get_error_string(1,0);
    sprintf(&DAT_451b_1d28,pcVar3,pcVar4);
    auto_str_fatal_error((undefined1 *)&DAT_451b_1d28,0x4125);
  }
  pvVar2 = (void *)((ulong)DAT_451b_002a >> 0x10);
  write(iVar1,(void *)CONCAT22(*(undefined2 *)((int)DAT_451b_002a + 0x1066),pvVar2),unaff_SI);
  close(iVar1);
  return;
}


