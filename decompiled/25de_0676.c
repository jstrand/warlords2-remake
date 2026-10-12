
void __cdecl16near FUN_25de_0676(void)

{
  char *pcVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  int iVar4;
  char *pcVar5;
  undefined2 unaff_ES;
  
  if (*(int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 8) == 0) {
    return;
  }
  if (((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x1c] != -0x16) {
    if (*(int *)&DAT_4125_0002 != 0) {
      FUN_25de_0757();
    }
    uVar2 = *(undefined2 *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0xc);
    iVar4 = *(int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 8);
    pcVar5 = (char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x1c;
    do {
      uVar3 = *(undefined2 *)(pcVar5 + 2);
      *pcVar5 = -0x16;
      pcVar1 = pcVar5 + 3;
      *(undefined2 *)(pcVar5 + 1) = uVar3;
      pcVar5 = pcVar5 + 5;
      *(undefined2 *)pcVar1 = uVar2;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
  return;
}


