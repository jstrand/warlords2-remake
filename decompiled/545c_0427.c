
void __cdecl16far auto_file_udb_cur(void)

{
  int iVar1;
  uint unaff_SI;
  
  iVar1 = open((char *)0x4125058a,1);
  if (-1 < iVar1) {
    read(iVar1,(void *)0x84125,unaff_SI);
    close(iVar1);
    for (iVar1 = 0; iVar1 < 4; iVar1 = iVar1 + 1) {
      auto_file_udb_dat_3(((undefined2 *)&DAT_451b_0042)[iVar1],(undefined2 *)&DAT_451b_004a + iVar1
                          ,0x4125);
    }
  }
  return;
}


