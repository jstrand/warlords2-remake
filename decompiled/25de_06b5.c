
void __cdecl16near FUN_25de_06b5(void)

{
  char *pcVar1;
  undefined2 uVar2;
  int iVar3;
  char *pcVar4;
  undefined2 unaff_ES;
  
  if (((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x1c] != -0x33) {
    uVar2 = 0;
    FUN_25de_0757();
    *(undefined2 *)&DAT_4125_0002 = uVar2;
    iVar3 = *(int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 8);
    pcVar4 = (char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x1c;
    do {
      uVar2 = *(undefined2 *)(pcVar4 + 1);
      *(undefined2 *)pcVar4 = DAT_4125_0110;
      pcVar1 = pcVar4 + 4;
      *(undefined2 *)(pcVar4 + 2) = uVar2;
      pcVar4 = pcVar4 + 5;
      *pcVar1 = '\0';
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  return;
}


