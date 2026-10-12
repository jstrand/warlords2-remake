
void __cdecl16far FUN_7087_072e(undefined2 param_1,undefined2 param_2,int param_3)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_SS;
  int local_16 [4];
  int local_e;
  undefined1 local_c [8];
  int local_4;
  
  if ((param_3 == 1) && ((bRam00000417 & 3) != 0)) {
    FUN_7087_028b(param_1,param_2,1);
    DAT_4125_0e94 = 0;
  }
  else {
    iVar1 = FUN_828e_0651(param_1,param_2);
    FUN_7087_0410(iVar1,local_c);
    FUN_7087_0410(DAT_4125_0e98,local_16);
    if (((DAT_4125_0e94 == 2) &&
        ((((0 < local_e && (local_e + local_4 < 5)) && (local_16[0] != iVar1)) &&
         ((local_16[1] != iVar1 && (local_16[2] != iVar1)))))) && (local_16[3] != iVar1)) {
      for (iVar2 = 0; iVar2 < local_e; iVar2 = iVar2 + 1) {
        if (iVar1 < 100) {
          *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + local_16[iVar2] * 0x41) =
               *(undefined2 *)(iVar1 * 0x41 + 0x157d);
          *(undefined2 *)(local_16[iVar2] * 0x41 + 0x15b2) = *(undefined2 *)(iVar1 * 0x41 + 0x157f);
        }
        else {
          *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + local_16[iVar2] * 0x41) = 0xfffe;
          *(undefined2 *)(local_16[iVar2] * 0x41 + 0x15b2) = 0xfffe;
        }
      }
    }
    if (DAT_4125_0e94 == 1) {
      FUN_7087_028b(param_1,param_2,param_3);
    }
    iVar2 = DAT_4125_0e98;
    if ((DAT_4125_0e94 != 1) && (iVar1 < 100)) {
      iVar2 = iVar1;
    }
    DAT_4125_0e94 = 0;
    auto_tutorial_hook_3
              (3,*(undefined2 *)(iVar2 * 0x41 + 0x157d),*(undefined2 *)(iVar2 * 0x41 + 0x157f));
    FUN_7087_08e4();
    auto_ui_vector_help(0);
    auto_ui_vector_help(1);
    FUN_7204_03a9();
  }
  return;
}


