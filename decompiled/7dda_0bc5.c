
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far sound_options_load(void)

{
  int iVar1;
  uint unaff_SI;
  void *unaff_SS;
  char *pcVar2;
  char local_6;
  char local_5;
  char local_4;
  
  iVar1 = 2;
  pcVar2 = get_file_string(0x44,0);
  iVar1 = open(pcVar2,iVar1);
  strcpy((char *)CONCAT22(unaff_SS,&local_6),(char *)0x4125266f);
  if (-1 < iVar1) {
    read(iVar1,(void *)CONCAT22(3,unaff_SS),unaff_SI);
    close(iVar1);
  }
  _DAT_4125_0bee = local_6 + -0x30;
  DAT_4125_0bec = local_5 + -0x30;
  if (DAT_4125_37a3 == '\0') {
    _DAT_4125_0bee = 0;
  }
  if (DAT_4125_37a2 == '\0') {
    DAT_4125_0bec = 0;
  }
  if (DAT_4125_26c8 == 0) {
    DAT_4125_26c8 = (uint)(local_4 == '0');
  }
  else {
    DAT_4125_26c8 = 1;
  }
  if (DAT_4125_37a2 != '\0') {
    pcVar2 = get_file_string(0x40,0);
    DAT_451b_1a14 = sample_load(pcVar2);
    pcVar2 = get_file_string(0x40,1);
    DAT_451b_1a16 = sample_load(pcVar2);
    pcVar2 = get_file_string(0x40,2);
    DAT_451b_1a18 = sample_load(pcVar2);
    pcVar2 = get_file_string(0x40,5);
    DAT_451b_1a1e = sample_load(pcVar2);
  }
  return;
}


