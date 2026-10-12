
void __cdecl16far FUN_67cc_1a41(int param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_SS;
  undefined4 local_12;
  undefined4 local_e;
  undefined1 local_a [8];
  
  FUN_8961_0227(DAT_3c04_017d,DAT_3c04_017f);
  FUN_8611_0053();
  FUN_1997_034a();
  __F_SCOPY(param_3 * 8 + 0xd02,0x4125,&local_12,unaff_SS);
  iVar1 = (param_1 - DAT_3c04_0177) * 0x28;
  iVar2 = (param_2 - DAT_3c04_0175) * 0x28;
  (*DAT_451b_2a68)(0x1000,0x5731,0x4125,local_12,local_e,0x6795,0x4125,iVar1,iVar2,0x56f6,0x4125,
                   local_12,iVar1,iVar2);
  FUN_216d_002e(local_a);
  FUN_24d0_009d(0x6795,0x4125,local_a);
  FUN_8611_1927();
  return;
}


