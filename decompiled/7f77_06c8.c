
undefined2 __cdecl16far auto_file_scenario_dat(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined2 uVar2;
  int iVar3;
  uint unaff_DI;
  int iVar4;
  char *name;
  
  iVar1 = 2;
  name = get_file_string(0,0);
  iVar1 = open(name,iVar1);
  if (iVar1 < 0) {
LAB_7f77_06ed:
    uVar2 = 0;
  }
  else {
    for (iVar4 = 0; iVar4 <= param_1; iVar4 = iVar4 + 1) {
      iVar3 = read(iVar1,(void *)CONCAT22(0x54,(void *)((ulong)param_2 >> 0x10)),unaff_DI);
      if (iVar3 == 0) {
        close(iVar1);
        goto LAB_7f77_06ed;
      }
    }
    close(iVar1);
    uVar2 = 1;
  }
  return uVar2;
}


