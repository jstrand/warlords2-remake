
void FUN_4030_0054(void)

{
  byte *pbVar1;
  byte bVar2;
  code *pcVar3;
  byte bVar4;
  byte bVar5;
  char cVar6;
  undefined2 in_AX;
  long in_ECX;
  int in_BX;
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_SS;
  char in_CF;
  byte in_AF;
  char *pcVar7;
  
  bVar4 = (char)in_AX + -0x77 + in_CF;
  pbVar1 = (byte *)(in_BX + unaff_SI);
  bVar2 = *pbVar1;
  *pbVar1 = *pbVar1 + bVar4;
  *(int *)(unaff_BP + unaff_SI + -0x16) = unaff_BP;
  *(int *)(unaff_BP + unaff_SI + -0x16) = unaff_BP;
  *(char *)(in_ECX + 0x6a) = *(char *)(in_ECX + 0x6a) + (char)in_ECX + CARRY1(bVar2,bVar4);
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + bVar4;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + bVar4;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + bVar4;
  bVar2 = 9 < (bVar4 & 0xf) | in_AF;
  bVar4 = bVar4 + bVar2 * -6;
  bVar5 = bVar4 & 0xf;
  pcVar7 = (char *)(CONCAT11((char)((uint)in_AX >> 8) - bVar2,bVar4) & 0xff0f);
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + bVar5;
  cVar6 = bVar5 + pcVar7[in_BX];
  *pcVar7 = (*pcVar7 - cVar6) - CARRY1(bVar5,pcVar7[in_BX]);
  *pcVar7 = *pcVar7 + cVar6;
  pcVar7[in_BX] = pcVar7[in_BX] + cVar6;
  pcVar7[in_BX] = pcVar7[in_BX] + cVar6;
  pcVar7[in_BX] = pcVar7[in_BX] + cVar6;
  pcVar7[in_BX] = pcVar7[in_BX] + cVar6;
  pcVar7[in_BX] = pcVar7[in_BX] + cVar6;
  pcVar7[in_BX] = pcVar7[in_BX] + cVar6;
  pcVar7[in_BX] = pcVar7[in_BX] + cVar6;
  pcVar7[in_BX] = pcVar7[in_BX] + cVar6;
  pcVar7[in_BX] = pcVar7[in_BX] + cVar6;
  pcVar7[in_BX] = pcVar7[in_BX] + cVar6;
  pcVar3 = (code *)swi(3);
  (*pcVar3)(0x4030,0xffea);
  return;
}


