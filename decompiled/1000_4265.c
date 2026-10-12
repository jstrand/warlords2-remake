
char * __cdecl16far getenv(char *name)

{
  char *pcVar1;
  char *pcVar2;
  char *pcVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  char *pcVar8;
  undefined2 uVar9;
  int iVar10;
  bool bVar11;
  undefined4 *local_6;
  
  uVar9 = (undefined2)((ulong)name >> 0x10);
  if (name != (char *)0x0) {
    uVar4 = 0xffff;
    pcVar3 = (char *)name;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      pcVar1 = pcVar3;
      pcVar3 = pcVar3 + 1;
    } while (*pcVar1 != '\0');
    iVar5 = ~uVar4 - 1;
    if (iVar5 != 0) {
      iVar10 = (int)((ulong)DAT_4125_3ed0 >> 0x10);
      local_6 = (undefined4 *)DAT_4125_3ed0;
      if (iVar10 != 0 || local_6 != (undefined4 *)0x0) {
        while( true ) {
          pcVar1 = (char *)*local_6;
          iVar7 = (int)((ulong)pcVar1 >> 0x10);
          pcVar3 = (char *)pcVar1;
          if ((iVar7 == 0 && pcVar3 == (char *)0x0) || (*pcVar1 == '\0')) break;
          if ((*name == *pcVar1) && (pcVar3[iVar5] == '=')) {
            bVar11 = true;
            iVar6 = iVar5;
            pcVar8 = (char *)name;
            do {
              if (iVar6 == 0) break;
              iVar6 = iVar6 + -1;
              pcVar2 = pcVar3;
              pcVar3 = pcVar3 + 1;
              pcVar1 = pcVar8;
              pcVar8 = pcVar8 + 1;
              bVar11 = *pcVar1 == *pcVar2;
            } while (bVar11);
            if (bVar11) {
              pcVar3 = pcVar3 + 1;
              goto LAB_1000_42cf;
            }
          }
          local_6 = local_6 + 1;
        }
      }
    }
  }
  iVar7 = 0;
  pcVar3 = (char *)0x0;
LAB_1000_42cf:
  return (char *)CONCAT22(iVar7,pcVar3);
}


