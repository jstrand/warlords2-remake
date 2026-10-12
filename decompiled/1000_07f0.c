
void __cdecl16far asm_strcat(char *param_1,char *param_2)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  char *pcVar4;
  char *pcVar5;
  undefined2 uVar6;
  
  uVar6 = (undefined2)((ulong)param_1 >> 0x10);
  pcVar5 = (char *)param_1;
  iVar3 = -1;
  do {
    if (iVar3 == 0) break;
    iVar3 = iVar3 + -1;
    pcVar2 = pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (*pcVar2 != '\0');
  pcVar5 = pcVar5 + -1;
  pcVar4 = (char *)param_2;
  do {
    pcVar2 = pcVar4;
    pcVar4 = pcVar4 + 1;
    cVar1 = *pcVar2;
    pcVar2 = pcVar5;
    pcVar5 = pcVar5 + 1;
    *pcVar2 = cVar1;
  } while (cVar1 != '\0');
  return;
}


