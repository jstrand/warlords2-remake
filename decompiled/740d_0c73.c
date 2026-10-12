
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_740d_0c73(int param_1)

{
  int unaff_SS;
  undefined2 uVar1;
  undefined2 uVar2;
  int iVar3;
  char *pcVar4;
  
  if (*(char *)(param_1 * 0x41 + 0x1592) == '\x0f') {
    uVar2 = *(undefined2 *)(current_player * 2 + 0xb0);
    uVar1 = *(undefined2 *)(current_player * 2 + 0xa0);
  }
  else {
    uVar2 = *(undefined2 *)(*(char *)(param_1 * 0x41 + 0x1592) * 2 + 0xb0);
    uVar1 = *(undefined2 *)(*(char *)(param_1 * 0x41 + 0x1592) * 2 + 0xa0);
  }
  FUN_78a8_06ae(2,uVar1,uVar2,3);
  FUN_7ecb_00d6(DAT_451b_0528 + (int)DAT_451b_052c / 2 + -8,DAT_451b_052a + 0xb,
                param_1 * 0x41 + 0x1581,0x3c04);
  if (*(char *)(*(byte *)(*(int *)(param_1 * 0x41 + 0x157f) * 0xe0 +
                         *(int *)(param_1 * 0x41 + 0x157d) * 2) + 0x710) == '\n') {
    if (*(char *)(param_1 * 0x41 + 0x1592) < '\b') {
      FUN_8611_0bf7((int)*(char *)(param_1 * 0x41 + 0x1592),3,0x342a,0x4125,DAT_451b_0528 + 0x18,
                    DAT_451b_052a + 10);
      FUN_8611_0bf7((int)*(char *)(param_1 * 0x41 + 0x1592),3,0x342a,0x4125,
                    DAT_451b_0528 + (int)DAT_451b_052c + -0x30,DAT_451b_052a + 10);
    }
    iVar3 = _LAB_4125_1042 + DAT_451b_0532;
    __F_SPUSH(_LAB_4125_1040 + DAT_451b_0530,iVar3);
    FUN_1997_0129(8);
    sprintf((char *)CONCAT22(unaff_SS,&stack0xfff4),(char *)0x412510df,
            *(undefined2 *)(param_1 * 0x41 + 0x15a7));
    FUN_7ecb_00bf(unaff_SS + 0x20,iVar3 + -3,&stack0xfff4);
    __F_SPUSH(_LAB_4125_1044 + DAT_451b_0530,iVar3);
    FUN_1997_0129(8);
    pcVar4 = &stack0xfff4;
    sprintf((char *)CONCAT22(unaff_SS,pcVar4),(char *)0x412510e2);
    FUN_7ecb_00bf(pcVar4 + 0x20);
  }
  else {
    FUN_7ecb_00d6(DAT_451b_0528 + (int)DAT_451b_052c / 2 + -8,DAT_451b_052a + 0x23,0x10d8,0x4125);
  }
  iVar3 = 0;
  while( true ) {
    if (7 < iVar3) {
      return;
    }
    if ((*(int *)(iVar3 * 0x14 + 0x193) == *(int *)(param_1 * 0x41 + 0x157d)) &&
       (*(int *)(iVar3 * 0x14 + 0x195) == *(int *)(param_1 * 0x41 + 0x157f))) break;
    iVar3 = iVar3 + 1;
  }
  if (*(char *)(*(byte *)(*(int *)(param_1 * 0x41 + 0x157f) * 0xe0 +
                         *(int *)(param_1 * 0x41 + 0x157d) * 2) + 0x710) != '\n') {
    FUN_8611_0bf7(iVar3,4,0x342a);
  }
  FUN_8611_0bf7(iVar3,4,0x342a);
  return;
}


