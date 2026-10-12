
void __cdecl16far
FUN_8065_1471(char *param_1,char *param_2,char *param_3,char *param_4,char *param_5,char *param_6,
             char *param_7)

{
  int iVar1;
  undefined2 uVar2;
  
  FUN_78a8_06ae(2,0x10007);
  FUN_22bf_07a5();
  iVar1 = FUN_54f6_093a();
  if (iVar1 == 0xd) {
    if ((param_1 != (char *)0x0) && (*param_1 != '\0')) {
      FUN_21e2_0a8e((char *)param_1,(int)((ulong)param_1 >> 0x10));
      FUN_24d0_0002();
      FUN_2133_02fe();
      FUN_7ecb_00d6(0x8c0148,param_1);
    }
    if ((param_2 != (char *)0x0) && (*param_2 != '\0')) {
      FUN_7ecb_00d6(0xa50148,(char *)param_2,param_2._2_2_);
    }
    if ((param_3 != (char *)0x0) && (*param_3 != '\0')) {
      FUN_7ecb_00d6(0xb90148,(char *)param_3,param_3._2_2_);
    }
    if ((param_4 != (char *)0x0) && (*param_4 != '\0')) {
      FUN_7ecb_00d6(0xcd0148,(char *)param_4,param_4._2_2_);
    }
    if ((param_5 != (char *)0x0) && (*param_5 != '\0')) {
      FUN_7ecb_00d6(0xe10148,(char *)param_5,param_5._2_2_);
    }
    if ((param_6 != (char *)0x0) && (*param_6 != '\0')) {
      FUN_7ecb_00d6(0xf50148,(char *)param_6,param_6._2_2_);
    }
    if ((param_7 == (char *)0x0) || (*param_7 == '\0')) goto LAB_8065_1685;
    uVar2 = 0x109;
  }
  else {
    if ((param_2 != (char *)0x0) && (*param_2 != '\0')) {
      FUN_7ecb_00d6(0xa10148,(char *)param_2,param_2._2_2_);
    }
    if ((param_3 != (char *)0x0) && (*param_3 != '\0')) {
      FUN_7ecb_00d6(0xb50148,(char *)param_3,param_3._2_2_);
    }
    if ((param_4 != (char *)0x0) && (*param_4 != '\0')) {
      FUN_7ecb_00d6(0xc90148,(char *)param_4,param_4._2_2_);
    }
    if ((param_5 != (char *)0x0) && (*param_5 != '\0')) {
      FUN_7ecb_00d6(0xdd0148,(char *)param_5,param_5._2_2_);
    }
    if ((param_6 != (char *)0x0) && (*param_6 != '\0')) {
      FUN_7ecb_00d6(0xf10148,(char *)param_6,param_6._2_2_);
    }
    if ((param_7 == (char *)0x0) || (*param_7 == '\0')) goto LAB_8065_1685;
    uVar2 = 0x105;
  }
  FUN_7ecb_00d6(0x148,uVar2,param_7);
LAB_8065_1685:
  FUN_22bf_0757();
  return;
}


