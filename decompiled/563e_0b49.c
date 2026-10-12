
int __cdecl16far FUN_563e_0b49(undefined2 param_1,undefined4 param_2,int param_3,undefined2 param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 in_DX;
  int iVar4;
  int local_10;
  int local_e;
  undefined1 local_8 [2];
  undefined2 local_6;
  undefined2 uStack_4;
  
  local_6 = FUN_1555_1b0a();
  local_e = -1;
  local_10 = 1000;
  iVar3 = city_count;
  uStack_4 = in_DX;
  while (iVar4 = iVar3 + -1, iVar3 != 0) {
    iVar3 = iVar4;
    if (*(char *)(iVar4 * 0x41 + 0x1592) == param_3) {
      iVar1 = FUN_59bf_0a85(iVar4,local_8);
      if ((iVar1 < 0x32) && (iVar1 < DAT_451b_1ea0)) {
        iVar2 = FUN_623c_15c5(*(undefined2 *)(iVar4 * 0x41 + 0x157d),
                              *(undefined2 *)(iVar4 * 0x41 + 0x157f));
        if ((0x4b < iVar2) && (iVar1 < local_10)) {
          local_10 = iVar1;
          local_e = iVar4;
        }
      }
    }
  }
  iVar3 = FUN_563e_0c32(param_1,param_2,param_3,param_4,local_6,uStack_4);
  if ((iVar3 != 0) && (local_e != -1)) {
    iVar3 = FUN_563e_0f1b(param_1,(int)param_2,(int)((ulong)param_2 >> 0x10),local_e);
  }
  FUN_1555_1b2e();
  return iVar3;
}


