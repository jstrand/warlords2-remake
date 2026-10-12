
undefined4 __cdecl16near __sbrk(uint param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  undefined2 uVar3;
  int iVar4;
  undefined2 uVar5;
  undefined1 uVar6;
  undefined1 uVar7;
  
  iVar4 = 0;
  uVar1 = __F_LXLSH();
  iVar2 = uVar1 + DAT_4125_008b + param_1;
  uVar1 = iVar4 + (uint)CARRY2(uVar1,DAT_4125_008b) + param_2 +
          (uint)CARRY2(uVar1 + DAT_4125_008b,param_1);
  uVar6 = uVar1 < 0xf;
  uVar7 = uVar1 == 0xf;
  if ((int)uVar1 < 0xf) {
LAB_1000_36ae:
    uVar5 = DAT_4125_008d;
    uVar3 = __F_PADD();
    __N_PCMP();
    if (((!(bool)uVar6) && (__N_PCMP(), (bool)uVar6 || (bool)uVar7)) &&
       (iVar2 = __brk(uVar3,uVar5), iVar2 != 0)) goto LAB_1000_3704;
  }
  else if ((int)uVar1 < 0x10) {
    uVar6 = iVar2 != -1;
    uVar7 = iVar2 == -1;
    goto LAB_1000_36ae;
  }
  uVar5 = 0xffff;
  uVar3 = 0xffff;
LAB_1000_3704:
  return CONCAT22(uVar5,uVar3);
}


