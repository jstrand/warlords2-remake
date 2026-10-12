
undefined2 FUN_297c_0306(void)

{
  code *pcVar1;
  byte bVar2;
  undefined2 in_AX;
  undefined1 uVar4;
  uint uVar3;
  int iVar5;
  undefined2 unaff_SS;
  undefined2 uVar6;
  undefined2 in_FPUStatusWord;
  undefined4 in_stack_00000000;
  undefined2 uVar7;
  undefined2 local_4;
  
  uVar7 = 0x4125;
  uVar6 = (undefined2)((ulong)in_stack_00000000 >> 0x10);
  iVar5 = (int)in_stack_00000000;
  FUN_297c_02f0(0x4125);
  uVar3 = *(uint *)(iVar5 + -1);
  uVar4 = (undefined1)(uVar3 >> 8);
  bVar2 = (char)uVar3 - 0x34;
  local_4 = in_AX;
  if (bVar2 < 8) {
    if (CONCAT11(uVar4,bVar2) == -0x1cfd) {
      *(undefined2 *)0x0 = (undefined2 *)&DAT_451b_01a0;
      *(undefined2 *)&DAT_4125_0002 = 0x3f;
      ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x20)[0] = '\0';
      ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x20)[1] = '\0';
      *(undefined2 *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 8) =
           *(undefined2 *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0xc);
      ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0] = '\0';
      ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[1] = '\0';
    }
    else if ((CONCAT11(uVar4,bVar2) != -0x1ff9) ||
            (local_4 = in_FPUStatusWord,
            '\x01' < ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[2])) {
      *(undefined1 *)(iVar5 + -2) = 0x9b;
      *(char *)(iVar5 + -1) = *(char *)(iVar5 + -1) + -0x5c;
      local_4 = in_AX;
    }
  }
  else if (bVar2 < 9) {
    if ((uVar3 & 0x2000) != 0) goto LAB_297c_03af;
    *(undefined1 *)(iVar5 + -2) = 0x9b;
    uVar3 = CONCAT11(uVar4,uVar4) & 0x7c0;
    *(int *)(iVar5 + -1) = (CONCAT11((char)(uVar3 >> 8),(byte)uVar3 >> 3) ^ 0x18) + 0xd826;
  }
  else {
    if (9 < bVar2) {
LAB_297c_03af:
      pcVar1 = (code *)swi(3);
      uVar6 = (*pcVar1)();
      return uVar6;
    }
    *(undefined2 *)(iVar5 + -2) = 0x9b90;
  }
  FUN_297c_02f0(uVar7);
  return local_4;
}


