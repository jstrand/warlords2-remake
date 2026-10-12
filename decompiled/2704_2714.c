
/* WARNING: Unable to track spacebase fully for stack */

undefined2 __cdecl16near FUN_2704_2714(void)

{
  code *pcVar1;
  byte bVar2;
  undefined2 in_AX;
  undefined2 uVar3;
  byte bVar4;
  undefined2 in_CX;
  uint uVar5;
  undefined2 in_BX;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  
  uVar5 = CONCAT11((char)((uint)in_CX >> 8),*(undefined1 *)(undefined2 *)&DAT_4125_0002) & 0xff7f;
  bVar4 = (byte)uVar5 ^ 0x7f;
  bVar2 = *(byte *)0x0 | (byte)(uVar5 >> 8);
  if (((bVar2 & bVar4 ^ bVar2) & 8) != 0) {
    bVar2 = bVar2 | 0x20;
  }
  if ((bVar4 & bVar2) != 0) {
    *(byte *)0x0 = bVar2 | 0x80;
    if ((bVar2 & bVar4) == 0x20) {
      return in_AX;
    }
    FUN_1000_10bf(0x4125);
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    *(undefined2 *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x11) = in_BX;
    *(undefined2 *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x13) = unaff_ES;
                    /* WARNING: Could not recover jumptable at 0x000297b2. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar3 = (*(code *)(ulong)*(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x11))
                      ();
    return uVar3;
  }
  *(byte *)0x0 = bVar2 & 0x7f;
  return in_AX;
}


